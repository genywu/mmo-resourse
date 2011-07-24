#if !defined(AFX_MSGWND_H__0A898AD8_AD2D_4D2F_A5DC_30D6DEF75553__INCLUDED_)
#define AFX_MSGWND_H__0A898AD8_AD2D_4D2F_A5DC_30D6DEF75553__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgWnd window

class CMsgWnd : public CWnd
{
// Construction
public:
	CMsgWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMsgWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgWnd)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
    
    LRESULT OnDefaultMessage(WPARAM wParam, LPARAM lParam);
    
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGWND_H__0A898AD8_AD2D_4D2F_A5DC_30D6DEF75553__INCLUDED_)
