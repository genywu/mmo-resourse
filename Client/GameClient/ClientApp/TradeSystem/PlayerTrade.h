/**
* @filename:	PlayerTrade.h
* @date:		2010/7/23
* @author:		wing (totti19851101@gmail.com) 

* @purpose: 玩家交易
*/

#pragma once

#include "../../../../Public/Module/GUID.h"
#include "../Goods/Goods.h"
#include "../Player.h"

class PlayerTrade
{
public:
	enum tagPriceType
	{
		PT_Money,			// 货币
		PT_YuanBao,			// 元宝
	};

	enum tagTradeObject
	{
		Trade_Other,					///< 对象为对方玩家
		Trade_Me,						///< 对象为自己
	};

	struct tagTradeGoods 
	{
		long lNum;						///< 物品的数量
		long lPos;						///< 物品在交易页面上的位置
		ulong dwIconId;					///< 物品的图标ID
		CGUID guid;						///< 物品的GUID
	};

public:
	PlayerTrade(void);

	~PlayerTrade(void) {}

	/// 设置交易参数
	void SetTradePara(const CGUID& sessionID, const CGUID& myID,long myPlugID,
		const CGUID& otherID,long otherPlugID);

	/// 取得对方
	CPlayer* GetOther(void);

	/// 取得交易格子数目
	long GetTradeGridNum(void) { return GRIDNUM; }

	/// 取得交易Session ID
	const CGUID& GetSessionID(void) { return m_sessionID; }

	/// 取得己方交易Plug ID
	long GetMyPlugID(void) { return m_myPlugID; }

	/// 设置己方交易金额
	void SetMyGold(ulong golds) { m_myGolds = golds; }
	/// 取得己方交易金额
	ulong GetMyGold(void) { return m_myGolds; }
	/// 设置对方交易金额
	void SetOtherGold(ulong golds) { m_otherGolds = golds; }
	/// 取得对方交易金额
	ulong GetOtherGold(void) { return m_otherGolds; }

	/// 取得己方交易物品列表
	map<int,tagTradeGoods>& GetMyTradeGoodsList(void) { return m_myTradeGoodsList; }

	/// 取得对方交易物品列表
	map<int,CGoods>& GetOtherTradeGoodsList(void){ return m_otherTradeGoodesList; }

	/// 添加己方交易物品
	void AddMyTradeGoods(CGoods* pGoods,int iPos);

	/// 添加对方交易物品
	void AddOtherTradeGoods(CGoods* pGoods,int iPos);

	bool OnInputGoods(CPlayer::tagGoods* pstGoods);							// 处理交易界面打开时，背包中物品被右击的响应功能

	/// 取得己方指定交易栏位物品数据
	tagTradeGoods* GetMyGoodsData(int nIndex);
	/// 取得对方指定交易栏位物品数据
	CGoods* GetOtherGoodsData(int nIndex);

	void UpdataTradeInfo(WORD wPriceType,long lPrice,int iWhose);

	/// 关闭交易
	void CloseTrade(void);

	/// 设置锁定状态
	void SetLockedState(bool bState,int iWhose);

	/// 取得某方的锁定状态
	bool GetLockedState(int iWhose);

	/// 设置交易结果状态（交易是否圆满结束）
	void SetResult(bool b)	{m_result = b;}

	/// 上一次交易是否圆满结束
	bool GetResult() {return m_result;}

	/// 我方是否进入锁定状态了
	bool IsReady()		{return m_myIsLocked;}

	/// 从交易界面中移除物品
	void RemoveTradeGoods(int nPos,int iWhose);

public:
	void SendClosePageMes(void);							///< 发送关闭消息
	void SendTradingGoodsMes(void);							///< 发送交易消息
	void SendChangeTradeGoodsMes(void);						///< 发送更改交易消息
	void SendReadyTradeGoodsMes(void);						///< 发送确认消息
	void SendRemoveTradeGoodsMes(int iPos);					///< 发送移除交易物品消息
	void SendAddTradeGoodsMes(int nPos);					///< 发送放物品到页面上的消息
	void SendCoinMes(void);									///< 发送交易金额更新消息

private:
	static const long GRIDNUM;								///< 交易页面的交易栏位数

	CGUID m_sessionID;										///< 交易会话ID
	CGUID m_myID;											///< 己方玩家ID
	CGUID m_otherID;										///< 对方玩家ID
	long m_myPlugID;										///< 己方PlugID
	long m_otherPlugID;										///< 对方PlugID
	ulong m_myGolds;										///< 己方金币
	ulong m_otherGolds;										///< 对方金币

	bool m_myIsLocked;										///< 己方锁定状态标志
	bool m_otherIsLocked;									///< 对方锁定状态标志

	bool m_result;											///< 交易流程是否圆满结束（当交易中途中断的时候此值为false）

	map<int,tagTradeGoods> m_myTradeGoodsList;				///< 己方交易物品列表
	map<int,CGoods> m_otherTradeGoodesList;					///< 对方交易物品列表
};