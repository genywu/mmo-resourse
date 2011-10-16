/**
* @filename:	PlayerTradeMessage.cpp
* @date:		2010/7/22
* @author:		wing (totti19851101@gmail.com) 

* @purpose: 玩家交易消息处理
*/

#pragma once
#include "stdafx.h"
#include "Message.h"

#include "../Player.h"
#include "../ClientRegion.h"
#include "../GameClient/Game.h"
#include "../Other/AudioList.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../TradeSystem/PlayerTrade.h"
#include "../../../UI/GamePage/ChatList/ChatListPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace  PlayerTradeMessage
{
	struct  tagTrade
	{
		CGUID  SourID;
		CGUID  DestID;
	};
}

void RequestTrade(MsgEvent& event,eMsgResult UserId)
{
	PlayerTradeMessage::tagTrade* pTrade = (PlayerTradeMessage::tagTrade*)event.pData;
	if ( !pTrade ) return ;

	BYTE bResult = -1;
	// 同意交易
	if(UserId == S_Ok)
	{			
		bResult = 1;
		CPlayer* pPlayer = (CPlayer*)GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,pTrade->SourID);
		CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
		if (pPlayer!=NULL&&pMainPlayer!=NULL&&pMainPlayer->Distance(pPlayer)<6)
		{
			CMessage msg(MSG_C2S_PLAYERTRADE_RESPONSE);
			msg.Add(pPlayer->GetExID());
			msg.Add(bResult);
			msg.Send();
		}
		else if (pPlayer==NULL)
		{
			// 请求交易玩家不存在
            GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_13"),   //请求交易的玩家不在附近!
                0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		}
		else if (pMainPlayer!=NULL&&pPlayer!=NULL&&pMainPlayer->Distance(pPlayer)>6)
		{
			// 距离太远
			GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_14"),  //您与请求交易的玩家距离太远了!
                0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		}
	}
	else if (UserId == S_Cancel || UserId == S_Esc)
	{
		bResult = 0;
		CMessage msg(MSG_C2S_PLAYERTRADE_RESPONSE);
		msg.Add(pTrade->SourID);
		msg.Add(bResult);
		msg.Send();
	}

	SAFE_DELETE(pTrade);
}

/*
* 功能: 响应玩家交易消息
* 摘要: -
* 参数: pMsg - 服务器发送的消息
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*/

void OnPlayerTradeMessage(CMessage* pMsg)
{
	CPlayer* pPlayer=GetGame()->GetMainPlayer();
	if (!pPlayer)
		return;

	switch(pMsg->GetType())
	{
		//gameserver转发过来的玩家交易请求
	case MSG_S2C_PLAYERTRADE_REQUEST:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);

			// 游戏配置界面未完成，此处默认允许玩家交易
			bool bAllowExchange = true;//GetGame()->GetCGuiEx()->GetGameConfigPageEx()->GetOption( CGameConfigPageEx::OPTION_AllowExchange );
			if ( bAllowExchange ==false /*|| pPlayerTradePage->GetTradeRequestGUID()!=CGUID::GUID_INVALID*/)
			{
				CMessage msg(MSG_C2S_PLAYERTRADE_RESPONSE);
				msg.Add(PlayerID);
				msg.Add((BYTE)0);
				msg.Send();

				if(!bAllowExchange)
                    GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_12") //您没有打开交易开关，自动拒绝了对方玩家的交易请求！
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);

				return;
			}

			CPlayer* pPlayer=(CPlayer*)(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID));
			if(pPlayer!=NULL)
			{
				PlayerTradeMessage::tagTrade* pTrade = new PlayerTradeMessage::tagTrade;
				pTrade->SourID = PlayerID;
				pTrade->DestID = GetGame()->GetMainPlayer()->GetExID();

				char strInfo[255];
                sprintf(strInfo,AppFrame::GetText("Trade_1")   //%s 请求和你交易，是否同意？
                    ,pPlayer->GetName());
				GetInst(MsgEventManager).PushEvent(Msg_Agree, strInfo, RequestTrade, pTrade);

				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
			}
		}
		break;

		//开始交易
	case MSG_S2C_PLAYERTRADE_BEGIN:
		{
			CGUID SessionID,RequestPlayerID,ResponsePlayerID;
			pMsg->GetGUID(SessionID);
			pMsg->GetGUID(RequestPlayerID);
			long lRequestPlugID = pMsg->GetLong();
			pMsg->GetGUID(ResponsePlayerID);
			long lResponsePlugID = pMsg->GetLong();

			if (pPlayer->GetExID()==RequestPlayerID)
			{
				GetInst(PlayerTrade).SetTradePara(SessionID,RequestPlayerID,lRequestPlugID,ResponsePlayerID,lResponsePlugID);
				FireUIEvent("PlayerTrade","Opened");
			}
			else if (pPlayer->GetExID()==ResponsePlayerID)
			{
				GetInst(PlayerTrade).SetTradePara(SessionID,ResponsePlayerID,lResponsePlugID,RequestPlayerID,lRequestPlugID);
				FireUIEvent("PlayerTrade","Opened");
			}
		}
		break;

		// 对方已处于锁定状态
	case MSG_S2C_PLAYERTRADE_LOCK:
		{
			GetInst(PlayerTrade).SetLockedState(true, PlayerTrade::Trade_Other);
			FireUIEvent("PlayerTrade","UpdatePage");
		}
		break;

		//玩家进入交易等待状态的返回消息
	case MSG_S2C_PLAYERTRADE_WAIT:
		{
			long lPlugID = pMsg->GetLong();
			BYTE bReady = pMsg->GetByte();
			if (bReady==1)
			{
				GetInst(PlayerTrade).SetLockedState(true,PlayerTrade::Trade_Other);
			}
			else
			{
				GetInst(PlayerTrade).SetLockedState(false,PlayerTrade::Trade_Other);
				GetInst(PlayerTrade).SetLockedState(false,PlayerTrade::Trade_Me);
			}
			FireUIEvent("PlayerTrade","UpdatePage");
		}
		break;

		//玩家取消交易的返回消息
	case MSG_S2C_PLAYERTRADE_CANCEL:
		{
			if (!GetInst(PlayerTrade).GetResult())
			{
                GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_9") //交易取消
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);

				FireUIEvent("PlayerTrade","ClosePage");
				//更新背包
				//if (pItemPage!=NULL)
				//{
				//	pItemPage->UpdataAllSubPack();
				//	pItemPage->UpdataAllItems();
				//}
			}
		}
		break;

		//gameserver返回的交易完成结果
	case MSG_S2C_PLAYERTRADE_RESULT:
		{	
			BYTE result=pMsg->GetByte();
			//交易成功，并结束交易
			if(result==1)
			{
                GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_8") //交易成功
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-35.wav");
			}
			// 交易失败，并结束交易
			else if (result==0)
			{
                GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Trade_6") //交易失败
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			}
			// 交易失败，但不结束交易，回到重新设置交易状态，可以继续此次交易
			else if (result==2)
			{
				//if (pTradePage!=NULL&&pTradePage->IsOpen())
				//{
				//pTradePage->SetLockedState(false,CPlayerTradePageEx::Trade_Other);
				//pTradePage->SetLockedState(false,CPlayerTradePageEx::Trade_Me);
				//pTradePage->UpdatePage();
				//}

				GetInst(PlayerTrade).SetLockedState(false,PlayerTrade::Trade_Other);
				GetInst(PlayerTrade).SetLockedState(false,PlayerTrade::Trade_Me);
				FireUIEvent("PlayerTrade","UpdatePage");
			}
			
			GetInst(PlayerTrade).SetResult(true);
			FireUIEvent("PlayerTrade","ClosePage");
			//更新背包
			//	if (pItemPage!=NULL)
			//	{
			//		pItemPage->UpdataAllSubPack();
			//		pItemPage->UpdataAllItems();
			//	}
		}
		break;
	}
}