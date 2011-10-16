#if !defined(AFX_TREETOOLTIPCTRL_H__0B1FAB30_E01F_47D6_918B_703E1A0C0951__INCLUDED_)
#define AFX_TREETOOLTIPCTRL_H__0B1FAB30_E01F_47D6_918B_703E1A0C0951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TreeToolTipCtrl.h : header file
/*
	Code written by Gary Andrew Lewis
	Please include these comments in your code if you wish to 
	use this control.  Thank you!
	
	Any questions then email: realm_123@hotmail.com 
*/

/////////////////////////////////////////////////////////////////////////////
// CTreeToolTipCtrl window

class CTreeToolTipCtrl : public CToolTipCtrl
{
// Construction
public:
	CTreeToolTipCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeToolTipCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void PassPoint(CPoint point);	
	virtual ~CTreeToolTipCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeToolTipCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREETOOLTIPCTRL_H__0B1FAB30_E01F_47D6_918B_703E1A0C0951__INCLUDED_)
