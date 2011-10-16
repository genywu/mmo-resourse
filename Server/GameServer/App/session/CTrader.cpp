// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CTrader.h"
#include "CSession.h"
#include "CSessionFactory.h"
#include "../Player.h"
#include "../MoveShape.h"
#include "../Goods/CGoodsFactory.h"
#include "../BillApp/BillTrade.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"
#include "../MessagePackage/CS2CContainerObjectAmountChange.h"
#include "../Listener/CGoodsListListener.h"
#include "../Container/CTestContainer.h"
#include "../../Server/BillClient.h"

#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool PerTradeProcessing(const CGUID &SessionID, long lPlugID )
{
	CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
	if( !pSession || !pSession->IsSessionAvailable() )
		return false;

	CTrader* pBuyerPlug = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID)));
	if( !pBuyerPlug )
		return false;

	CTrader* pSellerPlug = dynamic_cast<CTrader*>( const_cast<CPlug*>(CSessionFactory::QueryPlug(pBuyerPlug->GetContraryID())));
	if( !pSellerPlug )
		return false;

	CPlayer *pBuyer = GetGame()->FindPlayer(pBuyerPlug->GetOwnerID());
	CPlayer *pSeller = GetGame()->FindPlayer(pSellerPlug->GetOwnerID());
	if( !pBuyer || !pSeller )
		return false;

	if( pBuyerPlug->GetOwnerID() != pBuyer->GetExID() )
	{
		CTrader *pTrader = pBuyerPlug;
		pBuyerPlug = pSellerPlug;
		pSellerPlug = pTrader;
	}

	bool bRes = pBuyerPlug->Trade();
	pSession->End();
	return bRes;
}

//##ModelId=41E0DC330251
CTrader::CTrader()
{
	// TODO: Add your specialized code here.
	m_bBeReadyToTrade = FALSE;
	m_bLocked=FALSE;
	m_cSample.SetContainerVolume( 7,2 );
	m_cSample.AddListener( this );
	m_cGoldCoins.SetGoodsAmountLimit( 1 );
	m_cGoldCoins.AddListener( this );
	m_listBuyGoodsInfo.clear();

}

void CTrader::AddBuyGoodsInfo(CGUID& id,DWORD dwNum)
{
	tagTradeGoodsInfo stGoodsInfo;
	stGoodsInfo.goods_Guid=id;
	stGoodsInfo.dwNum=dwNum;
	m_listBuyGoodsInfo.push_back(stGoodsInfo);
}

//##ModelId=41E0DC3900EA
//CTrader::CTrader(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41E0DC430232
CTrader::~CTrader()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41E0DC5002CE
BOOL CTrader::IsPlugAvailable()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = TRUE;
	CPlayer* pOwner = dynamic_cast<CPlayer*>( GetOwner() );
	if( pOwner == NULL )
	{
		bResult = FALSE;
	}
	else 
	{
		if( pOwner->IsDied() )
		{
			bResult = FALSE;
		}
		else if( pOwner->GetCurrentProgress() != CPlayer::PROGRESS_TRADING )
		{
			bResult = FALSE;
		}
	}
	return bResult;
}

//##ModelId=41E0DC5C005D
BOOL CTrader::OnSessionEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##直接把所有的货物放回原来的位置。
	return EndTrading();
}

//##ModelId=41E0DC64032C
BOOL CTrader::OnSessionAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##直接把所有的货物放回原来的位置。
	return EndTrading();
}

//##ModelId=41E0DC6E00AB
BOOL CTrader::OnPlugInserted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_cSample.SetOwner( TYPE_SESSION, m_SessionID );
	m_cSample.SetContainerExtendID( Get32ID() << 8 );

	m_cGoldCoins.SetOwner( TYPE_SESSION, m_SessionID );
	m_cGoldCoins.SetContainerExtendID( ( Get32ID() << 8 ) | 0x00000001 );
	//##把监听器加入到玩家的容器内。
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer ->getOriginPack()->AddListener( &m_cSample );
		pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->m_cEquipment.AddListener( &m_cSample );
		pPlayer->GetPackage()->GetWallet().AddListener( &m_cGoldCoins );		
	}

	return TRUE;
}

//##ModelId=41E0DC740157
BOOL CTrader::OnPlugAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##把监听器从玩家身上移出。
	return EndTrading();
}

//##ModelId=41E0DC7C0196
BOOL CTrader::OnPlugEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return EndTrading();
}

//##ModelId=41E0DC840251
BOOL CTrader::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	switch( lStateID )
	{
	case CPlug::PS_INSERTED:
		break;
	case CPlug::PS_REMOVED:
		break;
	case CPlug::PS_ABORTED:
		break;
	case CTrader::TS_TRADE_READY:
		{
			//#后告诉自己的Owner，对方或自己的当前状态。

			if( GetOwnerType() == TYPE_PLAYER )
			{
				CMessage msg( MSG_S2C_PLAYERTRADE_WAIT );
				msg.Add( lPlugID );
				msg.Add( static_cast<BYTE>(TRUE) );
				msg.SendToPlayer( GetOwnerID() );
			}

			//##如果自己的状态也是等待，就开始交易
			if( IsReady() )
			{
				CSession* pSession = GetSession();
				CTrader* pBuyerPlug		= this;
				CTrader* pSellerPlug	= dynamic_cast<CTrader*>( const_cast<CPlug*>(CSessionFactory::QueryPlug(GetContraryID())));
				if( !pSession || !pSession->IsSessionAvailable() || !pBuyerPlug || !pSellerPlug )
					break;
				long lTotalAmount = 0;
				CPlayer *pBuyer		= dynamic_cast<CPlayer*>(pBuyerPlug->GetOwner());
				CPlayer *pSeller	= dynamic_cast<CPlayer*>(pSellerPlug->GetOwner());
				if( CheckTradeCondition(lTotalAmount)  && pBuyer && pSeller)
				{
					if( lTotalAmount != 0 )
					{
						//-------------------------------------------------------------------------------
						// 个人交易帐户预冻. by Fox.		2008-02-20
						// 预冻成功后完成个人交易
						//-------------------------------------------------------------------------------
						
						if( !pBuyer || !pSeller )
							break;

						if( lTotalAmount < 0 )
						{
							CPlayer *pTemp = pBuyer;
							pBuyer	= pSeller;
							pSeller	= pTemp;
							lTotalAmount = -lTotalAmount;
						}
					}
					else
					{
						Trade();
						pSession->End();
					}
				}
				else
				{
					CMessage msgRet(MSG_S2C_PLAYERTRADE_RESULT);					
					msgRet.Add((BYTE)2);					
					if(pBuyer)
						msgRet.SendToPlayer(pBuyer->GetExID());
					if(pSeller)
						msgRet.SendToPlayer(pSeller->GetExID());
					pBuyerPlug->SetLockState(FALSE);
					pBuyerPlug->SetTradeState(FALSE);	
					pSellerPlug->SetLockState(FALSE);
					pSellerPlug->SetTradeState(FALSE);
				}
			}
		}
		break;
	case CTrader::TS_TRADE_LOCK:
		{
			if(GetOwnerType()==TYPE_PLAYER)
			{
				CMessage msg( MSG_S2C_PLAYERTRADE_LOCK );
				//msg.Add( lPlugID );
				//msg.Add( static_cast<BYTE>(TRUE) );
				msg.SendToPlayer( GetOwnerID() );
			}
		}
		break;
	case CTrader::TS_TRADE_PREPARING:
		{
			//##告诉自己的Owner，对方或自己的当前状态。
			if( GetOwnerType() == TYPE_PLAYER )
			{
				CMessage msg( MSG_S2C_PLAYERTRADE_WAIT );
				msg.Add( lPlugID );
				msg.Add( static_cast<BYTE>(FALSE) );
				msg.SendToPlayer( GetOwnerID() );

				SetLockState(FALSE);
				SetTradeState(FALSE);
			}
		}
		break;
	default:
		break;
	}
	return bResult;
}

//##ModelId=41E1E29600DA
void CTrader::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bBeReadyToTrade = FALSE;
	m_cSample.Release();
	m_cGoldCoins.Release();	
}
	
//##ModelId=41E1EEE001E4
BOOL CTrader::EndTrading()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer->ClearPersonalShopGoodsData();
		pPlayer ->getOriginPack()->RemoveListener( &m_cSample );
		pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->m_cEquipment.RemoveListener( &m_cSample );
		pPlayer->GetPackage()->GetWallet().RemoveListener( &m_cGoldCoins );		
		if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );

		CMessage msg( MSG_S2C_PLAYERTRADE_CANCEL );
		msg.SendToPlayer( pPlayer->GetExID() );

		bResult = TRUE;
	}

	return bResult;
}


//##ModelId=41F5DF6B0120
BOOL CTrader::GetClonePacket(CVolumeLimitGoodsContainer* pCloneContainer)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pCloneContainer )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
		if( pPlayer )
		{
			if( pPlayer ->getOriginPack()->Clone(pCloneContainer) )
			{
				CGoodsListListener lGoodsList;
				m_cSample.TraversingContainer( &lGoodsList );
				if( lGoodsList.m_bIsAllGoodsExist )
				{
					size_t i = 0;
					for( ; i < lGoodsList.m_vGoodsID.size(); i ++ )
					{
						if( pCloneContainer->Remove(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) == NULL )
						{
							if( pPlayer->m_cEquipment.Find(TYPE_GOODS, lGoodsList.m_vGoodsID[i]) == NULL )
							{
								break;
							}
						}
					}
					if( i == lGoodsList.m_vGoodsID.size() )
					{
						bResult = TRUE;
					}
				}
			}
		}
	}
	return bResult;
}




// lTotalAmount为this Trader的支付元宝数量，为负表示对方支付
BOOL CTrader::CheckTradeCondition(long &lTotalAmount)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult	= FALSE;
	//##首先检查2者之间准备的物品是否都有效,然后再检查2者准备的空间是否足够.
	//##最后检查2者的金钱是否足够,上下限是否足够.如果足够就开始交易.
	CSession* pSession = GetSession();
	if( pSession )
	{
		CHAR szMsg[256];
		list<LONG>::iterator it = pSession->GetPlugList().begin();
		DWORD dwGoodsNullNum=0;
		for(; it != pSession->GetPlugList().end(); ++it )
		{
			CTrader* pTraderPlug = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );
			if( pTraderPlug )
			{
				vector<CGoods*> vecTradeGoods;
				pTraderPlug->GetTradeGoods(vecTradeGoods);
				if(vecTradeGoods.size()==0 && pTraderPlug->m_cGoldCoins.GetGoldCoinsAmount()==0)
				{
					dwGoodsNullNum++;
				}
			}
		}
		if(dwGoodsNullNum==2)
		{
			const char* strRes=AppFrame::GetText("21");
			if(strRes)
				pSession->SendNotification(strRes);
			return FALSE;
		}

		it = pSession->GetPlugList().begin();
		for(; it != pSession->GetPlugList().end(); ++it )
		{
			CTrader* pTraderPlug = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );
			if( pTraderPlug )
			{
				CPlayer* pTrader = dynamic_cast<CPlayer*>( pTraderPlug->GetOwner() );
 				if( pTrader )
				{
					//##首先，准备好自己的背包容器的克隆，然后把自己要交易的物品先从克隆容器中移出，
					//##然后获取对方的交易物品。然后添加到自己的背包克隆容器中。
					vector<CGoods*> vGoods;
					vector<CGoods*> vSelfGoods;
					if(pTraderPlug->GetContraryTradeGoods(vGoods) && 
						pTraderPlug->GetTradeGoods(vSelfGoods))
					{
						//这里修改为CTestContainer进行物品删除添加测试
						CTestContainer testContainer;
						testContainer.Init(pTrader);
						BOOL bDelOk=testContainer.DelTest(vSelfGoods);
						BOOL bAddOk=testContainer.AddTest(vGoods);
						if(bDelOk && bAddOk)
						{
							bResult=TRUE;
						}
						else
						{
							const char* strRes=AppFrame::GetText("17");
							pSession->SendNotification( strRes );
							bResult = FALSE;
						}						
					}

					//##首先检查自己的钱是否足够。
					if( pTrader->GetMoney() < pTraderPlug->m_cGoldCoins.GetGoldCoinsAmount() )
					{
						const char* strRes=AppFrame::GetText("18");
						_snprintf( szMsg, 256, strRes, pTrader->GetName() );
						pSession->SendNotification( szMsg );
						bResult = FALSE;
					}
					else 
					{
						//##检查金钱的上限是否足够
						DWORD dwCurrentGoldCoins = pTrader->GetMoney() - pTraderPlug->m_cGoldCoins.GetGoldCoinsAmount();
						if( dwCurrentGoldCoins + pTraderPlug->GetContraryValumeOfTrade() > 
							GoodsSetup::QueryGoodsMaxStackNumber( GoodsSetup::GetGoldCoinIndex() ) )
						{
							const char* strRes=AppFrame::GetText("19");
							_snprintf( szMsg, 256, strRes, pTrader->GetName() );
							pSession->SendNotification( szMsg );
							bResult = FALSE;
						}
					}
				}
				else
				{
					bResult = FALSE;
				}
			}
			else 
			{
				bResult = FALSE;
			}

			if( bResult == FALSE )
			{
				break;
			}
		}
	}
	return bResult;
}


bool CTrader::Trade()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;		

	CGUID PlayerID[2];
	DWORD dwPlayerCurMoney[2] = {0,0};
	DWORD dwMapID[2] = {0, 0};
	DWORD dwPlayerPos[2][2] = { {0,0}, {0,0} };

	map<LONG, vector<CGoods*> > mTradeGoods;
	vector<CGoods*> vGoods;
	map<LONG, DWORD> mGoldCoins;

	map<LONG, DWORD> mYuanBao;

	CSession* pSession = GetSession();
	if( pSession )
	{
		BOOL bChecked=TRUE;
		list<LONG>::iterator plugIter = pSession->GetPlugList().begin();		
		for( ; plugIter != pSession->GetPlugList().end(); plugIter ++ )
		{
			CTrader* pTrader = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*plugIter) ) );
			if( pTrader )
			{
				vGoods.clear();
				if( pTrader->GetTradeGoods(vGoods) )
				{
					CTrader* pContraryPlug = dynamic_cast<CTrader*>( pTrader->GetContraryPlug() );
					BOOL bRet=pContraryPlug->CheckBuyGoodsInfo(vGoods);
					if(!bRet)
					{
						bChecked=FALSE;
						break;
					}

				}
				else 
				{
					bChecked=FALSE;
					break;
				}
			}
			else 
			{
				bChecked=FALSE;
				break;
			}
		}


		BOOL bIsPhaseSucceed = FALSE;
		list<LONG>::iterator it = pSession->GetPlugList().begin();
		//##首先获取双方要交易的货物。
		if(bChecked)
		{		
		
		for( ; it != pSession->GetPlugList().end(); it ++ )
		{
			CTrader* pTrader = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );
			if( pTrader )
			{
				vGoods.clear();
				if( pTrader->RemoveTradeGoods(vGoods) )
				{
					mTradeGoods[pTrader->Get32ID()] = vGoods;
					bIsPhaseSucceed = TRUE;

					CTrader* pContraryPlug = dynamic_cast<CTrader*>( pTrader->GetContraryPlug() );
					BOOL bChecked=pContraryPlug->CheckBuyGoodsInfo(vGoods);
					if(!bChecked)
					{
						bIsPhaseSucceed = FALSE;
						break;
					}

				}
				else 
				{
					mTradeGoods[pTrader->Get32ID()] = vGoods;
					bIsPhaseSucceed = FALSE;
					break;
				}
			}
			else 
			{
				bIsPhaseSucceed = FALSE;
			}
		}
		}

		if( bIsPhaseSucceed )
		{
			int j = 0;
			bIsPhaseSucceed = TRUE;
			//##获取到了相应的物品。然后就开始填充对方的物品
			it = pSession->GetPlugList().begin();

			for( ; it != pSession->GetPlugList().end(); it ++ )
			{
				CTrader* pTrader = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );

				if( pTrader )
				{
					CTrader* pContraryPlug = dynamic_cast<CTrader*>( pTrader->GetContraryPlug() );
					CPlayer* pTraderPlayer=dynamic_cast<CPlayer*>(pTrader->GetOwner());
					if( pContraryPlug )
					{
						CPlayer* pPlayer = dynamic_cast<CPlayer*>( pContraryPlug->GetOwner() );
						if( pPlayer )
						{
							PlayerID[j] = pPlayer->GetExID();
							dwPlayerCurMoney[j] = pPlayer->GetMoney();
							dwMapID[j] = pPlayer->GetRegionID();
							dwPlayerPos[j][0] = pPlayer->GetTileX();
							dwPlayerPos[j][1] = pPlayer->GetTileY();
							++j;
							vGoods.clear();
							vGoods = mTradeGoods[(*it)];

							if(vGoods.size()>0)
							{
								CTestContainer testContainer;
								testContainer.Init(pPlayer);
								vector<tagTestResult> vecResult;
								BOOL bSucced=testContainer.AddTest(vGoods,&vecResult);	
								if(bSucced && vecResult.size()==vGoods.size())
								{
									for(int i=0;i<(int)vecResult.size();i++)
									{
										CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(pPlayer->FindContainer(vecResult[i].dwContainerId));
										if(pVolumeContainer)
										{
											if(vecResult[i].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
											{
												//添加新物品							
												CS2CContainerObjectMove comMsg;
												DWORD dwGoodsId=vGoods[i]->GetBasePropertiesIndex();
												const char* strGoodsName=GoodsSetup::QueryGoodsName(dwGoodsId);
												CGUID goodsGuid=vGoods[i]->GetExID();
												DWORD dwGoodsNum=vGoods[i]->GetAmount();
												if(pVolumeContainer->Add(vecResult[i].dwPos, vGoods[i], NULL, &comMsg) )
												{
													comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
													comMsg.SetDestinationContainerExtendID( vecResult[i].dwContainerId );
													vGoods[i]->SerializeForOldClient( &comMsg.GetObjectStream() );
													comMsg.Send( pPlayer->GetExID() );
#ifdef _GOODSLOG1_
													char pszGoodsLog[1024]="";
													char playerGUID[50]="";
													pPlayer->GetExID().tostring(playerGUID);
													char srcGoodsGUID[50]="";
													vGoods[i]->GetExID().tostring(srcGoodsGUID);					
													_snprintf(pszGoodsLog,1024,"[操作:玩家交易获得新物品][玩家名:%s][玩家GUID:%s][容器ID:%d][容器位置:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
														pPlayer->GetName(),playerGUID,vecResult[i].dwContainerId,vecResult[i].dwPos,vGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vGoods[i]->GetAmount());
													CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	

													GetLogicLogInterface()->logT010_goods_log_trade(pTraderPlayer,pPlayer,dwGoodsId,goodsGuid,strGoodsName,0,0,dwGoodsNum,1);                                              

												}
											}
											else if(vecResult[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
											{
												//改变物品数量
												DWORD dwGoodsNum=vGoods[i]->GetAmount();
												DWORD dwGoodsId=vGoods[i]->GetBasePropertiesIndex();
												const char* strGoodsName=GoodsSetup::QueryGoodsName(dwGoodsId);
												CGUID goodsGuid=vGoods[i]->GetExID();
												if(pVolumeContainer->Add(vecResult[i].dwPos, vGoods[i], NULL) )
												{
													CGoods* pExistedGoods=pVolumeContainer->GetGoods(vecResult[i].dwPos);
													if(pExistedGoods)
													{
														CS2CContainerObjectAmountChange coacMsg;
														coacMsg.SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), 
															vecResult[i].dwPos );
														coacMsg.SetSourceContainerExtendID( vecResult[i].dwContainerId );
														coacMsg.SetObject( pExistedGoods->GetType(), pExistedGoods->GetExID() );
														coacMsg.SetObjectAmount( pExistedGoods->GetAmount() );
														coacMsg.Send( pPlayer->GetExID() );
#ifdef _GOODSLOG1_
														char pszGoodsLog[1024]="";
														char playerGUID[50]="";
														pPlayer->GetExID().tostring(playerGUID);
														char srcGoodsGUID[50]="";
														pExistedGoods->GetExID().tostring(srcGoodsGUID);					
														_snprintf(pszGoodsLog,1024,"[操作:玩家交易获得叠加物品][玩家名:%s][玩家GUID:%s][容器ID:%d][容器位置:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
															pPlayer->GetName(),playerGUID,vecResult[i].dwContainerId,vecResult[i].dwPos,pExistedGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pExistedGoods->GetBasePropertiesIndex()),srcGoodsGUID,dwGoodsNum);
														CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	

														GetLogicLogInterface()->logT010_goods_log_trade(pTraderPlayer,pPlayer,dwGoodsId,goodsGuid,strGoodsName,0,0,dwGoodsNum,1);
                                                       
													}								
												}
											}											
										}
									}
								}
							}							
						} // end if pPlayer
					}
				}
			} // end for
		}

		if(bIsPhaseSucceed)
		{
			//##获取双方的交易额
			it = pSession->GetPlugList().begin();
			for( ; it != pSession->GetPlugList().end(); it ++ )
			{
				CTrader* pTrader = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );
				if( pTrader )
				{
					mGoldCoins[*it] = pTrader->m_cGoldCoins.GetGoldCoinsAmount();
				}
			}

			//##接下来修改金钱
			it = pSession->GetPlugList().begin();
			for( ; it != pSession->GetPlugList().end(); it ++ )
			{
				CTrader* pTrader = dynamic_cast<CTrader*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) ) );
				if( pTrader )
				{
					CTrader* pContraryPlug = dynamic_cast<CTrader*>( pTrader->GetContraryPlug() );
					CPlayer* pContraryPlayer=NULL;
					if(pContraryPlug)
						pContraryPlayer=dynamic_cast<CPlayer*>( pContraryPlug->GetOwner() );
					CPlayer* pPlayer = dynamic_cast<CPlayer*>( pTrader->GetOwner() );
					if( pPlayer && pContraryPlayer)
					{
						INT64 nCurrentMoney = pPlayer->GetMoney();
						map<LONG, DWORD>::iterator itValumeOfTrade = mGoldCoins.begin();
						for( ; itValumeOfTrade != mGoldCoins.end(); itValumeOfTrade ++ )
						{
							DWORD dwMoneyVal=itValumeOfTrade->second;
							if( itValumeOfTrade->first == *it )
							{
								nCurrentMoney -= dwMoneyVal;
								//pPlayer给pContraryPlayer金币
								GetLogicLogInterface()->logT010_goods_log_trade(pPlayer,pContraryPlayer,GlobalSetup::GetSetup()->dwGoldGoodsId,
									CGUID::GUID_INVALID,GoodsSetup::QueryGoodsName(GlobalSetup::GetSetup()->dwGoldGoodsId),0,pPlayer->GetMoney(),dwMoneyVal,1);
                                

                                
							}
							else
							{
								nCurrentMoney +=dwMoneyVal;
							}
						}
						if( nCurrentMoney < 0 ) nCurrentMoney = 0;
						pPlayer->SetMoney( static_cast<DWORD>(nCurrentMoney),37 );
					}
				}
			}			

			bResult = true;
		} // end if bIsPhaseSucceed
	} // end if Session

	CPlayer *pBuyer = GetGame()->FindPlayer(PlayerID[0]);
	CPlayer *pSeller = GetGame()->FindPlayer(PlayerID[1]);
	CMessage msgRet(MSG_S2C_PLAYERTRADE_RESULT);
	if(bResult)
	{
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACKET,0);
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACK1,0);
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACK2,0);
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACK3,0);
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACK4,0);
		GetGame()->UpdatePlayerPack(pBuyer,CS2CContainerObjectMove::PEI_PACK5,0);

		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACKET,0);
		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK1,0);
		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK2,0);
		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK3,0);
		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK4,0);
		GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK5,0);

		msgRet.Add((BYTE)1);
	}
	else
	{
		msgRet.Add((BYTE)0);
	}
	if(pBuyer)
		msgRet.SendToPlayer(pBuyer->GetExID());
	if(pSeller)
		msgRet.SendToPlayer(pSeller->GetExID());

	return bResult;
}

BOOL CTrader::CheckBuyGoodsInfo(vector<CGoods*>& goodsVec)
{
	for(list<tagTradeGoodsInfo>::iterator infoIter=m_listBuyGoodsInfo.begin();infoIter!=m_listBuyGoodsInfo.end();infoIter++)
	{
		BOOL bFound=FALSE;
		for(vector<CGoods*>::iterator goodsIter=goodsVec.begin();goodsIter!=goodsVec.end();goodsIter++)
		{
			if((*infoIter).goods_Guid==(*goodsIter)->GetExID() && 
				(*infoIter).dwNum==(*goodsIter)->GetAmount())
			{
				bFound=TRUE;
				break;
			}
		}
		if(!bFound)
			return FALSE;
	}
	return TRUE;
}


//##ModelId=41F6018A0306
BOOL CTrader::RemoveTradeGoods(vector<CGoods*>& vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(GetOwner());
	CGoodsListListener lGoodsList;
	m_cSample.TraversingContainer( &lGoodsList );
	if( lGoodsList.m_bIsAllGoodsExist )
	{
		size_t i = 0;
		for( ; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			LONG lOriginalContainer = m_cSample.GetGoodsSourceContainerExtendID( lGoodsList.m_vGoodsID[i] );

			CS2CContainerObjectMove comMsg;
			CGoods* pGoods = dynamic_cast<CGoods*>( m_cSample.Remove( 
				static_cast<LONG>(TYPE_GOODS), lGoodsList.m_vGoodsID[i], &comMsg ) );
			if( pGoods )
			{
				
					GetLogicLogInterface()->logT010_goods_log_trade(pPlayer,pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),
                        GoodsSetup::QueryGoodsName(pGoods->GetBasePropertiesIndex()),0,(long)lGoodsList.m_vGoodsID.size(),pGoods->GetAmount(),100);

				vGoods.push_back( pGoods );
				comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
				comMsg.SetSourceContainerExtendID( lOriginalContainer );
				comMsg.Send( GetOwnerID() );
			}
			else 
			{
				break;
			}
		}
		if( i == lGoodsList.m_vGoodsID.size() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CTrader::GetTradeGoods(vector<CGoods*>& vGoodsList)
{
	BOOL bResult = FALSE;

	CGoodsListListener lGoodsList;
	m_cSample.TraversingContainer( &lGoodsList );
	if( lGoodsList.m_bIsAllGoodsExist )
	{
		size_t i = 0;
		for( ; i < lGoodsList.m_vGoodsID.size(); i ++ )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( m_cSample.Find( 
				TYPE_GOODS, lGoodsList.m_vGoodsID[i]) );
			if( pGoods )
			{
				vGoodsList.push_back( pGoods );
			}
			else 
			{
				break;
			}
		}
		if( i == lGoodsList.m_vGoodsID.size() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41E223DB005D
BOOL CTrader::GetContraryTradeGoods(vector<CGoods*>& vGoodsList)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CTrader* pContraryPlug = dynamic_cast<CTrader*>( GetContraryPlug() );
	if( pContraryPlug )
	{
		bResult = pContraryPlug->GetTradeGoods( vGoodsList );
	}
	return bResult;
}

//##ModelId=41E2442B038A
DWORD CTrader::GetContraryValumeOfTrade()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	CTrader* pContraryPlug = dynamic_cast<CTrader*>( GetContraryPlug() );
	if( pContraryPlug )
	{
		dwResult = pContraryPlug->m_cGoldCoins.GetGoldCoinsAmount();
	}
	return dwResult;
}


//##ModelId=41EF0D9E008C
CContainer* CTrader::GetContainer(LONG lID)
{
	// TODO: Add your specialized code here.
	CContainer* pResult = NULL;
	if( lID == 0 )
	{
		pResult = &m_cSample;
	}
	else if( lID == 1 )
	{
		pResult = &m_cGoldCoins;
	}
	return pResult;
}


//##ModelId=41F5B78C03CC
BOOL CTrader::IsReady()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_bBeReadyToTrade;
}

//##ModelId=41F5C35C0332
void CTrader::SetTradeState(BOOL bReady)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bBeReadyToTrade = bReady;
}

//##ModelId=41F601CA0140
LONG CTrader::GetContraryID()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	LONG lResult = 0;
	CSession* pSession = GetSession();
	if( pSession )
	{
		list<LONG>::iterator it = pSession->GetPlugList().begin();
		for( ; it != pSession->GetPlugList().end(); it ++ )
		{
			if( (*it) != Get32ID() )
			{
				lResult = (*it);
				break;
			}
		}
	}
	return lResult;
}

//##ModelId=41F601D302A8
CPlug* CTrader::GetContraryPlug()
{
	// TODO: Add your specialized code here.
	return const_cast<CPlug*>( CSessionFactory::QueryPlug( GetContraryID() ) );
}

bool CTrader::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	SetTradeState( FALSE );
	//ChangeState( CTrader::TS_TRADE_PREPARING, NULL );
	//OnChangeState( Get32ID(), CTrader::TS_TRADE_PREPARING, NULL );

	CTrader* pContrary = dynamic_cast<CTrader*>( GetContraryPlug() );
	if( pContrary )
	{
		pContrary->SetTradeState( FALSE );
		//pContrary->ChangeState( CTrader::TS_TRADE_PREPARING, NULL );
		//pContrary->OnChangeState( pContrary->Get32ID(), CTrader::TS_TRADE_PREPARING, NULL );
	}

	return true;
}

bool CTrader::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	SetTradeState( FALSE );
	ChangeState( CTrader::TS_TRADE_PREPARING, NULL );
	OnChangeState( Get32ID(), CTrader::TS_TRADE_PREPARING, NULL );

	CTrader* pContrary = dynamic_cast<CTrader*>( GetContraryPlug() );
	if( pContrary )
	{
		pContrary->SetTradeState( FALSE );
		pContrary->ChangeState( CTrader::TS_TRADE_PREPARING, NULL );
		pContrary->OnChangeState( pContrary->Get32ID(), CTrader::TS_TRADE_PREPARING, NULL );
	}

	return true;
}