#if !defined(AFX_SITEDLG_H__2B428B07_4BF0_49F4_8858_0730E340B866__INCLUDED_)
#define AFX_SITEDLG_H__2B428B07_4BF0_49F4_8858_0730E340B866__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SiteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSiteDlg dialog

class CSiteDlg : public CDialog
{
// Construction
public:
	int nStart;
	CStringArray *site_list;
	CSiteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSiteDlg)
	enum { IDD = IDD_DIALOG2 };
	CListCtrl	m_SiteList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSiteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSiteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedSitelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SITEDLG_H__2B428B07_4BF0_49F4_8858_0730E340B866__INCLUDED_)
