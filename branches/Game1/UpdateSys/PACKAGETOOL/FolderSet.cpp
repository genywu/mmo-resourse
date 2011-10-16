// FolderSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PackageTool.h"
#include "FolderSet.h"
#include ".\folderset.h"


// CFolderSet 对话框

IMPLEMENT_DYNAMIC(CFolderSet, CDialog)
CFolderSet::CFolderSet(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderSet::IDD, pParent)
{
	m_strTitle		= "文件配置";

	m_PackageSets.clear();
	m_CompresSets.clear();

	m_nLastPackType = m_nPackType		= 1;
	m_nLastCompressType = m_nCompressType	= 1;

	m_bForceModifyPackType				= false;
	m_bForceModifyCompressType			= false;
}

CFolderSet::CFolderSet(string& strTitle,PackTypeInfos& PackInfo,CompTypeInfos& CompInfo,
						int nPackType,int nCompressType,CWnd* pParent)
	: CDialog(CFolderSet::IDD, pParent)
{
	m_bForceModifyPackType				= false;
	m_bForceModifyCompressType			= false;

	SetParam(strTitle,PackInfo,CompInfo,nPackType,nCompressType);
}

CFolderSet::~CFolderSet()
{
}

void CFolderSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void	CFolderSet::SetParam(string& strTitle,PackTypeInfos& PackInfo,CompTypeInfos& CompInfo,
							int nPackType,int nCompressType)
{
	m_strTitle		= strTitle;
	m_PackageSets	= PackInfo;
	m_CompresSets	= CompInfo;
	m_nLastPackType = m_nPackType		= nPackType;
	m_nLastCompressType = m_nCompressType	= nCompressType;
}

bool	CFolderSet::Initial()
{
	CComboBox* pComboPack = (CComboBox*)GetDlgItem(IDC_COMBO_PACKAGE_TYPE);	
	if( pComboPack == NULL)	return false;
	pComboPack->Clear();
	itTypeInfo it = m_PackageSets.begin();
	for(;it != m_PackageSets.end();it++)
	{
		pComboPack->AddString( (*it).strFileName.c_str());
	}
	UpdatePackSet();
	CButton* pForcePack = (CButton*)GetDlgItem(IDC_CHECK_FORCE_MODIFY_PACKTYPE);
	if(pForcePack)
		pForcePack->SetCheck(m_bForceModifyPackType);

	CComboBox* pComboCompress= (CComboBox*)GetDlgItem(IDC_COMBO_COMPRESS_TYPE);
	if(pComboCompress == NULL)	return false;
	pComboCompress->Clear();
	it = m_CompresSets.begin();
	for(;it != m_CompresSets.end();it++)
	{
		pComboCompress->AddString( (*it).strFileName.c_str());
	}
	UpdateCompressSet();
	CButton* pForceCompress = (CButton*)GetDlgItem(IDC_CHECK_FORCE_MODIFY_COMPRESSTYPE);
	if(pForceCompress)
		pForceCompress->SetCheck(m_bForceModifyCompressType);
	return true;
}

void CFolderSet::UpdatePackSet()
{
	CButton* pPack = (CButton*)GetDlgItem(IDC_RADIO_PACK);
	CButton* pUnPack = (CButton*)GetDlgItem(IDC_RADIO_UNPACK);
	CComboBox* pComboPack = (CComboBox*)GetDlgItem(IDC_COMBO_PACKAGE_TYPE);	
	if(pPack == NULL || pUnPack == NULL || pComboPack == NULL)	return;

	CButton* pCompress = (CButton*)GetDlgItem(IDC_RADIO_COMPRESS);
	CButton* pUnCompress = (CButton*)GetDlgItem(IDC_RADIO_UNCOMPRESS);
	CComboBox* pComboCompress= (CComboBox*)GetDlgItem(IDC_COMBO_COMPRESS_TYPE);
	if(pCompress == NULL || pUnCompress == NULL || pComboCompress == NULL)	return;

	if( (m_nPackType & 0x1) == 1)
	{
		pPack->SetCheck(BST_UNCHECKED);
		pUnPack->SetCheck(BST_CHECKED);
		pComboPack->EnableWindow(false);
		pComboPack->SetCurSel(-1);
		//设置为不压缩
		pCompress->SetCheck(BST_UNCHECKED);
		pCompress->EnableWindow(false);
		pUnCompress->SetCheck(BST_CHECKED);
		pComboCompress->EnableWindow(false);
	}
	else
	{
		pPack->SetCheck(BST_CHECKED);
		pUnPack->SetCheck(BST_UNCHECKED);
		pComboPack->EnableWindow(true);
		int nPos = GetComcoPackPosByID(m_nPackType);
		pComboPack->SetCurSel(nPos);

		//设置为不压缩
		pCompress->EnableWindow(true);
		pComboCompress->EnableWindow(true);
	}
}

void CFolderSet::UpdateCompressSet()
{
	CButton* pCompress = (CButton*)GetDlgItem(IDC_RADIO_COMPRESS);
	CButton* pUnCompress = (CButton*)GetDlgItem(IDC_RADIO_UNCOMPRESS);
	CComboBox* pComboCompress= (CComboBox*)GetDlgItem(IDC_COMBO_COMPRESS_TYPE);
	if(pCompress == NULL || pUnCompress == NULL || pComboCompress == NULL)	return;
	
	if( (m_nCompressType & 0x1) == 1)
	{
		pCompress->SetCheck(BST_UNCHECKED);
		pUnCompress->SetCheck(BST_CHECKED);
		pComboCompress->EnableWindow(false);
		pComboCompress->SetCurSel(-1);
	}
	else
	{
		pCompress->SetCheck(BST_CHECKED);
		pUnCompress->SetCheck(BST_UNCHECKED);
		pComboCompress->EnableWindow(true);
		int nPos = GetComcoCompPosByID(m_nCompressType);
		pComboCompress->SetCurSel(nPos);
	}
}

int		CFolderSet::GetComcoPackPosByID(int nPackID)
{
	int nPos = 0;
	itTypeInfo it = m_PackageSets.begin();
	for(;it != m_PackageSets.end();it++,nPos++)
	{
		if( (*it).dwID == nPackID )	return nPos;
	}
	return 0;
}

int		CFolderSet::GetPackIDByPos(int nPos)
{
	int n = 0;
	itTypeInfo it = m_PackageSets.begin();
	for(;it != m_PackageSets.end();it++,n++)
	{
		if( n == nPos )	return (*it).dwID;
	}
	return n;
}
int		CFolderSet::GetPackIDByName(string strName)
{
	itTypeInfo it = m_PackageSets.begin();
	for(;it != m_PackageSets.end();it++)
	{
		if( (*it).strFileName == strName )	return (*it).dwID;
	}
	return 0;
}


int		CFolderSet::GetComcoCompPosByID(int nCompID)
{
	int nPos = 0;
	itTypeInfo it = m_CompresSets.begin();
	for(;it != m_CompresSets.end();it++,nPos++)
	{
		if( (*it).dwID == nCompID )	return nPos;
	}
	return 0;
}
int		CFolderSet::GetCompIDByPos(int nPos)
{
	int n = 0;
	itTypeInfo it = m_CompresSets.begin();
	for(;it != m_CompresSets.end();it++,n++)
	{
		if( n == nPos )	return (*it).dwID;
	}
	return n;
}
int		CFolderSet::GetCompIDByName(string strName)
{
	itTypeInfo it = m_CompresSets.begin();
	for(;it != m_CompresSets.end();it++)
	{
		if( (*it).strFileName == strName )	return (*it).dwID;
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CFolderSet, CDialog)
	ON_BN_CLICKED(IDC_RADIO_PACK, OnBnClickedRadioPack)
	ON_BN_CLICKED(IDC_RADIO_UNPACK, OnBnClickedRadioUnpack)
	ON_BN_CLICKED(IDC_RADIO_COMPRESS, OnBnClickedRadioCompress)
	ON_BN_CLICKED(IDC_RADIO_UNCOMPRESS, OnBnClickedRadioUncompress)
	ON_CBN_SELCHANGE(IDC_COMBO_PACKAGE_TYPE, OnCbnSelchangeComboPackageType)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPRESS_TYPE, OnCbnSelchangeComboCompressType)
	ON_BN_CLICKED(IDC_CHECK_FORCE_MODIFY_PACKTYPE, OnBnClickedCheckForceModifyPacktype)
	ON_BN_CLICKED(IDC_CHECK_FORCE_MODIFY_COMPRESSTYPE, OnBnClickedCheckForceModifyCompresstype)
END_MESSAGE_MAP()


// CFolderSet 消息处理程序

BOOL CFolderSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	Initial();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFolderSet::OnBnClickedRadioPack()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_nLastPackType > 1)
		m_nPackType = m_nLastPackType;
	else
		m_nPackType = 2;
	UpdatePackSet();
}

void CFolderSet::OnBnClickedRadioUnpack()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_nPackType > 1)
		m_nLastPackType = m_nPackType;
	m_nPackType		= 1;
	UpdatePackSet();
	m_nCompressType	= 1;

	UpdateCompressSet();
}

void CFolderSet::OnBnClickedRadioCompress()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_nLastCompressType > 1)
		m_nCompressType = m_nLastCompressType;
	else
		m_nCompressType = 2;
	UpdateCompressSet();
}

void CFolderSet::OnBnClickedRadioUncompress()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_nCompressType > 1)
		m_nLastCompressType = m_nCompressType;
	m_nCompressType		= 1;
	UpdateCompressSet();
}

void CFolderSet::OnCbnSelchangeComboPackageType()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComboPack = (CComboBox*)GetDlgItem(IDC_COMBO_PACKAGE_TYPE);	
	if( pComboPack == NULL)	return;

	int nPos = pComboPack->GetCurSel();
	int nPackID = GetPackIDByPos(nPos);

	m_nLastPackType = m_nPackType = nPackID;
}

void CFolderSet::OnCbnSelchangeComboCompressType()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComboCompress= (CComboBox*)GetDlgItem(IDC_COMBO_COMPRESS_TYPE);
	if( pComboCompress == NULL)	return;

	int nPos = pComboCompress->GetCurSel();
	int nCompID = GetCompIDByPos(nPos);

	m_nLastCompressType = m_nCompressType = nCompID;
}

void CFolderSet::OnBnClickedCheckForceModifyPacktype()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pForcePack = (CButton*)GetDlgItem(IDC_CHECK_FORCE_MODIFY_PACKTYPE);
	if(pForcePack)
		m_bForceModifyPackType = pForcePack->GetCheck();
}

void CFolderSet::OnBnClickedCheckForceModifyCompresstype()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pForceCompress = (CButton*)GetDlgItem(IDC_CHECK_FORCE_MODIFY_COMPRESSTYPE);
	if(pForceCompress)
		m_bForceModifyCompressType = pForceCompress->GetCheck();
}
