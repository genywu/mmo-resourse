///
/// @file BusinessGoodsDef.h
/// @brief 跑商系统-商品属性描述
///
#ifndef ___BUSINESS_GOODS_DEF_H_
#define ___BUSINESS_GOODS_DEF_H_

#include <string>

namespace Business
{
#define MAX_STR_LEN 512
	///
	/// 商品价值分类
	///
	enum eValueType
	{
		VT_NORMAL = 0,
		VT_SPECIAL,
		VT_LUXURY
	};

	///
	/// 商品属性
	///
	struct tagGoods
	{
		ulong dwIndex;					/// 索引
		char strOrigName[MAX_STR_LEN];	/// 原始名
		uchar VTType;					/// 价值分类
		ulong dwCredit;					/// 声望要求
		ulong dwPersonalInvest;			/// 个人投资额
		ulong dwCountryInvest;			/// 国家投资额
		ulong dwSpotDevelop;			/// 贸易点开发度
		uchar Local;					/// 是否是本地商品(1/0)
		ulong dwBaseBuyLimit;			/// 基本限购额
		ulong dwVolume;					/// 承载量
		float fVolumeQuo;				/// 承载量随发展度变化系数
		ulong dwBasePrice;				/// 基本价格
		float fBuyMinQuo;				/// 买入最低系数
		float fBuyMaxQuo;				/// 买入最高系数
		float fSellMinQuo;				/// 卖出最低系数
		float fSellMaxQuo;				/// 卖出最高系数
		long dwMinVol;					/// 最小承载量
		ulong dwMaxVol;					/// 最大承载量
		float fBuyMinAddonQuo;			/// 最小承载量再变化系数
		float fSellMaxAddonQuo;			/// 最大承载量再变化系数
		ulong dwFallPrice;				/// 瞬间暴跌金额
		float fCurVolumeQuo;			/// 当前承载量随时间变化系数
	};

	///
	/// 发送给客户端的商品属性，购买界面使用(S2C)
	///
	struct tagGoodsClient
	{
		ulong dwIndex;		/// 物编中的ID
		ulong dwBuyLimit;	/// 当前限购额
		ulong dwPrice;		/// 卖出价格
		int PriceScale;		/// 物价比 PriceScale%
	};

	/// 该结构主要用于描述卖给交易所的物品的单位数据（C2S）
	struct tagGoodsInfoForSell
	{
		CGUID	guid;						// 物品的GUID
		ulong	dwCurNum;					// 该物品的卖出的数量
		ulong	dwPrice;					// 卖给交易所的价格
	};

	///
	/// 出售界面中发送给客户端的商品价格属性，用于显示(S2C)
	///
	struct tagGoodsSellClient
	{
		ulong dwIndex;
		ulong dwPrice;
	};

	///
	/// 价格变化走势
	///
	enum 
	{
		PRICE_EQUAL,
		PRICE_GREAT,		
		PRICE_LESS,
	};

	///
	/// 物价指数查询界面所需的物品信息
	///
	struct tagGoodsCheckingClient
	{
		ulong dwIndex;
		ulong dwPrice;
		int iPriceScale;		/// 物价比 PriceScale%
		int iPriceState;		
	};

	///
	/// 开启跑商商店子消息类型
	///
	enum 
	{
		BUSINESS_BUYPAGE = 0,
		BUSINESS_SELLPAGE,
		BUSINESS_CHECKPAGE,

		BUSINESS_PAGENUM,
	};
}

#endif // ___BUSINESS_GOODS_DEF_H_