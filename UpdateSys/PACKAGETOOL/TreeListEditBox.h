#if !defined(AFX_TREELISTEDITBOX_H__20B75CB3_D653_11D4_895C_00062999F1C5__INCLUDED_)
#define AFX_TREELISTEDITBOX_H__20B75CB3_D653_11D4_895C_00062999F1C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TreeListEditBox.h : header file
/*
	Code written by Gary Andrew Lewis
	Please include these comments in your code if you wish to 
	use this control.  Thank you!
	
	Any questions then email: realm_123@hotmail.com 
*/

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditBox window

// TreeListEditBox.h : header file
//
class CTreeListEditBox : public CEdit
{
// Construction
public:
	CTreeListEditBox(HTREEITEM hItem, int iSubItem, CString sInitText);

// Attributes
public:
	BOOL bTab;
	BOOL bEscape;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListEditBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:	
	HTREEITEM GetItem();
	virtual ~CTreeListEditBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListEditBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:	
	HTREEITEM m_hItem;
	int m_iSubItem;
	CString m_sInitText;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTEDITBOX_H__20B75CB3_D653_11D4_895C_00062999F1C5__INCLUDED_)
