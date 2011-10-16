#pragma once
#include "afxwin.h"


// CQueryFrame 对话框

class CQueryFrame : public CDialog
{
	DECLARE_DYNAMIC(CQueryFrame)

public:
	CQueryFrame(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQueryFrame();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    long    m_Sel;
    CString m_questname;
    afx_msg void OnBnClickedButton1();
    CComboBox m_ComBox;
};
