#include "stdafx.h"
#include ".\billingplayermanager.h"
#include "..\BillingServer\Game.h"
#include "..\Nets\NetBilling\ServerForGS.h"
#include "..\Nets\NetBilling\Message.h"
#include "..\DBACCESS\DBBilling\RsPlayerAccount.h"

// ÏìÓ¦BillingÏûÏ¢
void OnBillingMessage(CMessage* pMsg)
{
	long lRes = 0;
	switch(pMsg->GetType())
	{
		// GS request client's account info
	case CMessage::MSG_S2B_BILLING_AUTHEN:
		{
			char szUserID[32] = "";
			pMsg->GetStr(szUserID, 32);
			if( !szUserID[0] )
				return;

			long lPlayerID = pMsg->GetLong();
			long lGSID = pMsg->GetClientSocketID();

			CBillingPlayerManager::tagAccInfo accInfo( lPlayerID, szUserID, lGSID );
			CBillingPlayerManager::PushACQueue(accInfo);

			char szDebug[256] = "";
			sprintf(szDebug, "%d[AUTHEN] %d(%s) ...", lGSID, lPlayerID, szUserID);
			AddLogText(szDebug);
		}
		break;

		// GS request foot the bill
	case CMessage::MSG_S2B_BILLING_INCBUY:
		{
			long lBuyerID = pMsg->GetLong();
			char szBuyerID[32] = "";
			char szBuyerIP[32] = "";
			char szBuyerName[32] = "";
			pMsg->GetStr(szBuyerID, 32);
			pMsg->GetStr(szBuyerIP, 32);
			pMsg->GetStr(szBuyerName, 32);
			DWORD dwYuanbao = pMsg->GetDWord();
			long lGoodsID = pMsg->GetDWord();
			long lGoodsNum = pMsg->GetDWord();
			long lDikouID = pMsg->GetDWord();
			long lLSID = pMsg->GetLong();
			long lWSID = pMsg->GetLong();
			long lGSID = pMsg->GetClientSocketID();

			CBillingPlayerManager::tagTradeNode trdNode( 0, lBuyerID, 0, szBuyerID, "", szBuyerIP, "", 
				szBuyerName, "", dwYuanbao, lGoodsID, lGoodsNum, lGSID, lDikouID, 0, lLSID, lWSID, CGUID::GUID_INVALID );
			CBillingPlayerManager::PushTRQueue(trdNode);

			char szDebug[256] = "";
			sprintf(szDebug, "[LS:%d][WS:%d]%d[INCBUY] Buyer: %d(%s:%s) From %s...",
				lLSID, lWSID, lGSID, lBuyerID, szBuyerID, szBuyerName, szBuyerIP);
			AddLogText(szDebug);
		}
		break;

		// GS request foot the bill
	case CMessage::MSG_S2B_BILLING_TRADE:
		{
			long lType = pMsg->GetLong();
			long lBuyerID = pMsg->GetLong();
			long lSellerID = pMsg->GetLong();
			char szBuyerID[32] = "";
			char szSellerID[32] = "";
			char szBuyerIP[32] = "";
			char szSellerIP[32] = "";
			char szBuyerName[32] = "";
			char szSellerName[32] = "";
			pMsg->GetStr(szBuyerID, 32);
			pMsg->GetStr(szSellerID, 32);
			pMsg->GetStr(szBuyerIP, 32);
			pMsg->GetStr(szSellerIP, 32);
			pMsg->GetStr(szBuyerName, 32);
			pMsg->GetStr(szSellerName, 32);
			DWORD dwYuanbao = pMsg->GetDWord();
			long lGoodsID = pMsg->GetLong();
			long lGoodsNum = pMsg->GetLong();
			long lSessID = pMsg->GetLong();
			long lPlugID = pMsg->GetLong();
			long lLSID = pMsg->GetLong();
			long lWSID = pMsg->GetLong();
			CGUID guidGoods;
			pMsg->GetGUID( guidGoods );
			long lGSID = pMsg->GetClientSocketID();

			CBillingPlayerManager::tagTradeNode trdNode( lType, lBuyerID, lSellerID, szBuyerID, szSellerID, szBuyerIP, szSellerIP, 
				szBuyerName, szSellerName, dwYuanbao, lGoodsID, lGoodsNum, lGSID, lSessID, lPlugID, lLSID, lWSID, guidGoods );
			CBillingPlayerManager::PushTRQueue(trdNode);

			char szDebug[256] = "";
			sprintf(szDebug, "[LS:%d][WS:%d]%d[TRADE] Buyer: %d(%s:%s) From %s, Seller: %d(%s:%s) From %s.....",
				lLSID, lWSID, lGSID, lBuyerID, szBuyerID, szBuyerName, szBuyerIP,
				lSellerID, szSellerID, szBuyerName, szSellerIP);
			AddLogText(szDebug);
		}
		break;
	}
}