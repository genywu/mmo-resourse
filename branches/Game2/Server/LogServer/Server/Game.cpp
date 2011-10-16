#include "StdAfx.h"
#include "Game.h"

#include "../App/LogManage.h"
#include "../Net/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD g_ms = 40;

// 调试用
DWORD g_lCurTime = 0;
DWORD g_lLSMessageTime = 0;
DWORD g_lGSMessageTime = 0;
DWORD g_lCSMessageTime = 0;

CGame* g_pGame= NULL;		// 游戏客户端数据

//_ConnectionPtr g_Connection = NULL;		// ADO主连接

HANDLE g_hGameThreadExitEvent=NULL;		// 主线程退出事件
bool g_bGameThreadExit=false;		// 是否退出主线程

HANDLE	hConnectThread=NULL;			// 连接线程句柄 
bool	bConnectThreadExit=false;		// 是否退出连接线程

HANDLE hConnectThread1=NULL;
bool	bConnectThreadExit1=false;

vector<HANDLE> g_vPlayerLoadThreads;		// 玩家读数据库线程
bool volatile g_bPlayerLoadThreadsExit = false;
HANDLE g_hSavingThread = NULL; // 存盘线程句柄

enum MyMenu
{
    LOAD_SETUP = MY_MENU * 2,     // 重读配置
    LOAD_CONFIG,                  // 重读日志的读写控制配置，并发送到所有连接的客户端
    OPER_KILLALL,                 // 踢世界所有人
    OPER_CREATETABLE,             // 按配置在DB中创建数据表
    OPER_CREATESQL,               // 生成数据表创建脚本
    OPER_OUTTABLE,                // 输出数据表信息到文件
};

void RefreshInfo();					// 刷新服务器信息
void RefreshInfo(const char *pInfoText)
{
	SetWindowText(AppFrame::GetWnd("info"), pInfoText);
}

// 创建游戏客户端数据
BOOL CreateGame()
{
	g_pGame = new CGame;
	return true;
}

// 释放游戏客户端数据
BOOL DeleteGame()
{
	if (g_pGame)
	{
		delete g_pGame;
		g_pGame = NULL;
	}
	return true;
}

// 获取游戏客户端数据
CGame* GetGame()
{
	return g_pGame;
}


CGame::CGame(void)
:m_pGameLogManage(NULL)
,m_AsynchronousExecute(TRUE)
,m_bCreateTable(FALSE)
,m_pDBAllocator(NULL)
{
	s_pNetServer = NULL;


	GetSetup()->strName = "LogServer";
	GetSetup()->dwListenPort = 8100;	


	m_dwLastReLoginTime = timeGetTime();

	m_bInPing = false;
	m_dwLastPingGameServerTime = timeGetTime();
}

CGame::~CGame(void)
{
}

bool CGame::LoadServerResource()
{
	CHAR* strPath = new CHAR[MAX_PATH];
	string strWorpath("");
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		strWorpath=string(strPath);
	}
	delete []strPath;

	// 初始化数据包
	extern CClientResource * g_pDefaultClientResource;
	SAFE_DELETE(g_pDefaultClientResource);
	string strFilesInfo("FilesInfo.ril");
	g_pDefaultClientResource = new CClientResource(GAME_RES,strWorpath,strFilesInfo);
	g_pDefaultClientResource->LoadEx();
	AppFrame::AddStdLog("Load package file OK!");
	return true;
}

bool CGame::LoadSetup()
{
	// 读取全局配置
	string strTemp;
	ifstream stream;
	stream.open("setup.ini");
	if( stream.is_open() )
	{
		stream  >> strTemp >> GetSetup()->dwNumber
			>> strTemp >> GetSetup()->strName
			>> strTemp >> GetSetup()->dwListenPort
			>> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwFreeSockOperNum
			>> strTemp >> GetSetup()->dwFreeIOOperNum
			>> strTemp >> GetSetup()->lIOOperDataBufNum

			>> strTemp >> GetSetup()->bCheckNet
			>> strTemp >> GetSetup()->dwBanIPTime
			>> strTemp >> GetSetup()->dwMaxMsgLen			
			>> strTemp >> GetSetup()->lMaxConnectNum
			>> strTemp >> GetSetup()->lMaxClientsNum
			>> strTemp >> GetSetup()->lPendingWrBufNum
			>> strTemp >> GetSetup()->lPendingRdBufNum
			>> strTemp >> GetSetup()->lMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lMaxBlockedSendMsgNum
			>> strTemp >> GetSetup()->lConPendingWrBufNum
			>> strTemp >> GetSetup()->lConPendingRdBufNum
			>> strTemp >> GetSetup()->lConMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lConMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum

			>> strTemp >> GetSetup()->dwRefeashInfoTime
			>> strTemp >> GetSetup()->dwSaveInfoTime
			// 超过这个时间的LoginList的Player会不清除掉
			>> strTemp >> GetSetup()->dwReleaseLoginPlayerTime
			// 日志系统数据库服务器配置
			//>> strTemp >> GetSetup()->bUseLogSys		//是否起用日志系统的数据库服务器
			//>> strTemp >> GetSetup()->strLogSysProvider	
			//>> strTemp >> GetSetup()->strLogSysServer
			//>> strTemp >> GetSetup()->strLogSysDatabase
			//>> strTemp >> GetSetup()->strLogSysUID
			//>> strTemp >> GetSetup()->strLogSysPWD

			>> strTemp >> GetSetup()->dwMinDbOptThreadNum		//! 最小DB操作线程保持数
			>> strTemp >> GetSetup()->dwMaxDbOptThreadNum		//! 最大DB操作线程数
			>> strTemp >> GetSetup()->dwThreadNumAdjustSpace	//! 自动调整线程数的间隔时间
			; //end ;

		stream.close();
	}
	else	// 从加密文件读取
	{
		int length = GetFileLength("setup.dat");

		FILE *fp;
		fp = fopen("setup.dat", "rb");
		if (fp == NULL)
		{
			return false;
		}

		char *strIn = new char[length+1];
		char *strOut = new char[(length+1)*2];
		if( strIn == NULL ) return false;
		memset(strIn, 0, length+1);
		memset(strOut, 0, (length+1)*2);

		fread(strIn, length, 1, fp);
		fclose(fp);

		IniDecoder(strIn, strOut, length);

		stringstream streamSetup;

		streamSetup << strOut;
		streamSetup.seekg(0, std::ios_base::beg);

		streamSetup  
			>> strTemp >> GetSetup()->dwNumber
			>> strTemp >> GetSetup()->strName
			>> strTemp >> GetSetup()->dwListenPort
			>> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwFreeSockOperNum
			>> strTemp >> GetSetup()->dwFreeIOOperNum
			>> strTemp >> GetSetup()->lIOOperDataBufNum

			>> strTemp >> GetSetup()->bCheckNet
			>> strTemp >> GetSetup()->dwBanIPTime
			>> strTemp >> GetSetup()->dwMaxMsgLen			
			>> strTemp >> GetSetup()->lMaxConnectNum
			>> strTemp >> GetSetup()->lMaxClientsNum
			>> strTemp >> GetSetup()->lPendingWrBufNum
			>> strTemp >> GetSetup()->lPendingRdBufNum
			>> strTemp >> GetSetup()->lMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lMaxBlockedSendMsgNum
			>> strTemp >> GetSetup()->lConPendingWrBufNum
			>> strTemp >> GetSetup()->lConPendingRdBufNum
			>> strTemp >> GetSetup()->lConMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lConMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum

			>> strTemp >> GetSetup()->dwRefeashInfoTime
			>> strTemp >> GetSetup()->dwSaveInfoTime
			// 超过这个时间的LoginList的Player会不清除掉
			>> strTemp >> GetSetup()->dwReleaseLoginPlayerTime
			// 日志系统数据库服务器配置
			//>> strTemp >> GetSetup()->bUseLogSys		//是否起用日志系统的数据库服务器
			//>> strTemp >> GetSetup()->strLogSysProvider	
			//>> strTemp >> GetSetup()->strLogSysServer
			//>> strTemp >> GetSetup()->strLogSysDatabase
			//>> strTemp >> GetSetup()->strLogSysUID
			//>> strTemp >> GetSetup()->strLogSysPWD

			>> strTemp >> GetSetup()->dwMinDbOptThreadNum		//! 最小DB操作线程保持数
			>> strTemp >> GetSetup()->dwMaxDbOptThreadNum		//! 最大DB操作线程数
			>> strTemp >> GetSetup()->dwThreadNumAdjustSpace	//! 自动调整线程数的间隔时间
			; //end ;

	}
    strTemp = "LogServer[";
    strTemp += GetSetup()->strName;
    strTemp += "]";
    if (!AppFrame::SetTitle(strTemp))
        return false;

	AppFrame::NotifyMessageBox("Can't find file setup.ini", "message", MB_OK);
	return false;
}

// 游戏初始化
BOOL CGame::Init()
{
	CreateDirectory( "log",	NULL);

	CHAR* strPath = new CHAR[MAX_PATH];
	string strWorpath("");
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		strWorpath=string(strPath);
	}
	delete []strPath;

	// 初始化数据包

	string strFilesInfo("FilesInfo.ril");
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorpath,strFilesInfo);
	
	srand((unsigned)time(NULL));
	random(100);

	PutDebugString("LogServer start!");

	//装载资源
	LoadServerResource();

	if (LoadSetup()==false)
	{
		return false;
	}

	//! test
	if(NULL == m_pGameLogManage)
	{
		m_pGameLogManage = new LogManage();
	}
	else
	{
#ifdef _DEBUG
		throw;
#endif
	}
	if (!m_pGameLogManage->Init())
	{
		return false;
	}

	//! 初始化异步执行对象
	GetAsynchronousExecute().SetWorkParam(
		GetSetup()->dwMinDbOptThreadNum, 
		GetSetup()->dwMaxDbOptThreadNum, 
		GetSetup()->dwThreadNumAdjustSpace
		);
	GetAsynchronousExecute().BeginWork(GetSetup()->dwMinDbOptThreadNum);

	if( !LoadServerSetup("serverSetup.ini") )
	{
		return false;
	}

	//分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);


	//// 进程使用ADO接口初始Connection string
	CMyAdoBase::Initialize("", "", "", "", "");
	//	GetSetup()->strSqlUserName, GetSetup()->strSqlPassWord);



	//装载无效文字系统
	//CWordsFilter::GetInstance()->Initial(string("setup/InvalidStr.ini"));


	//
	CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
	// 初始化服务器网络
	CMySocket::MySocketInit();

	// 创建服务器端
	if( !InitNetServer() )
	{
		AppFrame::NotifyMessageBox("Can't init NetServer!", "Message", MB_OK);
		return false;
	}

	if(!StartAccept())
	{
		return false;
	}

	return true;
}

// 读取游戏服务器配置
bool CGame::LoadServerSetup(const char *filename)
{
	ifstream stream;
	stream.open(filename);
	if( !stream.is_open() )
	{
		AppFrame::AddStdLog("Load %s lost!", filename);
		return false;
	}
	
	m_setClientIP.clear();
	string strIP;
	stream >> strIP;
	while (!stream.eof())
	{
		//strIP.clear();
		stream >> strIP;
		//! 转换IP
		DWORD dwIP = inet_addr(strIP.c_str());   
		//dwIP = htonl(dwIP);
		if(0 != dwIP)
			m_setClientIP.insert(dwIP);
	}
	stream.close();

	AppFrame::AddStdLog("Load %s succeed!", filename);
	return true;
}

void CGame::OnGameServerConnect(DWORD dwIP, LONG lSocket)
{
	set<DWORD>::iterator ite = m_setClientIP.find(dwIP);
	if(m_setClientIP.end() == ite)
	{
		char szTmp[128] = {0};
		UCHAR arrIp[4] = {0};
		memmove(arrIp, &dwIP, sizeof(LONG));
		AppFrame::AddStdLog("Nonlicet client[%u.%u.%u.%u] connect, It been kicked!!!", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);

		s_pNetServer->DisConn(lSocket);
	}
}

// 读取所有的脚本文件
void FindScriptFile(const char* strFile, list<string>* plistFileName)
{
	char strPath[MAX_PATH];
	GetFilePath(strFile, strPath);

	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(strFile, &stFindData);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		return;
	}

	BOOL bNext = true;
	while(1)
	{
		bNext = FindNextFile(hFind, &stFindData);
		if (bNext==0)
		{
			break;
		}

		// 是目录
		if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (stricmp(stFindData.cFileName, "..")!=0)
			{
				char str[MAX_PATH];
				sprintf(str, "%s%s/*.*", strPath, stFindData.cFileName);
				FindScriptFile(str, plistFileName);
			}
		}
		else	// 文件
		{
			char strExtName[MAX_PATH];
			GetFileExtName(stFindData.cFileName, strExtName);
			if (stricmp(strExtName, ".script")==0)
			{
				char str[MAX_PATH];
				sprintf(str, "%s%s", strPath, stFindData.cFileName);
				strlwr(str);	//  转换成小写
				plistFileName->push_back(str);
			}
		}
	}

	FindClose(hFind);
}








// 游戏释放
BOOL CGame::Release()
{
	PutDebugString("LogServer Exiting...");

	// 关闭网络连接
	if( GetNetServer() )
	{
		s_pNetServer->DisConn(m_lLogingSocket);
		s_pNetServer->DisConn(m_lCostSocket);
		s_pNetServer->Exit();

		SAFE_DELETE(s_pNetServer);
	}

	// 关闭存盘线程句柄
	WaitForSingleObject(g_hSavingThread, INFINITE);
	CloseHandle(g_hSavingThread);

	CMySocket::MySocketClearUp();

	CMessage::Release();

	GetNetSessionManager()->Release();

	// 关闭玩家数据库载入线程
	for(DWORD i=0; i<g_vPlayerLoadThreads.size(); ++i)
	{
		TerminateThread(g_vPlayerLoadThreads[i],0);
		CloseHandle(g_vPlayerLoadThreads[i]);
	}

	//! 先关闭日志对象
	if(NULL != m_pGameLogManage)
		m_pGameLogManage->Exit();
	//! 再等待剩余任务执行完成
	GetAsynchronousExecute().Exit();
	//! 最后销毁游戏日志管理对象
	if(NULL != m_pGameLogManage)
		m_pGameLogManage->Release();

	SAFE_DELETE(m_pGameLogManage);

	//释放资源管理器
	CClientResource* pClientResource = CClientResource::GetInstance();
	if(NULL != pClientResource)
		pClientResource->Release();
	CClientResource::ReleaseInstance();

	CMyAdoBase::Uninitalize();


	SAFE_DELETE(g_pDefaultClientResource);


	SAFE_DELETE(m_pDBAllocator);

	PutDebugString("LogServer Exited!");
	return true;
}



//初始化游戏网络服务器端，建立网络服务器端，装载数据
BOOL CGame::InitNetServer(void)
{
	s_pNetServer = new CServer;
	//做为服务器身份启动
	long lRet = s_pNetServer->Start(2,m_pDBAllocator,
		m_Setup.dwFreeSockOperNum,m_Setup.dwFreeIOOperNum,m_Setup.lIOOperDataBufNum,
		m_Setup.bCheckNet,m_Setup.dwBanIPTime,
		m_Setup.dwMaxMsgLen,m_Setup.lMaxConnectNum,m_Setup.lMaxClientsNum,
		m_Setup.lPendingWrBufNum,m_Setup.lPendingRdBufNum,
		m_Setup.lMaxSendSizePerSecond,m_Setup.lMaxRecvSizePerSecond,
		m_Setup.lMaxBlockedSendMsgNum);
	if( !lRet )
	{
		AppFrame::NotifyMessageBox("Start NetServer Error", "Message", MB_OK);
		return FALSE;
	}
	CMessage::SetServer(s_pNetServer);
	AppFrame::AddStdLog("netServer initializtion succeed!");
	return TRUE;
}

//开始作为服务器监听
bool CGame::StartAccept()
{
	if(NULL == s_pNetServer)	return false;
	BOOL bRet = s_pNetServer->Host(m_Setup.dwListenPort,NULL,NF_Server_Client,SOCK_STREAM);
	if(!bRet)
	{
		AppFrame::NotifyMessageBox("Start Host Error", "Message", MB_OK);
		return false;
	}
	AppFrame::AddStdLog("LogServer begin listen!");
	return true;
}

long g_lRebackMessageCount = 0;

// 消息处理
BOOL CGame::ProcessMessage()
{

	g_lCurTime = timeGetTime();
	long lMessageCount = GetNetServer()->GetRecvMessages()->GetSize();
	for(int i=0; i<lMessageCount; i++)
	{
		CMessage* pMsg = (CMessage*)GetNetServer()->GetRecvMessages()->PopMessage();
		if( pMsg )
		{
			pMsg->Run();
			CBaseMessage::FreeMsg(pMsg);
		}
	}
	return TRUE;
}

bool CGame::ProcMenus()
{
    bool res = false;
    if (AppFrame::MenuClicked(LOAD_SETUP)) // 重读配置
        res = LoadSetup();
    if (AppFrame::MenuClicked(LOAD_CONFIG)) // 重读日志的读写控制配置，并发送到所有连接的客户端
        ReLoadConfig();
    if (AppFrame::MenuClicked(OPER_KILLALL)) // 踢世界所有人
    {
        map<DWORD, CGame::tagGameServer>* pServerList = GetGameServerList();
        if( pServerList ) {
            CMessage msg( MSG_W2S_OTHER_KICKALL );
            for( map<DWORD, CGame::tagGameServer>::iterator itBegin = pServerList -> begin();
                itBegin != pServerList -> end(); itBegin ++ ) {
                    if( itBegin -> second.bConnected ) {				
                        msg.SendGSID( itBegin->first );
                    }
            }
        }
    }
    if (AppFrame::MenuClicked(OPER_CREATETABLE)) // 按配置在DB中创建数据表
        CreateTableInDB();
    if (AppFrame::MenuClicked(OPER_CREATESQL)) // 生成数据表创建脚本
        CreateTableInSqlFile();
    if (AppFrame::MenuClicked(OPER_OUTTABLE)) // 输出数据表信息到文件
        OutTableInfoToFile();
    return res;
}


// 游戏主循环
BOOL CGame::MainLoop()
{
    // 统计每个步骤的耗时
    static DWORD dwRefeashTextTime = 0,
        dwAITime = 0,
        dwMessageTime = 0,
        dwNetSessionTime = 0,
        dwFactionWarSysTime = 0,
        dwTimerTime = 0,
        dwProcessPlayerDataQueueTime = 0,
        dwSessionFactoryTime = 0,
        dwSavePointTime = 0;

    static DWORD dwCurTime = 0;

    static DWORD ai_tick, now, old = timeGetTime(), lastSaveTime=timeGetTime(), lastRefeashInfoTime = now, lastReleaseLoginPlayerTime = 0;
    now = timeGetTime();
    ai_tick++;

    ///////////////////////////////////////////////////////////
    //#define MY_TEST
#ifdef MY_TEST

    //! 测试代码
    static LONG lSendNum = 0;
    if(NULL != m_pGameLogManage && 200000 > lSendNum)
    {
        for (LONG i = 0; i < 500; ++i)
        {
            char szSQL[256] = {0};
            sprintf(szSQL, "insert into table_1(_data)values(N'说大话反抗技术%d')", rand());
            m_pGameLogManage->AddLog(2000, szSQL);
        }
        lSendNum += 500;
    }

#endif
    ///////////////////////////////////////////////////////////

    // 刷新显示
    dwCurTime = timeGetTime();
    static LONG lUpdateCount = 0;
    if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
    {
        lastRefeashInfoTime = now;

        if(NULL!= m_pGameLogManage)
        {
            string strShowInfo("Log Info:\r\n");

            DWORD dwAllCount = 0;
            for (map<LONG, map<string, LONG>>::iterator ite = m_pGameLogManage->m_WriteCount.begin();
                ite != m_pGameLogManage->m_WriteCount.end(); 
                ++ite)
            {
                //! 数据库编号
                char szDbNo[16] = {0};
                sprintf(szDbNo, "%d", ite->first);
                strShowInfo += "	DB No. = " + string(szDbNo) + "\r\n";

                strShowInfo += "		LOG TYPE			ITEM COUNT\r\n";
                for (map<string,LONG>::iterator iteDB = ite->second.begin(); iteDB != ite->second.end(); ++iteDB)
                {
                    char szTmp[512] = {0};
                    sprintf(szTmp, "		%s		%d\r\n", iteDB->first.c_str(), iteDB->second);
                    strShowInfo += string(szTmp);
                    dwAllCount += iteDB->second;
                }

            }

            char szInfo[512] = {0};

            sprintf(szInfo, "写入数据库日志总数已有[%u]条！\r\n", dwAllCount);

            RefreshInfo((szInfo + strShowInfo).c_str());
        }	
    }
    dwRefeashTextTime += timeGetTime() - dwCurTime;
    dwSavePointTime += timeGetTime() - dwCurTime;

    // 消息处理
    dwCurTime = timeGetTime();
    ProcessMessage();
    ProcMenus();
    dwMessageTime += timeGetTime() - dwCurTime;

    if(m_bCreateTable)
    {
        m_bCreateTable = FALSE;
        m_pGameLogManage->CreateTableInDb(false);
    }
    if(NULL != m_pGameLogManage)
        m_pGameLogManage->OnOutTime();
    //网络会话
    dwCurTime = timeGetTime();
    GetNetSessionManager()->Run();
    dwNetSessionTime += timeGetTime() - dwCurTime;	

    // 定时ping检测
    if (m_bInPing)
    {
        if (m_vectorPingGameServerInfo.size() >= GetConnectedGameServerCount() ||	// 成功收到所有GameServer返回
            now - m_dwLastPingGameServerTime > 5000)								// 出错，有GameServer异常
        {
            m_bInPing = false;
        }
    }

    // 桢数控制
    static DWORD new_timer = timeGetTime();
    static DWORD old_timer = new_timer;
    static DWORD Old_OrgRunTime = timeGetTime();
    new_timer = timeGetTime();

    Old_OrgRunTime = new_timer;

    new_timer = timeGetTime();
    if (new_timer - old_timer < g_ms)
    {
        WaitForSingleObject(GetCurrentProcess(), g_ms - (new_timer - old_timer));
    }
    old_timer += g_ms;
    if ((long)new_timer - (long)old_timer > 1000)
    {
        OutputDebugString("warning!!! 1 second not call AI()\n");
        old_timer = timeGetTime();
    }

#ifdef _DEBUG
    if( now - old > 600000 )
    {
        char str[128];
        sprintf(str, "600秒调用了 %d 次AI\n", ai_tick);
        OutputDebugString(str);
        ai_tick = 0;
        old = now;
    }
#endif
    new_timer = timeGetTime();
    if( new_timer - lastReleaseLoginPlayerTime > GetSetup()->dwReleaseLoginPlayerTime )
    {
        lastReleaseLoginPlayerTime = new_timer;
    }

    //检查资源装载情况
    static CClientResource *pClientResource = CClientResource::GetInstance();
    pClientResource->Run();

    return true;
}


// 获取GameServer的数据
CGame::tagGameServer* CGame::GetGameServer(const char* strIP, DWORD dwPort)
{
    for(map<DWORD, tagGameServer>::iterator it=s_mapGameServer.begin(); it!=s_mapGameServer.end(); it++)
    {
        tagGameServer *p = &it->second;
        if (p->strIP==strIP && p->dwPort==dwPort)
        {
            return p;
        }
    }
    return NULL;
}


//当某个GameServer断开连接的时候
void CGame::OnGameServerClose(long lSocket)
{
    if (NULL == m_pGameLogManage)
    {
        return;
    }
    pair<long, long> pairInfo = m_pGameLogManage->GetNoAndIpBySockte(lSocket);
    UCHAR szIP[4] = {0};
    if(m_setClientIP.end() != m_setClientIP.find(pairInfo.second))
    {
        memmove(szIP, &(pairInfo.second), sizeof(pairInfo.second)); 
        AppFrame::AddStdLog("!!!!!Client Server lost!!!!!!!!![ClientNo = %d], [ClientIP = %u.%u.%u.%u]", 
            pairInfo.first, szIP[0], szIP[1], szIP[2], szIP[3]);
    }
    // 在GetGame()->GetPlayerList()中清除在该GameServer上的player，并通知LoginSever
    OnGameServerLost(pairInfo.first);
    m_pGameLogManage->ClientLost(lSocket);
}

// 当某个GameServer断开时
void CGame::OnGameServerLost(DWORD dwIndex)
{

}


void CGame::SendMsg2GameServer(DWORD dwIndex,CMessage* pMsg)
{
    if(pMsg)
    {
        pMsg->SendGSID(dwIndex);
    }
}

//! [2007-7-9 add]
void CGame::SendAllGameServer(CMessage* pMsg)
{
    if(pMsg)
    {
        map<DWORD, tagGameServer>::iterator iteGS = s_mapGameServer.begin();
        for (; iteGS != s_mapGameServer.end(); ++iteGS)
        {
            long dwIndex = iteGS->first;
            pMsg->SendGSID(dwIndex);
        }
    }
}

// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
#ifndef _DEBUG
    __try
    {
#endif
        CreateGame();
        BOOL ret = GetGame()->Init();
        AppFrame::ToggleMenus();

        while(ret)
        {
            // 结束线程
            if (AppFrame::IsExit() || !GetGame()->MainLoop() )
            {
                break;
            }
        }
        if(GetGame())
            GetGame()->Release();
        DeleteGame();

#ifndef _DEBUG
    }
    __except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
    {
        if(GetGame())
            GetGame()->Release();
        DeleteGame();

        AppFrame::ExitThread();
        return 0;
    }
#endif

    AppFrame::ExitThread();
    return 0;
}




extern DWORD g_dwThisSaveStartTime;		// 本次存盘开始时间


long CGame::GetConnectedGameServerCount()
{
    long lCnt=0;
    map<DWORD,tagGameServer>::iterator it=s_mapGameServer.begin();
    for(;it!=s_mapGameServer.end();it++)
    {
        if(it->second.bConnected)
        {
            lCnt++;
        }
    }
    return lCnt;
}


void CGame::CreateTableInDB(void)
{
    m_bCreateTable = TRUE;
    if (NULL == m_pGameLogManage)
    {
        return;
    }
}

void CGame::CreateTableInSqlFile(void)
{
    if (NULL == m_pGameLogManage)
    {
        return;
    }
    m_pGameLogManage->CreateTableInDb(true, "");
    AppFrame::AddStdLog("create_table.sql文件已保存在服务目录下！");
}

void CGame::OutTableInfoToFile(void)
{
    if (NULL == m_pGameLogManage)
    {
        return;
    }
    m_pGameLogManage->OutTableInfoToFile();
}

void CGame::OnClientLogin(CMessage *pMsg)
{
    if (NULL == m_pGameLogManage)
    {
        return;
    }

    //! 不管客户端编号是否存在，都向客户端发送DB表格信息和限制条件
    vector<BYTE> tmpTableData;
    m_pGameLogManage->TableAddToByteArray(tmpTableData);
    CMessage msg(MSG_LOG20_SERVER_TABLE);
    msg.Add((long)tmpTableData.size());
    msg.AddEx((void*)&(tmpTableData[0]), tmpTableData.size());
    msg.SendToSocket(pMsg->GetSocketID());

    vector<BYTE> tmpConditionData;
    m_pGameLogManage->ConditionAddToByteArray(tmpConditionData);
    CMessage msgCondition(MSG_LOG20_SERVER_CONDITION);
    msgCondition.Add((long)tmpConditionData.size());
    msgCondition.AddEx((void*)&(tmpConditionData[0]), tmpConditionData.size());
    msgCondition.SendToSocket(pMsg->GetSocketID());

    //! 验证客户端编号是否存在
    long lClientNo = pMsg->GetLong();
    long lClientIp = pMsg->GetIP();
    BOOL isAffirm = m_pGameLogManage->AffirmClientNo(lClientNo, lClientIp, pMsg->GetSocketID());
    if (isAffirm)
    {
        CMessage::MapGSIDSocket(lClientNo,pMsg->GetSocketID());
    }
    CMessage msgAffirm(MSG_LOG20_SERVER_AFFIRM);
    msgAffirm.Add((isAffirm)? 1L : 0L);
    msgAffirm.SendToSocket(pMsg->GetSocketID());
}

void CGame::ReLoadConfig(void)
{
    vector<BYTE> tmpConditionData;
    m_pGameLogManage->ReLoadControlConfig();
    m_pGameLogManage->ConditionAddToByteArray(tmpConditionData);
    CMessage msgCondition(MSG_LOG20_SERVER_CONDITION);
    msgCondition.Add((long)tmpConditionData.size());
    msgCondition.AddEx((void*)&(tmpConditionData[0]), tmpConditionData.size());
    msgCondition.SendAll();
}

void CGame::OnLog(CMessage *pMsg)
{
    long lClientNo	= pMsg->GetLong();
    long lTableType = pMsg->GetLong();
    long lDataSize	= pMsg->GetLong();

    vector<BYTE> vData(lDataSize + 1);
    pMsg->GetEx(&vData[0], lDataSize);
    m_pGameLogManage->AddData(lClientNo, lTableType, &vData[0]);

}

void CGame::OnStrSqlLog(CMessage *pMsg)
{
    long lClientNo	= pMsg->GetLong();

    char szSql[MAX_SQL_STRING_SIZE] = {0};
    pMsg->GetStr(szSql, MAX_SQL_STRING_SIZE);

    m_pGameLogManage->AddLog(lClientNo, szSql);
}

void OnClientMessage(CMessage* pMsg)
{
    switch(pMsg->GetType())
    {
    case MSG_02LOG_SERVER_LOGIN:
        {
            GetGame()->OnClientLogin(pMsg);
        }
        break;
    case MSG_02LOG_SERVER_LOG:
        {
            GetGame()->OnLog(pMsg);
        }
        break;
    case MSG_02LOG_SERVER_LOG_SQLString:
        {
            GetGame()->OnStrSqlLog(pMsg);
        }
        break;
    }
}