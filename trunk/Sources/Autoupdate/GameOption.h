#if !defined(AFX_GAMEOPTION_H__6B4C9CDB_7C82_4B78_9BD9_ACC6F9CF8141__INCLUDED_)
#define AFX_GAMEOPTION_H__6B4C9CDB_7C82_4B78_9BD9_ACC6F9CF8141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GameOption dialog

class GameOption : public CDialog
{
// Construction
public:
	GameOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GameOption)
	enum { IDD = IDD_DIALOGBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GameOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GameOption)
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEOPTION_H__6B4C9CDB_7C82_4B78_9BD9_ACC6F9CF8141__INCLUDED_)
