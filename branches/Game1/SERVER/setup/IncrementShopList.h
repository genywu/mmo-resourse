#pragma once
#ifndef _CIncrementShopList_h_
#define _CIncrementShopList_h_


#include "..\..\public\tinystr.h"
#include "..\..\public\tinyxml.h"

class CIncrementShopList
{
public:
	CIncrementShopList(void);
	virtual ~CIncrementShopList(void);


public:
	enum eTradeMode
	{
		TM_YUANBAO,											//元宝交易 （星钻交易）
		TM_WEIMIAN,											//位面交易
		TM_TICKET,											//点券交易
	};

	enum eProfitMode
	{
		PM_NORMAL,
		PM_PROFIT,
	};

	enum eShopCloseMode
	{
		CM_NORMAL,
		CM_UPDATA,
	};

	typedef enum eMode
	{
		M_TOP10,
		M_NEWGOODS,
		M_GUIDETYPE,
		M_FILTER_XINGZUAN,
		M_FILTER_WEIMIAN,
		M_FILTER_TICKET,
	}eDecodeMode, eCodeMode;
	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
public:
	struct IncrementShopElem
	{
	public:
		DWORD	dwGoodsType;									//物品种类
		DWORD	dwGoodsIndex;									//物品编号
		DWORD	dwPrice;										//价格
		DWORD   dwGoodsNum;										//数量
		DWORD	dwDiscount;										//打折
		string	strTimeLimit;									//限时
		long	lQuantityLimit;									//限量
		long	lBuyLimit;										//限购

	public:
		IncrementShopElem()
			: dwGoodsType(0)
			, dwGoodsIndex(0)
			,dwPrice(0)
			, dwGoodsNum(0)
			,dwDiscount(0)
			,strTimeLimit("")
			,lQuantityLimit(0)
			,lBuyLimit(0)
		{
		}

		IncrementShopElem( DWORD GoodsType,DWORD GoodsIndex, DWORD Price, DWORD GoodsNum, 
			DWORD Discount, string TimeLimit, long QuantityLimit, long BuyLimit )
			: dwGoodsType(GoodsType)
			, dwGoodsIndex(GoodsIndex)
			,dwPrice(Price)
			, dwGoodsNum(GoodsNum)
			,dwDiscount(Discount)
			,strTimeLimit(TimeLimit)
			,lQuantityLimit(QuantityLimit)
			,lBuyLimit(BuyLimit)
		{
		}
	};

	//返还点卷相关的参数
	struct  tagReturnParam
	{
		float fNoramlRatio;										//普通返还比例
		float fSpecialRatio;									//特殊返还比例
		string strStartTime;									//特殊返还的开始时间
		string strEndTime;										//特殊返还的结束时间
		list<string> lSGoodsName;								//不执行返还的特殊物品列表
	};


	//购买记录属性表
	struct IncShopTradeLog
	{
	public:
		string	strBuyTime;										// 购买时间
		DWORD	dwPriceNum;										// 货币数量
		DWORD	dwPriceType;									// 货币类型
		DWORD   dwGoodsIdx;										// 商品ID
		DWORD	dwBuyNum;										// 购买数量
	public:
		IncShopTradeLog()
			:strBuyTime(""),dwPriceNum(0),dwPriceType(0),dwGoodsIdx(0),dwBuyNum(0)
		{}
		IncShopTradeLog( string BuyTime, DWORD PriceNum, DWORD PriceType, DWORD GoodsIdx, DWORD BuyNum )
			:strBuyTime(BuyTime),dwPriceNum(PriceNum), dwPriceType(PriceType),dwGoodsIdx(GoodsIdx),dwBuyNum(BuyNum)
		{}
	};

public:
	typedef vector<DWORD> VEC_GOODSIDX;										//vector<dwGoodsIndex>
	//热销商品&新品推荐数据结构
	typedef map< DWORD, VEC_GOODSIDX >    MAP_TOP10, MAP_NEWGOODS,			//< dwShopType, vector<dwGoodsIndex> >
										MAP_CLASSGOODS;						//< dwClassType, vector<dwGoodsIndex> >

	//导购&筛选 数据结构
	typedef map<string, VEC_GOODSIDX > MAP_TYPEGOODS;						//< strGuideType, vector<dwGoodsIndex> >
																			//< strFilterType, vector<dwGoodsIndex> >
	typedef map< DWORD, MAP_TYPEGOODS>   MAP_GUIDEGOODS,
										MAP_FILTERGOODS;					//< dwShopType & dwGoodsType, map<> >
	//物品详细信息数据结构
	typedef map<DWORD, IncrementShopElem>			MAP_INCSHOP;			// <dwGoodsIndex, IncrementShopElem>
	typedef map< DWORD, MAP_INCSHOP > MAP_INCGOODS;							//< dwShopType, map<> >
	typedef map<DWORD, MAP_CLASSGOODS> MAP_INCCLASSGOODS;					//< dwShopType, map<> >
	//分红映射结构
	typedef map<DWORD,DWORD> MAP_INCMAP;
	//购买记录列表
	typedef vector<IncShopTradeLog> VEC_BUYLIST;

	typedef VEC_GOODSIDX::iterator ITR_GOODSIDX;
	typedef MAP_TOP10::iterator ITR_TOP10;
	typedef MAP_TOP10::iterator ITR_NEWGOODS;
	typedef MAP_TYPEGOODS::iterator ITR_TYPEGOODS;
	typedef MAP_GUIDEGOODS::iterator ITR_GUIDEGOODSLIST;
	typedef MAP_FILTERGOODS::iterator ITR_FILTERTGOODSLIST;
	typedef MAP_INCSHOP::iterator ITR_INCSHOP;
	typedef MAP_INCGOODS::iterator ITR_INCGOODS;
	typedef VEC_BUYLIST::iterator ITR_BUYLIST;
	typedef VEC_BUYLIST::reverse_iterator RITR_BUYLIST;
	typedef MAP_CLASSGOODS::iterator ITR_CLASSGOODS;
	typedef MAP_INCCLASSGOODS::iterator ITR_INCCLASSGOODS;
	typedef MAP_INCMAP::iterator ITR_MAPLIST;

private:
	static string				m_sAffiche;										//公告

	static MAP_TOP10			m_mTop10List;									//热销商品列表
	static MAP_NEWGOODS			m_mNewGoodsList;								//新品推荐列表
	static MAP_GUIDEGOODS		m_mGuideGoodsList;								//导购商品列表
	static MAP_FILTERGOODS		m_mFilterListOfWShop;							//位面商店筛选商品列表
	static MAP_FILTERGOODS		m_mFilterListOfXShop;							//星钻商店筛选商品列表
	static MAP_FILTERGOODS		m_mFilterListOfTShop;							//点券商店筛选商品列表
	static MAP_INCGOODS			m_mIncGoodsList;								//商品详细信息列表
	static MAP_INCCLASSGOODS	m_mIncClassGoodsList;							//商品分类映射表

	static MAP_INCSHOP			m_mMapIncGoodsList;								//道具映射配置表
	static MAP_INCMAP			m_mMapList;
	static tagReturnParam		m_tgReturnParm;									//返还的相关参数

public:
	static string*			GetAffiche()			{ return &m_sAffiche; }
	static MAP_TOP10* GetTop10List() { return &m_mTop10List; }
	static MAP_NEWGOODS* GetNewGoodsList() { return &m_mNewGoodsList; }
	static MAP_GUIDEGOODS* GetGuideGoodsList() { return &m_mGuideGoodsList; }
	static MAP_FILTERGOODS* GetFilterListOfWShop() { return &m_mFilterListOfWShop; }
	static MAP_FILTERGOODS* GetFilterListOfXShop() { return &m_mFilterListOfXShop; }
	static MAP_FILTERGOODS* GetFilterListOfTShop() { return &m_mFilterListOfTShop; }
	static MAP_INCGOODS* GetIncGoodsList() { return &m_mIncGoodsList; }
	static MAP_INCCLASSGOODS* GetIncClassGoods() { return &m_mIncClassGoodsList; }

	static MAP_INCSHOP* GetMapIncShopList() {return &m_mMapIncGoodsList;}
	static MAP_INCMAP* GetMapIndexList() { return &m_mMapList; }
	static tagReturnParam* GetReturnParam() {return &m_tgReturnParm;}

public:
	static bool LoadIncrementShopList(const char* filename);						// 从文件读取列表
	static bool AddToByteArray(vector<BYTE>* pByteArray);							// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);						// 解码

	static long GetLimitNumByID( DWORD dwShopType, DWORD dwGoodsIdx );				//取得限购数目
	static long GetWorldBuyLimitNum( DWORD dwShopType, DWORD dwGoodsIdx );			//取得限量数目
	static string GetLimitTimeByID( DWORD dwShopType, DWORD dwGoodsIdx );			//取得限时信息
	static bool TimeLimited(const string& strTime);									//时间比较

	static bool IsProfitGoods(DWORD dwGoodsID);										// 查询一个物品是否是分红商品
	static bool IsReturnDianGuan(DWORD dwGoodsIdx);									//判断是否返回点卷
	static long GetReturnNum(long lTotalAmount,float &fReturnRatio);									//返还点卷数量
	static bool IsStartTime();														//判断起始时间日期
	static bool IsEndTime();														//判断结束时间日期
	static bool IsHour();															//判断小时


	//
	//public:
	//	static bool	GetReloadFlag()						{ return m_bReload; }
	//	static void SetReloadFlag(bool bReload = true)	{ m_bReload = bReload; }
	//private:
	//	static bool	m_bReload;

private:
	//解析物品列表
	static void ParseGoodsList( TiXmlElement* , VEC_GOODSIDX& );

	//编码
	//编码热销&新品推荐列表
	static void CodeShopGoodsList( vector<BYTE>* pByteArray, eCodeMode eCodeMode );
	static void CodeShopGoodsListToByteArray( vector<BYTE>* pByteArray, MAP_TOP10&  mapGoodsList );
	//编码导购&筛选列表
	static void CodeTypeGoodsList( vector<BYTE>* pByteArray, eCodeMode eCodeMode );
	static void CodeTypeGoodsListToByteArray( vector<BYTE>* pByteArray,MAP_GUIDEGOODS&  mapTypeGoodsList );
	//编码物品详细信息列表
	static bool CodeDetailedGoodsList( vector<BYTE>* pByteArray );
	//编码商品分类映射表
	static bool CodeIncClassGoodsList( vector<BYTE>* pByteArray );
	//编码道具映射配置表
	static bool CodeMapIncShopList( vector<BYTE>* pByteArray );
	//编码返还参数配置表
	static bool CodeReturnParam(vector<BYTE>* pByteArray);


	//解码
	//解码物品列表
	static void DecordGoodsList( BYTE* pByte, long& pos, VEC_GOODSIDX& vecGoodsList );
	//解码热销商品&新品推荐列表
	static void DecordShopGoodsList( BYTE* pByte, long& pos, eMode eDecordMode );
	//解码导购&筛选物品列表
	static void DecordTypeGoodsList( BYTE* pByte, long& pos, eMode eDecordMode );
	//解码物品详细信息列表
	static void DecordDetailedGoodsList( BYTE* pByte, long& pos );
	//解码商品分类映射表
	static void DecordIncClassGoodsList( BYTE* pByte, long& pos );
	//解码道具映射配置表
	static void DecordMapIncShopList( BYTE* pByte, long& pos );
	//解码返还参数配置表
	static void DecordReturnParam(BYTE* pByte, long& pos);


	//清除容器
	static void ClearIncShop();
};

#endif