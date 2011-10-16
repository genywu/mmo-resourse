#include "Monster.h"
#include "DlgEdit.h"

#pragma once

const CString strFileMonster = _T("data\\monsterlist.ini");	// 怪物列表文件

// CDlgMoninfo 对话框

class CDlgMoninfo : public CDlgEdit
{
	DECLARE_DYNAMIC(CDlgMoninfo)

public:
	CDlgMoninfo(CWnd* pParent = NULL);				// 标准构造函数
	virtual ~CDlgMoninfo();
	void SetDlg(CRegion *pRegion, CMonsterInfo *pmonsterInfo, BOOL bALL = FALSE);		// 显示MONSTER具体信息
	void Save(CMonsterInfo *pmonsterInfo);			// 保存MONSTER具体信息

// 对话框数据
	enum { IDD = IDD_MONSTERINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CMonsterInfo	*m_pMonsterInfo;
	CComboBox		*m_pMonsterList;
	BOOL m_Auto;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeMonsterDistance();
public:
	afx_msg void OnEnChangeMonsterIdentifier();
public:
	afx_msg void OnEnChangeMonsterLeader();
public:
	afx_msg void OnEnChangeMonsterProbability();
public:
	afx_msg void OnEnChangeMonsterScript();
public:
	afx_msg void OnCbnSelchangeMonsterList();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnEditchangeMonsterList();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnEnChangeMonsterPathid();
	afx_msg void OnEnChangeMonsterTalkscript();
	afx_msg void OnBnClickedTalkbrowse();
};
