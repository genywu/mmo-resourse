#if !defined(AFX_TREELISTCOMBOBOX_H__8F578D00_44FA_4B2D_A6C8_FF8C058D2C85__INCLUDED_)
#define AFX_TREELISTCOMBOBOX_H__8F578D00_44FA_4B2D_A6C8_FF8C058D2C85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TreeListComboBox.h : header file
/*
	Code written by Gary Andrew Lewis
	Please include these comments in your code if you wish to 
	use this control.  Thank you!
	
	Any questions then email: realm_123@hotmail.com 
*/

/////////////////////////////////////////////////////////////////////////////
// CTreeListComboBox window

class CTreeListComboBox : public CComboBox
{
// Construction
public:
	CTreeListComboBox(HTREEITEM hItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL bTab;
	HTREEITEM GetItem();
	virtual ~CTreeListComboBox();

private:
	HTREEITEM m_hItem;
	int m_iSubItem;
	CString m_sInitText;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCOMBOBOX_H__8F578D00_44FA_4B2D_A6C8_FF8C058D2C85__INCLUDED_)
