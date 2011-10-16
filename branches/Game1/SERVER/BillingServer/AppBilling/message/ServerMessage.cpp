#include "stdafx.h"
#include "..\..\..\..\nets\netbilling\message.h"
#include "../../../../public/GUID.h"
#include "../../../../public/GoodsExDef.h"
#include "../../../../nets/Servers.h"
#include "../../../../dbaccess/BillingDB/RsAccount.h"
#include "../CSessionFactory.h"
#include "../CSession.h"
#include "../CBillOper.h"
#include "../CGetBalanceOper.h"
#include "../CAccountLockOper.h"
#include "../CAccountUnlockOper.h"
#include "../CLockRollbackOper.h"
#include "../CBillDBManager.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern unsigned __stdcall UpdateOnlineUser2DB(void *param);

int g_nLockMsg=0;
int g_nUnlockMsg=0;
int g_nRollbackMsg=0;

int g_nLockSend=0;
int g_nUnlockSend=0;
int g_nRollbackSend=0;

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2B_SENDINFO:
		{
			char chVal=pMsg->GetChar();
			DWORD dwPort=pMsg->GetDWord();
			char strIP[256];
			pMsg->GetStr(strIP,256);
			long lSocketId=pMsg->GetSocketID();
			CGame::tagGameServerInfo* pInfo=GetGame()->GetGameServer(strIP,dwPort);
			if(pInfo)
			{
				CMessage::MapGSIDSocket(pInfo->dwIndex,lSocketId);
				long lPos=GetGame()->AddGS(pInfo->dwIndex,strIP);
				AddLogText("GameServer [%s] 连接建立 ！",strIP);
			}
			else
			{				
				GetGame()->GetNetServer_GS()->DisConn(lSocketId);
				AddLogText("GameServer [%s] 参数不正确，连接被断开!",strIP);
			}
		}
		break;
	case MSG_S2B_GET_BALANCE:
		{
			char strCdkey[256];
			pMsg->GetStr(strCdkey,256);
			CGUID guid;
			pMsg->GetGUID(guid);
			//通过CDKEY向数据库查询余额
			if(strcmp(strCdkey,"")!=0)
			{
				//CGetBalanceOper* pOper=new CGetBalanceOper(eBILL_OT_GET_BALANCE);
				CGetBalanceOper* pOper=new CGetBalanceOper(eBILL_OT_GET_BALANCE);
				pOper->SetCDKey(strCdkey);
				pOper->SetUserGuid(guid);
				pOper->SetGSID(pMsg->GetSocketID());
				GetGame()->GetBillDBManager()->AddOpers(pOper);
				/*
				DWORD dwBalance=GetGame()->GetRsAccount()->GetBalance(strCdkey);
				CMessage msg(MSG_B2S_GET_BALANCE);
				msg.Add(guid);
				msg.Add(dwBalance);
				msg.SendToGS(pMsg->GetSocketID());*/
			}
		}
		break;
	case MSG_S2B_ACCOUNT_LOCK:
		{
			/*
			* 帐户预冻消息 
			*/
			char strCdkey[256];

			g_nLockMsg++;

			CGUID playerGuid;
			BYTE btConsumeType=pMsg->GetByte();
			pMsg->GetGUID(playerGuid);
			pMsg->GetStr(strCdkey,256);
			if(strcmp(strCdkey,"")==0)
				return;
			CGUID attachGuid;
			pMsg->GetGUID(attachGuid);
			CAccountLockOper* pOper=new CAccountLockOper(eBILL_OT_ACCOUNT_LOCK);
			DWORD dwNum=pMsg->GetDWord();
			tagItemData stItemData;
			for(DWORD i=0;i<dwNum;i++)
			{
				ZeroMemory(&stItemData,sizeof(stItemData));
				stItemData.dwItemId=pMsg->GetDWord();
				stItemData.dwItemNum=pMsg->GetDWord();
				pOper->AddItemData(&stItemData);
			}
			
			long lTotalPrice=pMsg->GetLong();
			if(lTotalPrice<0)
				lTotalPrice=0;
			DWORD TradeMode = pMsg->GetDWord();
			pOper->SetConsumeType(btConsumeType);
			pOper->SetCDKey(strCdkey);
			pOper->SetUserGuid(playerGuid);
			pOper->SetGSID(pMsg->GetSocketID());
			pOper->SetTradeMode(TradeMode);
			pOper->SetLockNum(lTotalPrice);
			pOper->SetAttachGuid(attachGuid);
			GetGame()->GetBillDBManager()->AddOpers(pOper);			
		}
		break;
	case MSG_S2B_ACCOUNT_LOCK_ROLLBACK:
		{
			char strCdkey[256];

			g_nRollbackMsg++;

			pMsg->GetStr(strCdkey,256);
			if(strcmp(strCdkey,"")==0)
				return;
			LONG lRollbackNum=pMsg->GetLong();
			if(lRollbackNum<=0)
				return;
			CGUID operGuid;
			pMsg->GetGUID(operGuid);
			CLockRollbackOper* pOper=new CLockRollbackOper(eBILL_OT_ACCOUNT_LOCK_ROLLBACK);
			pOper->SetCDKey(strCdkey);
			pOper->SetLockNum(lRollbackNum);
			pOper->SetOperGuid(operGuid);
			pOper->SetGSID(pMsg->GetSocketID());
			GetGame()->GetBillDBManager()->AddOpers(pOper);
		}
		break;
	case MSG_S2B_ACCOUNT_UNLOCK:
		{
			/*
			msg.Add(strCdkey);//cd key
			msg.Add(pPlayer->GetName());//player name
			msg.Add(pPlayer->GetExID());//player guid			
			msg.Add(sessionGuid);//session guid
			msg.Add(dwGoodsId);//goods id
			msg.Add(dwGoodsNum);//goods num
			msg.Add(lLockNum);//lock num
			*/
			char strCdkey[256],strPlayerName[256],strGoodsName[256];		

			g_nUnlockMsg++;

			pMsg->GetStr(strCdkey,256);
			if(strcmp(strCdkey,"")==0)
				return;
			CGUID operGuid;
			pMsg->GetGUID(operGuid);
			pMsg->GetStr(strPlayerName,256);
			if(strcmp(strPlayerName,"")==0)
				return;
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			
			DWORD dwNum=pMsg->GetDWord();
			if(dwNum<1)
				return;			

			//通过guid查找session						
			CAccountUnlockOper* pOper=new CAccountUnlockOper(eBILL_OT_ACCOUNT_UNLOCK);
			for(DWORD i=0;i<dwNum;i++)
			{
				tagItemData stItemData;
				stItemData.dwItemId=pMsg->GetDWord();
				stItemData.dwItemNum=pMsg->GetDWord();
				pOper->AddItemData(&stItemData);
			}
			long lLockNum=pMsg->GetLong();
			DWORD dwWorldId=pMsg->GetDWord();
			pMsg->GetStr(strGoodsName,256);
			
			pOper->SetOperGuid(operGuid);
			pOper->SetUserGuid(playerGuid);
			pOper->SetCDKey(strCdkey);
			pOper->SetUserName(strPlayerName);
			//pOper->SetGoodsId(dwGoodsId);
			//pOper->SetGoodsNum(dwGoodsNum);
			pOper->SetGSID(pMsg->GetSocketID());
			pOper->SetUnlockNum(lLockNum);
			pOper->SetWorldId(dwWorldId);
			pOper->SetGoodsName(strGoodsName);
			GetGame()->GetBillDBManager()->AddOpers(pOper);			
			//GetGame()->GetRsAccount()->AccountUnlock(strCdkey,lLockNum);

			/*
			CMessage msg(MSG_B2S_ACCOUNT_UPDATED);
			msg.Add(strCdkey);
			msg.Add(sessionGuid);
			msg.SendToGS(pMsg->GetSocketID());
			*/						
		}
		break;
	}
}
