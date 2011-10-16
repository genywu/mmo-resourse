#include "StdAfx.h"
// 附加内存泄漏检测调试代码2008.12.16 ahc
//#include "vld.h"
#include "Game.h"

#include "../App/Entity/EntityManager.h"
#include "../App/Entity/EntityGroup.h"
#include "../App/Thread/DBAccessThread.h"
#include "../App/Thread/DBAdoBase.h"
#include "../App/Thread/Threaddatadef.h"
#include "../App/Thread/DBRankThread.h"

#include "../DB/rsEntity.h"
#include "../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../Public/Module/Tools.h"
#include "../../../Public/AppFrame/TextRes.h"

using namespace AppFrame;

#pragma warning(push)
#pragma warning(disable :  4244 4267)

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

CGame* g_pGame= NULL;		            // 游戏客户端数据
HANDLE g_hGameThreadExitEvent=NULL;		// 主线程退出事件
bool g_bGameThreadExit=false;		    // 是否退出主线程

HANDLE	hConnectThread=NULL;			// 连接线程句柄 
bool	bConnectThreadExit=false;		// 是否退出连接线程
HANDLE hConnectThread1=NULL;
bool	bConnectThreadExit1=false;

HANDLE g_hWriteLogThread = NULL;		// 日志线程
bool volatile g_bWriteLogThreadExit = false;	// 是否退出日志线程

vector<HANDLE> g_vPlayerLoadThreads;	// 玩家读数据库线程
bool volatile g_bPlayerLoadThreadsExit = false;
long g_vPlayerLoadThreadFlag;           // 玩家读数据库线程退出标志

DWORD g_dwThisSaveStartTime;		    // 本次存盘开始时间
HANDLE g_hSavingThread = NULL;          // 存盘线程句柄

long g_ThreadCountID = 0;               // load thread 计算ID

CRITICAL_SECTION g_CriticalSectionSavePlayerList;	// 待存盘玩家的线程同步(GameThread <-> SaveThread)
CRITICAL_SECTION g_CriticalSectionSaveThread;		// 存盘线程同步

extern unsigned __stdcall SaveThreadFunc(void* pArguments);

void RefreshInfo();					// 刷新服务器信息

enum MenuID
{
    LOAD_RESOURCE = MY_MENU * 2,    // 加载资源
    LOAD_TEXTRES,                   // 加载文本资源
    OPER_DELPLAYER,                 // 清理玩家数据
    OPER_SAVEALLDATA,               // 存盘
    OPER_REFRESHRANKS,              // 刷新排行榜
    OPER_UPDATEGOODS,               // 更新玩家物品
    OPER_UPDATEMAILGOODS,           // 更新邮件物品
};

// end define
// 刷新服务器信息
void RefreshInfo()
{
	static char str[102400];

	if (!GetGame() || !GetGame()->GetNetServer())
	{
		return;
	}

	static long lMaxConnects = 0;

	static long lMaxMapAccounts= 0;
	static long lMaxMapScripts= 0;
	static long lMaxMapProcs= 0;

	static long lMaxMapPlayers = 0;
	static long lMaxDepots = 0;
	static long lMaxPlayers = 0;
	static long lMaxOfflinePlayers = 0;
	static long lMaxLoginPlayers = 0;
	static long lMaxCreatePlayers = 0;
	static long lMaxDeletePlayers = 0;
	static long lMaxResumePlayers = 0;
	static long lMaxLoadingEntitys = 0;
	static long lMaxSavingEntitys = 0;
	static long lMaxTeams = 0;
	static long lMaxLargess = 0;
	static long lMaxWriteLog = 0;
	static long lMaxLoadPlayer = 0;
	static long lMaxRebackMessage = 0;

	static long lMaxMails = 0;
	static long lMaxFactions = 0;
	static long lBeginLoadNumMax = 0;
	static long lBeginSaveNumMax = 0;
	static long lEndNumMax = 0;

	static SIZE_T dwMaxAvaiVirMem = 0;
	MEMORYSTATUS msC;GlobalMemoryStatus(&msC);
	SIZE_T dwCurAvaiVirMem = msC.dwAvailVirtual/1048576;

	extern long g_lRebackMessageCount;
	long lRebackMessage = g_lRebackMessageCount;

	if (lMapAccounts>lMaxMapAccounts)
		lMaxMapAccounts = lMapAccounts;
	if (lMapScripts>lMaxMapScripts)
		lMaxMapScripts = lMapScripts;
	if (lMapProcs>lMaxMapProcs)
		lMaxMapProcs = lMapProcs;

	if (lConnects>lMaxConnects)
		lMaxConnects = lConnects;
	if (lMapPlayers>lMaxMapPlayers)
		lMaxMapPlayers = lMapPlayers;
	if (lPlayers>lMaxPlayers)
		lMaxPlayers = lPlayers;
	if (lOfflinePlayers>lMaxOfflinePlayers)
		lMaxOfflinePlayers = lOfflinePlayers;
	if (lLoginPlayers>lMaxLoginPlayers)
		lMaxLoginPlayers = lLoginPlayers;
	if (lCreatePlayers>lMaxCreatePlayers)
		lMaxCreatePlayers = lCreatePlayers;
	if (lDeletePlayers>lMaxDeletePlayers)
		lMaxDeletePlayers = lDeletePlayers;
	if (lResumePlayers>lMaxResumePlayers)
		lMaxResumePlayers = lResumePlayers;
	if (lOperingLoadEntitys>lMaxLoadingEntitys)
		lMaxLoadingEntitys = lOperingLoadEntitys;
	if (lOperingSaveEntitys>lMaxSavingEntitys)
		lMaxSavingEntitys = lOperingSaveEntitys;
	if (lTeams>lMaxTeams)
		lMaxTeams=lTeams;
	if (lLargess>lMaxLargess)
		lMaxLargess=lLargess;
	if( lWriteLog > lMaxWriteLog )
		lMaxWriteLog = lWriteLog;
	if( lLoadPlayer > lMaxLoadPlayer )
		lMaxLoadPlayer = lLoadPlayer;
	if (lRebackMessage > lMaxRebackMessage)
		lMaxRebackMessage = lRebackMessage;

	if(lMails > lMaxMails)
		lMaxMails = lMails;
	if(lFactions > lMaxFactions)
		lMaxFactions = lFactions;

	if(lBeginLoadNum > lBeginLoadNumMax)
		lBeginLoadNumMax = lBeginLoadNum;

	if(lBeginSaveNum > lBeginSaveNumMax)
		lBeginSaveNumMax = lBeginSaveNum;

	if(lEndNum > lEndNumMax)
		lEndNumMax = lEndNum;

	if(dwCurAvaiVirMem > dwMaxAvaiVirMem)
		dwMaxAvaiVirMem = dwCurAvaiVirMem;

	int nSave = 0;

	static char *strInfo[] = {"(正常)", "(异常!!!!)"};

    sprintf(str, "\
                 Last Save Time : %d 毫秒	AvaiVirMem=%d/%d(MB)\r\n\
                 Connects=%d/%d    Mails=%d/%d	Factions=%d/%d\r\n\
                 Account=%d/%d   Player=%d/%d	ScriptVar=%d/%d	Proc=%d/%d\r\n\
                 LoadingEntitys=%d/%d SavingEntitys=%d/%d\r\n\
                 BeginLoadNum=%d/%d   BeginSaveNum=%d/%d\r\n\
                 WriteLog    = %d/%d    ReMsg = %d/%d\r\n\
                 Oper Load Thread Num[%d] Oper Save Thread Num[%d]\r\n",
                 g_dwThisSaveStartTime,dwCurAvaiVirMem,dwMaxAvaiVirMem,
                 lConnects, lMaxConnects, lMails, lMaxMails,lFactions, lMaxFactions,
                 lMapAccounts, lMaxMapAccounts,lMapPlayers, lMaxMapPlayers, lMapScripts,lMaxMapScripts,lMapProcs,lMaxMapProcs,
                 lOperingLoadEntitys, lMaxLoadingEntitys, lOperingSaveEntitys, lMaxSavingEntitys,
                 lBeginLoadNum,lBeginLoadNumMax,lBeginSaveNum,lBeginSaveNumMax,
                 lWriteLog, lMaxWriteLog, lRebackMessage, lMaxRebackMessage,
                 GetGame()->GetEntityManager()->GetLoadThreadMap().size(),
                 GetGame()->GetEntityManager()->GetSaveThreadMap().size());

	// 复原存盘计时
	if(g_dwThisSaveStartTime != 0)
		g_dwThisSaveStartTime = 0;

	SetWindowText(AppFrame::GetWnd("info"), str);
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

const char* CGame::GetStatckFileName()	
{
	DWORD lThreadID = ::GetCurrentThreadId();
	sprintf(m_pszThreadRunStackFile, "[%d]_%s", lThreadID, m_pszRunStackFile);
	return m_pszThreadRunStackFile;
}

CGame::CGame(void)
:p_DBEntityManager(NULL),m_bIsSaveStartFlag(false)
{
	m_lWSSocket = 0;
	p_DBEntityManager = new CEntityManager;

	s_lAITick = 0;

	s_pNetServer = NULL;
	//	m_pRsCDKey = NULL;
	m_pRsEntity = NULL;


	m_pMapFile = NULL;

	GetSetup()->strLoginIP = "127.0.0.1";
	GetSetup()->dwLoginPort = 2345;
	GetSetup()->dwListenPort = 8100;	

	m_dwLastReLoginTime = timeGetTime();

	m_bInPing = false;
	m_dwLastPingGameServerTime = timeGetTime();

	memset(m_pszThreadRunStackFile, 0, sizeof(m_pszThreadRunStackFile));
}

CGame::~CGame(void)
{
	SAFE_DELETE(p_DBEntityManager);
}

BOOL CGame::InitStackFileName()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	_snprintf(m_pszRunStackFile,MAX_PATH,"RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);	
	return TRUE;
}

BOOL CGame::InitMainStackFileName()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	_snprintf(m_pszMainRunStackFile,MAX_PATH,"MainRunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);	
	return TRUE;
}
CEntityManager* CGame::GetEntityManager(void)
{
	return p_DBEntityManager;
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
	string strFilesInfo("PackRes.cfg");	
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorpath,strFilesInfo);
	if(!pClientResource->LoadEx())
	{
		//装载客户端资源信息出错
		//ASSERT(false);
		//return false;
	}
	Log4c::Trace(ROOT_MODULE,"Load package file OK!");
	//Log4c::Trace(ROOT_MODULE,"装载客户端资源信息成功!");
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
		stream  >> strTemp >> GetSetup()->strLoginIP
			>> strTemp >> GetSetup()->dwLoginPort
			>> strTemp >> GetSetup()->dwListenPort
			>> strTemp >> GetSetup()->strSqlConType
			>> strTemp >> GetSetup()->strSqlServerIP
			>> strTemp >> GetSetup()->strSqlUserName
			>> strTemp >> GetSetup()->strSqlPassWord
			>> strTemp >> GetSetup()->strDBName; //end ;			   

		stream.close();

        strTemp = "DBServer" + GetSetup()->strDBName;
        if (!AppFrame::SetTitle(strTemp))
            return false;
	}

	else	// 从加密文件读取
	{
		AppFrame::NotifyMessageBox("Can't find file setup.ini", "message", MB_OK);
		return false;
	}

	ifstream streamSetupEx;
	streamSetupEx.open("SetupEx.ini");
	if( streamSetupEx.is_open() )
	{
		streamSetupEx >> strTemp >> GetSetup()->dwDataBlockNum
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
			
			>>strTemp>>GetSetup()->dwTravleMaxSize
			>>strTemp>>GetSetup()->dwPlayerLoadThreadNumber
			>>strTemp>>GetSetup()->dwPlayerSaveThreadNumber

			>>strTemp>>GetSetup()->dwPingWorldServerTime
			>>strTemp>>GetSetup()->dwPingWorldServerErrorTime
			>>strTemp>>GetSetup()->dwClearPlayerDataTime
			>>strTemp>>GetSetup()->dwViewDbOperDeltaTime
            >>strTemp>>GetSetup()->bOpenRanks
            >>strTemp>>GetSetup()->dwRefreshRanksTime;
		streamSetupEx.close();
	}
	else
	{
		AppFrame::NotifyMessageBox("Can't find file SetupEx.ini", "message", MB_OK);
		return false;
	}

	return true;
}

// 游戏初始化
BOOL CGame::Init()
{
	srand((unsigned)time(NULL));
	random(100);
	CreateDirectory( "log",	NULL);

#ifdef _RUNSTACKINFO_
	InitStackFileName();
	InitMainStackFileName();
#endif	

	PutDebugString("DBServer start!");

	//装载资源
	LoadServerResource();

	//读取全局字符串
	AppFrame::LoadTextRes();
	
	if (LoadSetup()==false)
	{
		return false;
	}



	if( !LoadWSSetup("WorldInfoSetup.ini") )
	{
		return false;
	}
	

	//分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);

	// 进程使用ADO接口初始Connection string
	bool isADOConnected = CMyAdoBase::Initialize(GetSetup()->strSqlConType, GetSetup()->strSqlServerIP, GetSetup()->strDBName,
		GetSetup()->strSqlUserName, GetSetup()->strSqlPassWord);

	// [DB Connection Thread]
	GetGame()->GetEntityManager()->InitDBThread(GetSetup()->strSqlConType, GetSetup()->strSqlServerIP, GetSetup()->strDBName,
		GetSetup()->strSqlUserName, GetSetup()->strSqlPassWord);


	m_pRsEntity=new CRsEntity;
	
	CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
	// 初始化服务器网络
	CMySocket::MySocketInit();
	
	//装载内存影射文件配置
	LoadMapFileSetup();
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

	//释放资源管理器
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Release();
	CClientResource::ReleaseInstance();

#ifdef _THREAD_
	// 创建日志线程
	unsigned threadID;
	g_hWriteLogThread = (HANDLE)_beginthreadex(NULL,0,ProcessWriteLogDataFunc,NULL,0,&threadID);
#endif

	return true;
}

// 游戏释放
BOOL CGame::Release()
{
	PutDebugString("DBServer Exiting...");

#ifdef _RUNSTACKINFO_
	EndStackInfo();
#endif

	SAFE_DELETE(p_DBEntityManager);

	GetInst(CMemClient).Release();

//	CStringReading::Release();

	// 关闭网络连接
	if( GetNetServer() )
	{
		s_pNetServer->DisConn(m_lLogingSocket);
		s_pNetServer->DisConn(m_lCostSocket);
		s_pNetServer->Exit();

		SAFE_DELETE(s_pNetServer);
	}
	
	WaitForSingleObject(g_hSavingThread, INFINITE);
	CloseHandle(g_hSavingThread);

	CWordsFilter::GetInstance()->Release();
	GetTimer()->Release();

	SAFE_DELETE(s_pNetServer);
	SAFE_DELETE(m_pRsEntity);

	CMySocket::MySocketClearUp();

	CMessage::Release();

	GetNetSessionManager()->Release();

	// 等待日志线程结束
	g_bWriteLogThreadExit = true;
	WaitForSingleObject(g_hWriteLogThread, INFINITE);
	CloseHandle(g_hWriteLogThread);

	// 关闭玩家数据库载入线程
	g_vPlayerLoadThreadFlag = 0;
	for(DWORD i=0; i<g_vPlayerLoadThreads.size(); ++i)
	{
		CloseHandle(g_vPlayerLoadThreads[i]);
	}


	SAFE_DELETE(m_pDBAllocator);

	CMyAdoBase::Uninitalize();
	PutDebugString("DBServer Exited!");
	return true;
}

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
	Log4c::Trace(ROOT_MODULE,GetText("DB_GAME_2"));//网络服务初始化成功 !
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
	Log4c::Trace(ROOT_MODULE,GetText("DB_GAME_0"));//DBServer服务器初始化成功 !
	return true;
}


// 游戏AI
BOOL CGame::AI()
{
	return TRUE;
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
    if (AppFrame::MenuClicked(LOAD_RESOURCE))
        res = LoadServerResource();
    if (AppFrame::MenuClicked(LOAD_TEXTRES))
    	res = AppFrame::LoadTextRes();
    if (AppFrame::MenuClicked(OPER_DELPLAYER))
        res = GetEntityManager()->ClearEntitys();
    if (AppFrame::MenuClicked(OPER_SAVEALLDATA)) // 立刻存当前数据
        GetEntityManager()->SaveAllEntity();
    if (AppFrame::MenuClicked(OPER_REFRESHRANKS)) // 刷新排行信息
        GetEntityManager()->ReLoadRanks();
    if (AppFrame::MenuClicked(OPER_UPDATEGOODS))
        Test3GoodsImageDataBuild();
    if (AppFrame::MenuClicked(OPER_UPDATEMAILGOODS))
        Test3MailGoodsDataBuild();
    return res;
}

bool CGame::LoadMapFileSetup()
{
	string strTempt("");
	string strMemClientID("");
	string strMapObjName("");
	ifstream stream;
	stream.open("MapFileSetup.ini");
	if( stream.is_open() )
	{
		stream  >> strTempt>> strMemClientID
			>> strTempt >> strMapObjName;
		GetSetup()->dwGappID = atoi(strMemClientID.c_str());
		GetInst(CMemClient).Initial(strMapObjName.c_str(), m_pDBAllocator);
		return true;
	}
	return false;
}
// 游戏主循环
BOOL CGame::MainLoop()
{
	// 定时清除操作堆栈记录文件
#ifdef _RUNSTACKINFO_
		CMessage::AsyClearFile(GetGame()->GetMainStatckFileName());
#endif

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

	static DWORD ai_tick, now, old = timeGetTime(), dwAnsyFileClearTime = old, dwClearPlayerDataTime=old, lastSaveTime=old, lastRefeashInfoTime = now, lastReleaseLoginPlayerTime = 0;
	now = timeGetTime();
	
	ai_tick++;

	// 定时清除操作堆栈记录文件
#ifdef _DEBUG_SM_INFO_
	if(now - dwAnsyFileClearTime >= 3600 * 1000 * 1) // 1 小时清除一次
	{
		dwAnsyFileClearTime = now;
		CMessage::AsyClearFile(GetGame()->GetStatckFileName());
	}
#endif
	// 计费
	static DWORD LastCostTime=0;
	static DWORD LastLoadLargessTime=0;
	static DWORD LastTimeLoadCardLargess=0;
	static DWORD LastTimeCardLoop=0;

	//static CClientResource *pClientResource = CClientResource::GetInstance();
	//pClientResource->Run();

	if( now - old > 600000 )
	{
		Log4c::Trace(ROOT_MODULE,"%s",FormatText("DB_RUN_INFO",
		ai_tick,
		dwRefeashTextTime,
		dwAITime,
		dwMessageTime, g_lLSMessageTime, g_lGSMessageTime, g_lCSMessageTime,
		dwNetSessionTime,
		dwFactionWarSysTime,
		dwTimerTime,
		dwProcessPlayerDataQueueTime,
		dwSessionFactoryTime,
		dwSavePointTime));

		dwRefeashTextTime = 0;
		dwAITime = 0;
		dwMessageTime = 0; g_lLSMessageTime = 0; g_lGSMessageTime = 0; g_lCSMessageTime = 0;
		dwNetSessionTime = 0;
		dwFactionWarSysTime = 0;
		dwTimerTime = 0;
		dwProcessPlayerDataQueueTime = 0;
		dwSessionFactoryTime = 0;
		dwSavePointTime = 0;

		ai_tick = 0;
		old = now;
	}

	dwCurTime = timeGetTime();
	ProcessMessage();
    ProcMenus();
	dwMessageTime += timeGetTime() - dwCurTime;

	GetEntityManager()->TravelForGenerateThreadData();
	
	//##会话
	dwCurTime = timeGetTime();
	dwSessionFactoryTime += timeGetTime() - dwCurTime;

	//时间计时器
	dwCurTime = timeGetTime();
	GetTimer()->Run();
	dwTimerTime += timeGetTime() - dwCurTime;

	//网络会话
	dwCurTime = timeGetTime();
	GetNetSessionManager()->Run();
	dwNetSessionTime += timeGetTime() - dwCurTime;

	char szOutPutStr[1024];
#ifdef _RUNSTACKINFO_
	sprintf(szOutPutStr, "ProcessData() Start.");
	CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
	// 处理线程产生的数据并发送消息
	GetEntityManager()->ProcessRetThreadData();
#ifdef _RUNSTACKINFO_
	sprintf(szOutPutStr, "ProcessData() End.");
	CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif

	// 定时清除玩家数据
	if (now - dwClearPlayerDataTime > GetSetup()->dwClearPlayerDataTime)
	{
		dwClearPlayerDataTime = now;

#ifdef _RUNSTACKINFO_
		char szOutPutStr[1024];
		sprintf(szOutPutStr, "定时清除玩家数据() Start.");
		CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
		// 执行清除
		CGUID pGUID;
		long dbPlayerNum = 0;
		CEntityManager::AccountMapItr deleteItr = GetEntityManager()->GetAccountMap().begin();
		while(deleteItr != GetEntityManager()->GetAccountMap().end())
		{
			if( (deleteItr->second->GetLastLoginTime() != -1)
				&& ((now-deleteItr->second->GetLastLoginTime()) >= GetSetup()->dwClearPlayerDataTime) ) // 到达清除时间点
			{
				bool isWaitForSaveFlag = false;
				if(deleteItr->second->GetEntityGroupMap().size() < 1)
				{
					Log4c::Warn(ROOT_MODULE,"%s",FormatText("DB_GAME_3", deleteItr->first.c_str()));//MainLoop() Account[%s]的Group对象为空!
					deleteItr++;
					continue;
				}
				CEntityGroup* pLoginPlayerGroup = NULL;
				pLoginPlayerGroup = (CEntityGroup*)((deleteItr->second->GetEntityGroupMap().begin())->second);

				isWaitForSaveFlag = GetEntityManager()->FindAccountIsInOperQueue(deleteItr->first);
				// 判断是否在操作队列中
				if(!isWaitForSaveFlag)
				{
					if(pLoginPlayerGroup)
					{
						map<CGUID, CBaseEntity*>::iterator playeritr = pLoginPlayerGroup->GetEntityGroupMap().begin();
						for(; playeritr != pLoginPlayerGroup->GetEntityGroupMap().end(); playeritr++)
						{
							isWaitForSaveFlag = GetEntityManager()->FindEntityIsInOperQueue(playeritr->first);
							if(isWaitForSaveFlag)
								break;
						}
					}
				}

				if(isWaitForSaveFlag)
				{
					++deleteItr;
					continue;
				}

				if(pLoginPlayerGroup)
				{
					map<CGUID, CBaseEntity*>::iterator guidItr = pLoginPlayerGroup->GetEntityGroupMap().begin();
					while(guidItr != pLoginPlayerGroup->GetEntityGroupMap().end())
					{
						GetEntityManager()->DelOneRootEntity(string("[player]"), guidItr->first);// 清除DBPlayer
						guidItr++;
						dbPlayerNum++;
					}
				}

				// 完全清除,删除该账号对象
				delete deleteItr->second;
				deleteItr = GetEntityManager()->GetAccountMap().erase(deleteItr);
				dbPlayerNum++;
				continue;
			}
			
			deleteItr++;
		}
		// 清除DBEntity
#ifdef _RUNSTACKINFO_
		sprintf(szOutPutStr, "定时清除玩家数据[%d]个() End.", dbPlayerNum);
		CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
	}

	// 刷新显示
	dwCurTime = timeGetTime();
	if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
	{
		lastRefeashInfoTime = now;
		lMapAccounts = 0;
		lMapAccounts += GetEntityManager()->GetAccountMap().size();
		CEntityManager::DBEntityComponentMapItr  enGroupItr = GetEntityManager()->GetBaseEntityMap().find(std::string("[player]"));
		if( enGroupItr != GetEntityManager()->GetBaseEntityMap().end() )
		{
			lMapPlayers = 0;
			lMapPlayers += enGroupItr->second.size();
		}

		enGroupItr = GetEntityManager()->GetBaseEntityMap().find(std::string("[scriptvargroup]"));
		if( enGroupItr != GetEntityManager()->GetBaseEntityMap().end() )
		{
			map<CGUID, CEntityGroup*>::iterator leafit = enGroupItr->second.begin();
			for(; leafit != enGroupItr->second.end(); leafit++)
			{
				if(leafit->second)
				{
					lMapScripts = 0;
					lMapScripts += leafit->second->GetEntityGroupMap().size();
				}
			}
		}

		enGroupItr = GetEntityManager()->GetBaseEntityMap().find(std::string("[procedure]"));
		if( enGroupItr != GetEntityManager()->GetBaseEntityMap().end() )
		{
			lMapProcs = 0;
			lMapProcs += enGroupItr->second.size();
		}

		enGroupItr = GetEntityManager()->GetBaseEntityMap().find(std::string("[factiongroup]"));
		if( enGroupItr != GetEntityManager()->GetBaseEntityMap().end() )
		{
			map<CGUID, CEntityGroup*>::iterator leafit = enGroupItr->second.begin();
			for(; leafit != enGroupItr->second.end(); leafit++)
			{
				if(leafit->second)
				{
					lFactions = 0;
					lFactions += leafit->second->GetEntityGroupMap().size();
				}
			}
		}

		lOperingLoadEntitys = GetEntityManager()->GetLoadWaitOperEntityNum();
		lOperingSaveEntitys = GetEntityManager()->GetSaveWaitOperEntityNum();

		RefreshInfo();
	}
	dwRefeashTextTime += timeGetTime() - dwCurTime;

	if(GetSaveStartFlag())
	{
		if(	   GetEntityManager()->GetLoadWaitOperEntityNum() == 0 
			&& GetEntityManager()->GetSaveWaitOperEntityNum() == 0 
			&& GetEntityManager()->GetBeginLoadEntityNum()    == 0
			&& GetEntityManager()->GetBeginSaveEntityNum()    == 0 )
		{
			Log4c::Trace(ROOT_MODULE,GetText("DB_GAME_4"));//保存所有数据完成！
			SetSaveStartFlag(false);
		}
	}

    Sleep(1);
	return true;
}
bool CGame::LoadWSSetup(const char *filename)
{
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "Can't find file %s", filename);
		AppFrame::NotifyMessageBox(str, "message", MB_OK);
		return false;
	}

	string strTemp;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	// 读WS服务器信息
	tagWorldServer stWS;
	long lWorldServerNum = 0;
	stream >> strTemp >> lWorldServerNum;
	for(int i=0; i<lWorldServerNum; i++)
	{
		ReadTo(stream, "#");
		stream >> stWS.dwIndex >> stWS.strIP >> stWS.dwPort;
		stWS.bConnected = false;
		s_mapWorldServer[stWS.dwIndex] = stWS;
	}
	Log4c::Trace(ROOT_MODULE,GetText("DB_GAME_1"));//DBS服务器配置读取完毕
	return true;
}

long CGame::GetConnectedWorldServerCount()
{
	long lCnt=0;
	map<DWORD,tagWorldServer>::iterator it=s_mapWorldServer.begin();
	for(;it!=s_mapWorldServer.end();it++)
	{
		if(it->second.bConnected)
		{
			lCnt++;
		}
	}
	return lCnt;
}


CGame::tagWorldServer* CGame::GetWorldServer(const char* strIP, DWORD dwPort)
{
	for(map<DWORD, tagWorldServer>::iterator it=s_mapWorldServer.begin(); it!=s_mapWorldServer.end(); it++)
	{
		tagWorldServer *p = &it->second;
		if (p->strIP==strIP && p->dwPort==dwPort)
		{
			return p;
		}
	}
	return NULL;
}


//当某个WorldServer断开连接的时候
bool CGame::OnWorldServerClose(DWORD dwIndex)
{
	tagWorldServer* pWorldServer = GetWorldServer(dwIndex);
	if (pWorldServer)
	{
		pWorldServer->bConnected = false;
		// 删除Player列表
		char str[256];
		sprintf(str, "%s [%d]", pWorldServer->strIP.c_str(), pWorldServer->dwPort);
        AppFrame::DelFromList("list", str);
		Log4c::Warn(ROOT_MODULE,"%s",FormatText("DB_GAME_6", pWorldServer->strIP.c_str(), pWorldServer->dwPort));//WorldServer %s [%d] lost!

		// 清除帮会数据
		CEntityManager::DBEntityComponentMapItr  enGroupItr = GetEntityManager()->GetBaseEntityMap().find(string("[factiongroup]"));
		if( enGroupItr != GetEntityManager()->GetBaseEntityMap().end() )
		{
			map<CGUID, CEntityGroup*>::iterator facItr = enGroupItr->second.begin();
			for(; facItr != enGroupItr->second.end(); facItr++)
			{
				delete facItr->second;
			}
			GetEntityManager()->GetBaseEntityMap().erase(enGroupItr);
			Log4c::Trace(ROOT_MODULE,GetText("DB_GAME_5"));//WS掉线清除帮会数据完成!
		}
	}
	else
	{
		return false;
	}

	return true;
}

//! [2007-7-9 add end]
void CGame::SendMsg2WS(DWORD dwIndex,CMessage* pMsg)
{
	if(pMsg)
	{
		pMsg->SendWSID(dwIndex);
	}
}

//! [2007-7-9 add]
void CGame::SendAllWS(CMessage* pMsg)
{
	if(pMsg)
	{
		map<DWORD, tagWorldServer>::iterator iteGS = s_mapWorldServer.begin();
		for (; iteGS != s_mapWorldServer.end(); ++iteGS)
		{
			long dwIndex = iteGS->first;
			pMsg->SendWSID(dwIndex);
		}
	}
}
// 发送数据库配置信息
void CGame::SendDBTableInfo(long socketID)
{
	CMessage msg(MSG_DB2W_SERVER_SENDSETUP);
	
	long objAttrDefNum = GetEntityManager()->GetObjAttrDef().size();
	msg.Add(objAttrDefNum);
	vector<BYTE> pBA;
	std::map<string, tagEntityBuildInfo*>::iterator attrItr = GetEntityManager()->GetObjAttrDef().begin();
	for(; attrItr != GetEntityManager()->GetObjAttrDef().end(); attrItr++)
	{
		attrItr->second->AddToByteArray(pBA);
	}
	msg.Add((long)pBA.size());
	if(pBA.size())
		msg.AddEx(&pBA[0], pBA.size());
	msg.SendToSocket(socketID);
}
//! [2007-7-9 add end]

void CGame::Test3GoodsImageDataBuild()
{
	_RecordsetPtr rs;
	long lTranNumber=0;
	
	try
	{			
        char szSql[1024];
        sprintf(szSql, "SELECT * FROM TBL_PLAYER_GOODS");
        DBAdoBase *pDBAdoBase = GetEntityManager()->GetAccessThread()->GetAdoBasePtr();
        pDBAdoBase->CreateRs(rs);
        pDBAdoBase->OpenRs(szSql, rs);
		DWORD dwRecordNum=1;
		while( !rs->GetadoEOF() )
		{			
			long nHoleNum=(int)rs->GetCollect("holeNum");
			long nHoleIdx1=(int)rs->GetCollect("hole1Idx");
			long nHoleIdx2=(int)rs->GetCollect("hole2Idx");
			long nHoleIdx3=(int)rs->GetCollect("hole3Idx");
			long nHoleIdx4=(int)rs->GetCollect("hole4Idx");
			long nHoleIdx5=(int)rs->GetCollect("hole5Idx");
			long lSize=0;
			FieldsPtr fldPtr = rs->GetFields();
			FieldPtr itemPtr = fldPtr->GetItem("AddonProperty");
			lSize = itemPtr->ActualSize;

			if(lSize > 0)
			{
				_variant_t varBLOB;
				varBLOB = itemPtr->GetChunk(lSize);

				byte *pBuf=NULL;
				SafeArrayAccessData(varBLOB.parray,(void**)&pBuf);
				SafeArrayUnaccessData(varBLOB.parray);

				//计算新的SIZE
				long nMakerNameLen=0;
				long nAddonSize=sizeof(long)*2 + sizeof(long)*5;
				long nNewDataSize=lSize+nAddonSize;		
				byte* pNewDataBuffer=new byte[nNewDataSize];
				ZeroMemory(pNewDataBuffer,nNewDataSize);
				memcpy(pNewDataBuffer,pBuf,lSize);
				long loffset=lSize;
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nMakerNameLen,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleNum,sizeof(long));
				loffset+=sizeof(long);

				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx1,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx2,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx3,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx4,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx5,sizeof(long));

				VARIANT varBLOBWrite;
				SAFEARRAY *psa = 0;
				SAFEARRAYBOUND rgsabound[1];
				rgsabound[0].lLbound = 0;				
				rgsabound->cElements = nNewDataSize;
				psa = SafeArrayCreate(VT_UI1, 1, rgsabound);					
				for(long i=0; i<nNewDataSize; i++)
					SafeArrayPutElement(psa, &i, &pNewDataBuffer[i]);
				varBLOBWrite.parray = psa;
				varBLOBWrite.vt = VT_ARRAY | VT_UI1;
				itemPtr->AppendChunk(varBLOBWrite);
				
				SAFE_DELETE_ARRAY(pNewDataBuffer);
				SafeArrayDestroy(psa);										
			}
			
			rs->MoveNext();
			dwRecordNum++;
		}
        pDBAdoBase->ReleaseRs(rs);
		string strMsg="成功生成物品IMAGE数据块！";
		
		MessageBox(NULL,strMsg.c_str(),"OK",MB_OK);
		
	}
	catch (_com_error &e)
	{
		char buf[1024];
		string strErr="";
		char* msg="-_-!";

		if (e.Error() == 0x80004005)
		{
			wsprintf( buf, "%s (一般性网络错误): 描述\r\n:%s", msg, (LPCSTR)((_bstr_t)e.Description()));
		}
		else
		{
			wsprintf( buf, "%s\r\n \
						   Code = %08lx\r\n \
						   Cnode meaning = %d\r\n \
						   Source = %s\r\n \
						   Description = %s\r\n",
						   msg ,e.Error(), e.ErrorMessage(), (LPCSTR)((_bstr_t)e.Source()), (LPCSTR)((_bstr_t)e.Description()));
			strErr=buf;
			strErr=strErr.substr(30);
		}
		MessageBox(NULL,"生成物品IMAGE数据块出错","错误",MB_OK);
		
	}
}

void CGame::Test3MailGoodsDataBuild()
{
	_RecordsetPtr rs;
	long lTranNumber=0;

	try
	{	
        char szSql[1024];
        sprintf(szSql, "SELECT * FROM TBL_MAIL_GOODS");
        DBAdoBase *pDBAdoBase = GetEntityManager()->GetAccessThread()->GetAdoBasePtr();
        pDBAdoBase->CreateRs(rs);
        pDBAdoBase->OpenRs(szSql, rs);
		DWORD dwRecordNum=1;
		while( !rs->GetadoEOF() )
		{			
			long nHoleNum=(int)rs->GetCollect("holeNum");
			long nHoleIdx1=(int)rs->GetCollect("hole1Idx");
			long nHoleIdx2=(int)rs->GetCollect("hole2Idx");
			long nHoleIdx3=(int)rs->GetCollect("hole3Idx");
			long nHoleIdx4=(int)rs->GetCollect("hole4Idx");
			long nHoleIdx5=(int)rs->GetCollect("hole5Idx");


			long lSize=0;
			FieldsPtr fldPtr = rs->GetFields();
			FieldPtr itemPtr = fldPtr->GetItem("AddonProperty");
			lSize = itemPtr->ActualSize;

			if(lSize > 0)
			{
				_variant_t varBLOB;
				varBLOB = itemPtr->GetChunk(lSize);

				byte *pBuf=NULL;
				SafeArrayAccessData(varBLOB.parray,(void**)&pBuf);
				SafeArrayUnaccessData(varBLOB.parray);

				//计算新的SIZE
				long nMakerNameLen=0;
				long nAddonSize=sizeof(long)*2 + sizeof(long)*5;
				long nNewDataSize=lSize+nAddonSize;		
				byte* pNewDataBuffer=new byte[nNewDataSize];
				ZeroMemory(pNewDataBuffer,nNewDataSize);
				memcpy(pNewDataBuffer,pBuf,lSize);
				long loffset=lSize;
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nMakerNameLen,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleNum,sizeof(long));
				loffset+=sizeof(long);

				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx1,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx2,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx3,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx4,sizeof(long));
				loffset+=sizeof(long);
				memcpy(&(((byte*)pNewDataBuffer)[loffset]),&nHoleIdx5,sizeof(long));

				VARIANT varBLOBWrite;
				SAFEARRAY *psa = 0;
				SAFEARRAYBOUND rgsabound[1];
				rgsabound[0].lLbound = 0;				
				rgsabound->cElements = nNewDataSize;
				psa = SafeArrayCreate(VT_UI1, 1, rgsabound);					
				for(long i=0; i<nNewDataSize; i++)
					SafeArrayPutElement(psa, &i, &pNewDataBuffer[i]);
				varBLOBWrite.parray = psa;
				varBLOBWrite.vt = VT_ARRAY | VT_UI1;
				itemPtr->AppendChunk(varBLOBWrite);

				SAFE_DELETE_ARRAY(pNewDataBuffer);
				SafeArrayDestroy(psa);										
			}

			rs->MoveNext();
			dwRecordNum++;
		}
        pDBAdoBase->ReleaseRs(rs);
		string strMsg="成功生成邮件物品IMAGE数据块！";
		MessageBox(NULL,strMsg.c_str(),"OK",MB_OK);

	}
	catch (_com_error &e)
	{
		char buf[1024];
		string strErr="";
		char* msg="-_-!";

		if (e.Error() == 0x80004005)
		{
			wsprintf( buf, "%s (一般性网络错误): 描述\r\n:%s", msg, (LPCSTR)((_bstr_t)e.Description()));
		}
		else
		{
			wsprintf( buf, "%s\r\n \
						   Code = %08lx\r\n \
						   Cnode meaning = %d\r\n \
						   Source = %s\r\n \
						   Description = %s\r\n",
						   msg ,e.Error(), e.ErrorMessage(), (LPCSTR)((_bstr_t)e.Source()), (LPCSTR)((_bstr_t)e.Description()));
			strErr=buf;
			strErr=strErr.substr(30);
		}
		MessageBox(NULL,"生成邮件物品IMAGE数据块出错","错误",MB_OK);

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

unsigned __stdcall ProcessWriteLogDataFunc(void *pArguments)
{
return 0;
}

void DoSaveData()
{
	DWORD dwBeginTime = timeGetTime();
	g_dwThisSaveStartTime = dwBeginTime;		// 本次存盘开始时间

	size_t nSavePlayerCnt = 0, 
		nCreationCnt = 0,
		nRestoreCnt = 0,
		nDeletionCnt = 0;
	long nTransactionCounter = 0;
	_ConnectionPtr cnSave;
}

void DoSaveLog()
{
	string cnstr;
	_ConnectionPtr cn;
}

#pragma warning(pop)
