#pragma once
#include "afxwin.h"


// CCheckInCase 对话框

class CCheckInCase : public CDialog
{
	DECLARE_DYNAMIC(CCheckInCase)

public:
	CCheckInCase(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckInCase();

// 对话框数据
	enum { IDD = IDD_DIALOG_CheckInCase };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCasePath;
	afx_msg void OnBnClickedCheck();
	CEdit m_edCasePath;
	CString m_strClassName;
};
