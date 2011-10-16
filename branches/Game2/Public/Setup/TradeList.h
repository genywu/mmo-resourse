//======================================================================================
/**
*	@file		TradeList.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

/**
*	@class TradeList
*
*	@brief 交易列表配置
*/

class TradeList
{
public:
	/// 构造
	TradeList(void);

	/// 析构
	~TradeList(void);

public:

	///  商店类型
	enum eShopType
	{
		/// 0防具店
		ARMOR_SHOP = 0,	
		/// 1道具店						
		ITEM_SHOP,	
		/// 2首饰店							
		JEWEL_SHOP,		
		/// 3武器店						
		WEAPON_SHOP,		
		/// 4商店的种类数					
		SHOP_NUM								
	};
	
	/// 交易类型
	enum eTradeType
	{
		/// 用钱买
		ETT_MONEY = 1,		
		/// 用物品换					
		ETT_GOODS,	
		/// 银币买							
		ETT_SILVER								
	};

	/// 声望类型
	enum eCreditType
	{
		/// 无声望
		CREDIT_NOT = 0,	
		/// 区域声望			
		CREDIT_AREA,						
		/// 组织声望	
		CREDIT_ORGAN							
	};

	/// 交易物品结构
	struct tagTradeGoods
	{
		/// 要交换物品的ID
		DWORD dwGoodsChgId;					
		/// 要交换物品数量	
		DWORD dwChgAmount;						
	};

	/// 交易数据
	struct tagTradeData
	{
		/// 物品ID
		DWORD dwGoodsId;		
		/// 物品数量				
		DWORD dwGoodsNum;		
		/// 所需声望ID				
		DWORD dwCreditId;				
		/// 所需声望值		
		DWORD dwCreditVal;						
		/// 以物易物所需物品列表	
		vector<tagTradeGoods>_vecTreadGoods;	

		/// 构造
		tagTradeData()
			:dwCreditId(0),dwCreditVal(0){}
	};

	///  一个NPC交易的物品列表
	struct tagTrade
	{
		/// NPC原始名	
		string	strNpcName;				
		/// 商店类型		
		eShopType shopType;						
		/// 交易方式
		eTradeType tradeType;					
		/// 声望类型
		eCreditType _creditType;				
		/// 物品列表
		vector<tagTradeData>	vecGoods;		
	};

private:
	/// 所有的NPC交易列表 map<NPC原始名，交易列表>
	static map<string, tagTrade>	m_mapTradeList;

public:
	/// 从文件加载交易列表
	static bool LoadTradeList(const char* filename);		

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 根据NPC商店名取得交易列表
	static tagTrade* GetTradeList(const char* strNpcName);
};
