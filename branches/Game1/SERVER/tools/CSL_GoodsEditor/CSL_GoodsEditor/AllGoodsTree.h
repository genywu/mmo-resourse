#pragma once
#include "afxcmn.h"
#include "ContainerPage.h"



// CAllGoodsTree 窗体视图

class CAllGoodsTree : public CFormView
{
	DECLARE_DYNCREATE(CAllGoodsTree)

public:
	CAllGoodsTree();           // 动态创建所使用的受保护的构造函数
	virtual ~CAllGoodsTree();
	void InitMyCtrls();
	void SetNewItemFlag(bool bVal){m_bNewItem=bVal;}
	bool GetNewItemFlag(){return m_bNewItem;}
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

public:
	enum { IDD = IDD_GODSTREEDIALOG };
	HTREEITEM m_hTreeRoot;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	bool m_bNewItem;
	bool m_bCtrlInit;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult);
	CTreeCtrl m_cGoodsTree;
	CTreeCtrl* GetSelGoodsTree()	{ return &m_cGoodsTree;}
	HTREEITEM GetRootItem()	{ return m_hTreeRoot;}
	afx_msg void OnNMDblclkAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult);
	friend class CContainerPage;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickAllgoodstree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On32785();
};


