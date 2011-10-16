#include "Monster.h"
#include "DlgEdit.h"
#pragma once

// CDlgMonster 对话框
class CDlgMonster : public CDlgEdit
{
	DECLARE_DYNAMIC(CDlgMonster)

public:
	CDlgMonster(CWnd* pParent = NULL);						// 标准构造函数
	virtual ~CDlgMonster();
	void SetDlg(CRegion *pRegion, CMonster *pmonster, BOOL bALL = FALSE);		// 显示MONSTER批次信息
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 对话框数据
	enum { IDD = IDD_MONSTER };

public:
	afx_msg void OnEnChangeMonsterNo();
	afx_msg void OnEnChangeMonsterCoorLeft();
	afx_msg void OnEnChangeMonsterCoorTop();
	afx_msg void OnEnChangeMonsterCoorRight();
	afx_msg void OnEnChangeMonsterCoorBottom();
	afx_msg void OnEnChangeMonsterDirection();
	afx_msg void OnEnChangeMonsterQuantity();
	afx_msg void OnEnChangeMonsterTimeBorn();
	afx_msg void OnEnChangeMonsterTimeStart();
	afx_msg void OnBnClickedCheckBorn();
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CMonster		*m_pMonster;						// 当前对应Monster指针
	CRegion			*m_pRegion;							// 当前对应Region指针
	BOOL			m_bDlgEdit;
	BOOL			m_bError;
};
