#pragma once
#include "..\..\public\tinystr.h"
#include "..\..\public\tinyxml.h"
class CTradeList
{
public:
	CTradeList(void);
	virtual ~CTradeList(void);

//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:

	// 商店类型
	enum eShopType
	{
		ARMOR_SHOP=0,				//0防具店
		ITEM_SHOP,					//1道具店
		JEWEL_SHOP,					//2首饰店
		WEAPON_SHOP,				//3武器店
		SHOP_NUM					//4商店的种类数
	};

	enum eTradeType
	{
		ETT_MONEY=1,	//用钱买
		ETT_GOODS,		//用物品换
		ETT_SILVER	//银币买
	};

	enum eCreditType
	{
		CREDIT_NOT=0,
		CREDIT_AREA,	 //区域声望
		CREDIT_ORGAN	//组织声望
	};
	struct tagTradeGoods
	{
		DWORD dwGoodsChgId;	//要交换物品的ID
		DWORD dwChgAmount;	//要交换物品数量
	};
	struct tagTradeData
	{
		DWORD dwGoodsId;	//物品ID
		DWORD dwGoodsNum;	//物品数量
		DWORD dwCreditId;		//所需声望ID
		DWORD dwCreditVal;		//所需声望值
		vector<tagTradeGoods>_vecTreadGoods;//以物易物所需物品列表	
		tagTradeData():dwCreditId(0),dwCreditVal(0){}
	};

	// 一个NPC交易的物品列表
	struct tagTrade
	{
		string	strNpcName;					// NPC原始名	
		eShopType shopType;					// 商店类型
		eTradeType tradeType;				// 交易方式
		eCreditType _creditType;			// 声望类型
		vector<tagTradeData>	vecGoods;	// 物品列表
	};

private:
	static map<string, tagTrade>	m_mapTradeList;		// 所有的NPC交易列表 map<NPC原始名，交易列表>

public:
	static tagTrade* GetTradeList(const char* strNpcName)
	{
		return m_mapTradeList.find(strNpcName)!=m_mapTradeList.end()?
		(&m_mapTradeList[strNpcName]):NULL;
	}

public:
	static BOOL LoadTradeList(const char* filename);			// 从文件读取列表
	static BOOL SaveTradeList(const char* filename);			// 保存到文件列表

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

	static bool	AddToClient(tagTrade* pTrade,DBWriteSet& dbWriteSet);// 编码商店物品列表
};
