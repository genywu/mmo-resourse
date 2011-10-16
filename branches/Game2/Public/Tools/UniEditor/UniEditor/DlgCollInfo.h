#pragma once
#include "DlgEdit.h"
#include "Collection.h"
// CDlgCollInfo 对话框
const CString strFileCollection = _T("data\\CollectionList.xml");	// 怪物列表文件
class CDlgCollInfo : public CDlgEdit
{
	DECLARE_DYNAMIC(CDlgCollInfo)

public:
	CDlgCollInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCollInfo();
	void SetDlg(CRegion *pRegion, CCollectionInfo *pCollectionInfo, BOOL bALL = FALSE);		// 显示采集物详细信息
// 对话框数据
	enum { IDD = IDD_COLLECTIONINFO };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL PreTranslateMessage(MSG* pMsg);
	CCollectionInfo		*m_pCollInfo;					// 当前对应Collection指针
	CRegion			*m_pRegion;							// 当前对应Region指针
	BOOL			m_bDlgEdit;
	BOOL			m_bError;
	CComboBox		*m_pCollectionList;
	BOOL m_Auto;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCollList();
	afx_msg void OnEnChangeCollProbability();
	afx_msg void OnCbnEditchangeCollList();
};
