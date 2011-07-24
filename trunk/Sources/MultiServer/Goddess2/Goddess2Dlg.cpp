// Goddess2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Goddess2.h"
#include "Goddess2Dlg.h"
#include "winsock2.h"
#include <string>
#include "tstring.h"
#include "IniFile.h"
#include "Utils.h"

#include "src/IDBRoleServer.h"
#include "src/KSG_RoleProcess.h"

using namespace std;
using namespace OnlineGameLib::Win32;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CreateDatabaseEngine(HWND hwnd);
void DestroyDatabaseEngine();

void LoadSetting();
void SaveSetting();

static size_t g_nMaxRoleCount = 3;

static unsigned short g_uServerPort = 5001;

static size_t g_BackupSleepTime = 1;  //备份线程自动挂起时间
static bool g_IsBackupSuspend = false;  //备份线程是否手工挂起


int g_nKSG_RoleProcess_InitFlag = false;


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoddess2Dlg dialog

CGoddess2Dlg::CGoddess2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoddess2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoddess2Dlg)
	m_OpenPort = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GIDDESS2);
}

void CGoddess2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoddess2Dlg)
	DDX_Control(pDX, IDC_EDIT_OPEN_PORT, m_OpenPortCtl);
	DDX_Control(pDX, IDC_BUTTON_START_DB_SERVICE, m_StartDBService);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_ClientList);
	DDX_Text(pDX, IDC_EDIT_OPEN_PORT, m_OpenPort);
	DDV_MinMaxUInt(pDX, m_OpenPort, 0, 65535);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGoddess2Dlg, CDialog)
	//{{AFX_MSG_MAP(CGoddess2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_WM_CLOSE()
    ON_MESSAGE(WM_ADD_CLIENT, OnAddClient) 
    ON_MESSAGE(WM_DEL_CLIENT, OnDelClient) 
	ON_BN_CLICKED(IDC_BUTTON_START_DB_SERVICE, OnButtonStartDbService)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoddess2Dlg message handlers

BOOL CGoddess2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

    LoadSetting();

	m_OpenPort = g_uServerPort;
    UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGoddess2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGoddess2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGoddess2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGoddess2Dlg::OnOk() 
{
    SendMessage(WM_CLOSE, 0, 0);
}

void CGoddess2Dlg::OnClose() 
{
    UpdateData(TRUE);

	// TODO: Add your message handler code here and/or call default
    int nRetCode = ::MessageBox(
        GetSafeHwnd(), 
        "Are you sure quit?", 
        "Info", 
        MB_YESNO | MB_ICONQUESTION
    ); 
    if (IDYES == nRetCode)
    {
        if (g_nKSG_RoleProcess_InitFlag)
        {
            KSG_RoleProcess_UnInit();
            g_nKSG_RoleProcess_InitFlag = false;
        }

        DestroyDatabaseEngine();

        SaveSetting();

    	CDialog::OnClose();
    }
}

void CGoddess2Dlg::OnButtonStartDbService() 
{
    int nResult = false;

    int nRetCode = false;
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);

    nRetCode = CreateDatabaseEngine(m_hWnd);
    if (!nRetCode)
        goto Exit0;

    nRetCode = KSG_RoleProcess_Init(m_OpenPort, m_hWnd);  // will call in OnStartDBService
    if (!nRetCode)
    {
		::MessageBox(m_hWnd, "Role Process Init Failed!", "Error", MB_OK | MB_ICONERROR);

        goto Exit0;
    }
    g_nKSG_RoleProcess_InitFlag = true;

    nRetCode = KSG_RoleProcess_Start();
    if (!nRetCode)
    {   
		::MessageBox(m_hWnd, "Role Process Start Fail!", "Error", MB_OK | MB_ICONERROR);

        goto Exit0;
    }

    ::EnableWindow(m_StartDBService.m_hWnd, false);
    ::EnableWindow(m_OpenPortCtl.m_hWnd, false);


    SetWindowText("Goddess2 [Enable]");

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (g_nKSG_RoleProcess_InitFlag)
        {
            KSG_RoleProcess_UnInit();
            g_nKSG_RoleProcess_InitFlag = false;
        }

    }
    
}


LRESULT CGoddess2Dlg::OnAddClient(WPARAM wParam, LPARAM lParam)
{
    int addr_from_len = wParam;
    struct sockaddr_in *paddr_from = (struct sockaddr_in *)lParam;

    char szClientInfo[256];

    char *pszRetAddress = NULL;
    char szAddress[256];

    if (!addr_from_len)
        return 0;

    if (!paddr_from)
        return 0;

    pszRetAddress = inet_ntoa(paddr_from->sin_addr);
    if (!pszRetAddress)
        return 0;

    strncpy(szAddress, pszRetAddress, 256);
    szAddress[256 - 1] = '\0';

    sprintf(szClientInfo, "%s:%u", szAddress, (unsigned)ntohs(paddr_from->sin_port));
    
    m_ClientList.AddString(szClientInfo);
    
    return 0;
}

LRESULT CGoddess2Dlg::OnDelClient(WPARAM wParam, LPARAM lParam)
{
    int addr_from_len = wParam;
    struct sockaddr_in *paddr_from = (struct sockaddr_in *)lParam;

    char szClientInfo[256];

    char *pszRetAddress = NULL;
    char szAddress[256];

    if (!addr_from_len)
        return 0;

    if (!paddr_from)
        return 0;

    pszRetAddress = inet_ntoa(paddr_from->sin_addr);
    if (!pszRetAddress)
        return 0;

    strncpy(szAddress, pszRetAddress, 256);
    szAddress[256 - 1] = '\0';

    sprintf(szClientInfo, "%s:%u", szAddress, (unsigned)ntohs(paddr_from->sin_port));

    int nRetIdx = m_ClientList.FindStringExact(-1, szClientInfo);
    if (nRetIdx == LB_ERR)
       return 0;
    
    m_ClientList.DeleteString(nRetIdx);
    
    return 0;
}

bool CreateDatabaseEngine( HWND hwnd )
{
	if ( !InitDBInterface( g_nMaxRoleCount ) )
	{
		::MessageBox( NULL, "Setup dbserver is failed!", "Warning", MB_OK | MB_ICONSTOP );
		
		return false;
	}
	
//	StartBackupTimer(g_BackupSleepTime); //开始运行备份线程
//	::SetWindowText(GetDlgItem( hwnd, IDC_LAB_BACKUP_STATUS ),
//								"Backup thread status: Running");
//	/*
//	 * Open this server to client
//	 */
//	pfnCreateServerInterface pFactroyFun = ( pfnCreateServerInterface )( g_theHeavenLibrary.GetProcAddress( "CreateInterface" ) );
//
//	IServerFactory *pServerFactory = NULL;
//
//	if ( pFactroyFun && SUCCEEDED( pFactroyFun( IID_IServerFactory, reinterpret_cast< void ** >( &pServerFactory ) ) ) )
//	{
//		pServerFactory->SetEnvironment( 10, 10, 20, 1024 * 1024 );
//		
//		pServerFactory->CreateServerInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &g_pServer ) );
//		
//		pServerFactory->Release();
//	}
//
//	if ( !g_pServer )
//	{
//		::MessageBox( NULL, "Initialization failed! Don't find a correct heaven.dll", "Warning", MB_OK | MB_ICONSTOP );
//
//		return false;
//	}
//
//	g_pServer->Startup();
//
//	g_pServer->RegisterMsgFilter( ( void * )hwnd, ServerEventNotify );
//
//	if ( FAILED( g_pServer->OpenService( INADDR_ANY, g_nServerPort ) ) )
//	{
//		return false;
//	}
//
//	DWORD dwThreadID = 0L;
//
//	IServer *pCloneServer = NULL;
//	g_pServer->QueryInterface( IID_IIOCPServer, ( void ** )&pCloneServer );
//
//	g_hThread = ::CreateThread( NULL, 0, ThreadProcess, ( void * )pCloneServer, 0, &dwThreadID );
//
//	if ( !g_hThread )
//	{
//		return false;
//	}
//
//	CClientNode::Start( g_pServer );
//
//	g_hStartWorkEvent.Set();
	
	return true;
}

void DestroyDatabaseEngine()
{
//	CClientNode::End();
//
//	g_hQuitEvent.Set();
//	
//	DWORD result = ::WaitForSingleObject( g_hThread, 50000 );
//
//	if ( result == WAIT_TIMEOUT )
//	{
//		::TerminateThread( g_hThread, ( DWORD )( -1 ) );
//	}
//
//	SAFE_CLOSEHANDLE( g_hThread );
//
//	if ( g_pServer )
//	{
//		g_pServer->CloseService();
//		g_pServer = NULL;
//	}
//
//	SAFE_RELEASE( g_pServer );

	ReleaseDBInterface();
}



void LoadSetting()
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Goddess.cfg";

	theConfigFile.SetFile( sConfigFile.c_str() );

	g_uServerPort = theConfigFile.ReadInteger( "Setting", "Port", 5001 );

	g_nMaxRoleCount = theConfigFile.ReadInteger( "Setting", "MaxRoleCount", 3 );

	g_BackupSleepTime = theConfigFile.ReadInteger( "Setting", "BackupSleepTime", 3 );
}

void SaveSetting()
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Goddess.cfg";

	theConfigFile.SetFile( sConfigFile.c_str() );

	theConfigFile.WriteInteger( "Setting", "Port", g_uServerPort );

	theConfigFile.WriteInteger(  "Setting", "MaxRoleCount", g_nMaxRoleCount );

	theConfigFile.WriteInteger(  "Setting", "BackupSleepTime", g_BackupSleepTime );
}
