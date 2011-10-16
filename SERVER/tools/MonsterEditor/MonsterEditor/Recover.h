#pragma once


// CRecover 对话框

class CRecover : public CDialog
{
	DECLARE_DYNAMIC(CRecover)

public:
	CRecover(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRecover();

// 对话框数据
	enum { IDD = IDD_RECOVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenMonsterFile();//打开属性文件
	afx_msg void OnOpenMonsterSkillFile();//打开技能文件
	afx_msg void OnRecover();//恢复按钮，调用CMonsterEditorDlg的RecoverFile函数
};
