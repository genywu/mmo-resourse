#include "stdafx.h"
#include "..\Player.h"
#include "..\ServerRegion.h"
#include "..\nets\Servers.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"
#include "..\Message Packaging\CC2SContainerObjectMove.h"
#include "..\session\CSession.h"
#include "..\session\CTrader.h"
#include "..\session\CEquipmentEnchase.h"
#include "..\session\CPlug.h"
#include "..\session\CSessionFactory.h"
#include "..\goods\CGoodsFactory.h"
#include "..\container\CVolumeLimitGoodsShadowContainer.h"
#include "../../../../public/GoodsExDef.h"

VOID OnContainerMessage( CMessage *pMsg )
{	
	if( pMsg == NULL ) return;

	CPlayer* pPlayer = pMsg -> GetPlayer();
	if( pPlayer == NULL )
	{
		return;
	}

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pMsg -> GetRegion() );
	if( pRegion == NULL )
	{
		return;
	}

	switch( pMsg -> GetType() )
	{
	case MSG_C2S_CONTAINER_OBJECT_MOVE:
		{
			CC2SContainerObjectMove cOperator;
			CS2CContainerObjectMove cResult;
			
			if( cOperator.Receive(pMsg) )
			{
				CGoodsFactory::SetNoDelFlag(TRUE);
				cOperator.Move( &cResult );
				CGoodsFactory::SetNoDelFlag(FALSE);
				CGoodsFactory::NoDelGoodsVec vecGoods=CGoodsFactory::GetNoDelGoodsVec();
				static char debugStr[512]="";
				for(size_t i=0;i<vecGoods.size();i++)
				{
					if(!vecGoods[i])
						continue;
					BOOL bInSrc=FALSE,bInDst=FALSE;
					DWORD dwContainerId=0,dwPos=0;
					DWORD dwGoodsId=vecGoods[i]->GetBasePropertiesIndex();
					if(cOperator.m_lSourceContainerType==TYPE_PLAYER)
					{
						if(pPlayer->FindGoodsAllPlaceRemove(vecGoods[i]->GetExID()))
						{
							bInSrc=TRUE;
						}
					}
					else if(cOperator.m_lSourceContainerType==TYPE_REGION)
					{
						if(pRegion->FindChildObject(TYPE_GOODS,vecGoods[i]->GetExID()))
						{
							pRegion->RemoveObject(vecGoods[i]);
							bInSrc=TRUE;
						}
					}

					if(cOperator.m_lDestinationContainerType==TYPE_PLAYER)
					{
						if(pPlayer->FindGoodsAllPlaceRemove(vecGoods[i]->GetExID()))
							bInDst=TRUE;
					}
					else if(cOperator.m_lDestinationContainerType==TYPE_REGION)
					{
						if(pRegion->FindChildObject(TYPE_GOODS,vecGoods[i]->GetExID()))
						{
							pRegion->RemoveObject(vecGoods[i]);
							bInDst=TRUE;
						}
					}
					ZeroMemory(&debugStr,512);
					
					if(bInSrc && bInDst) //异常
					{
						sprintf_s(debugStr,"flag:%d id:%d name:%s SRC_T:%d SRC_CONID%d SRC_POS%d DST_T:%d DST_CONID:%d DST_POS:%d",
							1,dwGoodsId,CGoodsFactory::QueryGoodsName(dwGoodsId),cOperator.m_lSourceContainerType, cOperator.m_lSourceContainerExtendID, cOperator.m_dwSourceContainerPosition,
							cOperator.m_lDestinationContainerType,cOperator.m_lDestinationContainerExtendID,cOperator.m_dwDestinationContainerPosition);
						PutDebugString(debugStr);
					}
					else if(bInSrc && !bInDst)//异常
					{
						sprintf_s(debugStr,"flag:%d id:%d name:%s SRC_T:%d SRC_CONID%d SRC_POS%d DST_T:%d DST_CONID:%d DST_POS:%d",
							2,dwGoodsId,CGoodsFactory::QueryGoodsName(dwGoodsId),cOperator.m_lSourceContainerType, cOperator.m_lSourceContainerExtendID, cOperator.m_dwSourceContainerPosition,
							cOperator.m_lDestinationContainerType,cOperator.m_lDestinationContainerExtendID,cOperator.m_dwDestinationContainerPosition);
						PutDebugString(debugStr);
					}
					else if(!bInSrc && bInDst)//
					{
						sprintf_s(debugStr,"flag:%d id:%d name:%s SRC_T:%d SRC_CONID%d SRC_POS%d DST_T:%d DST_CONID:%d DST_POS:%d",
							3,dwGoodsId,CGoodsFactory::QueryGoodsName(dwGoodsId),cOperator.m_lSourceContainerType, cOperator.m_lSourceContainerExtendID, cOperator.m_dwSourceContainerPosition,
							cOperator.m_lDestinationContainerType,cOperator.m_lDestinationContainerExtendID,cOperator.m_dwDestinationContainerPosition);
						PutDebugString(debugStr);
					}
				}	
				CGoodsFactory::ReleaseNoDelGoods();


				BOOL bSendToAround = FALSE;
				if( cResult.GetOperation() != CS2CContainerObjectMove::OT_ROLL_BACK )
				{
					if( cResult.GetDestinationContainerType() == TYPE_REGION ||
						cResult.GetSourceContainerType() == TYPE_REGION )
					{
						bSendToAround = TRUE;
					}
				}

				//##首先给自己发一个move消息,然后给其他人发一个new消息。
				CPlayer* pDestPlayer = cResult.GetDestPlayer();
				if (NULL != pDestPlayer && pDestPlayer -> GetExID() != pPlayer -> GetExID())
				{
					cResult.Send( pDestPlayer -> GetExID() );
				}

				cResult.Send( pPlayer -> GetExID() );
				pPlayer->EquipBackToPack();


				//##然后给别人发消息。
				if( bSendToAround )
				{
					if( cResult.GetDestinationContainerType() == TYPE_REGION )
					{
						//目标是场景
						// 发送消息给玩家
						CGoods* pGoods = NULL;
						if( cResult.GetDestinationObjectType() == 0 || 
							cResult.GetguDestinationObjectID() == CGUID::GUID_INVALID )
						{
							//##移动消息
							pGoods = dynamic_cast<CGoods*>( pRegion -> FindChildObject( cResult.GetSourceObjectType(), 
								cResult.GetguSourceObjectID() ) );
						}
						else
						{
							//##拆分消息
							pGoods = dynamic_cast<CGoods*>( pRegion -> FindChildObject( cResult.GetDestinationObjectType(), 
								cResult.GetguDestinationObjectID() ) );
						}

						if( pGoods )
						{
							CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGoods->GetExID());
							MsgSend.Add( pGoods->GetType() );
							MsgSend.Add( pGoods->GetExID() );
							DBWriteSet setWriteDB;
							MsgSend.GetDBWriteSet(setWriteDB);
							pGoods->SerializeForRgnDisplay(setWriteDB);
							
							MsgSend.SendToAround(pGoods);
						}
						/*
						CGoods* pGoods = NULL;
						if( cResult.GetDestinationObjectType() == 0 || 
							cResult.GetguDestinationObjectID() == CGUID::GUID_INVALID )
						{
							//##移动消息
							pGoods = dynamic_cast<CGoods*>( pRegion -> FindChildObject( cResult.GetSourceObjectType(), 
								cResult.GetguSourceObjectID() ) );
						}
						else
						{
							//##拆分消息
							pGoods = dynamic_cast<CGoods*>( pRegion -> FindChildObject( cResult.GetDestinationObjectType(), 
								cResult.GetguDestinationObjectID() ) );
						}

						if( pGoods )
						{
							cResult.SetSourceContainer( 0, NULL_GUID, 0 );
							cResult.SetSourceContainerExtendID( 0 );

							if( cResult.GetDestinationObjectType() == 0 || 
								cResult.GetguDestinationObjectID() == CGUID::GUID_INVALID )
							{
								//##移动消息
								cResult.SetDestinationObject( cResult.GetSourceObjectType(),cResult.GetguSourceObjectID());
								cResult.SetDestinationObjectAmount( cResult.GetSourceObjectAmount() );
								cResult.SetSourceObject( 0,CGUID::GUID_INVALID);
								cResult.SetSourceObjectAmount( 0 );
							}
							else
							{
								//##拆分消息
								cResult.SetSourceObject( 0,CGUID::GUID_INVALID );
								cResult.SetSourceObjectAmount( 0 );
							}
							pGoods -> SerializeForOldClient( &cResult.GetObjectStream() );
							cResult.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
						}*/
					}
					else if( cResult.GetSourceContainerType() == TYPE_REGION )
					{
						/*
						CMessage Msg( MSG_S2C_RGN_DELSHAPE );
						Msg.Add( pGoods->GetType() );
						Msg.Add( pGoods->GetExID() );
						Msg.Add( (long)0 );
						Msg.SendToAround( pServerRgn,pGoods->GetTileX(),pGoods->GetTileY());
						*/

						
						cResult.SetDestinationContainer( 0, NULL_GUID, 0 );
						cResult.SetDestinationContainerExtendID( 0 );
						cResult.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						
					}
					cResult.SendToAround( pPlayer );
				}
			}
		}
		break;
	case MSG_C2S_CONTAINER_TIDY:
		{
			bool bSort = (pMsg->GetByte()==1)?true:false;
			pPlayer->SetSortContainer(bSort);
		};
		break;
	case MSG_C2S_CONTAINER_OBJECT_UNLOCK:
		{
			CGUID sessionId;
			CGUID goodsId;
			eGoodsExWindowType eWt;
			eWt=static_cast<eGoodsExWindowType>(pMsg->GetLong());
			pMsg->GetGUID(sessionId);
			LONG lPlugId=pMsg->GetLong();
			LONG lPos=pMsg->GetLong();
			pMsg->GetGUID(goodsId);

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(sessionId) );
			if(!pSession)
				return;
			CPlug* pPlug = pSession -> QueryPlugByID( lPlugId >> 8 );
			if(!pPlug)
				return;
			BOOL bRemove=FALSE;
			switch(eWt)
			{
			case eGWT_PlayerTradeWindow:
				{
					CTrader* pTrader=dynamic_cast<CTrader*>(pPlug);
					if(pTrader)
					{
						if(pTrader->IsLocked())
							return;
						CVolumeLimitGoodsShadowContainer* pShadowContainer=pTrader->GetShadowContainer();
						bRemove=pShadowContainer->RemoveShadowEx(goodsId);
						if(bRemove)
							pPlayer->RemovePersonalShopGoodsData(goodsId);
					}					
				}
				break;
			case eGWT_CardFuntionWindow:
				{
					CEquipmentEnchase* pEnchase=dynamic_cast<CEquipmentEnchase*>(pPlug);
					if(pEnchase)
					{
						CEquipmentEnchaseShadowContainer* pShadowContainer=pEnchase->GetShadowContainer();
						bRemove=pShadowContainer->RemoveShadowEx(goodsId);
					}										
				}
				break;
			}
			/*CTrader* pTrader=dynamic_cast<CTrader*>(pPlug);
			CVolumeLimitGoodsShadowContainer* pShadowContainer=pTrader->GetShadowContainer();
			BOOL bRemove=pShadowContainer->RemoveShadowEx(goodsId);*/
			CMessage msg(MSG_S2C_CONTAINER_OBJECT_UNLOCK);
			msg.Add(sessionId);
			msg.Add(lPlugId>>8);
			msg.Add(lPos);
			msg.Add(goodsId);

			list<long>::iterator it = pSession -> GetPlugList().begin();
			for( ; it != pSession -> GetPlugList().end(); it ++ )
			{
				CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
				if( pPlug && pPlug -> GetOwnerType() == TYPE_PLAYER )
				{
					msg.SendToPlayer(pPlug ->GetOwnerID());
				}
			}
		}
		break;
	}
}