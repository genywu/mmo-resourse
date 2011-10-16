#pragma once
#include "AllGoodsPage.h"
#include "GoodAttrPage.h"

// CContainerPage 窗体视图

class CContainerPage : public CFormView
{
	DECLARE_DYNCREATE(CContainerPage)

protected:
	CContainerPage();           // 动态创建所使用的受保护的构造函数
	virtual ~CContainerPage();

public:
	CAllGoodsPage m_wndAllGoodsPage;
	CGoodAttrPage m_wndGoodAttrPage;
	enum { IDD = IDD_CONTAINER };
	

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


