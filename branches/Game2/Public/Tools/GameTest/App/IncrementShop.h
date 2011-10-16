



#ifndef _increment_shop_h_
#define _increment_shop_h_
#pragma once


enum eIncrShopClass
{
	//	@	元宝、星钻
	eSHOP_STAR,
	//	@ 位面
	eSHOP_PLANES,
	//	@	点券
	eSHOP_TICKET,
	//	@
	eSHOP_NUM,
};

enum eShopGoodsList
{
	eGOODS_All,
	eGOODS_Top,
	eGOODS_New,
	eGOODS_Guide,
	eGOODS_2,
	eGOODS_3,
	eGOODS_NUM,
};


//	----------------------
//	@	name:	增值商店类
//	@	brief:	增值商店在整个游戏中只会有一个唯一的实体存在
//	-------------------------------------------------

class CIncrementShop
{
public:
	~CIncrementShop();

	static CIncrementShop*	GetIncrShop(void);

	void	DecodeIncrShop(class CMessage*	pMsg);

	bool	IsOpend(void)	{ return m_bOpend; }

	// @	此接口专门为gt提供 [11/10/2009 Square]
	DWORD	RandomOneGoodsIndex(DWORD dwShopID,DWORD dwGoodsListID);

	//	bool	AddGoods(DWORD dwShopID,DWORD dwListID,DWORD dwGoodsIndex);

private:
	CIncrementShop();

	static CIncrementShop*	m_pIncrShop;

	//	!@ 是否打开过
	bool	m_bOpend;

	//	@	商店物品列表
	vector<DWORD>		m_vecIncrShopGoodsIndexList[eSHOP_NUM][eGOODS_NUM];
};
#endif//_increment_shop_h_