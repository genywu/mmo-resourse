// Autoupdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Autoupdate.h"
#include "AutoupdateDlg.h"

#include "../Engine/Src/KWin32.h"
#include "../Engine/Src/KPakList.h"
#include "../Engine/Src/KFilePath.h"
#include "../Engine/Src/KIniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KPakList g_PakList;

const int nCanUpdateVersionNumber = 20;
/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp

BEGIN_MESSAGE_MAP(CAutoupdateApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoupdateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp construction

CAutoupdateApp::CAutoupdateApp()

{
	m_hMutex = NULL;
}

CAutoupdateApp::~CAutoupdateApp()
{
	if (m_hMutex)
    {
		CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}

BOOL CAutoupdateApp::IsTheSelfRun()
{
	int max_retry = 8;
	while(1)
	{
		m_hMutex = CreateMutex(NULL,FALSE, "Autoupdate");
		if(m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_hMutex);
            m_hMutex = NULL;
			if(--max_retry == 0) {
				return TRUE;
			}
			Sleep(1000);
		}
		else 
        {
			break;
		}
	}
	return FALSE;
}

BOOL CAutoupdateApp::IsTheGameRun()
{
	HANDLE hGMutex = CreateMutex(NULL, FALSE, m_strMutex);
	if (hGMutex)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hGMutex);
			return TRUE;
		}
	}
	CloseHandle(hGMutex);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoupdateApp object

CAutoupdateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp initialization
BOOL CAutoupdateApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));

	
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath, '\\');
    ASSERT(pszOffset);
    pszOffset[1] = '\0';
	g_SetRootPath(szModulePath);




	
	g_PakList.Open("\\config.ini");
	KIniFile	IniFile;
	if (!IniFile.Load("\\settings\\autoupdate.ini"))
		return FALSE;

	char szMutex[MAX_PATH];
	IniFile.GetString("main", "mutex", "Worldeditor", szMutex, MAX_PATH);
	m_strMutex = szMutex;
	if (m_strMutex.IsEmpty())
		return FALSE;

	if (IsTheSelfRun())
	{
		AfxMessageBox("自动升级程序正在运行中，请稍候 ！");
		return FALSE;
	}
	if (IsTheGameRun())
	{
		AfxMessageBox("游戏程序正在运行中，不能进行自动升级，请关闭游戏后再试 ！");
		return FALSE;
	}

	CAutoupdateDlg dlg;
	BOOL b = dlg.InitAutoUpdate();
	g_PakList.Close();
	if (!b)
	{
//		AfxMessageBox("自动升级程序配置错误，不能进行自动升级 ！");
		return FALSE;
	}
	m_pMainWnd = &dlg;
//	AfxMessageBox("开始显示界面");
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CAutoupdateApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
