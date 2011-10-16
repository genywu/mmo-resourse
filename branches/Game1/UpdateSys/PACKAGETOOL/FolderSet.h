#pragma once
#include "../../PUBLIC/FilesInfo.h"

// CFolderSet 对话框

class CFolderSet : public CDialog
{
	DECLARE_DYNAMIC(CFolderSet)

public:
	CFolderSet(CWnd* pParent = NULL);   // 标准构造函数
	CFolderSet(string& strTitle,PackTypeInfos& PackInfo,CompTypeInfos& CompInfo,
						int nPackType,int nCompressType,CWnd* pParent = NULL);
	virtual ~CFolderSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_FILESETS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int		m_nLastPackType;		//暂存变量
	int		m_nLastCompressType;

public:
	string	m_strTitle;

	int		m_nPackType;			//打包号
	int		m_nCompressType;		//压缩类型

	bool	m_bForceModifyPackType;
	bool	m_bForceModifyCompressType;

	PackTypeInfos	m_PackageSets;
	CompTypeInfos	m_CompresSets;

public:
	void	SetParam(string& strTitle,PackTypeInfos& PackInfo,CompTypeInfos& CompInfo,
						int nPackType,int nCompressType);

	bool	Initial();
	void	UpdatePackSet();
	void	UpdateCompressSet();
	int		GetComcoPackPosByID(int nPackID);
	int		GetPackIDByPos(int nPos);
	int		GetPackIDByName(string strName);

	int		GetComcoCompPosByID(int nCompID);
	int		GetCompIDByPos(int nPos);
	int		GetCompIDByName(string strName);


	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRadioPack();
	afx_msg void OnBnClickedRadioUnpack();
	afx_msg void OnBnClickedRadioCompress();
	afx_msg void OnBnClickedRadioUncompress();
	afx_msg void OnCbnSelchangeComboPackageType();
	afx_msg void OnCbnSelchangeComboCompressType();
	afx_msg void OnBnClickedCheckForceModifyPacktype();
	afx_msg void OnBnClickedCheckForceModifyCompresstype();
};
