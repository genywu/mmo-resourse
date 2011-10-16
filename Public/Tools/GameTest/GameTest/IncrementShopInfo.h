#pragma once
#include "afxcmn.h"


// CIncrementShopInfo 对话框

class CIncrementShopInfo : public CDialog
{
	DECLARE_DYNAMIC(CIncrementShopInfo)

public:
	CIncrementShopInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIncrementShopInfo();

// 对话框数据
	enum { IDD = IDD_IncrementShopInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void SetDisplayIncrementShop();
	CListCtrl m_IncreShopGoodsList;
	virtual BOOL OnInitDialog();
//////////////////////////////////////////////////////////////////////////////////////////////
//////
	////						11.30Add						///////
public:
	struct tagShopGoods
	{
		DWORD	dwGoodsId;					// 物品的ID
		DWORD	dwPrice;					// 要显示的单价
		DWORD	dwNum;						// 要显示的总数量
		DWORD	dwReadyTradeNum;			// 该项物品的预备交易数
		bool	bIsReadyTrade;				// 是否属于待交易项
		string	strGoodsName;				// 物品名字
//		CButton *pItem;						// 对应的物品项目
	//	vector<tagTradeGoodsInfo> vecTradeGoodsInfo;	// 该物品列表项的交易信息(此结构只用于玩家把身上的物品卖给NPC时保存要卖的物品的信息)

		tagShopGoods()
		{
			dwGoodsId = 0;
			dwPrice = 0;
			dwNum = 0;
			dwReadyTradeNum = 0;
			bIsReadyTrade = false;
			strGoodsName = "";
//			pItem = NULL;
		};
	};
private:
//	tagGoodsItem *FindtagGoods(CButton *pItem,int iPage);		// 根据项目组件来查找对应页面的物品结构
	vector<tagShopGoods>	m_vecMyGoodsItem;		// 玩家身上的物品项目列表
	vector<tagShopGoods>	m_vecShopGoodsItem;		// 商店的物品项目列表
public:
	vector<tagShopGoods>&  GetShopGoods() {return m_vecShopGoodsItem;}
	void SetShopGoods(vector<tagShopGoods>&vec) {m_vecShopGoodsItem = vec;}
};
