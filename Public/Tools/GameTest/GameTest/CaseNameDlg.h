#pragma once


// CCaseNameDlg 对话框

class CCaseNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CCaseNameDlg)

public:
	CCaseNameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCaseNameDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_AddCaseName };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCaseName;
	CString m_strTitle;
	CString m_Name;
};
