 /*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
* 
* 文件名称：bsip.h
* 摘    要：统一计费系统消息头定义
*           client与LBS、CBS间通讯用的消息体定义
* 当前版本：1.4.2
* 作    者：李闻
* 完成日期：2005年9月6日
*
* 修改内容：在1.3的版本上进行修改，删除弃用的消息
*           添加寄售交易使用的消息
* 修改时间：2005年9月6日
*
* 修改内容：变更版本号Version
* 修改时间：2005年10月8日
*
* 修改内容：添加Notify消息汇总接口和相关宏定义
* 修改时间：2005年10月13日
*
* 修改内容：修改包头定义，添加ip、port、send_time等字段
* 修改时间：2005年10月13日
*
* 修改内容：修改版本号
* 修改时间：2005年10月19日
*
* 修改内容：修改版本号
* 修改时间：2005年11月25日
*
* 修改内容：修改版本，增加NORMAL_STAT消息
* 修改时间：2005年12月27日
*
* 修改内容：修改了 AUTHEN消息, GOLDCONSUME 消息, DEPOSIT 消息
* 修改时间：2006年2月21日
*
* 修改内容：增加了 account_lock_ex,account_unlock_ex 消息
* 修改时间：2006年2月24日
*
* 修改内容：增加了 award_ack_res 消息
* 修改时间：2006年2月27日
*
* 修改内容：增加了 item_sync 消息 和 gs_route 消息
* 修改时间：2006年8月16日
*
* 修改内容：增加了 统一对战平台用的消息
* 修改时间：2006年11月15日
*
* 修改内容：增加ConsignTransfer消息
* 修改时间：2007-5-3 9:25
*
* 修改内容：增加EkeyBind消息
* 修改时间：2007-6-19 15:54
*
* 修改内容：修改BsipNormalStatReq消息补全1.6中的13个消息，修改BsipStat加入响应延迟统计
* 修改时间：2007-8-1
*
*/

/*
*通讯用的数据包，包括消息头和消息体
*/
#ifndef _BSIP_H_
#define _BSIP_H_

#include "bsipmsgbs.h"

//define the message type
#define BSIP_AUTHOR_REQ					0x00000001	//认证请求消息
#define BSIP_AUTHOR_RES					0x80000001	//认证响应消息
#define BSIP_ACCOUNT_REQ				0x00000002  //扣费请求消息
#define BSIP_ACCOUNT_RES				0x80000002	//扣费响应消息
#define BSIP_AUTHEN_ACCOUNT_REQ			0x00000003	//认证请求消息
#define BSIP_AUTHEN_ACCOUNT_RES			0x80000003	//认证响应消息
#define BSIP_LOCK_ACCOUNT_REQ			0x00000004	//用户账号资金预冻结请求消息
#define BSIP_LOCK_ACCOUNT_RES			0x80000004	//用户账号资金预冻结响应消息
#define BSIP_UNLOCK_ACCOUNT_REQ			0x00000005	//用户账号资金扣减请求消息
#define BSIP_UNLOCK_ACCOUNT_RES			0x80000005	//用户账号资金扣减响应消息
#define BSIP_AWARD_AUTHEN_REQ			0x00000006	//发奖认证类型
#define BSIP_AWARD_AUTHEN_RES			0x80000006	//发奖响应类型
#define BSIP_AWARD_ACK					0x00000007	//发奖确认消息类型
#define BSIP_AWARD_ACK_RES				0x80000007	//发奖确认消息类型
#define BSIP_LOCK_CONSIGN_REQ			0x00000008	//寄售用户账号资金预冻结请求消息
#define BSIP_LOCK_CONSIGN_RES			0x80000008	//寄售用户账号资金预冻结响应消息
#define BSIP_UNLOCK_CONSIGN_REQ			0x00000009	//寄售用户账号资金扣减请求消息
#define BSIP_UNLOCK_CONSIGN_RES			0x80000009	//寄售用户账号资金扣减响应消息
#define BSIP_CONSIGN_DEPOSIT_REQ		0x00000010	//寄售充值请求消息
#define BSIP_CONSIGN_DEPOSIT_RES		0x80000010	//寄售充值响应消息
#define BSIP_GOLDDEPOSIT_REQ			0x00000011	//元宝充值请求消息
#define BSIP_GOLDDEPOSIT_RES			0x80000011	//元宝充值响应消息

#define BSIP_GOLDCONSUME_LOCK_REQ		0x00000012	//元宝消耗预冻请求消息
#define BSIP_GOLDCONSUME_LOCK_RES		0x80000012	//元宝消耗预冻响应消息
#define BSIP_GOLDCONSUME_UNLOCK_REQ		0x00000020	//元宝消耗解冻请求消息
#define BSIP_GOLDCONSUME_UNLOCK_RES		0x80000020	//元宝消耗解冻响应消息

#define BSIP_ACCOUNT_IMPORT_REQ			0x00000021	//给用户充值消息
#define BSIP_ACCOUNT_IMPORT_RES			0x80000021	//给用户充值响应消息

#define BSIP_NOTIFY_REQ					0x00000015	//心跳请求消息
#define BSIP_NOTIFY_RES					0x80000015	//心跳响应消息
#define BSIP_NORMAL_STAT_REQ			0x00000016	//GS消息统计消息
#define BSIP_LOCK_ACCOUNT_EX_REQ		0x00000024	//用户账号资金预冻结请求消息
#define BSIP_LOCK_ACCOUNT_EX_RES		0x80000024	//用户账号资金预冻结响应消息
#define BSIP_UNLOCK_ACCOUNT_EX_REQ		0x00000025	//用户账号资金扣减请求消息
#define BSIP_UNLOCK_ACCOUNT_EX_RES		0x80000025	//用户账号资金扣减响应消息
#define BSIP_ITEMSYNC_REQ       		0x00000026	//物品同步请求消息，cbs->client
#define BSIP_ITEMSYNC_RES       		0x80000026	//物品同步响应消息，client->cbs
#define BSIP_GSROUTE_REQ        		0x00000027	//GS路由信息请求消息，lbs->cbs
#define BSIP_GSROUTE_RES        		0x80000027	//GS路由信息响应消息，cbs->lbs
#define BSIP_TRANSFER_REQ				0x00000028	//转账请求消息
#define BSIP_TRANSFER_RES				0x80000028	//转账响应消息

#define BSIP_TOKENDEPOSIT_REQ			0x00000029	//抵用券充值请求消息
#define BSIP_TOKENDEPOSIT_RES			0x80000029	//抵用券充值响应消息

// 2007-3-3 14:40 zlc add
#define BSIP_CONSIGN_TRANSFER_REQ		0x00000030	//寄售转帐请求消息
#define BSIP_CONSIGN_TRANSFER_RES		0x80000030	//寄售转帐响应消息

// 2007-6-19 15:53 zlc add
#define BSIP_EKEY_BIND_REQ              0x00000031	//寄售转帐请求消息
#define BSIP_EKEY_BIND_RES              0x80000031	//寄售转帐响应消息

/*-------------------------------时长计费-------------------------------*/
/*---------------------------------授权---------------------------------*/
typedef struct BsipAuthorInfoReq
{
	BsipHeadDef			msg_head;				// 消息类型，如BSIP_AUTHOR_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAuthorReqDef	msg_body;
}BsipAuthorInfoReqDef;

typedef struct BsipAuthorInfoRes
{
	BsipHeadDef			msg_head;				// 消息类型，如BSIP_AUTHOR_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAuthorResDef	msg_body;
}BsipAuthorInfoResDef;

/*---------------------------------扣费---------------------------------*/
typedef struct BsipAccountInfoReq
{
	BsipHeadDef			msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountReqDef	msg_body;
}BsipAccountInfoReqDef;
                                                 
typedef struct BsipAccountInfoRes
{
	BsipHeadDef			msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountResDef	msg_body;
}BsipAccountInfoResDef;

/*-------------------------------按次计费-------------------------------*/
/*-------------------------------账户认证-------------------------------*/
typedef struct BsipAccountAuthenInfoReq
{
	BsipHeadDef			        msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountAuthenReqDef	    msg_body;

}BsipAccountAuthenInfoReqDef;

typedef struct BsipAccountAuthenInfoRes
{
	BsipHeadDef			        msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountAuthenResDef	    msg_body;
}BsipAccountAuthenInfoResDef;

/*---------------------------------预冻---------------------------------*/
typedef struct BsipAccountLockInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountLockReqDef	msg_body;				
}BsipAccountLockInfoReqDef;

typedef struct BsipAccountLockInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountLockResDef	msg_body;			
}BsipAccountLockInfoResDef;

/*---------------------------------解冻---------------------------------*/
typedef struct BsipAccountUnlockInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, Bsip_LOCK_ACCOUNT_RES等
	BsipAccountUnlockReqDef	msg_body;			
}BsipAccountUnlockInfoReqDef;

typedef struct BsipAccountUnlockInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountUnlockResDef	msg_body;			
}BsipAccountUnlockInfoResDef;

// 2007-3-3 14:40 zlc add
/*-------------------------------寄售充值-------------------------------*/
typedef struct BsipConsignTransferInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, Bsip_LOCK_ACCOUNT_RES等
	BsipConsignTransferReqDef	msg_body;			
}BsipConsignTransferInfoReqDef;

typedef struct BsipConsignTransferInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignTransferResDef	msg_body;			
}BsipConsignTransferInfoResDef;

/*---------------------------------预冻扩展---------------------------------*/
typedef struct BsipAccountLockExInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountLockExReqDef	msg_body;				
}BsipAccountLockExInfoReqDef;

typedef struct BsipAccountLockExInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountLockExResDef	msg_body;			
}BsipAccountLockExInfoResDef;

/*---------------------------------解冻扩展---------------------------------*/
typedef struct BsipAccountUnlockExInfoReq
{
	BsipHeadDef					msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, Bsip_LOCK_ACCOUNT_RES等
	BsipAccountUnlockExReqDef	msg_body;			
}BsipAccountUnlockExInfoReqDef;

typedef struct BsipAccountUnlockExInfoRes
{
	BsipHeadDef					msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountUnlockExResDef	msg_body;			
}BsipAccountUnlockExInfoResDef;

/*---------------------------------领奖---------------------------------*/
/*-------------------------------领奖认证-------------------------------*/
typedef struct BsipAwardAuthenInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAwardAuthenReqDef	msg_body;				
}BsipAwardAuthenInfoReqDef;

typedef struct BsipAwardAuthenInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAwardAuthenResDef	msg_body;			
}BsipAwardAuthenInfoResDef;

/*-------------------------------领奖确认-------------------------------*/
typedef struct BsipAwardAckInfo
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAwardAckDef			msg_body;			
}BsipAwardAckInfoDef;

typedef struct BsipAwardAckInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAwardAckResDef		msg_body;			
}BsipAwardAckInfoResDef;

/*-------------------------------寄售交易-------------------------------*/
/*-------------------------------寄售预冻-------------------------------*/
typedef struct BsipConsignLockInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignLockReqDef	msg_body;				
}BsipConsignLockInfoReqDef;

typedef struct BsipConsignLockInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignLockResDef	msg_body;			
}BsipConsignLockInfoResDef;

/*-------------------------------寄售解冻-------------------------------*/
typedef struct BsipConsignUnlockInfoReq
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, Bsip_LOCK_ACCOUNT_RES等
	BsipConsignUnlockReqDef	msg_body;			
}BsipConsignUnlockInfoReqDef;

typedef struct BsipConsignUnlockInfoRes
{
	BsipHeadDef				msg_head;			// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignUnlockResDef	msg_body;			
}BsipConsignUnlockInfoResDef;

/*-------------------------------寄售充值-------------------------------*/
typedef struct BsipConsignDepositInfoReq
{
	BsipHeadDef				 msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignDepositReqDef msg_body;
}BsipConsignDepositInfoReqDef;

typedef struct BsipConsignDepositInfoRes
{
	BsipHeadDef				 msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipConsignDepositResDef msg_body;
}BsipConsignDepositInfoResDef;

/*-------------------------------元宝交易-------------------------------*/
/*-------------------------------元宝充值-------------------------------*/
typedef struct BsipGoldDepositInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipGoldDepositReqDef	msg_body;
}BsipGoldDepositInfoReqDef;

typedef struct BsipGoldDepositInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipGoldDepositResDef	msg_body;
}BsipGoldDepositInfoResDef;

//抵用券
typedef struct BsipTokenDepositInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipTokenDepositReqDef	msg_body;
}BsipTokenDepositInfoReqDef;

typedef struct BsipTokenDepositInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipTokenDepositResDef	msg_body;
}BsipTokenDepositInfoResDef;

/*-------------------------------元宝换时长-------------------------------*/
typedef struct BsipGoldConsumeLockInfoReq
{
	BsipHeadDef					msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipGoldConsumeLockReqDef	msg_body;
}BsipGoldConsumeLockInfoReqDef;

typedef struct BsipGoldConsumeLockInfoRes
{
	BsipHeadDef					msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipGoldConsumeLockResDef	msg_body;
}BsipGoldConsumeLockInfoResDef;

typedef struct BsipGoldConsumeUnlockInfoReq
{
	BsipHeadDef					msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_Unlock_ACCOUNT_RES等
	BsipGoldConsumeUnlockReqDef	msg_body;
}BsipGoldConsumeUnlockInfoReqDef;

typedef struct BsipGoldConsumeUnlockInfoRes
{
	BsipHeadDef					msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_Unlock_ACCOUNT_RES等
	BsipGoldConsumeUnlockResDef	msg_body;
}BsipGoldConsumeUnlockInfoResDef;

/*-------------------------------给用户充值-------------------------------*/
typedef struct BsipAccountImportInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountImportReqDef	msg_body;
}BsipAccountImportInfoReqDef;

typedef struct BsipAccountImportInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipAccountImportResDef	msg_body;
}BsipAccountImportInfoResDef;


/*-------------------------------统计消息-------------------------------*/
typedef struct BsipNormalStatInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipNormalStatReqDef	msg_body;
}BsipNormalStatInfoReqDef;

/*-------------------------------发货系统-------------------------------*/
typedef struct BsipItemSyncInfoReq
{
	BsipHeadDef       		msg_head;
	BsipItemSyncReqDef  	msg_body;
} BsipItemSyncInfoReqDef;    						//物品同步请求消息

typedef struct BsipItemSyncInfoRes
{
	BsipHeadDef       		msg_head;
	BsipItemSyncResDef  	msg_body;
} BsipItemSyncInfoResDef;    						//物品同步响应消息

/*--------------------------------GS路由--------------------------------*/
typedef struct BsipGsRouteInfoReq
{
	BsipHeadDef       		msg_head;
	BsipGsRouteReqDef  		msg_body;
} BsipGsRouteInfoReqDef;							//GS路由请求消息

typedef struct BsipGsRouteInfoRes
{
	BsipHeadDef       		msg_head;
	BsipGsRouteResDef  		msg_body;
} BsipGsRouteInfoResDef;    						//GS路由消息响应

/*-------------------------------密保绑定-------------------------------*/
typedef struct BsipEkeyBindInfoReq
{
	BsipHeadDef             msg_head;
	BsipEkeyBindReqDef      msg_body;
}BsipEkeyBindInfoReqDef;                            // 密宝绑定消息

typedef struct BsipEkeyBindInfoRes
{
	BsipHeadDef       		msg_head;
	BsipEkeyBindResDef  		msg_body;
}BsipEkeyBindInfoResDef;    						//密宝绑定消息响应

#endif
