#pragma once


// CAddClassDlg 对话框

class CAddClassDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddClassDlg)

public:
	CAddClassDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddClassDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_AddClass };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strClassName;
	CString m_strOneCaseName;
};
