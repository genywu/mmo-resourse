// UpdateDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UpdateDLL.h"	   
#include "UpdateExport.h"


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CUpdateDLLApp

BEGIN_MESSAGE_MAP(CUpdateDLLApp, CWinApp)
	//{{AFX_MSG_MAP(CUpdateDLLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDLLApp construction

CUpdateDLLApp::CUpdateDLLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_nInitFlag = false;
	m_nProcessStep = DOWNINDEX_STEPINDEX;
	m_nResultCode = defUPDATE_RESULT_INIT_FAILED;
	m_pCurDownItem = NULL;
	m_nCurEnableResume = true;	
	m_nInitWSA = false;
    m_nInitCheckThread = 0;
    
    ZeroMemory(&m_KavUserInfo, sizeof(m_KavUserInfo));

}

CUpdateDLLApp::~CUpdateDLLApp()
{
    if (m_wndMessage.GetSafeHwnd() != NULL) {
		try {
			DestroyWindow(m_wndMessage.GetSafeHwnd());
		}
		catch(...) {
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUpdateDLLApp object

CUpdateDLLApp g_theApp;

BOOL CUpdateDLLApp::PreTranslateMessage(MSG* pMsg) 
{
	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CUpdateDLLApp::InitInstance() 
{
    // Create the main window. 
    
	return CWinApp::InitInstance();
}

int CUpdateDLLApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
    
	return CWinApp::ExitInstance();
}

BOOL CUpdateDLLApp::DownDispatch(CONST MSG *pMsg)
{
    BOOL bRetCode;
    ULONG ulResult = 0;
    
    bRetCode =  CDownNotify::IsNotifyMessage(pMsg, &ulResult);
    if (bRetCode)
        return (BOOL)ulResult;
    
    return FALSE;
}
