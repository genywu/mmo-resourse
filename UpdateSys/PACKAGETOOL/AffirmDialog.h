#pragma once
#include "Resource.h"

// CAffirmDialog 对话框

class CAffirmDialog : public CDialog
{
	DECLARE_DYNCREATE(CAffirmDialog)

public:
	CAffirmDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAffirmDialog();
// 重写
	//HRESULT OnButtonOK(IHTMLElement *pElement);
	//HRESULT OnButtonCancel(IHTMLElement *pElement);

// 对话框数据
	enum { IDD = IDD_DIALOG_AFFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strText;
	BOOL m_bAlwaysCheck;
	int  m_iResult;
//	afx_msg void OnBnClickedOk();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedNo();
};
