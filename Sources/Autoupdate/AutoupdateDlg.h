// AutoupdateDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES

#if !defined(AFX_AUTOUPDATEDLG_H__8ECBCF19_56D6_4317_855A_CF114C5A5647__INCLUDED_)
#define AFX_AUTOUPDATEDLG_H__8ECBCF19_56D6_4317_855A_CF114C5A5647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateDlg dialog
#include "afxhtml.h"
#include "afxmt.h"
#include<string>
#include "AutoUpdateRes.h"
#include "GameOptionPanel.h"
#include "AnnuncePanel.h"
#include "Hyperlinkstatic.h"

using namespace std;

#define ConnectionProgress 50

void LoadBitMapRes(HBITMAP& BitMap, UINT BitMapRes,HINSTANCE AppHandle);

class CAutoupdateDlg : public CDialog
{
	// Construction
public:
	CAutoupdateDlg(CWnd* pParent = NULL);	// standard constructor
	~CAutoupdateDlg();
	
	BOOL InitAutoUpdate();
	void UpdateFromCurrentHost();
	int m_nCurrentHost;
	
	// Dialog Data
	//{{AFX_DATA(CAutoupdateDlg)
	enum { IDD = IDD_AUTOUPDATE_DIALOG };
	CButton	m_RetryButton;
	CHyperlinkStatic	m_RegUserCtl;
	CHyperlinkStatic	m_SinaCtl;
	CHyperlinkStatic	m_17173Ctl;
	CHyperlinkStatic m_HomeCtl;
	CHyperlinkStatic m_JoyoCtl;
	CHyperlinkStatic m_CardCtl;
	CButton	m_GameOption;
	CProgressCtrl	m_progressCurrent;
	CProgressCtrl	m_progress;
	CStatic	m_DisplayProgressCtl;
	CStatic	m_DisplayCurrentCtl;
	CStatic	m_DisplayVersionCtl;
	CButton	m_RegisterCtl;
	CButton	m_InvestCtl;
	CButton	m_CancelButton;
	CButton 	m_EnterGameButton;
	CString	m_strVersion;
	CString	m_strProgress;
	CString	m_strProgressCurrent;
	CWebBrowser2	m_web;
	int m_nConnectionStep;
	BOOL m_bEnableRun;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoupdateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	HMODULE m_ResModule;
	HICON m_hIcon;
//	void OnRefresh(LPSTATUSINFO pInfo);
	
	CString m_strUpdateSelf;
	CString m_strWebURL;
	CString m_strRun;
	CString m_strSiteList;
	CStringArray m_strHosts;
		
	// Generated message map functions
	//{{AFX_MSG(CAutoupdateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnRegister();
	afx_msg void OnInvest();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnConfig();
	afx_msg void OnNavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Frame, VARIANT FAR* StatusCode, BOOL FAR* Cancel);
	afx_msg void OnRetry();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CBitmap m_bmTitle;      // 标题背景
	CBitmap m_bmInfoBack;   // 信息提示背景
    

private:
	void LoadBitMapFromInstance(CBitmap& Dest,HINSTANCE Hinstance,WORD  wResourceID);
	CPoint m_MouseClickPosition;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATEDLG_H__8ECBCF19_56D6_4317_855A_CF114C5A5647__INCLUDED_)
