#pragma once
#include "SignPos.h"
#include "DlgEdit.h"
// CDlgSignPos 对话框

class CDlgSignPos : public  CDlgEdit
{
	DECLARE_DYNAMIC(CDlgSignPos)

public:
	CDlgSignPos(CWnd* pParent = NULL);   // 标准构造函数
	void SetDlg(CRegion *pRegion, CSignPos *pSignPos, BOOL bALL = FALSE);
	virtual ~CDlgSignPos();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_SIGNPOS };

protected:
	CSignPos		*m_pSignPos;
	CRegion			*m_pRegion;							// 当前对应Region指针
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeSignposNo();
	afx_msg void OnEnChangeSignposX();
	afx_msg void OnEnChangeSignposY();
};
