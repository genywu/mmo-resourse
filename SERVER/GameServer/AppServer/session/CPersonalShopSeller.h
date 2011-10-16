// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPERSONALSHOPSELLER_42266D9200FA_INCLUDED
#define _INC_CPERSONALSHOPSELLER_42266D9200FA_INCLUDED

#include "CPlug.h"
#include "..\Listener\CContainerListener.h"
#include "..\Container\CVolumeLimitGoodsShadowContainer.h"
#include "CPersonalShopBuyer.h"

class CContainer;
class CBaseObject;
class CGoods;
class CPersonalShopBuyer;

//定义保存的交易信息的最大条数
const int MAX_TRADE_INFO = 5;

//##ModelId=42266D9200FA
class CPersonalShopSeller 
: public CPlug
, public CContainerListener
{
private:
	struct tagPrice
	{
		DWORD dwSellType;					//物品出售的方式（分为两种：0、单个贩卖；1、成组贩卖）
		DWORD dwOneGroupCnt;			//物品的单组个数( 单个贩卖时设定为常数1 )
		DWORD dwGroupNum;				//物品的组数
		DWORD dwPriceType;
		DWORD dwPrice;
		CGUID guid;
		LONG lGoodsId;
	};

	struct tagSellInfo
	{
		CGoods* pGoods;
		DWORD dwSellType;					//物品出售的方式（分为两种：0、单个贩卖；1、成组贩卖）
		DWORD dwOneGroupCnt;			//物品的单组个数( 单个贩卖时设定为常数1 )
		DWORD dwNum;							//物品的组数
		DWORD dwPriceType;					//物品出售的货币类型（分为两种：0、金币；1、位面精华）
		DWORD dwPrice;
		LONG lContainerId;
		LONG lPos;
	};
public:
	typedef vector<CPersonalShopBuyer::tagBuyInfo>	VecBuyInfo;
	typedef map<long, VecBuyInfo* >	MapBuying;		// <lPlugID, tagBuyInfo>
	typedef MapBuying::iterator ItrBuying;

	//##ModelId=422E5347006D
	BOOL GetGoodsList(vector<BYTE>& vOut);
	VOID GetUpdateGoodsList(vector<BYTE>& vOut);

	//##ModelId=42266DFF0242
	CPersonalShopSeller();

	//##ModelId=42266E09037A
	//CPersonalShopSeller(LONG lID);

	//##ModelId=42266E1A0109
	~CPersonalShopSeller();

	//##ModelId=42266E2A035B
	virtual BOOL IsPlugAvailable();

	//##ModelId=42266E34005D
	virtual BOOL OnSessionEnded();

	//##ModelId=42266E3C02EE
	virtual BOOL OnSessionAborted();

	//##ModelId=42266E4B00FA
	virtual BOOL OnPlugInserted();

	//##ModelId=42266E54005D
	virtual BOOL OnPlugAborted();

	//##ModelId=42266E6900AB
	virtual BOOL OnPlugEnded();

	//##ModelId=42266E710232
	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);

	//##ModelId=42266FB40128
	virtual VOID Release();

	//这个函数返回卖东西的角色的影子容器
	//##ModelId=42266FBD031C
	virtual CContainer* GetContainer(LONG lID);

	//##ModelId=422E542D01A5
	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=422E543D0251
	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=422E545301D4
	virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);

	//##ModelId=42267E6701D4
	BOOL OpenForBusiness();

	//个人商店关张
	//##ModelId=4227C0520186
	BOOL CloseDown();

	//##ModelId=422FAFB40196
	BOOL DoesShopOpenedForBusiness();

	//##ModelId=422BAFA30148
	BOOL SetShopName(CHAR* strName);

	//##ModelId=422BAFBD007D
	char* GetShopName();

	//取得店主推介信息
	char* GetShopInfo();
	//设置店主推介信息
	BOOL SetShopInfo( string strShopInfo );
	//取得交易信息
	vector<string>& GetTradeInfo();
	//插入一条交易信息
	void InsertTradeInfo( const string& strTradeInfo );

	//##ModelId=422BB13A02EE
	//设置物品价格信息（包括贩卖方式信息）
	BOOL SetGoodsPrice(const CGUID& guid,LONG lGoodsId, WORD wSellType, 
		WORD dwPriceType, DWORD dwGroupNum,  DWORD dwOneGroupCnt,DWORD dwPrice);

	//##ModelId=422BB17300AB
	//取得物品价格信息
	BOOL GetGoodsPrice(const CGUID& guid,LONG lGoodsId,DWORD &dwPriceType, DWORD& dwPrice);
	//取得物品贩卖方式信息
	BOOL GetGoodsSellType( const CGUID& guid, LONG lGoodsId, WORD& wSellType, DWORD& dwGroupNum, DWORD& dwOneGroupCnt );

	//是否存在指定goodsid的物品价格
	BOOL GoodsPriceExist(LONG lGoodsId);


	//交易条件检测
	BOOL CheckCondition(LONG lPlugID, DWORD &dwTotalMoney, DWORD& dwTotalWeimian );
	//交易
	bool PurchaseEx(LONG lPlugID, DWORD dwTotalMoney, long lTotalYuanbao, long lTotalWeimian );

	VOID ClearData(){m_mGoodsPrice.clear();m_cSample.Clear();}

	//##ModelId=422FD7B40167
	DWORD GetValidGoodsAmount();
	DWORD GetValidGoodsAmountEx(); 

	CGoodsShadowContainer::tagGoodsShadow* RemoveByGoodsId(DWORD dwId);
	VOID AddShadowTag(CGoodsShadowContainer::tagGoodsShadow* pTag);

	void AddMapBuying(long lPlugID, VecBuyInfo *pVecBuyInfo);
	void DelMapBuying(long lPlugID);
	void ReleaseMapBuying();

private:
	//##ModelId=42267E500232
	BOOL m_bShopOpened;

	//##ModelId=422BAF8A000F
	//商店名称
	string m_strShopName;
	//店主推介
	string m_strShopInfo;
	//交易信息
	vector<string> m_vecTradeInfo;

	//##ModelId=422BB05B01D4
	map<CGUID, tagPrice,guid_compare> m_mGoodsPrice;
	typedef map<CGUID, tagPrice,guid_compare>::iterator itGoodsPrice;

	//##ModelId=4226A3780196
	CVolumeLimitGoodsShadowContainer m_cSample;

	MapBuying	m_mapBuying;

public:
	enum CheckResult
	{
		CR_WAIT_FINISH	= 0,
		CR_WAIT_YUANBAO,
		CR_MISS_MONEY,
		CR_MISS_WEIMIAN,				//位面不足
		CR_OTHR_ERROR,
	};

	//贩卖方式（单个贩卖/成组贩卖）
	enum eSellType
	{
		ST_SINGLE_SELL,
		ST_GROUP_SELL,
	};
	//价格类型（金币交易/位面交易）
	enum ePriceType
	{
		PT_GOLD_SELL,
		PT_WEIMIAN_SELL,
	};
};

#endif /* _INC_CPERSONALSHOPSELLER_42266D9200FA_INCLUDED */
