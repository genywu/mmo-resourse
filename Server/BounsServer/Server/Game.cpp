#include "StdAfx.h"
//#include <vld.h>
#include "Game.h"

#include "../DB/RsBouns.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGame* g_pGame = NULL;						// 游戏数据
bool g_bGameThreadExit = false;				// 是否退出主线程
HANDLE g_hGameThreadExitEvent = NULL;		// 主线程退出事件

bool bConnectServerThreadExit = false;		// 是否退出连接服务器线程
HANDLE hConnectServerThread = NULL;			// 连接服务器线程句柄

extern long g_lConnectedGSNum;

// 创建游戏数据
BOOL CreateGame()
{
	g_pGame = new CGame();
	return TRUE;
}

// 释放游戏数据
BOOL DeleteGame()
{
	if(g_pGame)
	{
		delete g_pGame;
		g_pGame = NULL;
	}
	return TRUE;
}

CGame* GetGame()
{
	return g_pGame;
}

//LogicLogInterface* GetGameLogInterface()
//{
//	return GetGame()->GetLogicLogInterface();
//}

CGame::CGame(void)
{
	s_pNetServer = NULL;
	m_pRsBouns = NULL;

	m_lServerInitSendFlag = 0;
	InitializeCriticalSection(&m_csServerInitSendFlag);

	m_pBounsDBManager = new CBounsDBManager();

	//m_pLogicLogInterface= new LogicLogInterface(NULL);

	m_mapBounsDes.clear();
}

CGame::~CGame(void)
{
	DeleteCriticalSection(&m_csServerInitSendFlag);
}

BOOL CGame::Init()
{
	// 创建日志保存目录
	CreateDirectory("log", NULL);

//#ifdef _RUNSTACKINFO_
//	InitStackFileName();
//#endif

	srand((unsigned)time(NULL));
	random(100);
	PutDebugString("Server start!!!");

	// 装载资源
	LoadServerResource();

	if(!LoadSetup())
	{
		return FALSE;
	}

	// 加载GameServer配置
	if(!LoadGSSetup("GSSetup.ini"))
	{
		return FALSE;
	}

	// 初始化CMyAdoBase
	CMyAdoBase::Initialize(m_tgSetup.strSqlConType, m_tgSetup.strSqlServerIP, m_tgSetup.strDBName,
		m_tgSetup.strSqlUserName, m_tgSetup.strSqlPassWord);


	m_pRsBouns = new CRsBouns();

	if(m_pRsBouns->LoadBounsDes(m_mapBounsDes) == S_OK )
	{
		AddLogText("加载奖励资源表成功!");
	}
	else
	{
		MessageBox(g_hWnd, "Load Bouns Failed!", "Message", MB_OK);
		return FALSE;
	}

	// 分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_tgSetup.dwDataBlockNum, m_tgSetup.dwDataBlockSize);

	GetGame()->GetBounsDBManager()->InitOperThread(m_tgSetup.strSqlConType, m_tgSetup.strSqlServerIP, m_tgSetup.strDBName,
		m_tgSetup.strSqlUserName, m_tgSetup.strSqlPassWord);

	CMessage::Initial(m_pDBAllocator, m_tgSetup.dwFreeMsgNum);
	// 初始化服务器网络
	CMySocket::MySocketInit();

	// 装载内存影射文件配置
	//LoadMapFileSetup();

	// 创建服务器端
	if( !InitNetServer() )
	{
		MessageBox(g_hWnd, "Initial NetServer failed!", "Message", MB_OK);
		return FALSE;
	}

	// 开始监听
	if( !StartAccept() )
	{
		return FALSE;
	}

	//// 日志服务器
	//if( m_tgSetup.lUseLogServer == 0 )
	//{
	//	AddLogText("连接LogServer配置为不连接，日志服务将不能使用!");
	//}
	//else
	//{
	//	if( !StartConnectLogServer() )
	//	{
	//		AddLogText("Connect LogServer Failed!");
	//		CreateConnectLogThread();
	//	}
	//	else
	//	{
	//		AddLogText("Connect LogServer Successfully!");
	//	}
	//}

	return TRUE;
}

BOOL CGame::InitStackFileName()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	_snprintf(m_pszRunStackFile,MAX_PATH,"RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		
	return TRUE;
}

bool CGame::LoadServerResource()
{
	CHAR* pszPath = new CHAR[MAX_PATH];
	string strWorkPath("");
	if(::GetCurrentDirectory(MAX_PATH, pszPath))
	{
		strWorkPath = string(pszPath);
	}
	delete[] pszPath;

	// 初始化数据包
	string strFileInfo("PackRes.cfg");
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES, strWorkPath, strFileInfo);
	if(!pClientResource->LoadEx())
	{
	//	AddLogText("Load server resource failed!");
	//	return false;
	}

	AddLogText("Load server resource OK!");

	return true;
}

bool CGame::LoadSetup()
{
	// 加载全局配置
	string strTemp;

	ifstream streamLog;

	streamLog.open("LogSetup.ini");
	if( streamLog.is_open() )
	{
		streamLog 
			>> strTemp >> GetSetup()->lUseLogServer
			>> strTemp >> GetSetup()->strLogIP
			>> strTemp >> GetSetup()->dwLogPort
			>> strTemp >> GetSetup()->strErrFile
			>> strTemp >> GetSetup()->strLogFile
			>> strTemp >> GetSetup()->strTxtLogFile
			>> strTemp >> GetSetup()->dwClientNo;
		streamLog.close();
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file LogSetup.ini", "message", MB_OK);
		return false;
	}

	ifstream isSetup;
	isSetup.open("Setup.ini");
	if( isSetup.is_open() )
	{
		isSetup
			>> strTemp >> m_tgSetup.dwNumber
			>> strTemp >> m_tgSetup.strName
			>> strTemp >> m_tgSetup.strLocalIP
			>> strTemp >> m_tgSetup.dwListenPort
			>> strTemp >> m_tgSetup.strSqlConType
			>> strTemp >> m_tgSetup.strSqlServerIP
			>> strTemp >> m_tgSetup.strSqlUserName
			>> strTemp >> m_tgSetup.strSqlPassWord
			>> strTemp >> m_tgSetup.strDBName
			>> strTemp >> m_tgSetup.dwWorkerThreadNum
			>> strTemp >> m_tgSetup.dwMaxBounsNum;
		isSetup.close();

		strTemp = "BounsServer";
		strTemp += "[";
		strTemp += m_tgSetup.strName;
		strTemp += "]-StarDust";

		// 查看此程序是否在运行中
		HWND hWnd = FindWindow(NULL, strTemp.c_str());
		if( hWnd != NULL )
		{
			char strMsg[256];
			sprintf(strMsg, "%s 程序正在运行中!", strTemp.c_str());
			MessageBox(g_hWnd, _T(strMsg), "error", MB_OK);
			return false;
		}

		SendMessage(g_hWnd, WM_SETTEXT, 0, (LPARAM)strTemp.c_str());
	}
	else	
	{
		MessageBox(g_hWnd, "Can't find file setup.ini", "message", MB_OK);
		return false;
	}

	// load setupex.ini
	ifstream isEx;
	isEx.open("SetupEx.ini");
	if( isEx.is_open() )
	{
		isEx
			>> strTemp >> m_tgSetup.dwDataBlockNum
			>> strTemp >> m_tgSetup.dwDataBlockSize
			>> strTemp >> m_tgSetup.dwFreeMsgNum
			
			>> strTemp >> m_tgSetup.dwFreeSockOperNum
			>> strTemp >> m_tgSetup.dwFreeIOOperNum
			>> strTemp >> m_tgSetup.lIOOperDataBufNum
			>> strTemp >> m_tgSetup.bCheckNet
			>> strTemp >> m_tgSetup.dwBanIPTime
			>> strTemp >> m_tgSetup.dwMaxMsgLen
			>> strTemp >> m_tgSetup.lMaxConnectNum
			>> strTemp >> m_tgSetup.lMaxClientsNum
			>> strTemp >> m_tgSetup.lPendingWrBufNum
			>> strTemp >> m_tgSetup.lPendingRdBufNum
			>> strTemp >> m_tgSetup.lMaxSendSizePerSecond
			>> strTemp >> m_tgSetup.lMaxRecvSizePerSecond
			>> strTemp >> m_tgSetup.lMaxBlockedSendMsgNum
			>> strTemp >> m_tgSetup.lConPendingWrBufNum
			>> strTemp >> m_tgSetup.lConPendingRdBufNum
			>> strTemp >> m_tgSetup.lConMaxSendSizePerSecond
			>> strTemp >> m_tgSetup.lConMaxRecvSizePerSecond
			>> strTemp >> m_tgSetup.lConMaxBlockedSendMsgNum

			>> strTemp >> m_tgSetup.dwRefeashInfoTime;

		isEx.close();
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setupex.ini", "message", MB_OK);
		return false;
	}
	return true;
}

bool CGame::LoadGSSetup(const char* pszFileName)
{
	CRFile* prfile = rfOpen(pszFileName);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "Can't find file %s", pszFileName);
		MessageBox(g_hWnd, str, "message", MB_OK);
		return false;
	}

	string strTemp;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	// 读游戏服务器信息
	tagClientInfo stCInfo;
	long lClientNum = 0;
	stream >> strTemp >> lClientNum;
	for(int i=0; i<lClientNum; i++)
	{
		ReadTo(stream, "#");
		stream >> stCInfo.dwIndex >> stCInfo.strIP;
		stCInfo.bConnected = false;
		s_mapGSInfo[stCInfo.dwIndex] = stCInfo;
	}

	AddLogText("Load GSSetup.ini OK!");
	return true;
}

bool CGame::ReLoadGSSetup()
{
	return LoadGSSetup("GSSetup.ini");
}

bool CGame::LoadMapFileSetup()
{
	string strTemp("");
	string strMemClientID("");
	string strMapObjName("");
	ifstream stream;
	stream.open("MapFileSetup.ini");
	if( stream.is_open() )
	{
		stream
			>> strTemp >> strMemClientID
			>> strTemp >> strMapObjName;
		m_tgSetup.dwGappID = atoi(strMemClientID.c_str());
		GetInst(CMemClient).Initial(strMapObjName.c_str(), m_pDBAllocator);
		return true;
	}
	return false;
}

void CGame::AddBounsDes(tagBounsDes &tgBD)
{
	map<DWORD, tagBounsDes>::iterator itor = m_mapBounsDes.find(tgBD.dwBounsID);
	if(itor != m_mapBounsDes.end())
	{
		// 已经存在有奖励描述了
		return;
	}

	m_mapBounsDes[tgBD.dwBounsID] = tgBD;
}

bool CGame::InitNetServer()
{
	s_pNetServer = new CServer;
	//做为服务器和客户端双重身份启动
	long lRet = s_pNetServer->Start(1|2,m_pDBAllocator,
		m_tgSetup.dwFreeSockOperNum,m_tgSetup.dwFreeIOOperNum,m_tgSetup.lIOOperDataBufNum,
		m_tgSetup.bCheckNet,m_tgSetup.dwBanIPTime,
		m_tgSetup.dwMaxMsgLen,m_tgSetup.lMaxConnectNum,m_tgSetup.lMaxClientsNum,
		m_tgSetup.lPendingWrBufNum,m_tgSetup.lPendingRdBufNum,
		m_tgSetup.lMaxSendSizePerSecond,m_tgSetup.lMaxRecvSizePerSecond,
		m_tgSetup.lMaxBlockedSendMsgNum,
		m_tgSetup.lConPendingWrBufNum,m_tgSetup.lConPendingRdBufNum,
		m_tgSetup.lConMaxSendSizePerSecond,m_tgSetup.lConMaxRecvSizePerSecond,
		m_tgSetup.lConMaxBlockedSendMsgNum);
	if( !lRet )
	{
		MessageBox(g_hWnd, "Start NetServer Error", "Message", MB_OK);
		return false;
	}
	CMessage::SetServer(s_pNetServer);

	return true;
}

bool CGame::StartAccept()
{
	if(NULL == s_pNetServer)	
	{
		return false;
	}
	BOOL bRet = s_pNetServer->Host(m_tgSetup.dwListenPort,NULL,NF_SERVER_CLIENT,SOCK_STREAM);
	if(!bRet)
	{
		MessageBox(g_hWnd, "Start Host Error", "Message", MB_OK);
		return false;
	}
	AddLogText("BounsServer As GS'server initial OK!");
	return true;
}


void CGame::SetServerInitSendFlag(eServerInitSendMsgFlag flag)
{
	EnterCriticalSection(&m_csServerInitSendFlag);
	m_lServerInitSendFlag = m_lServerInitSendFlag | flag;
	LeaveCriticalSection(&m_csServerInitSendFlag);
}

//// 连接日志服务器
//bool CGame::StartConnectLogServer()
//{
//	m_LogClient.SetSoket(s_pNetServer->Connect(m_tgSetup.strLogIP.c_str(), m_tgSetup.dwLogPort,NF_LOGSERVER_CLIENT));
//	if(m_LogClient.GetSoket() > 0)
//	{
//		CMessage::SetLogSocket(m_LogClient.GetSoket());
//		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CONNECTLOGSOK));
//
//		CMessage msg(MSG_02LOG_SERVER_LOGIN);
//		msg.Add(GetSetup()->dwClientNo);
//		msg.SendToSocket(m_LogClient.GetSoket());
//
//		return true;
//	}
//	return false;
//}
//
//bool CGame::ReConnectLogServer()
//{
//	return GetLogClient().bConnectThreadExit = StartConnectLogServer();
//}
//
//// 创建连接日志服务器线程
//void CGame::CreateConnectLogThread()
//{
//	m_LogClient.bConnectThreadExit=true;
//	WaitForSingleObject(m_LogClient.hConnectThread,INFINITE);
//	CloseHandle(m_LogClient.hConnectThread);
//
//	m_LogClient.bConnectThreadExit=false;
//
//	unsigned int threadID;
//	m_LogClient.hConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectLogServerFunc,NULL,0,&threadID);
//}
//
//void CGame::OnLostLogServer()
//{
//	AddLogText("============the LogServer closed==========!!!");
//	m_LogClient.SetSoket(0);
//	CMessage::SetLogSocket(0);
//	CreateConnectLogThread();
//}

bool CGame::AddGameServer(long lGSNum, const char *strGSIP)
{
	if( strGSIP == NULL )
	{
		return false;
	}
	map<DWORD, tagClientInfo>::iterator it = s_mapGSInfo.find(lGSNum);
	if( it == s_mapGSInfo.end() )
	{
		AddLogText("非法GameServer连接!");
		return false;
	}

	if( strcmp(it->second.strIP.c_str(), strGSIP) != 0 )
	{
		AddLogText("非法GameServer连接!");
		return false;
	}

	char szInfo[256];
	sprintf(szInfo, "%s[%d]", it->second.strIP.c_str(), it->second.dwIndex);
	SendMessage(g_hClientListBox, LB_INSERTSTRING, -1, (LPARAM)szInfo);
	g_lConnectedGSNum++;
	RefeashInfoText();
	return true;
}

bool CGame::DelGameServer(long lGSNum)
{
	map<DWORD, tagClientInfo>::iterator it = s_mapGSInfo.find(lGSNum);
	if( it != s_mapGSInfo.end() )
	{
		char szInfo[256];
		sprintf(szInfo, "%s[%d]", it->second.strIP.c_str(), it->second.dwIndex);
		LRESULT lPos = SendMessage(g_hClientListBox, LB_FINDSTRING, 0, (LPARAM)szInfo);
		if( LB_ERR != lPos )
		{
			SendMessage(g_hClientListBox, LB_DELETESTRING, lPos, 0);
			g_lConnectedGSNum--;
			RefeashInfoText();
			return true;
		}
	}

	return false;
}

void CGame::UpdateGSStateDisplay()
{
	LARGE_INTEGER lBegin,lEnd, lFre;
	double dbTick;
	::QueryPerformanceCounter(&lBegin);
	::QueryPerformanceFrequency(&lFre);
	dbTick = 1000000.0/lFre.QuadPart;

	
	SendMessage(g_hClientListBox,LB_RESETCONTENT,NULL,NULL);
	//更新控件里的服务器状态
	char szGSNum[16];
	extern long g_lConnectedGSNum;
	g_lConnectedGSNum = 0;
	map<DWORD, tagClientInfo>::iterator itor = s_mapGSInfo.begin();
	for(; itor!=s_mapGSInfo.end(); ++itor)
	{ 
		sprintf(szGSNum, "[%d]", itor->second.dwIndex);
		string strName = itor->second.strIP + szGSNum;
		SendMessage(g_hClientListBox,LB_INSERTSTRING,itor->second.dwIndex,(LPARAM)(strName.c_str()));
	}
	RefeashInfoText();

	::QueryPerformanceCounter(&lEnd);
	 char str[128];
	 sprintf(str, "%lf \n",(lEnd.QuadPart - lBegin.QuadPart)*dbTick);
	 ::OutputDebugStr(str);
}

CGame::tagClientInfo* CGame::GetGameServer(const char *strIP, DWORD dwGSNum)
{
	map<DWORD, tagClientInfo>::iterator itor = s_mapGSInfo.begin();
	for(; itor != s_mapGSInfo.end(); ++itor)
	{
		if( strcmp(strIP, itor->second.strIP.c_str()) == 0 
			&& dwGSNum == itor->second.dwIndex )
		{
			return &(itor->second);
		}
	}

	return NULL;
}

const char* CGame::GetGSIPByID(DWORD dwIndex)
{
	map<DWORD, tagClientInfo>::iterator itor = s_mapGSInfo.find(dwIndex);
	if(itor != s_mapGSInfo.end())
	{		
		return (*itor).second.strIP.c_str();
	}
	return NULL;
}

void CGame::OnGSLost(DWORD dwIndex)
{
	const char* strGSIP = GetGame()->GetGSIPByID(dwIndex);
	if(strGSIP)
	{
		AddLogText("GameServer [%s][%d] lost!", strGSIP, dwIndex);
	}
	GetGame()->DelGameServer(dwIndex);
}

////////////////////////////////////////////////////////////////////
////                  About Log									////
////////////////////////////////////////////////////////////////////
//void CGame::InitLogClient(CMessage *pMsg)
//{
//	LogClientInfo tmpLogClientInfo;
//	tmpLogClientInfo.ErrLogFileName = GetSetup()->strErrFile;
//	tmpLogClientInfo.LogFileName	= GetSetup()->strLogFile;
//	tmpLogClientInfo.TxtLogFileName = GetSetup()->strTxtLogFile;
//	tmpLogClientInfo.lClientNo		= GetSetup()->dwClientNo;
//
//	bool re = m_LogClient.Init(pMsg, tmpLogClientInfo);
//	if(re)
//	{
//		if (NULL != m_pLogicLogInterface)
//		{
//			SAFE_DELETE(m_pLogicLogInterface);
//		}
//		m_pLogicLogInterface = new LogicLogInterface(&m_LogClient);
//	}
//}
//
//void CGame::LogServerAffirm(CMessage *pMsg)
//{
//	if(0 == pMsg->GetLong())
//		m_LogClient.PassAffirm(false);
//	else
//		m_LogClient.PassAffirm(true);
//}
//
//void CGame::LogServerCondition(CMessage *pMsg)
//{
//	m_LogClient.UpdateCondition(pMsg);
//}
//
//LogicLogInterface*	CGame::GetLogicLogInterface()
//{
//	assert(NULL != m_pLogicLogInterface); 
//	return m_pLogicLogInterface;
//}

// 游戏释放
BOOL CGame::Release()
{
	PutDebugString("Server Exiting...");

#ifdef _RUNSTACKINFO_
	CMessage::AsyCloseFile(GetGame()->GetStackFileName());
#endif

	//GetInst(CMemClient).Release();

	// 等待重连服务器的线程结束
	bConnectServerThreadExit = true;
	WaitForSingleObject(hConnectServerThread, INFINITE);
	CloseHandle(hConnectServerThread);

	// 关闭网络连接
	if( GetNetServer() )
	{
		s_pNetServer->Exit();
		SAFE_DELETE(s_pNetServer);
	}

	SAFE_DELETE(m_pRsBouns);	
	SAFE_DELETE(m_pBounsDBManager);
	CMyAdoBase::Uninitalize();

	CMySocket::MySocketClearUp();
	CMessage::Release();

	SAFE_DELETE(m_pDBAllocator);
	
	//// 日志系统
	//SAFE_DELETE(m_pLogicLogInterface);
	//m_LogClient.Release();

	CClientResource* pClientResource = CClientResource::GetInstance();
	if(pClientResource)
	{
		pClientResource->Release();
		CClientResource::ReleaseInstance();
	}

	return TRUE;
}

// 游戏主循环
BOOL CGame::MainLoop()
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyClearFile(GetGame()->GetStackFileName());
	CMessage::AsyWriteFile(GetGame()->GetStackFileName(), "MainLoop() Start.");
#endif

	ProcessMessage();

	/*static DWORD now = timeGetTime(), lastRefreshTime = now;
	now = timeGetTime();
	static long lLastDealMsgNum = 0;
	if( now - lastRefreshTime > GetSetup()->dwRefeashInfoTime )
	{
		lastRefreshTime = now;
		 extern void RefeashInfoText();
		 RefeashInfoText();		 
	}*/

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStackFileName(), "MainLoop() End.");
#endif
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
	long lMessageCount = GetNetServer()->GetRecvMessages()->GetSize();
	for(long i=0; i<lMessageCount; i++)
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

//void OnLogServerMessage(CMessage *pMsg)
//{
//	switch(pMsg->GetType())
//	{
//	case MSG_LOG20_SERVER_TABLE://表格结构信息
//		{
//			GetGame()->InitLogClient(pMsg);
//		}
//		break;
//	case MSG_LOG20_SERVER_AFFIRM://确认服务器在LogServer上有配置的数据库
//		{
//			GetGame()->LogServerAffirm(pMsg);
//		}
//		break;
//	case MSG_LOG20_SERVER_CONDITION://消息的发送控制信息
//		{
//			GetGame()->LogServerCondition(pMsg);
//		}
//		break;
//	}
//}

// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
#ifndef _DEBUG
	_try
	{
#endif
		CreateGame();
		BOOL bRet = GetGame()->Init();
		
		if( bRet )
		{
			while( true )
			{
				if( g_bGameThreadExit )
				{
					break;
				}

				if( !GetGame()->MainLoop() )
				{
					break;
				}

				Sleep(1);
			}
		}

		if( GetGame() )
		{
			GetGame()->Release();
		}
		DeleteGame();

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		PutDebugString("程序出现错误，请查看最新的错误报告文件");
		AddErrorLogText("程序出现错误，请查看最新的错误报告文件");
		
		if( GetGame() )
		{
			GetGame()->Release();
		}
		DeleteGame();

		Sleep(5000);

		WinExec("Server.exe", SW_SHOW);
		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
		SetEvent(g_hGameThreadExitEvent);
		_endthreadex(0);
		return 0;
	}	
#endif

	SetEvent(g_hGameThreadExitEvent);
	PostMessage(g_hWnd, WM_CLOSE, 0, 0);
	_endthreadex(0);
	return 0;

}


//unsigned __stdcall ConnectLogServerFunc(void* pArguments)
//{
//	while(true)
//	{
//		if(GetGame()->GetLogClient().bConnectThreadExit)
//			break;
//		Sleep(8000);
//
//		if(GetGame()->ReConnectLogServer()==TRUE)
//		{
//			break;
//		}
//
//	}
//	_endthreadex( 0 );
//	return 0;
//}