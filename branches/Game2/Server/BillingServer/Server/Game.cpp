#include "StdAfx.h"
#include "Game.h"
#include "../App/CBillDBManager.h"
#include "../App/CSessionFactory.h"

#include "../DB/RsAccount.h"
#include "../DB/RsLogInfo.h"

#include "../Net/Message.h"


long g_lVersion =	200;		// 程序版本号（同GameClient统一）

CGame* g_pGame= NULL;		// 游戏客户端数据

HANDLE g_hGameThreadExitEvent=NULL;		// 主线程退出事件
bool	g_bGameThreadExit=false;		// 是否退出主线程

//////////////////////////////////////////////////////////////////////////
// 服务器状态日志部分
//////////////////////////////////////////////////////////////////////////
CRITICAL_SECTION g_csServerInfoLog;
HANDLE g_hServerInfoLog = NULL;

vector<CGame::tagPingWorldServerInfo> g_vWorldServerInfo;

string g_strServerInfoConnStr;
//////////////////////////////////////////////////////////////////////////



void AppendServerInfoLog(vector<CGame::tagPingWorldServerInfo> &vWorldServerInfo)
{
	if( TryEnterCriticalSection(&g_csServerInfoLog) )
	{
		for (size_t i=0; i<vWorldServerInfo.size(); i++)
		{
			// add wroldserver info to temp 
			CGame::tagPingWorldServerInfo stWorldServerInfo;
			stWorldServerInfo.strIP = vWorldServerInfo[i].strIP;
			stWorldServerInfo.dwPort = vWorldServerInfo[i].dwPort;
			stWorldServerInfo.dwPlayerNum = vWorldServerInfo[i].dwPlayerNum;

			for(size_t j=0; j<vWorldServerInfo[i].vectorGameServerInfo.size(); ++j)
			{
				// add gamerserver info to temp
				CGame::tagPingWorldServerInfo::tagPingGameServerInfo stGameServerInfo;

				stGameServerInfo.strIP = vWorldServerInfo[i].vectorGameServerInfo[j].strIP;
				stGameServerInfo.dwPort = vWorldServerInfo[i].vectorGameServerInfo[j].dwPort;
				stGameServerInfo.dwPlayerNum = vWorldServerInfo[i].vectorGameServerInfo[j].dwPlayerNum;

				stWorldServerInfo.vectorGameServerInfo.push_back(stGameServerInfo);
			}

			g_vWorldServerInfo.push_back(stWorldServerInfo);
		}

		LeaveCriticalSection(&g_csServerInfoLog);
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
{
	s_lAITick = 0;

	m_pRsLogInfo = NULL;
	m_pRsAccount = NULL;

	s_pNetServer_GS = NULL;
	m_bExit = false;
	
	m_StateLvl[0] = -1;
	m_StateLvl[1] = 10000;
	m_StateLvl[2] = 20000;
	m_StateLvl[3] = 30000;

	m_pBillDBManager=new CBillDBManager();	
}

CGame::~CGame(void)
{
	
}

bool CGame::LoadSetup()
{
	// 读入配置setup.ini
	string strTemp;
	string strSqlConType;//连接类型
	string strSqlIP;//数据库服务器IP
	string strSqlUserName;//数据库用户名
	string strSqlPassWord;//密码
	string strDBName;//DATABASE NAME
	ifstream stream;
	stream.open("setup.ini");
	if( stream.is_open() )
	{
		stream  >> strTemp >> GetSetup()->dwListenPort_GameServer			
			>> strTemp >> GetSetup()->strSqlConType
			>> strTemp >> GetSetup()->strSqlServerIP
			>> strTemp >> GetSetup()->strSqlUserName
			>> strTemp >> GetSetup()->strSqlPassWord
			>> strTemp >> GetSetup()->strDBName
			>> strTemp >> GetSetup()->bUseLogServer
			>> strTemp >> GetSetup()->strLogIP
			>> strTemp >> GetSetup()->dwLogPort
			>> strTemp >> GetSetup()->dwLogClientNo
			>> strTemp >> GetSetup()->dwWorkerThreadNum
			>> strTemp >> GetSetup()->bUseSystemRollback
			>> strTemp >> GetSetup()->dwSystemRollbackTime
			>> strTemp >> GetSetup()->dwRollbackLockTime


			>> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwGSFreeSockOperNum
			>> strTemp >> GetSetup()->dwGSFreeIOOperNum
			>> strTemp >> GetSetup()->lGSIOOperDataBufNum
			>> strTemp >> GetSetup()->bGSCheckNet
			>> strTemp >> GetSetup()->dwGSBanIPTime
			>> strTemp >> GetSetup()->dwGSMaxMsgLen			
			>> strTemp >> GetSetup()->lGSMaxConnectNum
			>> strTemp >> GetSetup()->lGSMaxClientsNum
			>> strTemp >> GetSetup()->lGSPendingWrBufNum
			>> strTemp >> GetSetup()->lGSPendingRdBufNum
			>> strTemp >> GetSetup()->lGSMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lGSMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lGSMaxBlockedSendMsgNum
			>> strTemp >> GetSetup()->lGSConPendingWrBufNum
			>> strTemp >> GetSetup()->lGSConPendingRdBufNum
			>> strTemp >> GetSetup()->lGSConMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lGSConMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lGSConMaxBlockedSendMsgNum

			>> strTemp >> GetSetup()->dwRefeashInfoTime
			>> strTemp >> GetSetup()->dwSaveInfoTime
			>> strTemp >> GetSetup()->dwDoQueueInter
			>> strTemp >> GetSetup()->dwSendMsgToQueInter			

			>> strTemp >> GetSetup()->dwPingGSTime
			>> strTemp >> GetSetup()->dwPingGSErrorTime

			>> strTemp >> GetSetup()->bCheckForbidIP
			>> strTemp >> GetSetup()->bCheckAllowIP
			>> strTemp >> GetSetup()->bCheckBetweenIP
			;

		stream.close();

		m_StateLvl[0] = -1;
		m_StateLvl[1] = (long)(GetSetup()->dwGSMaxPlayers * GetSetup()->fGSBusyScale);
		m_StateLvl[2] = (long)(GetSetup()->dwGSMaxPlayers * GetSetup()->fGSFullScale);
		m_StateLvl[3] = GetSetup()->dwGSMaxPlayers;

		ChangeAllWorldSate();
		
		AddLogText("载入配置文件...ok!");
		return true;
	}

	// 从加密文件读取
	else
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

		stream  >> strTemp >> GetSetup()->dwListenPort_GameServer			
			>> strTemp >> GetSetup()->strSqlConType
			>> strTemp >> GetSetup()->strSqlServerIP
			>> strTemp >> GetSetup()->strSqlUserName
			>> strTemp >> GetSetup()->strSqlPassWord
			>> strTemp >> GetSetup()->strDBName
			>> strTemp >> GetSetup()->bUseLogServer
			>> strTemp >> GetSetup()->strLogIP
			>> strTemp >> GetSetup()->dwLogPort
			>> strTemp >> GetSetup()->dwLogClientNo
			>> strTemp >> GetSetup()->dwWorkerThreadNum

			>> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwGSFreeSockOperNum
			>> strTemp >> GetSetup()->dwGSFreeIOOperNum
			>> strTemp >> GetSetup()->lGSIOOperDataBufNum
			>> strTemp >> GetSetup()->bGSCheckNet
			>> strTemp >> GetSetup()->dwGSBanIPTime
			>> strTemp >> GetSetup()->dwGSMaxMsgLen			
			>> strTemp >> GetSetup()->lGSMaxConnectNum
			>> strTemp >> GetSetup()->lGSMaxClientsNum
			>> strTemp >> GetSetup()->lGSPendingWrBufNum
			>> strTemp >> GetSetup()->lGSPendingRdBufNum
			>> strTemp >> GetSetup()->lGSMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lGSMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lGSMaxBlockedSendMsgNum
			>> strTemp >> GetSetup()->lGSConPendingWrBufNum
			>> strTemp >> GetSetup()->lGSConPendingRdBufNum
			>> strTemp >> GetSetup()->lGSConMaxSendSizePerSecond
			>> strTemp >> GetSetup()->lGSConMaxRecvSizePerSecond
			>> strTemp >> GetSetup()->lGSConMaxBlockedSendMsgNum

			>> strTemp >> GetSetup()->dwRefeashInfoTime
			>> strTemp >> GetSetup()->dwSaveInfoTime
			>> strTemp >> GetSetup()->dwDoQueueInter
			>> strTemp >> GetSetup()->dwSendMsgToQueInter			

			>> strTemp >> GetSetup()->dwPingGSTime
			>> strTemp >> GetSetup()->dwPingGSErrorTime

			>> strTemp >> GetSetup()->bCheckForbidIP
			>> strTemp >> GetSetup()->bCheckAllowIP
			>> strTemp >> GetSetup()->bCheckBetweenIP
			;

		m_StateLvl[0] = -1;
		m_StateLvl[1] = (long)(GetSetup()->dwGSMaxPlayers * GetSetup()->fGSBusyScale);
		m_StateLvl[2] = (long)(GetSetup()->dwGSMaxPlayers * GetSetup()->fGSFullScale);
		m_StateLvl[3] = GetSetup()->dwGSMaxPlayers;

		ChangeAllWorldSate();

		AddLogText("载入配置文件...ok!");
		return true;
	}

	return false;
}

CGame::tagGameServerInfo* CGame::GetGameServer(const char* strIP, DWORD dwPort)
{
	//map<long,tagGameServerInfo>		m_listGSInfo;
	tagGameServerInfo* pGSInfo=NULL;
	map<long,tagGameServerInfo>::iterator it=m_listGSInfo.begin();
	for(;it!=m_listGSInfo.end();it++)
	{
		if(strcmp(strIP,it->second.strIp.c_str())==0 && dwPort==it->second.dwPort)
		{
			return &(it->second);
		}
	}
	return NULL;
}

bool CGame::ReLoadSetup()
{
	LoadSetup();
	return true;
}

void	CGame::InitLogClient(CMessage *pMsg)
{
	//LogClientInfo tmpLogClientInfo;
	///*
	//tmpLogClientInfo.ErrLogFileName = GetSetup()->strErrFile;
	//tmpLogClientInfo.LogFileName	= GetSetup()->strLogFile;
	//tmpLogClientInfo.TxtLogFileName = GetSetup()->strTxtLogFile;*/
	//tmpLogClientInfo.lClientNo		= GetSetup()->dwLogClientNo;


	//bool re = m_LogBSClient.Init(pMsg, tmpLogClientInfo);	
}

void	CGame::LogServerAffirm(CMessage *pMsg)
{
	//if(0 == pMsg->GetLong())
	//	m_LogBSClient.PassAffirm(false);
	//else
	//	m_LogBSClient.PassAffirm(true);

	////test
	/*SYSTEMTIME	stTime;
	char		szTime[128] = {0};
	GetLocalTime(&stTime);
	sprintf(szTime,"%d-%d-%d %d:%d:%d" ,
	stTime.wYear,
	stTime.wMonth,
	stTime.wDay,
	stTime.wHour,
	stTime.wMinute,
	stTime.wSecond);

	string strTime = string(szTime);

	CGUID guid;
	CGUID::CreateGUID(guid);
	float a = 13.5f, b = 14.6f;
	long i = 0;
	while(i < 19999)
	{
	a = (float)i;
	m_LogClient.SendLogToServer( 
	35, szTime,
	110001,		"测试地图", 
	&a,		&b, 
	&guid,	"TeamName", 
	&guid,	"PlayerName");
	++i;
	}*/


	//m_LogClient.SendLogToServer(1, 1, 2, &guid, "sss", 1, 1, &guid, "player1", &guid, "player2", 1, &guid, "goods", 1, 1, 5);
	//m_LogClient.SendLogToServer(1, 1, 2, &guid, "sss", 3, 4, &guid, "风一样的男人", &guid, "player2", 5, &guid, "goods", 6, 7, 5);
	////test end


}

void	CGame::LogServerCondition(CMessage *pMsg)
{
	//m_LogBSClient.UpdateCondition(pMsg);
}

//////////////////////////////////////////////////////////////////////////
void OnLogServerMessage(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_LOG20_SERVER_TABLE://表格结构信息
		{
			GetGame()->InitLogClient(pMsg);
		}
		break;
	case MSG_LOG20_SERVER_AFFIRM://确认服务器在LogServer上有配置的数据库
		{
			GetGame()->LogServerAffirm(pMsg);
		}
		break;
	case MSG_LOG20_SERVER_CONDITION://消息的发送控制信息
		{
			GetGame()->LogServerCondition(pMsg);
		}
		break;

	}
}

bool CGame::ReLoadWorldSetup()
{
	LoadGSSetup();
	UpdateDisplayWorldInfo();

	return true;
}

bool CGame::LoadGSSetup()
{
	m_GSInfoSetup.clear();
	ifstream stream;
	stream.open("GSSetup.ini");
	if( !stream.is_open() )
		return false;
	
	while(ReadTo(stream, "#"))
	{
		tagGameServerInfo	stInfo;
		stream >> stInfo.dwIndex >> stInfo.strName >> stInfo.strIp >> stInfo.dwPort;
		m_GSInfoSetup[stInfo.dwIndex] = stInfo;
	}
	stream.close();
	return true;
}

void CGame::SetListWorldInfoBySetup()
{
	m_listGSInfo.clear();
	m_listGSInfo = m_GSInfoSetup;
	map<long,tagGameServerInfo>::iterator it = m_listGSInfo.begin();
	for(;it != m_listGSInfo.end();it++)
	{
		(*it).second.lStateLvl = 0;//默认为维护中
	}
}

// 游戏初始化
BOOL CGame::Init()
{
	srand((unsigned)time(NULL));
	random(100);
	CreateDirectory( "log",	NULL);

	InitializeCriticalSection(&g_csServerInfoLog);
	
	PutDebugString("BillingServer start!");

	if (LoadSetup()==false)
	{
		MessageBox(g_hWnd, "Can't find file setup.ini", "message", MB_OK);
		return false;
	}

	char strVersion[256];
	extern long g_lVersion;
	sprintf(strVersion,"BillingServer[%d][%d]-StarDust", g_lVersion, GetSetup()->dwListenPort_GameServer);

	//查看此程序是否在运行中
	HWND hWnd = FindWindow(NULL,strVersion);
	if (hWnd != NULL)
	{
		char strMsgBox[256]="";
		sprintf(strMsgBox,"%s 程序正在运行中!",strVersion);
		MessageBox(g_hWnd,_T(strMsgBox), "error", MB_OK);
		return false;
	}

	SetWindowText(g_hWnd, strVersion);

	
	

	if(LoadGSSetup() == false)
	{
		AddLogText("Can't find file GSSetup.ini");
		return false;
	}
	SetListWorldInfoBySetup();
	UpdateDisplayWorldInfo();	

	//初始化CMyAdoBase
	CMyAdoBase::Initialize(GetSetup()->strSqlConType,GetSetup()->strSqlServerIP,GetSetup()->strDBName,
		GetSetup()->strSqlUserName,GetSetup()->strSqlPassWord); 	
	
	

	m_pRsAccount=new CRsAccount;

	m_pRsLogInfo=new CRsLogInfo;

	//分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);

    GetGame()->GetBillDBManager()->InitWorkerThread(GetSetup()->strSqlConType,GetSetup()->strSqlServerIP,GetSetup()->strDBName,
		GetSetup()->strSqlUserName,GetSetup()->strSqlPassWord);

	//初始
	CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
	// 初始化服务器网络
	CMySocket::MySocketInit();

	// 创建服务器端
	if( !InitNetServer_GS() )
	{
 		MessageBox(g_hWnd, "Can't init NetServer!", "Message", MB_OK);
		return false;
	}

	// 连接到LogServer
	if (0 == GetSetup()->bUseLogServer)
	{
		AddLogText("连接LogServer配置为不连接，日志服务将不能使用!");
	}
	else
	{
		if( !ConnectLogServer() )
		{
			MessageBox(g_hWnd, "连接LogServer失败, 请先运行LogServer!", "Message", MB_OK);
			CreateConnectLogThread();
		}
		else
		{
			AddLogText("连接LogServer成功!");
		}
	}

	return true;
}

void CGame::OnLOGSLost()
{
	AddLogText("LogServer网络连接断开!");
	//m_LogBSClient.SetLogSocket(0);
	CMessage::SetLogSocket(0);
	CreateConnectLogThread();
}

void CGame::CreateConnectLogThread(void)
{
	// m_LogBSClient.bConnectThreadExit=true;
	//WaitForSingleObject(m_LogBSClient.hConnectThread,INFINITE);
	//CloseHandle(m_LogBSClient.hConnectThread);

	//m_LogBSClient.bConnectThreadExit=false;


	// m_LogBSClient.hConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectLogServerFunc,NULL,0,&threadID3);
}



unsigned __stdcall ConnectLogServerFunc( void* pArguments )
{
	//while(true)
	//{
	//	if(GetGame()->GetLogBSClient().bConnectThreadExit)
	//		break;
	//	Sleep(8000);

	//	if(GetGame()->ReConnectLogServer()==TRUE)
	//	{
	//		break;
	//	}

	//}
	_endthreadex( 0 );
	return 0;
}


BOOL CGame::ConnectLogServer()
{
	long lSocketId=s_pNetServer_GS->Connect(GetSetup()->strLogIP.c_str(), GetSetup()->dwLogPort,NF_Log_Client);
	// m_LogBSClient.SetLogSocket(lSocketId);
	//if(m_LogBSClient.GetLogSocket() > 0)
	//{
	//	CMessage::SetLogSocket(m_LogBSClient.GetLogSocket());
	//	AddLogText("连接LogServer 成功 !");

	//	CMessage msg(MSG_02LOG_SERVER_LOGIN);
	//	msg.Add(GetSetup()->dwLogClientNo);
	//	msg.SendToSocket(m_LogBSClient.GetLogSocket());

	//	return TRUE;
	//}
	return FALSE;
}

bool CGame::ReConnectLogServer()
{
	return false; //GetLogBSClient().bConnectThreadExit = ConnectLogServer();
}

// 游戏释放
BOOL CGame::Release()
{
	PutDebugString("BillingServer Exiting...");
	if( !m_bExit )
	{
		m_bExit = true;		

		if( GetNetServer_GS() )	
		{
			s_pNetServer_GS->Exit();			
		}

		SAFE_DELETE(s_pNetServer_GS);

		CMySocket::MySocketClearUp();		
		SAFE_DELETE(m_pRsAccount);
		SAFE_DELETE(m_pRsLogInfo);

		SAFE_DELETE(m_pBillDBManager);

		
		// 释放CMyAdoBase;
		CMyAdoBase::Uninitalize();
		//
		CMessage::Release();		

		CSessionFactory::Release();

		SAFE_DELETE(m_pDBAllocator);

		DeleteCriticalSection(&g_csServerInfoLog);
	}

	PutDebugString("BillingServer Exited!");
	return true;
}

//初始化游戏网络服务器端，建立网络服务器端，装载数据
BOOL CGame::InitNetServer_GS(void)
{
	s_pNetServer_GS = new CServer;
	long lRet = s_pNetServer_GS->Start(3,m_pDBAllocator,
		m_Setup.dwGSFreeSockOperNum,m_Setup.dwGSFreeIOOperNum,m_Setup.lGSIOOperDataBufNum,
		m_Setup.bGSCheckNet,m_Setup.dwGSBanIPTime,
		m_Setup.dwGSMaxMsgLen,m_Setup.lGSMaxConnectNum,m_Setup.lGSMaxClientsNum,
		m_Setup.lGSPendingWrBufNum,m_Setup.lGSPendingRdBufNum,
		m_Setup.lGSMaxSendSizePerSecond,m_Setup.lGSMaxRecvSizePerSecond,
		m_Setup.lGSMaxBlockedSendMsgNum);
	if( !lRet )
	{
		MessageBox(g_hWnd, "Start GameServer Net server Error", "Message", MB_OK);
		return FALSE;
	}

	BOOL bRet = s_pNetServer_GS->Host(m_Setup.dwListenPort_GameServer,NULL,NF_Server_World,SOCK_STREAM);
	if(!bRet)
	{
		MessageBox(g_hWnd, "Start Host for GameServer Error", "Message", MB_OK);
	}
	
	CMessage::SetGSServer(s_pNetServer_GS);
	AddLogText("BillingServer作为GameServer的服务器初始化成功 !");
	return TRUE;
}



// 游戏AI
BOOL CGame::AI()
{
	return TRUE;
}

// 消息处理
BOOL CGame::ProcessMessage()
{

	if(GetNetServer_GS())
	{
		long lMessageCount = GetNetServer_GS()->GetRecvMessages()->GetSize();

		for(int i=0; i<lMessageCount; i++)
		{
			CMessage* pMsg =(CMessage*) GetNetServer_GS()->GetRecvMessages()->PopMessage();
			if( pMsg )
			{				
				pMsg->Run();
				CBaseMessage::FreeMsg(pMsg);
			}
		}
	}
	
	return TRUE;
}

// 游戏主循环
BOOL CGame::MainLoop()
{
	static DWORD  now = timeGetTime(), lastRefeashInfoTime = now,lastRollbackLockTime=now;
	now = timeGetTime();

	static DWORD ServerInfoLogLastTime = now;
	// 刷新显示
	if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
	{
		lastRefeashInfoTime = now;
		RefeashInfoText();
	}

	// 结束线程
	if( m_bExit )
	{
		return false;
	}

	if(GetSetup()->bUseSystemRollback)
	{
		if(now - lastRollbackLockTime > GetSetup()->dwSystemRollbackTime)
		{
			m_pRsAccount->SystemRollback();
			lastRollbackLockTime=timeGetTime();
		}
	}

	// 消息处理
	ProcessMessage();	

	// 定时ping其下的WorldServer信息
	static DWORD lastPingWorldTime = now;
	static bool bInPing = false;

	if (bInPing==false)	// 检测是否需要ping的过程
	{
		if (now - lastPingWorldTime > GetSetup()->dwPingGSTime)
		{
			lastPingWorldTime = now;
			bInPing = true;

			GetGame()->m_vectorPingWorldServerInfo.clear();

			CMessage msg(MSG_L2W_SERVER_QUEST_PING);
			msg.SendAllGS();
		}
	}
	else	// 等待所有WorldServer返回的过程
	{
		// 全部收到WorldServer的返回
		if ((long)m_vectorPingWorldServerInfo.size() >= GetWorldNum())
		{
			bInPing = false;

			AddLogText("------------ 正常 -------------");
			for (int i=0; i<(int)m_vectorPingWorldServerInfo.size(); i++)
			{
				AddLogText("World %s:%d has %d players!",
					m_vectorPingWorldServerInfo[i].strIP.c_str(),
					m_vectorPingWorldServerInfo[i].dwPort,
					m_vectorPingWorldServerInfo[i].dwPlayerNum);
			}
			AddLogText("-------------------------------");
		}
		// 出错，认为有的WorldServer已经失去响应
		else if (now - lastPingWorldTime > GetSetup()->dwPingGSErrorTime)
		{
			bInPing = false;

			//AddLogText("------------ 有异常！！ -------------");

			for (int i=0; i<(int)m_vectorPingWorldServerInfo.size(); i++)
			{
				/*
				AddLogText("World %s:%d has %d players!",
					m_vectorPingWorldServerInfo[i].strIP.c_str(),
					m_vectorPingWorldServerInfo[i].dwPort,
					m_vectorPingWorldServerInfo[i].dwPlayerNum);*/
			}
			//AddLogText("-------------------------------");
		}
	}

	Sleep(1);
	return true;
}


/////////////////////////////////////////////////////////////////////////
// WorldServer信息列表
/////////////////////////////////////////////////////////////////////////

// 添加世界
long CGame::AddGS(long lGSNumber,const char* strGSName)
{
	if(strGSName==NULL)
		return -1;

	map<long,tagGameServerInfo>::iterator it = m_listGSInfo.find(lGSNumber);
	if(it == m_listGSInfo.end())
	{
		AddLogText("非法的GameServer Server连接!");
		return -1;
	}
	if( strcmp(it->second.strIp.c_str(),strGSName)!=0)
	{
		AddLogText("非法的GameServer Server连接!");
		return -1;
	}

	(*it).second.lStateLvl = 1;

	//更新客户端World的显示信息	
	UpdateDisplayWorldInfo();

	long lSize = (long)m_listGSInfo.size();
	
	return lSize;
}

// 删除世界
long CGame::DelGS(long lNum)
{
	map<long,tagGameServerInfo>::iterator it = m_listGSInfo.find(lNum);
	if(it != m_listGSInfo.end())
	{
		(*it).second.lStateLvl = 0;
	}
	UpdateDisplayWorldInfo();
	
	long lSize = (long)m_listGSInfo.size();
	
	return lSize;
}

const char *CGame::GetGSNameByID(long lNum)
{
	std::map<long,tagGameServerInfo>::iterator it = m_listGSInfo.find(lNum);
	if(it != m_listGSInfo.end())
	{
		return (*it).second.strName.c_str();
	}
	return NULL;
}

long  CGame::GetGSIDByName(const char* strGSName)
{
	if(strGSName==NULL)
		return -1;
	std::map<long,tagGameServerInfo>::iterator it = m_listGSInfo.begin();
	for(;it != m_listGSInfo.end();it++)
	{
		if(strcmp(strGSName,(*it).second.strName.c_str())==0)
		{
			if( (*it).second.lStateLvl == 0 )	return -1;
			else return (*it).first;
		}
	}
	return -1;
}

bool CGame::WorldServerIsOpenState(long lNum)
{
	map<long,tagGameServerInfo>::iterator it = m_GSInfoSetup.find(lNum);
	if(it != m_GSInfoSetup.end())
	{
		return (*it).second.lStateLvl != 0;
	}
	return false;
}

void CGame::AddWorldInfoToMsg(CMessage& msg,string &strCdkey)
{
	/*
	msg.Add( (short)GetGame()->GetWorldNum() );	//当前有效的worldserver数
	std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
	for(;it != m_listWorldInfo.end();it++)
	{
		//判断这个服务器是否是开启状态和是否是不排队的CDKEY
		if( WorldServerIsOpenState((*it).first) ||
			m_pLoginQueue->IsInNoQueueList(strCdkey.c_str()) )
		{
			msg.Add( (*it).second.lStateLvl );
		}
		else
		{
			msg.Add( (long)0 );
		}
		msg.Add( (char*)((*it).second.strName.c_str()) );
	}
	*/
}

bool CGame::IsExitWorld(const char* strWorldName)
{
	if(strWorldName == NULL)
		return false;

	if( GetGSIDByName(strWorldName) != -1 )
		return true;
	return false;
}

bool CGame::SendMsg2World(CMessage* pMsg,long lWorldServerID)
{
	if(!pMsg)
		return false;
	
	pMsg->SendToWorldNumID(lWorldServerID);
	return true;
}



long CGame::GetWorldSateLvl(long lPlayerNum)
{
	int i = 1;
	for(i = 1;i <= 3;i++)
	{
		if( lPlayerNum < m_StateLvl[i] )
		{
			break;
		}
	}
	if(i > 3)
		i = 3;
	return i;
}


void CGame::OnChangeStateLvl(long lWorldNum,long lPlayerNum)
{
	long lStateLvl = GetWorldSateLvl(lPlayerNum);

	map<long,tagGameServerInfo>::iterator it = m_listGSInfo.find(lWorldNum);
	if(it != m_listGSInfo.end())
		(*it).second.lStateLvl = lStateLvl;
}

void CGame::ChangeAllWorldSate()
{
	
}


// 踢出一个用户,如果用户处于登陆状态,那么直接断掉并发消息给对应的WorldServer去清除;
// 如果已经进入游戏,那么发消息给对应的WorldServer去清除.
bool CGame::KickOut(const char *szCdkey)
{
	/*
	//是否已经有玩家使用该CDKEY
	//这里查找正在连接LOGINSERVER的玩家
	if( GetGame()->GetLoginCdkeyWorldServer(szCdkey)!= NULL )
	{
		//断掉相应的客户端
		long lSocketID = CMessage::GetSocketByCdkey(szCdkey);
		if(lSocketID)
			GetGame()->GetNetServer_Client()->DisConn(lSocketID);
	}
	
	int nWordNum = FindCdkey(szCdkey);
	if(nWordNum != -1)
	{
		//##发送消息给World Server，告诉World Server有同一帐号尝试登陆。
		CMessage msgToWorldServer( MSG_L2W_LOG_REPEAT_LOGIN );
		msgToWorldServer.Add( (char*)(szCdkey) );
		GetGame() -> SendMsg2World( &msgToWorldServer, nWordNum);
		return true;
	}
	*/
	return false;
}

void CGame::UpdateDisplayWorldInfo()
{
	SendMessage(g_hWorldListBox,LB_RESETCONTENT,NULL,NULL);
	//更新控件里的服务器状态
	std::map<long,tagGameServerInfo>::iterator it = m_listGSInfo.begin();
	for(;it != m_listGSInfo.end();it++)
	{ 
		string strName = (*it).second.strName;
		strName += "[";
		long lFlag = (*it).second.lStateLvl;
		map<long,tagGameServerInfo>::iterator itSetup =
								m_GSInfoSetup.find( (*it).first );			
		if(itSetup != m_GSInfoSetup.end())
		{
			if(lFlag != 0 && (*itSetup).second.lStateLvl == 0)
			{
				lFlag = -1;
			}
		}
		if(lFlag == 0)
		{
			strName += "断开";
		}
		else if(lFlag == -1)
		{
			strName += "连接";
		}
		else
		{
			strName += "开启";
		}
		strName += "]";
		SendMessage(g_hWorldListBox,LB_INSERTSTRING,-1,(LPARAM)(strName.c_str()));
	}

}

void CGame::OnNetGSClose(long lGSID)
{
	const char* strGSName = GetGame()->GetGSNameByID(lGSID);
	if( strGSName )
	{
		AddLogText("GameServer [%s] lost!",strGSName);		
	}
	GetGame()->DelGS(lGSID);
}

int CGame::EnterGame( const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName, bool bLimit, bool bMobileUser )
{
	if( !szCdkey || !dwIP || !lSockID || !szWSName ) return 1;

	// 如果已经在游戏中,那么踢掉该帐号
	if( !strlen(szWSName) && GetGame()->KickOut(szCdkey) ) 	{
		//##如果帐号在游戏中,通知玩家,暂时无法登陆.
		CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
		msg.Add( static_cast<char>(LOGIN_CHECKOUT) );
		//msg.SendToClient(lSockID);
		return 1;
	}

	//给客户端套接字关联Cdkey
	//CMessage::MapCDkeySocket(szCdkey, lSockID);
	//SetLoginCdkeyWorldServer(szCdkey,"");

	// 帐户登录日志（插入）
	m_pRsLogInfo->AccountEnterLog(szCdkey, dwIP);

	//	AddLogText("%s 连接成功!",strCDkey);

	// 直接进入选人界面（退到选人界面代码）
	if (szWSName[0]!=0)
	{
		/*
		if( L2W_PlayerBase_Send(szWSName,szCdkey) )//向worldserver请求该客户的基本信息
		{
			SetLoginCdkeyWorldServer(szCdkey,szWSName);//设置该连接所选择的worldserver名
		}*/
	}
	else
	{

		AddLogText("MSG_L2C_LOG_ANSWERCDKEY!MSGID[718081]");
		// 返回client
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((char)LOGIN_LOGINSERVEROK);
		GetGame()->AddWorldInfoToMsg(msg,string(szCdkey));
		//msg.SendToClient(lSockID);
	}

	//// 添加一个手机绑定帐号
	//if( is_mobile_user )
	//{
	//    AddMobileUser( szPtAccount );
	//}

	//if( !strlen(szWsName) && /*GetGame()->GetSetupEx()->_use_aas &&*/ bLimit ) {
	//	CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
	//	msg.Add( (char)(LOGIN_FCM_Notice) );
	//	msg.SendToClient(nSockID);
	//}
	return 0;
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

		if (ret)
		while(1)
		{
			// 结束线程
			if( g_bGameThreadExit )
			{
				break;
			}
			
			if( !GetGame()->MainLoop() )
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
		MessageBox(g_hWnd, "程序出现错误，请查看最新的错误报告文件", "Error", MB_OK);

		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
		SetEvent(g_hGameThreadExitEvent);
		_endthreadex( 0 );
		return 0;
	}
#endif

	SetEvent(g_hGameThreadExitEvent);
	PostMessage(g_hWnd, WM_CLOSE, 0, 0);

	_endthreadex( 0 );
	return 0;
}


unsigned __stdcall UpdateOnlineUser2DB(void *param) {

	::CoInitialize(NULL);
	if(param) {
		struct tagTreadArgs {
			long wid;
			list<string> *list;

			~tagTreadArgs() { list->clear(); SAFE_DELETE(list); }
		};
	
		char sql[1024];
		_ConnectionPtr cn;
		_RecordsetPtr rs;
		tagTreadArgs *tempParam = (tagTreadArgs *)param;

		if( CMyAdoBase::CreateCn(cn) && CMyAdoBase::OpenCn(cn) ) {

			sprintf(sql, "DELETE FROM online_user WHERE worldNumber=%d", tempParam->wid);
			CMyAdoBase::ExecuteCn(sql,cn);

			for( list<string>::iterator i = tempParam->list->begin(); i != tempParam->list->end(); ++i ) {
				sprintf(sql, "INSERT INTO online_user(userAccount, worldNumber) VALUES('%s',%d)",
					(*i).data(), tempParam->wid);
				CMyAdoBase::ExecuteCn(sql, cn);
			}

			CMyAdoBase::CloseCn(cn);
		}
		CMyAdoBase::ReleaseCn(cn);

		SAFE_DELETE(param);
	}

	::CoUninitialize();
	_endthreadex(0);
	return 0;
}
