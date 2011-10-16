// MainFrm.h : CMainFrame 类的接口
#pragma once
#include "AllGoodsPage.h"
#include "AllGoodsTree.h"
#include "GoodAttrPage.h"
#include "Search.h"
#include <list>
#include <string>
class CMainFrame : public CFrameWnd
{

protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// 属性
protected:
	CSplitterWnd m_wndSplitter;

public:
	CGoodAttrPage m_wndGoodAttr;
	CAllGoodsTree m_wndTree;
	CAllGoodsPage m_wndAllGoods;


	// 操作
public:
	CSplitterWnd& GetSplitterWnd() { return m_wndSplitter; }

	void SaveGoodsB();
	void LoadGoodsB();
	void SaveGoodsClient();

    BOOL IsEdit(CWnd* pWnd);

	// 重写
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnUpdateNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI *pCmdUI);
	afx_msg void OnNew();
	afx_msg void OnSave();
	afx_msg void OnCreateok();
	afx_msg void OnUpdateCreateok(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditok(CCmdUI *pCmdUI);
	afx_msg void On32783();
	afx_msg void OnClose();
	afx_msg void OnCountEquip();
	afx_msg void OnCountShow();
	afx_msg void OnCountRandom();
	afx_msg void OnCountRandomnum();
	afx_msg void OnCountAdd20();
	afx_msg void OnAddTypeex();
	afx_msg void OnConvertE();
	afx_msg void OnCheckDataToEduce();
	afx_msg void OnCheckDataToEduceE();
public:
/*	bool CountConvetEGoodsProperty(tagGoodAttr* pGoods);*/
	bool CountEquipDisplayProperty(tagGoodAttr *pGoods);
	afx_msg void OnAddAttrByBatAdd();

private:
	void UpdateChangeGoodsAddAttr(tagGoodAttr* pGoods,map<string,map<long,string>> & mapTemp);
//	list<string>	m_listBatOrgName;
//	list<tagAddAttr>		m_listBatAddAttrType;
public:
	void GetGoodsAddrByOrgName(const char* strorgname,DWORD& dwindex);
	afx_msg void OnAddArrtBatDel();
	afx_msg void OnEduceOneAttribute();
	afx_msg void OnCalculateAddAttrValue2();
	afx_msg void OnSearch();

	friend class Search;
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
};


