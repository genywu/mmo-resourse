#pragma once
#include "Region.h"
#include "RegionTree.h"

// CDlgEdit 对话框

class CDlgEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgEdit)

public:
	CDlgEdit(UINT nIDTemplate, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEdit();
	const inline void GetModifyNode(CString & str){str = m_szModifyNode;}
	virtual void SetDlg(CRegion *pRegion, void *pEdit, BOOL bALL = FALSE);		// 显示对话框信息

// 对话框数据
	enum { IDD = IDD_DLGEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


protected:
	CString			m_szRegionName;
	CRegionTree		*m_ptreeRegion;
	CWnd			*m_pWndParent;
	HTREEITEM		m_hItem;
	unsigned		m_uOffset;
	CString			m_szModifyNode;
	long			m_lRegionWidth;
	long			m_lRegionHeight;
};
