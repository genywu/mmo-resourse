/**
* @filename:	PlayerTradeMessage.cpp
* @date:		2010/7/22
* @author:		wing (totti19851101@gmail.com) 

* @purpose: 玩家交易消息处理
*/

#pragma once
#include "stdafx.h"
#include "../Player.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../GoodsExManage.h"
#include "../Session/CSessionFactory.h"
#include "../Session/CSession.h"
#include "../Session/CPlug.h"
#include "../Session/CTrader.h"

#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnPlayerTradeMessage(CMessage* pMsg)
{
	CPlayer* pPlayer = pMsg->GetPlayer();
	if (!pPlayer)
		return;

	switch(pMsg->GetType())
	{
		//玩家发出一个交易请求
	case MSG_C2S_PLAYERTRADE_REQUEST:
		{
			// 如果当前在进行其他活动,则不允许发送交易请求.
			if(!pPlayer->IsDied())
			{				
				if(pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_NONE)
				{
					// 发送消息给当前玩家.
					const char* strRes = AppFrame::GetText("GS_TRADE_5");
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
					return;
				}

				const CGUID& PlayerID = pPlayer->GetExID();
				CGUID TradePlayerID;
				pMsg->GetGUID(TradePlayerID);	//请求目标玩家的ID
				CPlayer* pTradePlayer = GetGame()->FindPlayer(TradePlayerID);
				if (!pTradePlayer)
				{
					//##无法找到被请求的玩家,通知该用户.
					const char* strRes = AppFrame::GetText("GS_TRADE_10");
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
					return;
				}

				CRegion* pLocalRgn = static_cast<CRegion*>(pTradePlayer->GetFather());
				if(pLocalRgn)
				{
					if( pTradePlayer->GetHideFlag() == 1 && !pTradePlayer->IsGM() && pLocalRgn->GetRgnHideFlag() != 2
						&& (pPlayer->GetTeamID() != pTradePlayer->GetTeamID() || pPlayer->GetTeamID()==NULL_GUID || pTradePlayer->GetTeamID()==NULL_GUID) )// 隐匿者,非GM
					{
						return;
					}
				}

				//请求消息转发
				if(!pTradePlayer->IsDied())
				{
					//##如果被请求的玩家当前没有任何活动,则允许发送消息.
					if(pTradePlayer->GetCurrentProgress() == CPlayer::PROGRESS_NONE)
					{
						if(pTradePlayer->GetPlayerDepot()->TestSession())
						{
							const char* strRes = AppFrame::GetText("GS_TRADE_4");
							if(strRes)
								pPlayer -> SendNotifyMessage(strRes);
							return;
						}
						CServerRegion* pRegion1 = dynamic_cast<CServerRegion*>(pPlayer->GetFather());
						CServerRegion* pRegion2 = dynamic_cast<CServerRegion*>(pTradePlayer->GetFather());
						if(pRegion1 && pRegion2 && (pRegion2==pRegion1))
						{						
							if(pPlayer->Distance(pTradePlayer) <= 8)
							{
								CMessage msg(MSG_S2C_PLAYERTRADE_REQUEST);
								msg.Add(PlayerID);
								msg.SendToPlayer(pTradePlayer->GetExID());

								pPlayer->ProduceEvent(ET_Trade);
								const char* strRes = AppFrame::GetText("GS_TRADE_6");
								if(strRes)
									pPlayer->SendNotifyMessage(strRes);
							}
							else
							{
								// 不是面对面
								const char* strRes = AppFrame::GetText("GS_TRADE_7");
								if(strRes)
									pPlayer->SendNotifyMessage(strRes);
							}
						}
						else
						{
							const char* strRes = AppFrame::GetText("GS_TRADE_22");
							if(strRes)
								pPlayer->SendNotifyMessage(strRes);
						}
					}
					else
					{
						// 发送消息给当前玩家.
						const char* strRes = AppFrame::GetText("GS_TRADE_8");
						if(strRes)
							pPlayer->SendNotifyMessage(strRes);
					}
				}
				else
				{
					// 对方已经死亡
					const char* strRes = AppFrame::GetText("GS_TRADE_9");
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
				}
			}
			else
			{
				// 其实玩家已经死亡
				const char* strRes = AppFrame::GetText("GS_TRADE_11");
				if(strRes)
					pPlayer->SendNotifyMessage(strRes);
			}
		}
		break;

		//玩家对一个交易请求的响应
	case MSG_C2S_PLAYERTRADE_RESPONSE:
		{
			// 响应交易请求的人
			if(pPlayer->IsDied() == FALSE )
			{
				//发出请求的玩家ID
				CGUID PlayerID;
				pMsg->GetGUID(PlayerID);
				BYTE bAgree = pMsg->GetByte(); //是否同意交易

				CPlayer* pSender = GetGame()->FindPlayer(PlayerID);

				if( pPlayer->GetCurrentProgress() == CPlayer::PROGRESS_NONE )
				{
					if( pSender && pSender->IsDied() == FALSE )
					{
						if( pSender->GetCurrentProgress() == CPlayer::PROGRESS_NONE )
						{
							if( pSender->Distance( pPlayer ) <= 8 )
							{
								if(bAgree)
								{
									//##进入交易状态
									pPlayer->SetCurrentProgress(CPlayer::PROGRESS_TRADING);
									pSender->SetCurrentProgress(CPlayer::PROGRESS_TRADING);
									//##创建会话
									const CGUID& SessionID = CSessionFactory::CreateSession(2, 2);
									CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
									if(pSession && pSession->Start())
									{
										//! 关闭其他物品功能窗口
										pPlayer->GetPlayerDepot()->OnLockRequest(NULL);
										GoodsExManage::GetInstance().CloseGoodsExWindow(pPlayer);
										pSender->GetPlayerDepot()->OnLockRequest(NULL);
										GoodsExManage::GetInstance().CloseGoodsExWindow(pSender);

										pPlayer->ClearPersonalShopGoodsData();
										pSender->ClearPersonalShopGoodsData();
										long RequesterPlugID = CSessionFactory::CreatePlug(CSessionFactory::PT_TRADER, TYPE_PLAYER, PlayerID);
										long ResponserPlugID = CSessionFactory::CreatePlug(CSessionFactory::PT_TRADER, TYPE_PLAYER, pPlayer->GetExID());
										CSessionFactory::InsertPlug(SessionID, RequesterPlugID);
										CSessionFactory::InsertPlug(SessionID, ResponserPlugID);

										pPlayer->SetTradeSessionId(SessionID);
										pPlayer->SetTradePlugId(ResponserPlugID);

										pSender->SetTradeSessionId(SessionID);
										pSender->SetTradePlugId(RequesterPlugID);

										CMessage msg(MSG_S2C_PLAYERTRADE_BEGIN);
										msg.Add(SessionID);				//本次交易session id
										msg.Add(PlayerID);				//发出请求的玩家ID
										msg.Add(RequesterPlugID);		//请求者PLUG_ID
										msg.Add(pPlayer->GetExID());	//相应的玩家ID
										msg.Add(ResponserPlugID);		//相应的玩家的PLUG_ID

										//消息发给双方
										msg.SendToPlayer(PlayerID);
										msg.SendToPlayer(pPlayer->GetExID());
									}
								}
								else 
								{
									const char* strRes = AppFrame::GetText("GS_TRADE_12");
									pSender->SendNotifyMessage(strRes);
								}
							}
							else
							{
								//##交易时双方没有面对面
								const char* strRes = AppFrame::GetText("GS_TRADE_7");
								pPlayer->SendNotifyMessage(strRes);
								pSender->SendNotifyMessage(strRes);
							}
						}
						else
						{
							//##发送邀请的人在作其他事情
							const char* strRes = AppFrame::GetText("GS_TRADE_13");
							pPlayer->SendNotifyMessage(strRes);
							strRes = AppFrame::GetText("GS_TRADE_5");
							pSender->SendNotifyMessage(strRes);
						}
					}
					else if(pSender)
					{
						const char* strRes = AppFrame::GetText("GS_TRADE_14");
						pPlayer->SendNotifyMessage(strRes);
						strRes = AppFrame::GetText("GS_TRADE_11");
						pSender->SendNotifyMessage(strRes);
					}
					else
					{
						//##找不到邀请方
						const char* strRes = AppFrame::GetText("GS_TRADE_15");
						pPlayer->SendNotifyMessage(strRes);
					}
				}
				else
					//##响应交易请求的人处于其他状态。
				{
					const char* strRes = AppFrame::GetText("GS_TRADE_5");
					pPlayer->SendNotifyMessage(strRes);
					if(pSender)
					{
						strRes = AppFrame::GetText("GS_TRADE_16");
						pSender->SendNotifyMessage(strRes);
					}
				}
			}
			else if(pPlayer)
			{
				//##其实玩家已经死亡
				const char* strRes = AppFrame::GetText("GS_TRADE_11");
				pPlayer->SendNotifyMessage(strRes);
			}
		}
		break;

		//修改交易区的金币数量
	case MSG_C2S_PLAYERTRADE_CHANGEMONEY:
		{
			CGUID sessionGuid;
			pMsg->GetGUID(sessionGuid);
			//查找交易session
			CSession* pSession=const_cast<CSession*>(CSessionFactory::QuerySession(sessionGuid));				
			if( pSession )
			{
				long lPlugId=pMsg->GetLong();
				if(pPlayer->GetTradeSessionId()!=sessionGuid || pPlayer->GetTradePlugId()!=lPlugId)
					return;
				//查找交易PLUG
				CPlug* pPlug = pSession -> QueryPlugByID(lPlugId);				
				CTrader* pTrader=dynamic_cast<CTrader*>(pPlug);
				if(pTrader)
				{
					//如果PLUG已经锁定 则不能改变金钱
					if(pTrader->IsLocked())
						return;
				}
				else
					return;
				//需要改变的数量
				DWORD dwAmount=pMsg->GetDWord();
				if(pPlayer->GetMoney() < dwAmount)
					return;				
				CGoods* pGoods=CGoodsFactory::CreateGoods(GlobalSetup::GetSetup()->dwGoldGoodsId,345);
				pGoods->SetAmount(dwAmount);
				CGoodsContainer* pContainer = pTrader->GetGoldContainer();
				if(!pContainer)
					return;
				CContainer::tagPreviousContainer stPreviousContainer;
				stPreviousContainer.lContainerType=TYPE_PLAYER;
				stPreviousContainer.ContainerID=pPlayer->GetExID();
				stPreviousContainer.lContainerExtendID=CS2CContainerObjectMove::PEI_WALLET;
				stPreviousContainer.dwGoodsPosition=0;
				BOOL bResult=pContainer->Add( 0,pGoods,&stPreviousContainer,NULL);
				CGoodsFactory::GarbageCollect(&pGoods,345);
			}
		}
		break;

	case MSG_C2S_PLAYERTRADE_LOCK://玩家按下锁定按钮
		{			
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long lPlugId=pMsg->GetLong();

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID));
			if(pSession )
			{
				if(pPlayer->GetTradeSessionId()!=SessionID || pPlayer->GetTradePlugId()!=lPlugId)
					return;
				CTrader* pPlug = dynamic_cast<CTrader*>( pSession -> QueryPlugByOwner( 
					pPlayer -> GetType(), pPlayer -> GetExID() ) );
				if(pSession->IsSessionAvailable() && pPlug /*&& !pPlug->IsLocked()*/)
				{	
					if(pPlug->IsLocked())//目前已经锁定，则改变到准备
					{
						pPlug->SetLockState(FALSE);
						pPlug->SetTradeState(FALSE);
						pSession->OnPlugChangeState(lPlugId,CTrader::TS_TRADE_PREPARING,NULL);
					}
					else
					{
						pPlug->SetLockState(TRUE);
						pSession->OnPlugChangeState(lPlugId,CTrader::TS_TRADE_LOCK,NULL);
					}					
				}
			}
		}
		break;		
	case MSG_C2S_PLAYERTRADE_TRADING://玩家按下交易按钮,进入交易等待状态
		{
			CGUID SessionID;
			pMsg -> GetGUID(SessionID);
			long PlugID = pMsg->GetLong();

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if(pSession )
			{
				if(pPlayer->GetTradeSessionId()!=SessionID || pPlayer->GetTradePlugId()!=PlugID)
					return;
				CTrader* pPlug = dynamic_cast<CTrader*>( pSession -> QueryPlugByOwner( 
					pPlayer -> GetType(), pPlayer -> GetExID() ) );
				if( pPlug && pPlug -> Get32ID() == PlugID )
				{
					if( pSession -> IsSessionAvailable() )
					{
						if( pPlug -> IsReady() )
						{
							pPlug -> SetTradeState( FALSE );
							pSession -> OnPlugChangeState( PlugID, CTrader::TS_TRADE_PREPARING, NULL );
						}
						else
						{
							pPlug -> SetTradeState( TRUE );
							pSession -> OnPlugChangeState( PlugID, CTrader::TS_TRADE_READY, NULL );
						}
					}
				}
			}
		}
		break;
	case MSG_C2S_PLAYERTRADE_CANCEL://玩家交易过程中取消交易
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();
			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if(pSession )
			{
				CTrader* pPlug = 
					dynamic_cast<CTrader*>(pSession->QueryPlugByOwner(pPlayer->GetType(), pPlayer->GetExID()));
				if( pPlug && pPlug->Get32ID() == PlugID )
				{
					if( pSession->IsSessionAvailable() )
					{
						pSession->Abort();
					}
				}
			}
		}
		break;
	}
}