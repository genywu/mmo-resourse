// Goddess2.h : main header file for the GODDESS2 application
//

#if !defined(AFX_GODDESS2_H__9B09CC04_F0A1_4C6D_AB89_58121844ECF1__INCLUDED_)
#define AFX_GODDESS2_H__9B09CC04_F0A1_4C6D_AB89_58121844ECF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGoddess2App:
// See Goddess2.cpp for the implementation of this class
//

class CGoddess2App : public CWinApp
{
public:
	CGoddess2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoddess2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGoddess2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GODDESS2_H__9B09CC04_F0A1_4C6D_AB89_58121844ECF1__INCLUDED_)
