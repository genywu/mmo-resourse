// Goddess2Dlg.h : header file
//

#if !defined(AFX_GODDESS2DLG_H__4BF4F0DB_653C_40CA_BD3D_FA0C094D8C45__INCLUDED_)
#define AFX_GODDESS2DLG_H__4BF4F0DB_653C_40CA_BD3D_FA0C094D8C45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGoddess2Dlg dialog

class CGoddess2Dlg : public CDialog
{
// Construction
public:
	CGoddess2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGoddess2Dlg)
	enum { IDD = IDD_GODDESS2_DIALOG };
	CEdit	m_OpenPortCtl;
	CButton	m_StartDBService;
	CListBox	m_ClientList;
	UINT	m_OpenPort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoddess2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGoddess2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	afx_msg void OnClose();
    afx_msg LRESULT OnAddClient(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDelClient(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonStartDbService();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GODDESS2DLG_H__4BF4F0DB_653C_40CA_BD3D_FA0C094D8C45__INCLUDED_)
