//======================================================================================
/**
*	@file		IncrementShopList.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once

/**
*	@class CIncrementShopList
*
*	@brief 增值商店配置类
*/
class CIncrementShopList
{
public:
	/// 构造
	CIncrementShopList(void);

	/// 析构
	~CIncrementShopList(void);

public:
	/// 交易类型
	enum eTradeMode
	{
		TM_YUANBAO,											//元宝交易 （星钻交易）
		TM_WEIMIAN,											//位面交易
		TM_TICKET,											//点券交易
	};

	/// 编解码类型
	typedef enum eMode
	{
		M_TOP10,
		M_NEWGOODS,
		M_GUIDETYPE,
		M_FILTER_XINGZUAN,
		M_FILTER_WEIMIAN,
		M_FILTER_TICKET,
	}eDecodeMode, eCodeMode;

public:
	/// 增值商店详细信息结构
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

	/// 购买记录属性表
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

private:
	/// 公告
	static string				m_sAffiche;
	/// 热销商品列表
	static MAP_TOP10			m_mTop10List;									
	/// 新品推荐列表
	static MAP_NEWGOODS			m_mNewGoodsList;								
	/// 导购商品列表
	static MAP_GUIDEGOODS		m_mGuideGoodsList;								
	/// 位面商店筛选商品列表
	static MAP_FILTERGOODS		m_mFilterListOfWShop;							
	/// 星钻商店筛选商品列表
	static MAP_FILTERGOODS		m_mFilterListOfXShop;							
	/// 点券商店筛选商品列表
	static MAP_FILTERGOODS		m_mFilterListOfTShop;							
	/// 商品详细信息列表
	static MAP_INCGOODS			m_mIncGoodsList;								
	/// 商品分类映射表
	static MAP_INCCLASSGOODS	m_mIncClassGoodsList;							

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

public:
	/// 从文件加载增值商店列表
	static bool LoadIncrementShopList(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 取得限购数目
	static long GetLimitNumByID(DWORD dwShopType, DWORD dwGoodsIdx);	

	/// 取得限量数目
	static long GetWorldBuyLimitNum(DWORD dwShopType, DWORD dwGoodsIdx);	

	/// 取得限时信息
	static string GetLimitTimeByID(DWORD dwShopType, DWORD dwGoodsIdx);	

	/// 时间比较
	static bool TimeLimited(const string& strTime);

private:
	/// 解析物品列表
	static void ParseGoodsList( TiXmlElement* , VEC_GOODSIDX& );

	//编码
	/// 编码热销&新品推荐列表
	static void CodeShopGoodsList( vector<uchar>* pByteArray, eCodeMode eCodeMode );
	static void CodeShopGoodsListToByteArray( vector<uchar>* pByteArray, MAP_TOP10&  mapGoodsList );
	//编码导购&筛选列表
	static void CodeTypeGoodsList( vector<uchar>* pByteArray, eCodeMode eCodeMode );
	static void CodeTypeGoodsListToByteArray( vector<uchar>* pByteArray,MAP_GUIDEGOODS&  mapTypeGoodsList );
	//编码物品详细信息列表
	static bool CodeDetailedGoodsList( vector<uchar>* pByteArray );
	//编码商品分类映射表
	static bool CodeIncClassGoodsList( vector<uchar>* pByteArray );

	//解码
	//解码物品列表
	static void DecordGoodsList( uchar* pByte, long& pos, VEC_GOODSIDX& vecGoodsList );
	//解码热销商品&新品推荐列表
	static void DecordShopGoodsList( uchar* pByte, long& pos, eMode eDecordMode );
	//解码导购&筛选物品列表
	static void DecordTypeGoodsList( uchar* pByte, long& pos, eMode eDecordMode );
	//解码物品详细信息列表
	static void DecordDetailedGoodsList( uchar* pByte, long& pos );
	//解码商品分类映射表
	static void DecordIncClassGoodsList( uchar* pByte, long& pos );

	//清除容器
	static void ClearIncShop();
};
