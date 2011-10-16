#include "stdafx.h"
#include "..\nets\networld\message.h"
//#include "..\..\..\..\dbaccess\worlddb\rscdkey.h"
//#include "..\..\..\..\dbaccess\worlddb\rsplayer.h"
#include "..\goods\cgoods.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDepotMessage(CMessage* pMsg)
{
	//switch(pMsg->GetType()) 
	//{
	//case MSG_S2W_DEPOT_OPEN:
	//	{
	//		char strCDKey[50];
	//	    long lPlayerID=0;
	//		pMsg->GetStr(strCDKey,50);
	//		lPlayerID=pMsg->GetLong();
	//		CPlayer* pPlayer=GetGame()->GetPlayerByID(lPlayerID);
	//		if(pPlayer)
	//		{
	//			int nRet=GetGame()->GetRsCDKey()->DepotHavePassWord(strCDKey);
	//			if(nRet==CRsCDKey::ADO_ERROR || nRet==CRsCDKey::CDKEY_NOTFOUND)
	//			{
	//				/*CMessage msg(MSG_W2S_DEPOT_OPEN);
	//				msg.Add((BYTE)0);
	//				msg.SendToSocket(pMsg->GetSocketID());*/
	//			}
	//			else if(nRet==CRsCDKey::PASSWORD_ISNULL)
	//			{
	//				GetGame()->GetRsPlayer()->LoadDepot(pPlayer);
	//				CMessage msg(MSG_W2S_DEPOT_OPEN);
	//				msg.Add(lPlayerID);
	//				msg.Add((BYTE)1); //succeed
	//				msg.Add((BYTE)0); //password is null
	//				msg.Add(BYTE(pPlayer->GetDepotList().size()));	//size of depot
	//				list<CPlayer::tagGoods>::iterator GoodsIter=pPlayer->GetDepotList().begin();
	//				for(;GoodsIter!=pPlayer->GetDepotList().end();GoodsIter++)
	//				{
	//					CPlayer::tagGoods* pstGoods = &(*GoodsIter);
	//					_AddToByteArray(&msg.m_MsgData, pstGoods, long(sizeof(bool)+sizeof(BYTE)*2));//先压前3个字节			
	//					if (pstGoods->bChange)
	//					{				
	//						GoodsIter->pGoods->AddToByteArray(&msg.m_MsgData);
	//					}
	//					else
	//					{
	//						_AddToByteArray(&msg.m_MsgData, GoodsIter->dwType);
	//					}
	//				}
	//				msg.Update();
	//				msg.SendToSocket(pMsg->GetSocketID());
	//			}
	//			else if(nRet=CRsCDKey::PASSWORD_ISNOTNULL)
	//			{
	//				CMessage msg(MSG_S2W_DEPOT_OPEN);
	//				msg.Add(lPlayerID);
	//				msg.Add((BYTE)1);
	//				msg.Add((BYTE)1);
	//				msg.SendToSocket(pMsg->GetSocketID());				
	//			}
	//		}
	//	}
	//	break;
	//
	//}
}