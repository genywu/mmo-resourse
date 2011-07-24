// TestUpdateDLLDlg.h : header file
//

#if !defined(AFX_TESTUPDATEDLLDLG_H__9F5C595D_3B93_410A_842F_5E7F2C44AB91__INCLUDED_)
#define AFX_TESTUPDATEDLLDLG_H__9F5C595D_3B93_410A_842F_5E7F2C44AB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestUpdateDLLDlg dialog

class CTestUpdateDLLDlg : public CDialog
{
// Construction
public:
	CTestUpdateDLLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestUpdateDLLDlg)
	enum { IDD = IDD_TESTUPDATEDLL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestUpdateDLLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestUpdateDLLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTUPDATEDLLDLG_H__9F5C595D_3B93_410A_842F_5E7F2C44AB91__INCLUDED_)
