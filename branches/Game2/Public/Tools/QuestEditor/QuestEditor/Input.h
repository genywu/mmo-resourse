#pragma once
// CInput 对话框
class CInput : public CDialog
{
	DECLARE_DYNAMIC(CInput)
public:
	CInput(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInput();
// 对话框数据
	enum { IDD = IDD_INPUT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	//确定控件的响应
	afx_msg void OnBnClickedButton1();
	//取消空间的响应
	afx_msg void OnBnClickedButton4();
	//对话框中输入的新建的任务ID
	int m_nInPutRoleID;
    afx_msg void OnBnClickedOk();
};
