// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CTRADER_41E0DB7C02FD_INCLUDED
#define _INC_CTRADER_41E0DB7C02FD_INCLUDED

#include "CPlug.h"
#include "../Container/CVolumeLimitGoodsShadowContainer.h"
#include "../Container/CShadowWallet.h"

class CVolumeLimitGoodsContainer;

struct tagTradeGoodsInfo 
{
	CGUID goods_Guid;
	DWORD dwNum;
};

//##ModelId=41E0DB7C02FD
class CTrader 
	: public CPlug, public CContainerListener
{
private:
	//##ModelId=41F5B77103DC
	BOOL m_bBeReadyToTrade;
	BOOL m_bLocked;
	list<tagTradeGoodsInfo> m_listBuyGoodsInfo;
public:

	//##ModelId=41E1F8590242
	enum TRADE_STATE
	{
		//##ModelId=41E1F859032C
		TS_TRADE_READY = CPlug::PS_USER_DEFINE_BEGIN,

		//##ModelId=41F5B01B0346
		TS_TRADE_PREPARING,

		TS_TRADE_LOCK,

		//##ModelId=41E1F859038A
		TS_CANCEL_TRADING,

		//##ModelId=41E1F8590399
		TS_DETERMINE_TRADING
	};

	//##ModelId=41E0DC330251
	CTrader();

	//##ModelId=41E0DC3900EA
	//CTrader(LONG lID);

	//##ModelId=41E0DC430232
	~CTrader();

	//##ModelId=41E0DC5002CE
	virtual BOOL IsPlugAvailable();

	//##ModelId=41E0DC5C005D
	virtual BOOL OnSessionEnded();

	//##ModelId=41E0DC64032C
	virtual BOOL OnSessionAborted();

	//##ModelId=41E0DC6E00AB
	virtual BOOL OnPlugInserted();

	//##ModelId=41E0DC740157
	virtual BOOL OnPlugAborted();

	//##ModelId=41E0DC7C0196
	virtual BOOL OnPlugEnded();

	//##ModelId=41E0DC840251
	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);

	//##ModelId=41E1E29600DA
	virtual void Release();

	//##获取Plug自身带的容器
	//##ModelId=41EF0D9E008C
	virtual CContainer* GetContainer(LONG lID);

	virtual bool OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	virtual bool OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	//##ModelId=41F5DF6B0120
	BOOL GetClonePacket(CVolumeLimitGoodsContainer* pCloneContainer);

	//##ModelId=41F601CA0140
	LONG GetContraryID();

	//##ModelId=41F601D302A8
	CPlug* GetContraryPlug();

	//##获取对方的交易物品
	//##ModelId=41E223DB005D
	BOOL GetContraryTradeGoods(vector<CGoods*>& vGoodsList);

	//##获取自身的交易物品
	BOOL GetTradeGoods(vector<CGoods*>& vGoodsList);		

	//##ModelId=41E2442B038A
	DWORD GetContraryValumeOfTrade();

	//##ModelId=41F5E44A0084
	BOOL CheckTradeCondition(long &lTotalAmount);

	//##ModelId=41E1EF3400EA
	bool Trade();

	//##ModelId=41E1EEE001E4
	BOOL EndTrading();

	//##ModelId=41F5B78C03CC
	BOOL IsReady();

	//##ModelId=41F5C35C0332
	void SetTradeState(BOOL bReady);

	BOOL IsLocked(){return m_bLocked;}

	void SetLockState(BOOL bLocked) {m_bLocked=bLocked;}

	//##ModelId=41F6018A0306
	BOOL RemoveTradeGoods(vector<CGoods*>& vGoods);	

	CVolumeLimitGoodsShadowContainer* GetShadowContainer(){return &m_cSample;}
	CShadowWallet* GetGoldContainer(){return & m_cGoldCoins;}

	void AddBuyGoodsInfo(CGUID& id,DWORD dwNum);
	void ClearBuyGoodsInfo(){m_listBuyGoodsInfo.clear();}
	BOOL CheckBuyGoodsInfo(vector<CGoods*>& goodsVec);

protected:
	CVolumeLimitGoodsShadowContainer m_cSample;
	CShadowWallet m_cGoldCoins;	
};

#endif /* _INC_CTRADER_41E0DB7C02FD_INCLUDED */
