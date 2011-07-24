// Goddess2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Goddess2.h"
#include "Goddess2Dlg.h"
#include "src/FilterTextLib.h"
#include "src/RoleNameFilter.h"
#include "src/KSG_RoleProcess.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFilterTextLib g_libFilterText;
CRoleNameFilter g_fltRoleName;


/////////////////////////////////////////////////////////////////////////////
// CGoddess2App

BEGIN_MESSAGE_MAP(CGoddess2App, CWinApp)
	//{{AFX_MSG_MAP(CGoddess2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoddess2App construction

CGoddess2App::CGoddess2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGoddess2App object

CGoddess2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CGoddess2App initialization

BOOL CGoddess2App::InitInstance()
{
    BOOL bResult = false;

    // Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
    int nRetCode = false;
    int nKSG_RoleProcess_InitFlag = false;
    int nlibFilterTextInitFlag = false;
    int nfltRoleNameInitFlag = false;

    //role name filter
    nRetCode = g_libFilterText.Initialize();
    if (!nRetCode)
    {
		MessageBox(NULL, "text filter's initing has failed", "error", MB_OK | MB_ICONERROR);
		goto Exit0;
    }

    nlibFilterTextInitFlag = true;

    nRetCode = g_fltRoleName.Initialize();
    if (!nRetCode)
    {
		MessageBox(NULL, "text filter's initing has failed", "error", MB_OK | MB_ICONERROR);
		goto Exit0;
    }

    nfltRoleNameInitFlag = true;

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	{
        CGoddess2Dlg dlg;
	    m_pMainWnd = &dlg;
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
    }

    bResult = TRUE;

Exit0:
    if (nfltRoleNameInitFlag)
    {
	    g_fltRoleName.Uninitialize();
        nfltRoleNameInitFlag = false;
    }

    if (nlibFilterTextInitFlag)
    {
    	g_libFilterText.Uninitialize();
        nlibFilterTextInitFlag = false;

    }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;  // cannot use bResult
}

