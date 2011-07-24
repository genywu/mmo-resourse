#if !defined(AFX_BITMAPPROGRESS_H__363C4046_995B_473C_808F_8BF85561CEA6__INCLUDED_)
#define AFX_BITMAPPROGRESS_H__363C4046_995B_473C_808F_8BF85561CEA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitMapProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BitMapProgress window
#include<string>
using namespace std;
void DisplayErrorInfo(string& ErrorInfo);
void LoadBitMapRes(HBITMAP& BitMap, UINT BitMapRes,HINSTANCE AppHandle);
class BitMapProgress : public CProgressCtrl
{
// Construction
public:
	BitMapProgress();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BitMapProgress)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBackColor(COLORREF BackColor);
	virtual ~BitMapProgress();
    void SetForegroundBmp(UINT ForegroundBmpID); 
	// Generated message map functions
protected:
	//{{AFX_MSG(BitMapProgress)
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int Mode;
	COLORREF m_BackColor;

	CBitmap m_cbForeground;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPPROGRESS_H__363C4046_995B_473C_808F_8BF85561CEA6__INCLUDED_)
