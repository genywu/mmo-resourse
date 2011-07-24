// UpdateDLL.h : main header file for the UPDATEDLL DLL
//

#if !defined(AFX_UPDATEDLL_H__67522B6E_5B24_44E4_BCA6_F6357D153091__INCLUDED_)
#define AFX_UPDATEDLL_H__67522B6E_5B24_44E4_BCA6_F6357D153091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ProcessIndex.h"
#include "resource.h"		// main symbols
#include "DownNotify.h"
#include "DataDefine.h"
#include "UpdateExport.h"
#include "CheckThread.h"
#include "UserCheck.h"
#include "src\MsgWnd.h"	// Added by ClassView

#define DOWNINDEX_STEPINDEX                 1
#define DOWNINDEX_STEPOVER					2
#define DOWNINDEX_STEPCHKSNHTTP             3
#define DOWNINDEX_STEPCHKSNUDP              4



/////////////////////////////////////////////////////////////////////////////
// CUpdateDLLApp
// See UpdateDLL.cpp for the implementation of this class
//

class CUpdateDLLApp 
	: public CWinApp,
	  public CDownNotify,
      public IKCheckCallback
{
protected:
	CMsgWnd m_wndMessage;
	int m_nInitFlag;
	int m_nResultCode;

    int m_nInitCheckThread;
    bool m_bCheckUserOK;
    USER_INFO m_KavUserInfo;
    CCheckThread    m_CheckThread;
	int m_nProcessStep;
	CString m_strIndexTempFile;
	CDownloadFile m_DownloadFile;

    CString m_strCgiTempFile;
	KUPDATE_ITEM *m_pCurDownItem;
	int m_nCurEnableResume;
	int m_nInitWSA;
    
    HANDLE m_hExitNotify;
public:
    FN_UPDATE_CALLBACK *m_pfnCallBackProc; 

protected:
    int InitCheckThread();
    int UnInitCheckThread();

	int DownNextItem(int nRedownCurrent = false);
    
protected:
    virtual ULONG OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus);
    
    virtual ULONG OnDownResult(ULONG ulOverResult);

    virtual int GetCheckInfo(
        ULONG   ulContextWith,
        int     *pnSendSize,
        unsigned char **ppbySendInfo
    );
    virtual int CheckRecvInfo(
        ULONG   ulContextWith,
        int     nRecvSize,
        unsigned char  *pbyRecvInfo
    );
    virtual void CheckResult(
        ULONG   ulContextWith,
        int     nServerResult
    );

private:
    int ProcessNextStep(int nDownStep);
    int ProcessUserCheck(int nCheckOK);

public:
	int Init(CONST KUPDATE_SETTING& UpdateSetting);
    int UnInit();
    int UserVerify();
    int CheckNeedUpdate();  
    int Download();
    int Update();
	int CancelDownload();
    
    BOOL DownDispatch(CONST MSG *pMsg);
public:
    
    CUpdateDLLApp();
    ~CUpdateDLLApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDLLApp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CUpdateDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

    protected:
    
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLL_H__67522B6E_5B24_44E4_BCA6_F6357D153091__INCLUDED_)
