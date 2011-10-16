/**
* @filename:	PlayerShop.h
* @date:		2010/8/2
* @author:		wing (totti19851101@gmail.com) 

* @purpose: 个人商店交易
*/

#pragma once

#include "../Goods/Goods.h"
#include "../Player.h"

class PlayerShop
{
public:
	/// 交易方式
	enum eTradeType
	{
		TT_SINGLE,			///< 单个交易
		TT_GROUP,			///< 成组交易
		TT_NUM,				///< 交易方式数
	};

	/// 货币类型
	enum tagPriceType
	{
		PT_Money,			///< 金币
		PT_WeiMian,			///< 位面精华
		PT_NUM				///< 货币类型数
	};

	/// 开店状态
	enum ePlayerShopState
	{
		SET_SHOP,			///< 0开店设置
		OPEN_SHOP,			///< 1开店中
		SHOPPING_SHOP,		///< 2购物（打开别个玩家商店）
		SHOP_STATE			///< 3商店状态数
	};

	/// 交易物品属性<服务器>
	struct tagTradeGoodsInfo
	{
		ulong goodsId;		///< 物品的ID
		ulong contentId;	///< 容器的ID
		ulong index;		///< 在容器中对应的栏位
		ulong curNum;		///< 该物品在本栏位的数量
	};

	/// 交易物品属性<客户端>
	struct tagGoodsItem
	{
		bool	isReadyTrade;			///< 是否属于待交易项
		ulong	readyTradeNum;			///< 该项物品的预备交易数

		eTradeType tradeType;			///< 交易方式（单个交易&成组交易）
		int		priceType;				///< 价格类型（金币&位面精华）
		ulong	price;					///< 价格
		ulong	groupNum;				///< 组数
		ulong	oneGroupNum;			///< 单组个数

		ulong	goodsId;				///< 物品ID
		ulong	goodsIconId;			///< 物品图标ID
		ulong	totalNum;					///< 要显示的背包中此类物品的总数量
		string	strGoodsName;			///< 物品名字
		CGUID   guid;					///< 物品的GUID

		vector<tagTradeGoodsInfo> vecTradeGoodsInfo;	///< 该物品列表项的交易信息
		CGoods* pItemGoods;				///< 物品指针

		tagGoodsItem()
		{
			isReadyTrade = false;
			readyTradeNum = 0;

			tradeType = TT_SINGLE;
			priceType = PT_Money;
			price = 0;
			groupNum = 0;
			oneGroupNum = 0;

			goodsId = 0;
			goodsIconId = 0;
			totalNum = 0;
			pItemGoods = NULL;
			strGoodsName = "";
			memset(&guid,0,sizeof(CGUID));
		};
	};

	/// 缓存的物品数据
	struct stCacheGoodsInfo 
	{		
		eTradeType	treadeType;				///< 交易方式
		int			priceType;				///< 价格类型
		ulong		price;					///< 价格
		ulong		oneGroupNum;			///< 单组物品数量
		ulong		groupNum;				///< 组数
		int			shopPos;				///< 在商店中的位置
		stCacheGoodsInfo(void)
		{
			treadeType = TT_SINGLE;
			priceType = PT_Money;
			price = 0;
			oneGroupNum = 1;
			groupNum = 0;
			shopPos = 0;
		}
		stCacheGoodsInfo(eTradeType e, int pt, ulong p, ulong ogn, ulong gn, int sp)
		{
			treadeType = e;
			priceType = pt;
			price = p;
			oneGroupNum = ogn;
			groupNum = gn;
			shopPos = sp;
		}
	};


	typedef vector<tagGoodsItem> stGoodsItem;
	typedef stGoodsItem::iterator  GoodsItemIter;
	typedef map<ulong,stCacheGoodsInfo> stCacheGoods;
	typedef stCacheGoods::iterator stCacheIter;

public:
	PlayerShop(void);
	~PlayerShop(void);

public:
	/// 设置个人商店基本信息参数
	void SetShopPara(const CGUID& sessionID, long plugID, const CGUID& masterID);
	/// 设置当前的个人商店类型
	void SetCurShopState(int state);
	/// 获取当前的个人商店类型
	int	 GetCurShopState(void);
	/// 设置是否为打开个人商店状态
	void SetIsOpenShop(bool b);
	/// 取得个人商店格子数
	const int GetShopGridNum(void) const { return ITEMCOUNTS; }
	/// 设置Master的ID
	void SetMasterID(const CGUID& id) { m_masterID=id; }
	/// 获得Master的ID				
	const CGUID& GetMasterID() { return m_masterID; }
	/// 取得个人商店会话ID
	CGUID& GetSessionID(void) { return m_sessionID; }
	/// 取得开商店人PlugID
	long GetPlugID(void) const { return m_plugID; }
	/// 返回当前商店的物品数
	int GetCurGoodsNum()	{return m_curGoodsNum;}
	/// 设置当前商店的物品数
	void SetCurGoodsNum(int num)	{ m_curGoodsNum = num; }


	/// 取得交易预期收益金币数(卖家) & 交易预期花费金币数(买家)
	ulonglong GetTradeGold(void) const { return m_tradeGolds; }
	/// 查找上架物品信息
	tagGoodsItem* FindtagGoods(CGoods* pGoods);

	// 点击重设按钮时候，取消预备交易项
	void CancelAllTrade(void);

	/// 获取最新的个人商店所有物品列表。
	void UpdataPlayerShopGoodsList(void);


	bool OnCloseShop(void);
	void CloseShop();
	inline void ClearMyShopList()      { m_sellerShopGoodsList.clear();}		// 清空店主的物品列表
	


	// 消息发送接口
public:
	/// 发送加入物品消息
	//void SendAddGoodsMes(int nPos,long lPlugID);
	/// 发送开店消息
	void SendOpenShopMes(void);
	/// 发送关闭商店消息
	void SendCloseShopMes(void);
	/// 发送购买消息
	void SendBuyGoodsMes(void);
	/// 发送移除物品消息
	//void SendRemoveGoodsMes(int index);


	// 商品贩卖清单功能接口
public:
	/// 响应打开商品贩卖清单
	bool OnOrderPageOpen(CGoods* pGoods,int shopPos);
	/// 设置商品贩卖清单正在设置的物品
	void SetOrderGoods(CGoods* goods) { m_orderGoods = goods; }
	/// 取得商品贩卖清单正在设置的物品
	CGoods* GetOrderGoods(void) { return m_orderGoods; }
	/// 检测商品贩卖清单是否设置OK
	bool IsOrderSetupOK(void);
	/// 商品贩卖清单设置OK
	bool OnShopOrderOK(void);
	/// 根据物品ID在缓存列表中查找物品
	stCacheGoodsInfo* GetCacheGoodsInfo(ulong goodsID);



	// 个人商店卖家功能接口
public:
	/// 添加要贩卖的物品
	void AddMyShopGoods(CGoods *pGoods,int iPos/*在商店中的位置*/,eTradeType eDType,
		DWORD dwGoodsNumPerTeam,DWORD dwTeamNum ,DWORD dwPrice,int iPriceType );
	/// 将个人商店中指定序列的物品移出
	bool ReMoveShopGoods(long lGoodsID,CGUID guid);
	/// 取得自己摆摊物品列表
	vector<tagGoodsItem>& GetMyShopGoods(void) { return m_sellerShopGoodsList; }
	///更新店主的物品列表
	void UpdateMyShopList(const CGUID& guid,DWORD dwIndex,long lExtendID,DWORD dwPos,long lNum,int iAddOrSub);

	// 个人商店买家功能接口
public:
	/// 个人商店中加入店主的上架物品
	void AddPlayerShopGoods(CGoods* pGoods, ulong groupNum, ulong priceType,
		ulong price, ulong tradeType, ulong oneGroupNum);
	/// 判断交易项是否整备完整
	bool IsTradeItemReady(void);
	/// 根据当前准备交易物品项计算交易额
	void CalcTradeGolds(void);
	/// 买家设置确定购买标志
	void SetbSetTrue(bool bSetValue) {m_bSetSure = bSetValue;}	
	// 获取确定标志
	bool GetbSetTrue(void) {return m_bSetSure;}		
	/// 清除买家存储的店主上架物品列表
	void ClearPlayerShopList(void);
	/// 更新店主上架物品数量
	void UpdateSellGoodsNum(const CGUID& guid, ulong index,ulong groupNum);

private:
	// 处理个人商店打开时，背包中物品被右击的响应功能
	bool OnInputGoods(CPlayer::tagGoods* pstGoods);

	// 获取要买的物品种类个数
	long GetBuyGoodsNum(void);

private:
	static const int ITEMCOUNTS;			///< 个人商店格子数
	CGUID m_masterID;						///< 当前个人商店主人ID
	CGUID m_sessionID;						///< 个人商店会话ID
	long m_plugID;							///< 触发开商店人的ID

	int  m_curShopState;					///< 个人商店状态
	int  m_curGoodsNum;						///< 当前商店中物品的个数
	bool m_isOpenShop;						///< 个人商店打开标示

	ulonglong m_tradeGolds;					///< 交易预期收益金币数(卖家) & 交易预期花费金币数(买家)
	CGoods* m_orderGoods;					///< 交易清单中正在处理的某个（或某一类物品中的一个）的物品指针

	stGoodsItem m_buyerShopGoodsList;		///< 买家物品列表(查看商店时设置)
	stGoodsItem m_sellerShopGoodsList;		///< 卖家物品列表(摆摊时设置)
	stCacheGoods m_mapCacheGoods;			///< 管理缓存的物品信息数据


	string m_strShopName;					///< 缓存的商店名称
	string m_strOwnerRecommend;				///< 缓存的店主推荐
	string m_ShopName;								// 个人商店的名字
	bool m_bSetSure;								// 物品设置完后点确定时，此标志为True,表示物品被锁定了
	DWORD m_dwTradeWeiMian;						// 当前位面精华的交易量

	//DWORD m_dwShopNameCDTime;					// 修改商店名称的CD时间值
	//DWORD m_dwShopRecommendCDTime;				// 修改商店推荐内容的CD时间值
	//long m_lToChaterID;							// 要发送聊天信息的对象的PlugID
};

#define GetPlayerShop() GetInst(PlayerShop)