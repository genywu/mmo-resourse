//-------------------------------------------------------------------------------
// 统一计费会话. by Fox.		2008-01-23
// 处理 GS2BS 的一次请求到响应的异步回调
//-------------------------------------------------------------------------------

#pragma once

#include "Game.h"
#include "../App/bsinclude/bsip/bsipfun.h"
#include "../App/bsinclude/bsip/bsipfunbs.h"

class CCallbackAccountAuthen;
class CCallbackAccountProcess;
class CCallbackAwardAck;
class CCallbackAwardAuthen;
class CCallbackConsignTransfer;

enum eBillItem
{
	BI_NONBACK = -1,														// 无回调返回
	BI_NULL,																// 无效操作
	BI_ACCOUNTAUTHEN,														// 帐号查询
	BI_LOCK,																// 帐号预冻
	BI_UNLOCK,																// 帐号解冻
	BI_TRANSFER,															// 帐号转账
	BI_AWARDAUTHEN,															// 领奖查询
	BI_AWARDACK,															// 领奖确认
};

enum eBillType
{
	BT_NULL = 0,
	BT_AT_PATCH,															// 与帐号邦定的领奖
	BT_AT_NUMBER,															// 与领奖号绑定的领奖
	BT_AT_NOBIND,															// 不与帐号、领奖号绑定的领奖
	BT_AT_GETYB,															// 道具换元宝
	BT_AT_LEVEL,															// 金牌帐号

	BT_INCSHOP		= 17,														// 增值商店	
	BT_TRADER		= 20,														// 个人交易
	BT_PERSHOP		= 55,														// 个人商店
	BT_INCPRESCET   = 105,														// 赠值道具赠送
};

class CBillClient
{
public:
	CBillClient();
	~CBillClient();

//-------------------------------------------------------------------------------
// Encapsulate session
//-------------------------------------------------------------------------------
public:
	void Run();
	void Release();															// 释放资源

 public:
	struct tagAccAuthenReq
	{
		char szPlayerID[MAX_PTID_LEN+1];									// 账号id
		char szSessionID[MAX_SESSID_LEN+1];									// 会话id
		char szPlayerIP[IP_ADDRESS_LEN+1];									// 终端IP地址
	};

	struct tagAccLockReq
	{
		eBillType billType;													// 交易类型
		CGUID guidSessionID;												// 交易会话ID
		long lPlugID;														// 交易Plug ID
		long lTotalYuanbao;													// 总交易元宝数额
		long lTotalMoney;													// 总交易金钱数额
		long lGoodsID;														// 游戏道具编号
		long lGoodsNum;														// 游戏道具数量
		char szBuyerID[MAX_PTID_LEN+1];										// 买家账号id
		char szSellerID[MAX_PTID_LEN+1];									// 卖家账号id
		char szSessionID[MAX_SESSID_LEN+1];									// 会话id
		char szBuyerIP[IP_ADDRESS_LEN+1];									// 买家终端IP地址

		tagAccLockReq()
			:billType(BT_NULL)
			,lPlugID(0)
			,lTotalYuanbao(0)
			,lTotalMoney(0)
			,lGoodsID(0)
			,lGoodsNum(0)
		{
			strcpy_s(szBuyerID, MAX_PTID_LEN+1, "");
			strcpy_s(szSellerID, MAX_PTID_LEN+1, "");
			strcpy_s(szSessionID, MAX_SESSID_LEN+1, "");
			strcpy_s(szBuyerIP, IP_ADDRESS_LEN+1, "");
		}
		tagAccLockReq(const tagAccLockReq &req)
			:billType(req.billType)
			,lPlugID(req.lPlugID)
			,lTotalYuanbao(req.lTotalYuanbao)
			,lTotalMoney(req.lTotalMoney)
			,lGoodsID(req.lGoodsID)
			,lGoodsNum(req.lGoodsNum)
		{
			guidSessionID = req.guidSessionID;
			strcpy_s(szBuyerID, MAX_PTID_LEN+1, req.szBuyerID);
			strcpy_s(szSellerID, MAX_PTID_LEN+1, req.szSellerID);
			strcpy_s(szSessionID, MAX_SESSID_LEN+1, req.szSessionID);
			strcpy_s(szBuyerIP, IP_ADDRESS_LEN+1, req.szBuyerIP);
		}
	};

	struct tagAccUnlockReq
	{
		eBillType billType;													// 交易类型
		bool bConfirm;														// 交易成功为true，失败为false
		long lTotalYuanbao;													// 总交易额
		long lGoodsID;														// 游戏道具编号
		long lGoodsNum;														// 游戏道具数量
		char szContextID[MAX_ORDERID_LEN+1];								// 关联id
		char szBuyerID[MAX_PTID_LEN+1];										// 买家账号id
		char szSellerID[MAX_PTID_LEN+1];									// 卖家账号id
		char szSessionID[MAX_SESSID_LEN+1];									// 会话id
		char szBuyerIP[IP_ADDRESS_LEN+1];									// 买家终端IP地址

		tagAccUnlockReq()
			:billType(BT_NULL)
			,bConfirm(false)
			,lTotalYuanbao(0)
			,lGoodsID(0)
			,lGoodsNum(0)
		{
			strcpy_s(szContextID, MAX_ORDERID_LEN+1, "");
			strcpy_s(szBuyerID, MAX_PTID_LEN+1, "");
			strcpy_s(szSellerID, MAX_PTID_LEN+1, "");
			strcpy_s(szSessionID, MAX_SESSID_LEN+1, "");
			strcpy_s(szBuyerIP, IP_ADDRESS_LEN+1, "");
		}
		tagAccUnlockReq(const tagAccUnlockReq &req)
			:billType(req.billType)
			,bConfirm(req.bConfirm)
			,lTotalYuanbao(req.lTotalYuanbao)
			,lGoodsID(req.lGoodsID)
			,lGoodsNum(req.lGoodsNum)
		{
			strcpy_s(szContextID, MAX_ORDERID_LEN+1, req.szContextID);
			strcpy_s(szBuyerID, MAX_PTID_LEN+1, req.szBuyerID);
			strcpy_s(szSellerID, MAX_PTID_LEN+1, req.szSellerID);
			strcpy_s(szSessionID, MAX_SESSID_LEN+1, req.szSessionID);
			strcpy_s(szBuyerIP, IP_ADDRESS_LEN+1, req.szBuyerIP);
		}
	};

	struct tagAwdAuthenReq
	{
		eBillType billType;													// 交易类型
		char szPlayerID[MAX_PTID_LEN+1];									// 账号id
		char szPlayerName[32+1];											// 角色名
		char szSessionID[MAX_SESSID_LEN+1];									// 会话id
		char szPlayerIP[IP_ADDRESS_LEN+1];									// 终端IP地址
		char szAwardPatchID[MAX_AWARDPATCH_LEN+1];							// 领奖批次号
		char szAwardNum[MAX_AWARD_LEN+1];									// 领奖号码
	};

	struct tagAwdAckReq
	{
		eBillType billType;													// 交易类型
		bool bConfirm;														// 领奖成功为true，失败为false
		char szContextID[MAX_ORDERID_LEN+1];								// 关联id
		char szPlayerID[MAX_PTID_LEN+1];									// 账号id
		char szSessionID[MAX_SESSID_LEN+1];									// 会话id
		char szPlayerIP[IP_ADDRESS_LEN+1];									// 终端IP地址
		char szAwardNum[MAX_AWARD_LEN+1];									// 领奖号码
	};

private:
	struct tagBillItem
	{
		eBillItem	eItemType;
		void		*pItem;
	};

	typedef MulTiThMap<string, tagBillItem *>			mapBill;
	typedef MulTiThMap<string, tagBillItem *>::iterator itrBill;
	static mapBill										m_CallerQueue;		// 调用队列(回调类对象)
	static mapBill										m_CallbackQueue;	// 回调队列(供回调类对象使用的回调结果)

public:
	void SendAccountAuthen( tagAccAuthenReq *pReq, CCallbackAccountAuthen *pCallback );
	void SendAccountLock( tagAccLockReq *pReq, CCallbackAccountProcess *pCallback );
	void SendAccountUnlock( tagAccUnlockReq *pReq, CCallbackAccountProcess *pCallback );
	void SendConsignTransfer( tagAccUnlockReq *pReq, CCallbackAccountProcess *pCallback );
	void SendAwardAuthen( tagAwdAuthenReq *pReq, CCallbackAwardAuthen *pCallback );
	void SendAwardAck( tagAwdAckReq *pReq, CCallbackAwardAck *pCallback );

//-------------------------------------------------------------------------------
// Logic function
// caller functions & callback functions used by BillClient & BillServer(Dll)
//-------------------------------------------------------------------------------
private:
	typedef int (*FunGSInitialize)( GSCallbackFuncDef *callbackFun, const char *configfilename, int type );
	typedef int (*FunUninitialize)( int type );	
	typedef int (*FunSendAccountAuthenRequest)( const GSBsipAccountAuthenReqDef *req );
	typedef int (*FunSendAccountLockRequest)( const GSBsipAccountLockReqDef *req );
	typedef int (*FunSendAccountUnlockRequest)( const GSBsipAccountUnlockReqDef *req );
	typedef int (*FunSendConsignTransferRequest)( const GSBsipConsignTransferReqDef *req );
	typedef int (*FunSendAwardAuthenRequest)( const GSBsipAwardAuthenReqDef *req );
	typedef int (*FunSendAwardAck)( const GSBsipAwardAckDef *req );
	typedef int (*FunGetNewId)(char *id);
	typedef int (*FunGetNewIdByParam)(int intareaId,int intGroupId, char *id);
	typedef int (*FunGetId)(int spId, int servId, int areaId, int groupId, char *id);

	GSCallbackFuncDef m_CallbackFun;

public:
	FunGSInitialize					funGSInitialize;
	FunUninitialize					funUninitialize;
	FunGetNewId						funGetNewId;
	FunGetNewIdByParam				funGetNewIdByParam;
	FunSendAccountAuthenRequest		funSendAccountAuthen;
	FunSendAccountLockRequest		funSendAccountLock;
	FunSendAccountUnlockRequest		funSendAccountUnlock;
	FunSendConsignTransferRequest	funSendConsignTransfer;
	FunSendAwardAuthenRequest		funSendAwardAuthen;
	FunSendAwardAck					funSendAwardAck;

	static void		CallbackAccountAuthen(GSBsipAccountAuthenResDef* );
	static void		CallbackAccountLock(GSBsipAccountLockResDef* );
	static void		CallbackAccountUnlock(GSBsipAccountUnlockResDef* );
	static void		CallbackConsignTransfer(GSBsipConsignTransferResDef* );
	static void		CallbackAwardAuthen(GSBsipAwardAuthenResDef* );
	static void		CallbackAwardAck(GSBsipAwardAckResDef* );


	GSCallbackFuncDef*	GetCallbacks() { return &m_CallbackFun; }
	void			GetCurrentTimeString(char* str, size_t bufsize);
	int				BsipGetFunction(HINSTANCE hDll);
	int				BsipSetCallBackFunction();

//-------------------------------------------------------------------------------
// Singleton
//-------------------------------------------------------------------------------
private:
	static CBillClient *s_pInstance;

public:
	static CBillClient *GetInstance();
};

CBillClient *GetBillClient();