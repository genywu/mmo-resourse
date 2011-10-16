#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnCostMessage(CMessage* pMsg)
{
	// 传2不需要发计费信息 bugs 2006.06.05
	switch(pMsg->GetType())
	{
	// 改变场景
	case MSG_S2W_COST_CHANGEREGION:		
		//if(GetGame()->GetSetup()->bUseCostSys &&
		//	GetGame()->GetCostNetClient()!=NULL &&
		//	GetGame()->GetSetup()->bSendRegionMsgToCost)
		//{
		//	pMsg->SetType(MSG_W2COST_COST_CHANGE);
		//	pMsg->Add(GetGame()->GetSetup()->dwNumber);
		//	pMsg->Add(GetGame()->GetCostNetClient()->GetCostTimePoint());			
		//	pMsg->SendToCost();
		//}
		break;

	// 发送一个显示消息给玩家
	case MSG_COST2W_COST_INFORMATION:
		//{
		//	if(GetGame()->GetSetup()->bUseCostSys)
		//	{
		//		char strCDKey[256];
		//		char strInfo[256];
		//		pMsg -> GetStr( strCDKey, 256 );
		//		pMsg->GetStr(strInfo,256);

		//		list<UINT>::iterator it = GetGame() -> GetOnlinePlayerList() -> begin();
		//		for( ; it != GetGame() -> GetOnlinePlayerList() -> end(); it ++ )
		//		{
		//			CPlayer* pPlayer = GetGame()->GetMapPlayer( *it );
		//			if( pPlayer && stricmp( pPlayer -> GetAccount(), strCDKey ) == 0 )
		//			{
		//				CGame::tagGameServer *pServer = GetGame()->GetPlayerGameServer(pPlayer->GetID());
		//				if (pServer)
		//				{
		//					CMessage msg(MSG_W2S_COST_INFORMATION);
		//					msg.Add( pPlayer -> GetID() );
		//					msg.Add(strInfo);
		//					msg.SendGSID(pServer->dwIndex);
		//					return;
		//				}
		//			}
		//		}
		//	}
		//}
		break;

	// 费用不足
	case MSG_COST2W_COST_NOTENOUGHCOST:
		//{
		//	if(GetGame()->GetSetup()->bUseCostSys)
		//	{
		//		char strCDKey[256];
		//		char strInfo[256];
		//		pMsg -> GetStr( strCDKey, 256 );
		//		pMsg->GetStr(strInfo,256);

		//		list<UINT>::iterator it = GetGame() -> GetOnlinePlayerList() -> begin();
		//		for( ; it != GetGame() -> GetOnlinePlayerList() -> end(); it ++ )
		//		{
		//			CPlayer* pPlayer = GetGame()->GetMapPlayer( *it );
		//			if( pPlayer && stricmp( pPlayer -> GetAccount(), strCDKey ) == 0 )
		//			{
		//				CGame::tagGameServer *pServer = GetGame()->GetPlayerGameServer(pPlayer->GetID());
		//				if (pServer)
		//				{
		//					CMessage msg(MSG_W2S_COST_NOTENOUGHCOST);
		//					msg.Add( pPlayer -> GetID() );
		//					msg.Add(strInfo);
		//					msg.SendGSID(pServer->dwIndex);
		//					return;
		//				}
		//			}
		//		}
		//	}

		//}
		break;

	// 升级了
	case MSG_S2W_COST_CHANGELEVEL:
		//{
		//	if(GetGame()->GetSetup()->bUseCostSys && 
		//		GetGame()->GetCostNetClient()!=NULL)
		//	{
		//		long lPlayerID = pMsg->GetLong();
		//		DWORD dwExp = pMsg->GetDWord();
		//		BYTE byOldLel = pMsg->GetByte();
		//		BYTE byCurLel = pMsg->GetByte();
		//		CPlayer *pPlayer = GetGame()->GetOnlinePlayerByID(lPlayerID/*pMsg->GetLong()*/);
		//		if (pPlayer==NULL)
		//			return;
		//		long lRegionID=pPlayer->GetRegionID();

		//		CMessage msg(MSG_W2COST_COST_CHANGE);
		//		msg.Add(pPlayer->GetAccount());
		//		msg.Add(lPlayerID);
		//		msg.Add(lRegionID);
		//		msg.Add(byCurLel);
		//		msg.Add(GetGame()->GetSetup()->dwNumber);
		//		msg.Add(GetGame()->GetCostNetClient()->GetCostTimePoint());
		//		//msg.Add(time(NULL)+8*60*60);
		//		msg.SendToCost();
		//	}
		//}
		break;

	// 写玩家升级日志
	case MSG_S2W_WRITELOG_PLAYERLEVEL:
		//{
		//	if(GetGame()->GetSetup()->bUseCostSys &&
		//		GetGame()->GetCostNetClient()!=NULL)
		//	{
		//		long lPlayerID = pMsg->GetLong();
		//		CPlayer *pPlayer = GetGame()->GetMapPlayer(lPlayerID);
		//		if(pPlayer==NULL)
		//			return ;
		//		DWORD dwExp = pMsg->GetDWord();
		//		BYTE byOldLel = pMsg->GetByte();
		//		BYTE byCurLel = pMsg->GetByte();
		//		long lRegionID = pPlayer->GetRegionID();

		//		CMessage msg(MSG_W2COST_COST_CHANGE);
		//		msg.Add(pPlayer->GetAccount());
		//		msg.Add(lPlayerID);
		//		msg.Add(lRegionID);
		//		msg.Add(byCurLel);
		//		msg.Add(GetGame()->GetSetup()->dwNumber);
		//		msg.Add(GetGame()->GetCostNetClient()->GetCostTimePoint());
		//		//msg.Add(time(NULL)+8*60*60);
		//		msg.SendToCost();
		//	}
		//}
		break;

	// 返回帐户计费信息(登陆界面)
	case MSG_COST2W_COST_ANSWERINFO:
		//{
		//	char strCdkey[256];
		//	char strInfo[256];
		//	pMsg->GetStr(strCdkey, 256);
		//	pMsg->GetStr(strInfo, 256);

		//	pMsg->SetType(MSG_W2L_LOG_SENDACCOUNTINFO);
		//	pMsg->Send();
		//}
		break;
	} // end switch
} // end function
