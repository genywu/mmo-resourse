#pragma once
#include "../../PUBLIC/FilesInfo.h"

// CVersionDialog 对话框

class CVersionDialog : public CDialog
{
	DECLARE_DYNAMIC(CVersionDialog)

public:
	CVersionDialog(CWnd* pParent = NULL);   // 标准构造函数
	CVersionDialog(PackTypeInfos& PackInfo,bool bSelPack,CWnd* pParent = NULL);   // 构造函数
	virtual ~CVersionDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strOldVersion;
	CString m_strNewVersion;

	int		m_nPackType;					//补丁包号

private:
	PackTypeInfos	m_PackageSets;
	bool			m_bSelPack;
	int		GetPackIDByPos(int nPos);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPatchPack();
};
