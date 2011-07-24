#if !defined(AFX_CQIMAGEBUTTON_H__F4E8EDAB_3561_4ED4_9E7B_710306B9EB21__INCLUDED_)
#define AFX_CQIMAGEBUTTON_H__F4E8EDAB_3561_4ED4_9E7B_710306B9EB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CQImageButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCQImageButton window
#include<string>
using namespace std;
void DisplayErrorInfo(string& ErrorInfo);
void LoadBitMapRes(HBITMAP& BitMap, UINT BitMapRes,HINSTANCE AppHandle);


class CCQImageButton : public CButton

{
// Construction
public:
	CCQImageButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCQImageButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetButtonBmp(UINT csPushBmpID, UINT csNormalBmpID,UINT csFocusBmpID,HINSTANCE AppHandle);
	//CString m_csFocusBmpID;
	virtual ~CCQImageButton();
private:

	HBITMAP m_csDisableBmpID;
	HBITMAP m_csNormalBmpID;
	HBITMAP m_csFocusBmpID;

	// Generated message map functions
protected:
	void DrawItem(LPDRAWITEMSTRUCT);
	//{{AFX_MSG(CCQImageButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CQIMAGEBUTTON_H__F4E8EDAB_3561_4ED4_9E7B_710306B9EB21__INCLUDED_)
