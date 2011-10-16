/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：GameTest.cpp
*  功能：GametestMFC主界面框架
*  当前版本：1.2.0
*  作者： LGR
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：2009
*
*  1 .修改：增加托盘显示功能	 BY:LGR		 Date:2010.3.5   
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "GameTestDlg.h"
#include "Setup.h"
#include "WndMsgDef.h"
#include "../App/QuestSystem.h"
//#include "../../public/DataBlockAllocator.h"
#include "../App/GameClient.h"
#include "../Script/ScriptSys.h"
#include "../Net/Message.h"
#include "../GameTest/GameTestDlg.h"
#include "../App/CCDKey.h"
#include "../App/RegionList.h"
#include "..\..\..\Public\Module\CrashDumper.h"
#include "..\..\..\Public\ServerPublic\greta\syntax2.h"
#include "../Script/MapScriptSystem.h"
#include "TestReporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





class CAboutDlg : public CDialog
{
public:

	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CGameTestDlg::CGameTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGameTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bInitResult =FALSE;

	for(int i=0;i<2;i++)
	{
		m_WorkerExit[i] = CreateEvent(0,FALSE,FALSE,0);
		m_WorkerEvent[i] = CreateEvent(0,FALSE,FALSE,0);
	}

	m_pDBAllocator		  = NULL;
	//m_pFileMapDBAllocator = NULL;
	m_MultiGameClients.clear();
	m_pNetServer		  = NULL;
	m_pTimer		      = NULL;
	m_DriveClientManager = NULL;
	m_bTaskbarNotifier = false;
	m_bOnTbClicked = false;
	m_ltbWaitLoginCount = 0;
	//	m_pMapFile = new CMapFile();
}

//GT测试项目名控制标识
int  CGameTestDlg:: m_iProjName = PROJ_PANLONG;

void CGameTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TAB_PAGE,m_tabPage);
}

BEGIN_MESSAGE_MAP(CGameTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PAGE, &CGameTestDlg::OnTcnSelchangingTabPage)
	//托盘
	ON_MESSAGE(WM_TRAYICON, OnTrayIcon)
	ON_MESSAGE(WM_TASKBARNOTIFIERCLICKED,OnTaskbarNotifierClicked)

	ON_COMMAND(ID_TRAY_ALL,			&CGameTestDlg::OnShow)      
	ON_COMMAND(ID_TRAY_SHOW,		&CGameTestDlg::OnShowDLg)   
	ON_COMMAND(ID_VIEW_SHOWINFO,	&CGameTestDlg::OnShowInfo)
	ON_COMMAND(ID_VIEW_SHOWRESLUT,	&CGameTestDlg::OnShowTestReslut)
	ON_COMMAND(ID_TRAY_HIDE,		&CGameTestDlg::OnHideDlg)
	ON_COMMAND(ID_TRAY_EXIT,		&CGameTestDlg::OnExit)

END_MESSAGE_MAP()



BOOL CGameTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//创建一个log的保存目录
	CreateDirectory( "log",	NULL);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//托盘显示
	TrayNotifyMsg(NIM_ADD, IDR_MAINFRAME,"双击显示GT");
	m_TaskbarNotifier.Create(this);
	m_TaskbarNotifier.SetSkin(IDB_SKIN_MSN);
	m_TaskbarNotifier.SetTextFont("Arial",90,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
	m_TaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_TaskbarNotifier.SetTextRect(CRect(10,40,m_TaskbarNotifier.m_nSkinWidth-10,m_TaskbarNotifier.m_nSkinHeight-25));

	m_tabPage.InsertItem(0,"OperatePage");
	m_tabPage.InsertItem(1,"GameInfoPage");
	m_tabPage.InsertItem(2,"TestResultInfoPage");

	//m_tabPage.InsertItem(2,"驱动客户端");
	//m_tabPage.InsertItem(2,"分类测试");

	m_Info.Create(IDD_PROPPAGE_GAMEINFO,GetDlgItem(IDC_TAB_PAGE));
	m_Script.Create(IDD_PROPPAGE_TESTSCRIPT,GetDlgItem(IDC_TAB_PAGE));
	m_TsRlt.Create(IDD_DIALOG_TESTRESULT,GetDlgItem(IDC_TAB_PAGE));
	//m_DriveClientDlg.Create(IDD_DriveClient,GetDlgItem(IDC_TAB_PAGE));
	m_dlgAllTest.Create(IDD_DIALOG_AllTest,GetDlgItem(IDC_TAB_PAGE));

	CRect rs;
	m_tabPage.GetClientRect(rs);
	rs.top =+ 20;

	m_Script.MoveWindow(rs);
	m_Info.MoveWindow(rs);
	m_TsRlt.MoveWindow(rs);

	//m_DriveClientDlg.MoveWindow(rs);
	//m_dlgAllTest.MoveWindow(rs);

	m_tabPage.SetCurSel(0);
	m_Script.ShowWindow(TRUE);

	rs.top =- 10;
	m_dlgAllTest.MoveWindow(rs);

	//	::InitializeCriticalSection(&g_csMapFile);

	//读取配置文件
// 	CSetup::getInstance()->Load();
// 	CCDKey::InitCDKeyList("CDKey//CDKeyList.ini");
// 	InitTokenFile("setup//TokenSetup.ini");
// 
// 	char name[128];
// 	sprintf(name,"Data/goods/goods.dat");
// 	CGoodsList::LoadGoodsList(name);
// 
// 	::srand( time( NULL ) );
// 
 	if(!IniDBAllocator())
 	{
 		AfxMessageBox("初始化DBAllocator错误!",MB_OK);
 		return FALSE;
 	}
// 
 	if(!IniTimer())
 	{
 		AfxMessageBox("初始化Timer错误!",MB_OK);
 		return FALSE;
 	}
 
 	if(!IniGameClients())
 	{
 		AfxMessageBox("初始化Clients错误!",MB_OK);
 		return FALSE;
 	}
 	if(!IniMessage())
 	{
 		AfxMessageBox("初始化Message错误!",MB_OK);
 		return FALSE;
 	}
 	if(!IniNetServer())
 	{
 		AfxMessageBox("初始化Message错误!",MB_OK);
 		return FALSE;
 	}	
 	if( !IniScriptSys())
 	{
 		AfxMessageBox("初始化ScriptSys错误!",MB_OK);
 		return FALSE;
 	}

	if( !IniTestReporter())
	{
		AfxMessageBox("初始化TestReporter错误!",MB_OK);
		return FALSE;
	}

	//2009-8-3 By:G++
	//if( !IniQuestSys("quest/quest.xml"))
	//{
	//	AfxMessageBox("初始化QuestSystem错误",MB_OK);
	//	return FALSE;
	//}
	// 	if ( !IniMappingFileScriptSys())
	// 	{
	// 		AfxMessageBox("初始化MappingFileScriptSys错误!",MB_OK);
	// 		return FALSE;
	// 	}


	// added by KL
// 	char workPath[MAX_PATH];
// 	GetCurrentDirectory( sizeof(workPath)-1, workPath );
// 	CClientResource::InitialInstance();
// 	CClientResource* pClientResource = CClientResource::GetInstance();
// 	pClientResource->Initial(GAME_RES, std::string(workPath), std::string("PackRes.cfg") );
// 
// 	if( !CRegionList::LoadMap() )
// 	{
// 		AfxMessageBox("初始化LoadMap错误!",MB_OK);
// 		return FALSE;
// 	}

	//	if( !CStringReading::InitUniString("quest/c_globaltextres.xml","GlobalFile") )
	//	{
	//		AfxMessageBox("初始化任务描述错误",MB_OK);
	//		return FALSE;
	//	}

	// TODO:  在此添加额外的初始化
// 	char strOut[256] = "";										
// 	if( CScriptSys::getInstance()->LoadOneFileOrString("Script/RequireFile.lua") == FALSE )
// 	{
// 		sprintf(strOut,"加载脚本<%s>出错","/Script/RequireFile.lua");
// 		MessageBox(strOut,"警告",MB_OK);
// 	}
// 	if( CScriptSys::getInstance()->LoadOneFileOrString("Script/GlobalTableData.lua") == FALSE )
// 	{
// 		sprintf(strOut,"加载脚本<%s>出错","/Script/GlobalTableData.lua");
// 		MessageBox(strOut,"警告",MB_OK);
// 	}

	//   if( CMapScriptSystem::getInstance()->LoadOneScriptFile("Script/RequireFile.lua") == FALSE )
	//   {
	//  	sprintf(strOut,"加载脚本<%s>出错","/Script/RequireFile.lua");
	//  	MessageBox(strOut,"警告",MB_OK);
	//   }
	//  
	//   if( CMapScriptSystem::getInstance()->LoadOneScriptFile("Script/DriverGlobalTableData.lua") == FALSE )
	//   {
	//   	sprintf(strOut,"加载脚本<%s>出错","Script/DriverGlobalTableData.lua");
	//  	MessageBox(strOut,"警告",MB_OK);
	//   }
	// 	if( CMapScriptSystem::getInstance()->LoadOneScriptFile("Script/GlobalTableData.lua") == FALSE )
	// 	 {
	// 		sprintf(strOut,"加载脚本<%s>出错","/Script/GlobalTableData.lua");
	// 		MessageBox(strOut,"警告",MB_OK);
	// 	 }


	//--------------------------------------------------------------------------------------
	// MAPPING FILE
	//--------------------------------------------------------------------------------------
#ifdef __MAP__DRIVE__ 
	//初始化DiveClient
	m_DriveClientManager = new CDriveClient;
	if ( m_DriveClientManager == NULL )
	{
		m_DriveClientManager = new CDriveClient;
	}
	if ( m_DriveClientManager == NULL )
	{	
		::exit(0);
	}
	//初始化内存块
	//m_pFileMapDBAllocator = new CDataBlockAllocator(40960, 1024);
	m_DriveClientManager->_InitialFileMapping(m_pDBAllocator);	
	//m_DriveClientManager._InitialFileMapping(m_pDBAllocator);	
	//把DriveClient管理对象传给DiverClientDlg,进行管理操作
	m_DriveClientDlg.SetDriveClientManager(m_DriveClientManager);
	//把定时器传给DiverClientDlg
	m_DriveClientDlg.SetDriverClientDlgTimer(m_pTimer);
#endif
	//-----------------------------------

	//	创建线程
	m_ArrWorker[0] = AfxBeginThread(MessageWorkerFunc,this,0,0,0,NULL);
	m_ArrWorker[1] = AfxBeginThread(TimerWorkerFunc,this,0,0,0,NULL);

	m_bInitResult = TRUE;

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

/*
=========================================================================
函数功能:提供不同项目之间，读取资源函数接口,TestScriptDlg手动控件按钮调用   By: LGR
=========================================================================
*/
bool CGameTestDlg::LoadDifProjRes()
{
	//读取配置文件
	 	CSetup::getInstance()->Load();
	 	CCDKey::InitCDKeyList("CDKey//CDKeyList.ini");
	 	InitTokenFile("setup//TokenSetup.ini");
	 
	 	char name[128];
	 	sprintf(name,"Data/goods/goods.dat");
	 	CGoodsList::LoadGoodsList(name);


		// added by KL
		 	char workPath[MAX_PATH];
		 	GetCurrentDirectory( sizeof(workPath)-1, workPath );
		 	CClientResource::InitialInstance();
		 	CClientResource* pClientResource = CClientResource::GetInstance();
		 	pClientResource->Initial(GAME_RES, std::string(workPath), std::string("PackRes.cfg") );
		 
		 	if( !CRegionList::LoadMap() )
		 	{
		 		AfxMessageBox("初始化LoadMap错误!",MB_OK);
		 		return FALSE;
		 	}


	 
	 	char strOut[256] = "";										
	 	if( CScriptSys::getInstance()->LoadOneFileOrString("Script/RequireFile.lua") == FALSE )
	 	{
	 		sprintf(strOut,"加载脚本<%s>出错","/Script/RequireFile.lua");
	 		MessageBox(strOut,"警告",MB_OK);
	 	}
	 	if( CScriptSys::getInstance()->LoadOneFileOrString("Script/GlobalTableData.lua") == FALSE )
	 	{
	 		sprintf(strOut,"加载脚本<%s>出错","/Script/GlobalTableData.lua");
	 		MessageBox(strOut,"警告",MB_OK);
	 	}
	return true;
}


//初始化
bool CGameTestDlg::IniDBAllocator()
{
	m_pDBAllocator = new CDataBlockAllocator(1024,512);
	return true;
}


bool CGameTestDlg::IniTimer()
{
	m_pTimer = new Timer(4096,8);
	m_nMemMapTimerID = 3;
	m_nMemMapTimerID = SetTimer(m_nMemMapTimerID,1,NULL);				//MapFile timer

	SetTimer(1,1,NULL);
	return true;
}


bool CGameTestDlg::IniGameClients()
{
	m_MultiGameClients.clear();
	return true;
}


bool CGameTestDlg::IniScriptSys()
{
	CScriptSys * pScriptSys = CScriptSys::getInstance();
	if(!pScriptSys->Init(this))
		return false;
	return true;
}

bool CGameTestDlg::IniTestReporter()
{
	CTestReporter * pCTestReporter = CTestReporter::GetInstance();
	if(!pCTestReporter->Init(this))
		return false;
	return true;
}


// bool CGameTestDlg::IniMappingFileScriptSys()
// {
// 	CMapScriptSystem * pMapScriptSys = CMapScriptSystem::getInstance();
// 	if(!pMapScriptSys->Init(this, NULL))
// 		return false;
// 	return true;
// }


bool CGameTestDlg::IniMessage()
{
	CMessage::Initial(m_pDBAllocator, 512);
	//使者CMessage的MappingFile管理器
	return true;
}


bool CGameTestDlg::IniNetServer()
{
	CMySocket::MySocketInit();
	CMySocket::SetEncryptFlag(-1);

	m_pNetServer = new CServer();
	m_pNetServer->Start(
		0x1,				//	Mode
		m_pDBAllocator,		//	DBAlloc
		2048,				//	MaxFreeSockOperNum
		1024,				//	MaxFreeIOOperNum
		2,					//	IOOperDataBufNum
		false,				//	check
		1024,				// ForbidTime
		102400,1024,1024,8192,2048,20480,2048,20480,1048576,10240,10485760,10485760,10485760);
	//  BYTE bMode,CDataBlockAllocator* pDBAllocator,
	//	long nMaxFreeSockOperNum = 100,long nMaxFreeIOOperNum=100,long lIOOperDataBufNum = 2,
	//	bool bCheck=false,long lForbidTime=1000,
	//	long lMaxMsgLen=102400,long lMaxConNums=1000,long lTotalClientsNum=10,
	//	long lPendingWrBufNum=4096,long lPendingRdBufNum=2048,
	//	long lMaxSendSizePerSecond=8192,long lMaxRecvSizePerSecond =4096,
	//	long lMaxBlockSendMsgNum = 1024,
	//	long lConPendingWrBufNum=10240,long lConPendingRdBufNum=10240,
	//	long lConMaxSendSizePerSecond=81920,long lConMaxRecvSizePerSecond=81920,
	//	long lConMaxBlockSendMsgNum=1024);
	CMessage::SetServer(m_pNetServer);
	return true;
}


BOOL CGameTestDlg::IniQuestSys(const char* filename)
{
	if( CQuestContent::InitQuest(filename) )
		return TRUE;
	return FALSE;
}


//释放
bool CGameTestDlg::ReleaseDBAllocator()
{
	SAFE_DELETE(m_pDBAllocator);
	//SAFE_DELETE(m_pFileMapDBAllocator);

	return true;
}


bool CGameTestDlg::ReleaseTimer()
{
	//KillTimer(m_nGameTimerID);
	//m_nGameTimerID = 0;
	//KillTimer(m_nMessageTimerID);
	//m_nMessageTimerID = 0;
	//	KillTimer(m_nLuaGCTimerID);
	//	m_nLuaGCTimerID = 0;
	SAFE_DELETE(m_pTimer);
	return true;
}


bool CGameTestDlg::ReleaseGameClients()
{
	if (m_MultiGameClients.empty())	return true;
	itGC it = m_MultiGameClients.begin();
	for(;it!=m_MultiGameClients.end();it++)
	{
		SAFE_DELETE((*it).second);
	}
	m_MultiGameClients.clear();
	return true;
}


bool CGameTestDlg::ReleaseScriptSys()
{
	CScriptSys::Release();
	return true;
}

bool CGameTestDlg::ReleaseTestReporter()
{
	CTestReporter::Release();
	return true;
}

// bool CGameTestDlg::ReleaseMappingFileScriptSys()
// {
// 	CMapScriptSystem::Release();
// 	return true;
// }


bool CGameTestDlg::ReleaseNetServer()
{
	m_pNetServer->Exit();
	SAFE_DELETE(m_pNetServer);
	CMySocket::MySocketClearUp();
	return true;
}


bool CGameTestDlg::ReleaseMessage()
{
	CMessage::Release();
	return true;
}


void CGameTestDlg::CreateClient(long lSocketID)
{
	itGC it = m_MultiGameClients.find(lSocketID);
	if(it != m_MultiGameClients.end())
		return;
	CGameClient* pClient = new CGameClient(m_pTimer);
	
	m_MultiGameClients[lSocketID] = pClient;

	UpdataTreeGameClientAmount();
}


CGameClient* CGameTestDlg::GetGameClient(long lSocketID)
{
	itGC it = m_MultiGameClients.find(lSocketID);
	if(it != m_MultiGameClients.end() )
		return (*it).second;
	return NULL;
}


CGameClient* CGameTestDlg::GetOneGameClient()
{
	if ( !m_MultiGameClients.empty())
	{
		itGC it = m_MultiGameClients.begin();
		return (*it).second;
	}
	return NULL;
}


void CGameTestDlg::DeleteGameClient(long lSocketID)
{
	itGC it = m_MultiGameClients.find(lSocketID);
	if(it != m_MultiGameClients.end() )
	{
		CGameClient *pClient = (*it).second;
		SAFE_DELETE(pClient);
		m_MultiGameClients.erase(it);
		//
		UpdataTreeGameClientAmount();
	}
}


void CGameTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE)                        
	{
		SetTaskbarNotifierInfo("GT已最小化到托盘!");
		ShowWindow(FALSE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}

	// 	if ((nID & 0xFFF0) == SC_CLOSE)                        
	// 	{
	// 		ShowWindow(FALSE);
	// 	}
	// 	else
	// 	{
	// 		CDialog::OnSysCommand(nID, lParam);
	// 	}
}


void CGameTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) ;
		int y = (rect.Height() - cyIcon + 1);

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
		ReleaseDC(&dc);
	}
	else
	{
		CDialog::OnPaint();
	}
}



HCURSOR CGameTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGameTestDlg::OnCancel()
{
	//通知工作线程结束工作
	m_bTaskbarNotifier = false; 
	m_Info.KillTimer(1);
	//显示获取客户端数量终止
	HANDLE h = (HANDLE)_beginthreadex(NULL,0,ExitThread,this,0,NULL);
	CloseHandle(h);
	KillTimer(m_nMemMapTimerID);
	m_nMemMapTimerID = 0;
}

void CGameTestDlg::OnTimer(UINT nIDEvent)
{

#ifdef __MAP__DRIVE__ 

	if( nIDEvent == m_nMemMapTimerID)
	{
		OnMapTimer();
	}
#endif		

	if (m_bTaskbarNotifier)
	{
		if (nIDEvent == 1)
		{
			OnShowConnectInfo();
		}
	}
	CDialog::OnTimer(nIDEvent);
}


//Mapping File Message Loop
void CGameTestDlg::OnMapTimer()
{
	// 	//获取游戏客户端
	// 	CGameClient *pClient = m_DriveClientManager->_FindGameClient("GM1111");
	// 	if ( pClient )
	// 	{		
	// 
	// 		//char str[256]="";
	// 		//sprintf_s(str,"消息处理OnMapTimer:%d",timeGetTime(),256);
	// 		//PutDebugString(str);
	// 		//PutDebugString("----------------------------------------------------------------");
	// 
	// 		//EnterCriticalSection(&m_cs);
	// 
	// 		//获取消息队列
	// 		//msgQueue MsgQueue;
	// 		//设置消息队列
	// 		m_DriveClientManager->_GetRecvMsgs()->GetAllMessage(MsgQueueMapFile);
	// 
	// 		if ( MsgQueueMapFile.size()>0 )
	// 		{	
	// 			msgQueue::iterator it = MsgQueueMapFile.begin();
	// 			for ( ; it!=MsgQueueMapFile.end(); ++it)
	// 			{
	// 				CMessage *pMsg = (CMessage*)(*it);
	// 				if ( pMsg )
	// 				{	
	// 					pClient->OnMessage(pMsg);
	// 					CBaseMessage::FreeMsg(pMsg);
	// 				}
	// 			}
	// 			MsgQueueMapFile.clear();
	// 		}
	// 
	// 		if ( pClient->GetMainPlayer()->GetRegionID() > 0)
	// 		{	
	// 			CMapScriptSystem::getInstance()->PerformString("DriveOnTimer()");
	// 
	// 		}
	// 
	// 		//LeaveCriticalSection(&m_cs);
	// 	}
}


void CGameTestDlg::OnGameTimerOut()
{
	m_pTimer->Expire();
}


void CGameTestDlg::ResetDisplayInfoGameClient(long lSelClient)
{
	CGameClient *pClient = GetGameClient(lSelClient);
	m_Info.ResetSelGameClient(pClient);
}


void CGameTestDlg::OutputScriptInfo(const char* strInfo)
{
	m_Script.OutputScriptInfo(strInfo);
}


BOOL CGameTestDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
	case DISPLAY_INFO_RESET_GAME_CLIENT:
		{
			long lClientID = (long)wParam;
			long lCID = (long)lParam;
			if (lClientID)
				ResetDisplayInfoGameClient(lClientID);
		}
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CGameTestDlg::UpdataTreeGameClientAmount()
{
	if( !m_MultiGameClients.empty())
	{
		size_t lNum = m_MultiGameClients.size();
		char str[64];
		sprintf(str,"测试客户端( %d )个",(long)lNum);
		HTREEITEM hRoot = m_Info.GetTreeGameClient()->GetRootItem();
		m_Info.GetTreeGameClient()->SetItemText(hRoot,str);
	}
	else{
		HTREEITEM hRoot = m_Info.GetTreeGameClient()->GetRootItem();
		m_Info.GetTreeGameClient()->SetItemText(hRoot,"测试客户端( 0 )个");
	}
}


void CGameTestDlg::S2C_Login(CMessage* msg)
{
	long lType = msg->GetType();
	switch(lType)
	{
	case MSG_L2C_LOG_ANSWERCDKEY:
		break;
	case MSG_L2C_LOG_ANSWER_PLAYERBASELIST:
		break;
	case MSG_L2C_LOG_ANSWER_PLAYERDATA:
		break;
	case MSG_L2C_LOG_ANSWERCREATE:
		break;
	}
}


void CGameTestDlg::OnTcnSelchangingTabPage(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	int nCurSel = m_tabPage.GetCurSel();
	if( nCurSel == 0 )
	{
		m_Info.ShowWindow(SW_HIDE);
		m_Script.ShowWindow(SW_SHOW);
		m_TsRlt.ShowWindow(SW_HIDE);
		//m_DriveClientDlg.ShowWindow(FALSE);
		//m_dlgAllTest.ShowWindow(FALSE);
	}
	else if(nCurSel == 1)
	{
		m_Info.ShowWindow(SW_SHOW);
		m_Script.ShowWindow(SW_HIDE);
		m_TsRlt.ShowWindow(SW_HIDE);
		//m_DriveClientDlg.ShowWindow(FALSE);
		//m_dlgAllTest.ShowWindow(FALSE);
	}
	else if (nCurSel == 2)
	{
		m_Info.ShowWindow(SW_HIDE);
		m_Script.ShowWindow(SW_HIDE);
		m_TsRlt.ShowWindow(SW_SHOW);
	}
	//else if(nCurSel == 2)
	//{
	//	m_Info.ShowWindow(FALSE);
	//	m_Script.ShowWindow(FALSE);
	//	//m_DriveClientDlg.ShowWindow(TRUE);
	//	m_dlgAllTest.ShowWindow(FALSE);
	//}
	//else if( nCurSel == 2)
	//{
	//m_Info.ShowWindow(FALSE);
	//m_Script.ShowWindow(FALSE);
	//m_DriveClientDlg.ShowWindow(FALSE);
	//m_dlgAllTest.ShowWindow(TRUE);
	//}
	*pResult = 0;
}


UINT CGameTestDlg::ExitThread(LPVOID lParam)
{
	CGameTestDlg* pDlg = (CGameTestDlg*)lParam;
	if(pDlg)
	{
		pDlg->Exit();
	}
	return 4;
}


void CGameTestDlg::Exit()
{
	//通知线程结束
	SetEvent(m_WorkerExit[WORKER_MSG]);
	SetEvent(m_WorkerExit[WORKER_LUA]);
	//等待线程结束
	WaitForMultipleObjects(2,m_WorkerEvent,TRUE,INFINITE);

	for(int i=0;i<2;i++)
	{
		CloseHandle( m_WorkerEvent[i]);
		CloseHandle( m_WorkerExit[i]);
		//m_ArrWorker[i]->
	}

	//	m_pMessageWorker->Delete();
	//	m_pTimerWorker->Delete();

	//m_DriveClientManager->_ReleaseFileMapping();
	//m_DriveClientManager->_ReleaseGameClientManager();
	//SAFE_DELETE(m_DriveClientManager);

	m_Info.ReleaseTimer();

	//释放
	//ReleaseMappingFileScriptSys();	
	ReleaseScriptSys();
	ReleaseTestReporter();
	ReleaseNetServer();

	ReleaseTimer();

	ReleaseMessage();
	ReleaseGameClients();

	ReleaseDBAllocator();

	CSetup::Release();
	CCDKey::Release();
	CQuestContent::Release();
	ReleaseToken();

	itMapClientRegion it = m_mapClientRegion.begin();
	for(; it != m_mapClientRegion.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	m_mapClientRegion.clear();

	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Release();

	//释放资源管理器
	CClientResource::ReleaseInstance();

	CDialog::OnCancel();
}


//处理网络消息
UINT CGameTestDlg::MessageWorkerFunc(LPVOID lParam)
{
	__try
	{
		CGameTestDlg* pTestDlg = (CGameTestDlg*)lParam;
		if( pTestDlg )
		{
			pTestDlg->DoMessageWorker();
		}
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		throw("DoTimerWorker Error!!!");
	}
	return 1;
}


void CGameTestDlg::DoMessageWorker()
{
	static msgQueue RecvMsgQueue;
	CBaseMessage* pMsg = NULL;

	while( WaitForSingleObject(m_WorkerExit[WORKER_MSG],0) == WAIT_TIMEOUT)
	{
		m_pNetServer->GetRecvMessages()->GetAllMessage(RecvMsgQueue);
		deque<CBaseMessage*>::iterator it = RecvMsgQueue.begin();
		if( it != RecvMsgQueue.end())
		{
			for(;it != RecvMsgQueue.end();it++)
			{
				pMsg = (*it);
				CScriptSys::getInstance()->RunMessage((CMessage*)pMsg);
				CBaseMessage::FreeMsg(pMsg);
			}
			RecvMsgQueue.clear();
		}
		else
		{
			Sleep(1);
		}
	}
	SetEvent(m_WorkerEvent[WORKER_MSG]);
}


UINT CGameTestDlg::TimerWorkerFunc(LPVOID lParam)
{		
	__try
	{
		CGameTestDlg* pTestDlg = (CGameTestDlg*)lParam;
		if( pTestDlg )
		{
			pTestDlg->DoTimerWorker();
		}
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		throw("DoTimerWorker Error!!!");
	}
	return 1;
}


void CGameTestDlg::DoTimerWorker()
{
	while( WaitForSingleObject(m_WorkerExit[WORKER_LUA],1) == WAIT_TIMEOUT )
	{
		m_pTimer->Expire();
		//	Sleep(1);
	}
	SetEvent(m_WorkerEvent[WORKER_LUA]);
}


BOOL CGameTestDlg::InitTokenFile(const char* filename)
{
	std::ifstream pf;
	if( pf.fail() )
		return FALSE;

	pf.open(filename);
	string tokenfile("");
	pf>>tokenfile;
	pf.close();

	return InitPtSessionSetup(tokenfile.c_str());
}


BOOL CGameTestDlg::InitPtSessionSetup(const char* filename)
{
	string path = "setup//";
	path += filename;

	std::ifstream in_f;
	if( in_f.fail())
	{
		return FALSE;
	}

	in_f.open(path.c_str());
	if( !in_f.is_open() )
	{
		return FALSE;
	}

	string strValue,strCDKey,strSession;
	while(true)
	{
		if( in_f.eof())
			break;

		in_f>>strValue;
		if( strValue.empty() )
			break;

		int left = strValue.find('^');
		int right= strValue.rfind('^')+1;
		int len  = strValue.length();

		tagTokenCDKey * tokenKey = new tagTokenCDKey;
		tokenKey->bUsed = FALSE;
		tokenKey->strNumCDKey = strValue.substr(0,left);
		tokenKey->strToken = strValue.substr(right,len);

		m_listFreeTokenKey.push_back(tokenKey);
	}
	in_f.close();

	return TRUE;
}


CGameTestDlg::tagTokenCDKey* CGameTestDlg::AllocTokenCDKey()
{
	itListTokenCDKey it = m_listFreeTokenKey.begin();

	tagTokenCDKey* p = (*it);
	p->bUsed = TRUE;

	m_mapUsedTokenKey[ (*it)->strNumCDKey ] = *it;

	m_listFreeTokenKey.pop_front();
	return p;
}


void CGameTestDlg::FreeTokenCDKey(string const strNumKey)
{
	itMapTokenKey it = m_mapUsedTokenKey.find(strNumKey);
	if( it != m_mapUsedTokenKey.end() )
	{
		tagTokenCDKey* pToken = it->second;
		pToken->bUsed = FALSE;
		m_listFreeTokenKey.push_back( pToken );
		m_mapUsedTokenKey.erase(it);
	}
}


VOID CGameTestDlg::ReleaseToken()
{
	itListTokenCDKey itList = m_listFreeTokenKey.begin();
	for(; itList != m_listFreeTokenKey.end(); )
	{
		tagTokenCDKey* p = (*itList++);
		SAFE_DELETE(p);
		//m_listFreeTokenKey.erase(itList++);
	}
	m_listFreeTokenKey.clear();

	itMapTokenKey itMap = m_mapUsedTokenKey.end();
	for(; itMap != m_mapUsedTokenKey.end(); itMap++)
	{
		SAFE_DELETE(itMap->second);
	}
	m_mapUsedTokenKey.clear();
}


CClientRegion* CGameTestDlg::FindClientRegion(long lResourceID,const CGUID& guidRgn)
{
	itMapClientRegion it = m_mapClientRegion.find( guidRgn );
	if( it != m_mapClientRegion.end() )
	{
		return it->second;
	}

	CClientRegion* pRegion = new CClientRegion;
	pRegion->SetExID( guidRgn );
	pRegion->SetResourceID( lResourceID );
	pRegion->Load();

	m_mapClientRegion[ guidRgn ] = pRegion;

	return pRegion;
}


int CGameTestDlg::OperMessageBox(const char* pchar)
{
	if (pchar)
	{
		AfxMessageBox(pchar,MB_OK);
		return 1;
	}
	else
		return 0;
}


int CGameTestDlg::SetFlashWindows()
{
	FlashWindowEx(FLASHW_ALL,1,0);
	return 1;
}


BOOL CGameTestDlg::TrayNotifyMsg(DWORD Msg, UINT Icon, char* strTip)
{
	if (Icon == NULL)
	{
		Icon = IDR_MAINFRAME;
	}

	NOTIFYICONDATA data;

	data.cbSize           = sizeof(NOTIFYICONDATA);
	data.hIcon            = (HICON) LoadImage(AfxGetApp()->m_hInstance , MAKEINTRESOURCE(Icon), IMAGE_ICON, 16, 16, 0);
	data.hWnd             = this->GetSafeHwnd();
	data.uCallbackMessage = WM_TRAYICON;
	data.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	data.uID              = IDR_MAINFRAME;

	switch(Msg)
	{	
	case NIM_DELETE:
		{
			data.hIcon    = NULL;
			data.szTip[0] = '\0';
			break;
		}
	case NIM_ADD:
		{
			strcpy(data.szTip, strTip);
			break;
		}
	case NIM_MODIFY:
		{
			strcpy(data.szTip, strTip);
			break;
		}
	}
	return Shell_NotifyIcon(Msg, &data);
}


LRESULT CGameTestDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
		//双击鼠标左键
	case WM_LBUTTONDBLCLK:
		SetForegroundWindow();
		ShowWindow(1);
		break;  
		//单击鼠标右键
	case WM_RBUTTONDOWN:
		//设置光标位置
		CPoint point(0, 0);
		//得到光标位置
		GetCursorPos(&point);
		//该函数将创建指定窗口的线程设置到前台，并且激活该窗口
		SetForegroundWindow();
		CMenu menu; 
		// 装入菜单 
		menu.LoadMenu(IDR_MENU_TRAY);
		// 显示菜单 
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN| TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
		break;
	}
	return 0;
}


void CGameTestDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	PostMessage( WM_NCLBUTTONDOWN,HTCAPTION, MAKELPARAM( point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}


void CGameTestDlg::OnHideDlg()
{
	SetForegroundWindow();
	m_bTaskbarNotifier = false;
	ShowWindow(0);
}


void CGameTestDlg::OnShowDLg()
{
	ShowWindow(1);
}


void CGameTestDlg::OnShowTestReslut()
{

}

void CGameTestDlg::SetTaskbarNotifierInfo(const char* pchar)
{
	if (pchar)
	{
		m_TaskbarNotifier.Show(pchar);
	}
	else
		AfxMessageBox("无托盘消息");
}


void CGameTestDlg::OnShowConnectInfo()
{
	long   lClientCount = GetNetServer()->GetClientCount();
	long   lMultiGameClientsCount = m_MultiGameClients.size();
	long   lCount = GettbWaitLoginCount();
	char  pstr[256];
	sprintf(pstr,"准备登录数量: %d          进入LS数量: %d          进入GS数量: %d",lCount,lClientCount,lMultiGameClientsCount);
	SetTaskbarNotifierInfo(pstr);
}


void CGameTestDlg::OnExit()
{
	TrayNotifyMsg(NIM_DELETE,NULL,"0000");
	OnCancel();
}


LRESULT CGameTestDlg::OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam)
{
	if ( !m_bOnTbClicked )
	{
		ShowWindow(1);
		m_bOnTbClicked = true;
	}
	else
	{
		ShowWindow(0);
		m_bOnTbClicked = false;
	}
	return 0;
}