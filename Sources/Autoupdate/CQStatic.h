#if !defined(AFX_CQSTATIC_H__8673767F_0FF6_47D4_B606_3A41C786DC82__INCLUDED_)
#define AFX_CQSTATIC_H__8673767F_0FF6_47D4_B606_3A41C786DC82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CQStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCQStatic window
#include<string>
using namespace std;

void DisplayErrorInfo(string& ErrorInfo);
class CCQStatic : public CStatic
{
// Construction
public:
	CCQStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCQStatic)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBackGround(UINT BackImage,HINSTANCE AppHandle);
	virtual ~CCQStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCQStatic)
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	HBITMAP m_BackImage;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CQSTATIC_H__8673767F_0FF6_47D4_B606_3A41C786DC82__INCLUDED_)
