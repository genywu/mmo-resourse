// Autoupdate.h : main header file for the AUTOUPDATE application
//

#if !defined(AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_)
#define AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateApp:
// See Autoupdate.cpp for the implementation of this class
//


class CAutoupdateApp : public CWinApp
{
public:
	CAutoupdateApp();
	~CAutoupdateApp();
	BOOL IsTheSelfRun();
	BOOL IsTheGameRun();
	HANDLE m_hMutex;
	CString m_strMutex;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoupdateApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	
// Implementation

	//{{AFX_MSG(CAutoupdateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CAutoupdateApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATE_H__D4E4CC3B_07EF_4279_A014_4B15DA3760FA__INCLUDED_)
