// TestUpdateDLLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUpdateDLL.h"
#include "TestUpdateDLLDlg.h"
#include "UpdateExport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CTestUpdateDLLDlg dialog

CTestUpdateDLLDlg::CTestUpdateDLLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestUpdateDLLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestUpdateDLLDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestUpdateDLLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestUpdateDLLDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestUpdateDLLDlg, CDialog)
	//{{AFX_MSG_MAP(CTestUpdateDLLDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestUpdateDLLDlg message handlers

BOOL CTestUpdateDLLDlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestUpdateDLLDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestUpdateDLLDlg::OnPaint() 
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
HCURSOR CTestUpdateDLLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

static int TestDLL()
{
    typedef int (__stdcall *UPDATEA_INIT)(KUPDATE_SETTING UpdateSetting);
    typedef int (__stdcall *UPDATE_UNINIT)();
    typedef int (__stdcall *UPDATE_START)();
    typedef int (__stdcall *UPDATE_VERIFY)();
    typedef int (__stdcall *UPDATE_CHECKNEEDUPDATE)();
    typedef int (__stdcall *UPDATE_DOWNLOAD)();
    typedef int (__stdcall *UPDATE_UPDATE)();

    UPDATEA_INIT Update_Init;
    UPDATE_UNINIT Update_UnInit;
    UPDATE_START Update_Start;

    HMODULE hModule;
    KUPDATE_SETTING UpdateSet;
    int nRetCode = 0;
    CString sText;

    UpdateSet.bAutoTryNextHost = false;
    UpdateSet.bUseVerify = false;
    UpdateSet.bUseFastestHost = false;
    UpdateSet.pfnCallBackProc = NULL;
    UpdateSet.ProxySetting.bUpdateAuth = true; 
    UpdateSet.ProxySetting.nHostPort = 0;
    UpdateSet.ProxySetting.nProxyMethod = PROXY_METHOD_DIRECT;
    strcpy(UpdateSet.szVerifyInfo, "102400-010999-106075-054738");
    UpdateSet.ProxySetting.szHostAddr[0] = '\0';
    UpdateSet.ProxySetting.szPassword[0] = '\0';
    UpdateSet.ProxySetting.szUserName[0] = '\0';

	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath, '\\');
    ASSERT(pszOffset);
    pszOffset[1] = '\0';
    strcpy(UpdateSet.szDownloadPath, szModulePath);
	strcat(UpdateSet.szDownloadPath, "UpdateDownload\\");
    strcpy(UpdateSet.szUpdatePath, szModulePath);

	UpdateSet.nUpdateMode = 0;
    strcpy(UpdateSet.szUpdateSite, "ftp://linuxhome/incoming/UpdateDownLoad/");

	CString strVersion = szModulePath;
	UpdateSet.nVersion = ::GetPrivateProfileInt(
		"Version",
		"Version",
		0,
		strVersion + "Version.cfg");

    hModule = LoadLibrary("UpdateDLL.dll");
    if (!hModule)	
        goto Exit0;

    Update_Init = (UPDATEA_INIT)GetProcAddress(hModule, "Update_Init");
    if (!Update_Init)
        goto Exit0;

    Update_UnInit = (UPDATE_UNINIT)GetProcAddress(hModule, "Update_UnInit");
    if (!Update_UnInit)
        goto Exit0;

    Update_Start = (UPDATE_START)GetProcAddress(hModule, "Update_Start");
    if (!Update_Start)
        goto Exit0;

    nRetCode = Update_Init(UpdateSet);
    if (nRetCode == defUPDATE_RESULT_INIT_FAILED)
        goto Exit0;

    nRetCode = Update_Start();

    Update_UnInit();

Exit0:
    if (hModule)
        FreeLibrary(hModule);
    
    
    sText.Format("%d", nRetCode);
    MessageBox(NULL, sText, "Test", MB_OK);

    return 0;
}

ULONG WINAPI TestThread(LPVOID /*lpVoid*/)
{
    return TestDLL();
}

void CTestUpdateDLLDlg::OnButton1() 
{
    DWORD dwThreadID = 0;
    HANDLE hThread;
    
    hThread = CreateThread(NULL, 0, TestThread, 0, 0, &dwThreadID);
    if (hThread)
        CloseHandle(hThread);
}
