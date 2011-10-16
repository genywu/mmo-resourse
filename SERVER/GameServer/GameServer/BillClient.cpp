//-------------------------------------------------------------------------------
// 统一计费客户端管理. by Fox.		2008-01-22
//-------------------------------------------------------------------------------

#include "stdafx.h"
#include <time.h>
#include "billclient.h"
#include "BillSession.h"
#include "..\appserver\player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBillClient::mapBill		CBillClient::m_CallerQueue;	
CBillClient::mapBill		CBillClient::m_CallbackQueue;
CBillClient *				CBillClient::s_pInstance = NULL;

CBillClient::CBillClient()
{
}

CBillClient::~CBillClient()
{
}

void CBillClient::Release()
{
	funUninitialize(0);
	itrBill itCallback = m_CallbackQueue.begin();
	for( ; itCallback!=m_CallbackQueue.end(); itCallback++ )
	{
		SAFE_DELETE(itCallback->second);
	}
	m_CallbackQueue.clear();

	itrBill itCaller = m_CallerQueue.begin();
	for( ; itCaller!=m_CallerQueue.end(); itCaller++ )
	{
		SAFE_DELETE(itCaller->second);
	}
	m_CallerQueue.clear();

	if(s_pInstance != NULL)
	{
		SAFE_DELETE(s_pInstance);
	}
}

// 处理回调队列
void CBillClient::Run()
{
	map<string, tagBillItem *> mapTemp;
	m_CallbackQueue.Lock();
	mapTemp = m_CallbackQueue;
	m_CallbackQueue.clear();
	m_CallbackQueue.UnLock();

	for( map<string, tagBillItem *>::iterator it = mapTemp.begin(); it != mapTemp.end(); )
	{
		tagBillItem* pCallback = it->second;
		itrBill itCaller  = m_CallerQueue.find(it->first);
		tagBillItem* pCaller = itCaller->second;
		CCallbackBill *pCallbackBill = (CCallbackBill *)pCaller->pItem;
		if( pCallbackBill->GetLeftTime() > 0 )
		{
			pCallbackBill->SetLeftTime(pCallbackBill->GetLeftTime()-1);
			switch( pCallback->eItemType )
			{
			case BI_ACCOUNTAUTHEN:
				{
					((CCallbackAccountAuthen *)pCallbackBill)->OnCallback(it->first, (GSBsipAccountAuthenResDef*)(pCallback->pItem));
				}
				break;

			case BI_LOCK:
				{
					((CCallbackAccountProcess *)pCallbackBill)->OnCallback(it->first, (GSBsipAccountLockResDef*)(pCallback->pItem));
				}
				break;

			case BI_UNLOCK:
				{
					((CCallbackAccountProcess *)pCallbackBill)->OnCallback(it->first, (GSBsipAccountUnlockResDef*)(pCallback->pItem));
				}
				break;

			case BI_TRANSFER:
				{
					((CCallbackAccountProcess *)pCallbackBill)->OnCallback(it->first, (GSBsipConsignTransferResDef*)(pCallback->pItem));
				}
				break;

			case BI_AWARDAUTHEN:
				{
					((CCallbackAwardAuthen *)pCallbackBill)->OnCallback(it->first, (GSBsipAwardAuthenResDef*)(pCallback->pItem));
				}
				break;

			case BI_AWARDACK:
				{
					((CCallbackAwardAck *)pCallbackBill)->OnCallback(it->first, (GSBsipAwardAckResDef*)(pCallback->pItem));
				}
				break;

			default:
				break;
			}
			SAFE_DELETE(pCallback->pItem);		// 回调结果结构体变量
			SAFE_DELETE(pCallbackBill);
			SAFE_DELETE(pCaller);
			SAFE_DELETE(it->second);
			m_CallerQueue.erase(itCaller);
			it = mapTemp.erase(it);
		}
		else
		{
			pCallbackBill->OnTimeOut(itCaller->first);
			SAFE_DELETE(pCallback->pItem);		// 回调结果结构体变量
			SAFE_DELETE(pCallbackBill);
			SAFE_DELETE(pCaller);
			m_CallerQueue.erase(itCaller);
			it = mapTemp.erase(it);
		}
	}
}

void CBillClient::SendAccountAuthen( tagAccAuthenReq *pReq, CCallbackAccountAuthen *pCallback )
{
	GSBsipAccountAuthenReqDef req = {0};

	req.user_type			= 1;
	req.uid_type			= 1;
	req.pay_type			= 3;
	req.serv_id_player		= -1;
	req.area_id_player		= -1;
	req.group_id_player		= -1;
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.indication_authen	= 2;
	funGetNewId(req.sess_id);
	funGetNewId(req.order_id);
	strcpy_s(pReq->szSessionID, MAX_SESSID_LEN+1, req.sess_id);
	strcpy_s(req.user_id, MAX_PTID_LEN+1, pReq->szPlayerID);
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szPlayerIP);
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendAccountAuthen(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s AccountAuthen消息失败!",
			req.order_id, "0", nRet, req.user_id );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_ACCOUNTAUTHEN;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}

void CBillClient::SendAccountLock( tagAccLockReq *pReq, CCallbackAccountProcess *pCallback )
{
	GSBsipAccountLockReqDef req = {0};
	req.uid_type			= 1;								// PT账号
	req.app_type			= pReq->billType;					// 交易类型
	req.status				= 2;
	req.couple_type			= 2;
	req.fee					= 0;								// 填写手续费
	req.pay_type			= 3;								// 盛大点券
	req.lock_period			= 0;								// 以计费后台为准，默认为20分钟回滚
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.serv_id_player		= -1;
	req.area_id_player		= -1;
	req.group_id_player		= -1;
	req.indication_lock		= 1;
	req.item_num			= 1;
	req.goods_info[0].item_id		= pReq->lGoodsID;			// 填写道具ID
	req.goods_info[0].item_num		= pReq->lGoodsNum;			// 填写道具数量
	req.goods_info[0].item_amount	= pReq->lTotalYuanbao;		// 填写道具总价
	req.goods_info[0].serv_id_item	= -1;
	req.goods_info[0].area_id_item	= -1;
	req.goods_info[0].group_id_item = -1;
	req.goods_info[0].discount		= 100;
	funGetNewId(req.order_id);
	funGetNewId(req.context_id);
	strcpy_s(req.user_id, MAX_PTID_LEN+1, pReq->szBuyerID);
	strcpy_s(req.sess_id, MAX_SESSID_LEN+1, pReq->szSessionID);
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szBuyerIP);
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendAccountLock(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s AccountLock消息失败!",
			req.order_id, req.context_id, nRet, req.user_id );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_LOCK;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}

void CBillClient::SendAccountUnlock( tagAccUnlockReq *pReq, CCallbackAccountProcess *pCallback )
{
	GSBsipAccountUnlockReqDef req = {0};

	req.uid_type			= 1;
	req.app_type			= pReq->billType;
	req.status				= 2;
	req.confirm				= pReq->bConfirm ? 1 : 2;
	req.couple_type			= 2;
	req.responsable			= 1;
	req.pay_type			= 3;								// 盛大点券
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.serv_id_player		= -1;
	req.area_id_player		= -1;
	req.group_id_player		= -1;
	req.indication_unlock	= 1;
	req.item_num			= 1; 
	req.amount				= pReq->lTotalYuanbao;				// 填写消耗金额
	req.goods_info[0].item_id		= pReq->lGoodsID;			// 填写道具ID
	req.goods_info[0].item_num		= pReq->lGoodsNum;			// 填写道具数量
	req.goods_info[0].item_amount	= pReq->lTotalYuanbao;		// 填写道具总价
	req.goods_info[0].serv_id_item	= -1;
	req.goods_info[0].area_id_item	= -1;
	req.goods_info[0].group_id_item = -1;
	req.goods_info[0].discount		= 100;
	funGetNewId(req.order_id);
	strcpy_s(req.context_id, MAX_ORDERID_LEN+1, pReq->szContextID);
	strcpy_s(req.user_id, MAX_PTID_LEN+1, pReq->szBuyerID);
	strcpy_s(req.sess_id, MAX_SESSID_LEN+1, pReq->szSessionID);						// session id 为用户一次登入游戏的ID
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szBuyerIP);					// 填写玩家终端的IP地址
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendAccountUnlock(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s AccountUnlock消息失败!",
			req.order_id, req.context_id, nRet, req.user_id );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_UNLOCK;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}

void CBillClient::SendConsignTransfer( tagAccUnlockReq *pReq, CCallbackAccountProcess *pCallback )
{
	GSBsipConsignTransferReqDef req = {0};

	req.uid_type_dest		= 1;
	req.uid_type_src		= 1;
	req.app_type			= pReq->billType;
	req.fee_dest			= 0;
	req.fee_src				= 0;
	req.pay_type			= 3;								// 盛大点券
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.serv_id_player_dest = req.serv_id_player_src = -1;
	req.area_id_player_dest = req.area_id_player_src = -1;
	req.group_id_player_dest = req.group_id_player_src = -1;
	req.indication_unlock	= 1;
	req.item_num			= 1;
	req.amount				= pReq->lTotalYuanbao;				// 填写消耗金额
	req.goods_info[0].item_id		= pReq->lGoodsID;			// 填写道具ID
	req.goods_info[0].item_num		= pReq->lGoodsNum;			// 填写道具数量
	req.goods_info[0].item_amount	= pReq->lTotalYuanbao;		// 填写道具总价
	req.goods_info[0].serv_id_item = -1;
	req.goods_info[0].area_id_item = -1;
	req.goods_info[0].group_id_item = -1;
	req.goods_info[0].discount = 100;
	funGetNewId(req.order_id);
	strcpy_s(req.context_id, MAX_ORDERID_LEN+1, pReq->szContextID);
	strcpy_s(req.sess_id, MAX_SESSID_LEN+1, pReq->szSessionID);						// session id 为用户一次登入游戏的ID
	strcpy_s(req.user_id_src, MAX_PTID_LEN+1, pReq->szBuyerID);
	strcpy_s(req.user_id_dest, MAX_PTID_LEN+1, pReq->szSellerID);
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szBuyerIP);
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendConsignTransfer(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s ConsignTransfer消息失败!",
			req.order_id, req.context_id, nRet, req.user_id_src );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_TRANSFER;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}

void CBillClient::SendAwardAuthen( tagAwdAuthenReq *pReq, CCallbackAwardAuthen *pCallback )
{
	GSBsipAwardAuthenReqDef req = {0};

	req.user_type			= 1;
	req.uid_type			= 1;								// PT账号
	req.award_type			= pReq->billType;					// 与用户账号邦定的领奖（与award_patchid联合使用）
	req.couple_type			= 2;
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.serv_id_player		= -1;
	req.area_id_player		= -1;
	req.group_id_player		= -1;
	req.current_level		= 0;
	req.award_level			= 0;								// 游戏需要询问玩家要领取哪个级别的奖品
	funGetNewId(req.context_id);
	funGetNewId(req.order_id);
	strcpy_s(req.sess_id, MAX_SESSID_LEN+1, pReq->szSessionID);
	strcpy_s(req.user_id, MAX_PTID_LEN+1, pReq->szPlayerID);
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szPlayerIP);					// 填写玩家终端的IP地址
	strcpy_s(req.role_name, 32+1, pReq->szPlayerName);					// 角色名
	strcpy_s(req.award_patchid, MAX_AWARDPATCH_LEN+1, pReq->szAwardPatchID);			// 领奖批次号，一个批次号对应一个活动
	strcpy_s(req.award_num, MAX_AWARD_LEN+1, pReq->szAwardNum);					// 领奖号码
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendAwardAuthen(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s AwardAuthen消息失败!",
			req.order_id, req.context_id, nRet, req.user_id );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_AWARDAUTHEN;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}

void CBillClient::SendAwardAck( tagAwdAckReq *pReq, CCallbackAwardAck *pCallback )
{
	GSBsipAwardAckDef req = {0};

	req.confirm				= pReq->bConfirm ? 1 : 2;
	req.uid_type			= 1;								// PT账号
	req.award_type			= pReq->billType;
	req.platform_type		= 1;
	req.endpoint_type		= 1;
	req.serv_id_player		= -1;
	req.area_id_player		= -1;
	req.group_id_player		= -1;
	funGetNewId(req.order_id);
	strcpy_s(req.context_id, MAX_ORDERID_LEN+1, pReq->szContextID);
	strcpy_s(req.sess_id, MAX_SESSID_LEN+1, pReq->szSessionID);
	strcpy_s(req.user_id, MAX_PTID_LEN+1, pReq->szPlayerID);
	strcpy_s(req.endpoint_ip, IP_ADDRESS_LEN+1, pReq->szPlayerIP);					// 填写玩家终端的IP地址
	strcpy_s(req.award_num, MAX_AWARD_LEN+1, pReq->szAwardNum);
	GetCurrentTimeString(req.award_time, 20);
	GetCurrentTimeString(req.call_time, 20);

	int nRet = funSendAwardAck(&req);
	if (nRet != 0)
	{
		char strDebug[256] = "";
		_snprintf(strDebug, 256, "OI:%s, CI:%s, %d : 发送Player %s AwardAck消息失败!",
			req.order_id, req.context_id, nRet, req.user_id );
		PutStringToFile("UniBill", strDebug);
		return;
	}

	tagBillItem *pBillItem		= new tagBillItem;
	pBillItem->eItemType		= BI_AWARDACK;
	pBillItem->pItem			= pCallback;
	m_CallerQueue[req.order_id]	= pBillItem;
}


// 帐号查询
void CBillClient::CallbackAccountAuthen(GSBsipAccountAuthenResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_ACCOUNTAUTHEN;
	GSBsipAccountAuthenResDef *pRes = new GSBsipAccountAuthenResDef;
	memcpy(pRes, p, sizeof(GSBsipAccountAuthenResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::CallbackAccountLock(GSBsipAccountLockResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_LOCK;
	GSBsipAccountLockResDef *pRes = new GSBsipAccountLockResDef;
	memcpy(pRes, p, sizeof(GSBsipAccountLockResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::CallbackAccountUnlock(GSBsipAccountUnlockResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_UNLOCK;
	GSBsipAccountUnlockResDef *pRes = new GSBsipAccountUnlockResDef;
	memcpy(pRes, p, sizeof(GSBsipAccountUnlockResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::CallbackConsignTransfer(GSBsipConsignTransferResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_TRANSFER;
	GSBsipConsignTransferResDef *pRes = new GSBsipConsignTransferResDef;
	memcpy(pRes, p, sizeof(GSBsipConsignTransferResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::CallbackAwardAuthen(GSBsipAwardAuthenResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_AWARDAUTHEN;
	GSBsipAwardAuthenResDef *pRes = new GSBsipAwardAuthenResDef;
	memcpy(pRes, p, sizeof(GSBsipAwardAuthenResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::CallbackAwardAck(GSBsipAwardAckResDef *p)
{
	//锁住资源
	m_CallbackQueue.Lock();

	tagBillItem *pBillItem = new tagBillItem;
	pBillItem->eItemType = BI_AWARDACK;
	GSBsipAwardAckResDef *pRes = new GSBsipAwardAckResDef;
	memcpy(pRes, p, sizeof(GSBsipAwardAckResDef));
	pBillItem->pItem = pRes;
	m_CallbackQueue[p->order_id] = pBillItem;

	//释放资源
	m_CallbackQueue.UnLock();
}

void CBillClient::GetCurrentTimeString(char* str, size_t bufsize)
{
	time_t t;
	struct tm *tm_time;
	time(&t);
	tm_time = localtime(&t);
	_snprintf(str, bufsize, "%04d-%02d-%02d %02d:%02d:%02d", 
		tm_time->tm_year+1900, tm_time->tm_mon+1, 
		tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
}          

int CBillClient::BsipSetCallBackFunction()
{
	memset(&m_CallbackFun, 0 ,sizeof(GSCallbackFuncDef));
	m_CallbackFun.CallbackAccountAuthenRes		=	CallbackAccountAuthen;
	m_CallbackFun.CallbackAccountLockRes		=	CallbackAccountLock;
	m_CallbackFun.CallbackAccountUnlockRes  	=	CallbackAccountUnlock;
	m_CallbackFun.CallbackConsignTransferRes  	=	CallbackConsignTransfer;
	m_CallbackFun.CallbackAwardAuthenRes	    =	CallbackAwardAuthen;
	m_CallbackFun.CallbackAwardAckRes	        =	CallbackAwardAck;

	return 0;
}

int CBillClient::BsipGetFunction(HINSTANCE hDll)
{
	if (hDll == NULL )
	{
		return -1;
	}
	funGSInitialize               =  (FunGSInitialize)GetProcAddress(hDll, "GSInitialize");
	funUninitialize               =  (FunUninitialize)GetProcAddress(hDll, "Uninitialize");
	funSendAccountAuthen          =  (FunSendAccountAuthenRequest)GetProcAddress(hDll, "SendAccountAuthenRequest");
	funSendAccountLock            =  (FunSendAccountLockRequest)GetProcAddress(hDll, "SendAccountLockRequest");
	funSendAccountUnlock          =  (FunSendAccountUnlockRequest)GetProcAddress(hDll, "SendAccountUnlockRequest");
	funSendAwardAuthen            =  (FunSendAwardAuthenRequest)GetProcAddress(hDll, "SendAwardAuthenRequest");
	funSendAwardAck               =  (FunSendAwardAck)GetProcAddress(hDll, "SendAwardAck");
	funGetNewId                   =  (FunGetNewId)GetProcAddress(hDll, "GetNewId");
	funGetNewIdByParam            =  (FunGetNewIdByParam)GetProcAddress(hDll, "GetNewIdByParam");
	funSendConsignTransfer        =  (FunSendConsignTransferRequest)GetProcAddress(hDll, "SendConsignTransferRequest");
	if( 
		(funGSInitialize              ==NULL)||
		(funUninitialize              ==NULL)||
		(funSendAccountAuthen         ==NULL)||
		(funSendAwardAuthen           ==NULL)||
		(funSendAwardAck              ==NULL)||
		(funGetNewId                  ==NULL)||
		(funGetNewIdByParam           ==NULL)
		)
	{
		return -1;
	}
	return 0;
}

CBillClient *CBillClient::GetInstance()
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new CBillClient();
	}
	return s_pInstance;
}

CBillClient *GetBillClient()
{
	return CBillClient::GetInstance();
}