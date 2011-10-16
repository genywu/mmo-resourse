#include "StdAfx.h"
#include "BillReLive.h"
#include "../Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool IncShopProcessing( CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwNum, DWORD dwTradeMode, long lTotalAmount );
extern bool PerTradeProcessing(const CGUID &SessionID, long lPlugID );
extern bool PerShopProcessing(const CGUID &SessionID, long lPlugID, long lTotalMoney, long lTotalYuanbao );

// 帐户预冻
CBillReLive::CBillReLive(const CGUID &guidBuyer, const CGUID &guidSeller, const DWORD dwIP, const CBillClient::tagAccLockReq &reqLock)
: m_guidBuyer(guidBuyer)
, m_guidSeller(guidSeller)
, m_dwIP(dwIP)
, m_tagAccLockReq(reqLock)
{}

CBillReLive::~CBillReLive(void)
{}

void CBillReLive::OnTimeOut(const string &strOrderID)
{
	char szBuyerName[128] = "";
	char szBuyerAccount[128] = "";
	CPlayer *pBuyer = GetGame()->FindPlayer(m_guidBuyer);
	if ( pBuyer )
	{
		strcpy_s(szBuyerName, 128, pBuyer->GetName());
		strcpy_s(szBuyerAccount, 128, pBuyer->GetAccount());
	}
	Log4c::Warn(ROOT_MODULE, "Req OI:%s, CI:0, TM:%d : Buyer %s(PT:%s) AccountLockRes 异常超时!", 
		strOrderID.c_str(), m_tagAccLockReq.billType, szBuyerName, szBuyerAccount );
}

void CBillReLive::OnCallback(const string strOrderID, const GSBsipAccountLockResDef *pRes)
{
	char strLog[512] = "";
	CPlayer *pBuyer = GetGame()->FindPlayer(m_guidBuyer);

	if( !pRes )
	{
		_snprintf(strLog, 512, "Req OI:%s, CI:0, TM:%d : AccountLockRes 异常为NULL!", 
			strOrderID.c_str(), m_tagAccLockReq.billType);
	}
	else if( !pBuyer )
	{
		_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d : AccountLockRes Buyer不存在于此GS!", 
			pRes->order_id, pRes->context_id, m_tagAccLockReq.billType);
	}
	else
	{
		switch( pRes->result )
		{
		case 0:			// 预冻成功
			{
				CBillClient::tagAccUnlockReq req;
				req.billType			= m_tagAccLockReq.billType;			// 交易类型
				switch( req.billType )
				{
				case BT_INCSHOP:											// 增值商店购买
					{
						req.bConfirm	= IncShopProcessing( pBuyer, m_tagAccLockReq.lGoodsID,
							m_tagAccLockReq.lGoodsNum, CIncrementShopList::TM_YUANBAO, m_tagAccLockReq.lTotalYuanbao );
					}
					break;
				case BT_TRADER:												// 个人交易购买
					{
						req.bConfirm	= PerTradeProcessing( m_tagAccLockReq.guidSessionID,
							m_tagAccLockReq.lPlugID );
					}
					break;
				case BT_PERSHOP:											// 个人商店购买
					{
						req.bConfirm	= PerShopProcessing( m_tagAccLockReq.guidSessionID,  
							m_tagAccLockReq.lPlugID, m_tagAccLockReq.lTotalMoney, m_tagAccLockReq.lTotalYuanbao );
					}
					break;
				default:
					{
						req.bConfirm	= false;
					}
					break;
				}
				req.lTotalYuanbao		= m_tagAccLockReq.lTotalYuanbao;		// 总交易额
				req.lGoodsID			= m_tagAccLockReq.lGoodsID;				// 游戏道具编号
				req.lGoodsNum			= m_tagAccLockReq.lGoodsNum;			// 游戏道具数量
				strcpy_s(req.szBuyerID, MAX_PTID_LEN+1, pRes->user_id);
				in_addr addr;
				addr.s_addr = htonl(pBuyer->GetClientIP());
				//in_addr addr = {pBuyer->GetClientIP()};
				strcpy_s(req.szBuyerIP, IP_ADDRESS_LEN+1, inet_ntoa(addr));
				strcpy_s(req.szSessionID, MAX_SESSID_LEN+1, pRes->sess_id);
				strcpy_s(req.szContextID, MAX_ORDERID_LEN+1, pRes->context_id);

			}
			break;
		case -106114:	// 玩家帐户余额不足
			{
				_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d, ERR:%d : AccountLockRes Buyer %s(PT:%s) 帐户余额不足.", 
					pRes->order_id, pRes->context_id, m_tagAccLockReq.billType, pRes->result, pBuyer->GetName(), pBuyer->GetAccount() );
			}
			break;

		default:
			{
				_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d, ERR:%d : AccountLockRes Buyer %s(PT:%s) Other Error!!!!", 
					pRes->order_id, pRes->context_id, m_tagAccLockReq.billType, pRes->result, pBuyer->GetName(), pBuyer->GetAccount() );
			}
			break;
		}
	}
	if( strlen(strLog) )
    {
        Log4c::Trace(ROOT_MODULE,strLog);
    }
}

void CBillReLive::OnCallback(const string strOrderID, const GSBsipAccountUnlockResDef *pRes)
{
	char strLog[512] = "";
	CPlayer *pBuyer = GetGame()->FindPlayer(m_guidBuyer);

	if( !pRes )
	{
		_snprintf(strLog, 512, "Req OI:%s, CI:0, TM:%d : AccountUnockRes 异常为NULL!", 
			strOrderID.c_str(), m_tagAccLockReq.billType);
	}
	else if( !pBuyer )
	{
		_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d : AccountUnockRes Buyer不存在于此GS!", 
			pRes->order_id, pRes->context_id, m_tagAccLockReq.billType);
	}
	else
	{
		switch( pRes->result )
		{
		case 0:											// 解冻成功
			{
				//pBuyer->SetYuanBao(pRes->balance);
			}
			break;

		default:
			{
				_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d, ERR:%d : AccountUnlockRes Buyer %s(PT:%s) Other Error!!!!", 
					pRes->order_id, pRes->context_id, m_tagAccLockReq.billType, pRes->result, pBuyer->GetName(), pBuyer->GetAccount() );
			}
			break;
		}
	}
	if( strlen(strLog) ) 
    {
        Log4c::Trace(ROOT_MODULE, strLog);
    }
}

void CBillReLive::OnCallback(const string strOrderID, const GSBsipConsignTransferResDef *pRes)
{
	char strLog[512]	= "";
	CPlayer *pBuyer		= GetGame()->FindPlayer(m_guidBuyer);
	CPlayer *pSeller	= GetGame()->FindPlayer(m_guidSeller);

	if( !pRes )
	{
		_snprintf(strLog, 512, "Req OI:%s, CI:0, TM:%d : ConsignTransferRes 异常为NULL!", 
			strOrderID.c_str(), m_tagAccLockReq.billType);
	}
	else if( !pBuyer )
	{
		_snprintf(strLog, 512,"Res OI:%s, CI:%s, TM:%d : ConsignTransferRes Buyer不存在于此GS!", 
			pRes->order_id, pRes->context_id, m_tagAccLockReq.billType);
	}
	else if( !pSeller )
	{
		_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d : ConsignTransferRes Seller不存在于此GS!", 
			pRes->order_id, pRes->context_id, m_tagAccLockReq.billType);
	}
	else
	{
		switch( pRes->result )
		{
		case 0:											// 解冻成功
			{
				//pBuyer->SetYuanBao(pRes->balance_src);
				//pSeller->SetYuanBao(pRes->balance_dest);
			}
			break;

		default:
			{
				_snprintf(strLog, 512, "Res OI:%s, CI:%s, TM:%d, ERR:%d : AccountUnlockRes Buyer %s(PT:%s) Seller %s(PT:%s) Other Error!!!!", 
					pRes->order_id, pRes->context_id, m_tagAccLockReq.billType, pRes->result,
					pBuyer->GetName(), pBuyer->GetAccount(), pSeller->GetName(), pSeller->GetAccount() );
			}
			break;
		}
	}
	if( strlen(strLog) ) 
    {
        Log4c::Trace(ROOT_MODULE,"UniBill", strLog);
    }
}