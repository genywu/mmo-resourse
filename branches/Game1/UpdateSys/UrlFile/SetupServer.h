#pragma once
#include "afxwin.h"


// SetupServer 对话框

class SetupServer : public CDialog
{
	DECLARE_DYNAMIC(SetupServer)

public:
	SetupServer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SetupServer();
	afx_msg void OnBnClickedOk();
	void SetPage();
	virtual BOOL OnInitDialog();
	int GetCurSelect();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETUPSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_CCombo;
public:
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedCancel();
};
