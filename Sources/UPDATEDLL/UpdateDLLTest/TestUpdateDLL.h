// TestUpdateDLL.h : main header file for the TESTUPDATEDLL application
//

#if !defined(AFX_TESTUPDATEDLL_H__BD1A276B_E09C_48F8_96B9_C8F72E9C25CF__INCLUDED_)
#define AFX_TESTUPDATEDLL_H__BD1A276B_E09C_48F8_96B9_C8F72E9C25CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestUpdateDLLApp:
// See TestUpdateDLL.cpp for the implementation of this class
//

class CTestUpdateDLLApp : public CWinApp
{
public:
	CTestUpdateDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestUpdateDLLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestUpdateDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTUPDATEDLL_H__BD1A276B_E09C_48F8_96B9_C8F72E9C25CF__INCLUDED_)
