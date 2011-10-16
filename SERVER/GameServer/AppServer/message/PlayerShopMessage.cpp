#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "..\ServerRegion.h"
#include "..\Goods\CGoods.h"
#include "..\Session\CSession.h"
#include "..\Session\CSessionFactory.h"
#include "..\Session\CPersonalShopSeller.h"
#include "..\Session\CPersonalShopBuyer.h"
#include "..\..\..\..\Public\WordsFilter.h"
#include "..\goods\CGoodsFactory.h"
#include "..\container\CGoodsShadowContainer.h"
#include "../../../../public/StrUnifyInput.h"
#include "..\..\..\setup\IncrementShopList.h"
#include "..\GoodsExManage.h"


void OnPlayerShopMessage( CMessage* pMsg)
{
	if( pMsg == NULL ) return;

	CPlayer* pPlayer = pMsg->GetPlayer();
	if( pPlayer == NULL ) return;

	

	switch( pMsg->GetType() )
	{
		//##请求开店
	case MSG_C2S_PLAYERSHOP_CREATE:
		{			
			if( pPlayer->IsDied() ) return;

			if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_NONE )
			{
				//##发送消息给当前玩家.
				pPlayer->SendNotifyMessage( CStringReading::LoadString(21,3) );
				return;
			}

			if(pMsg->GetRegion() == NULL)	return;


			//判断是否在安全区
			if( pMsg->GetRegion()->GetSecurity( pPlayer->GetTileX(), pPlayer->GetTileY() ) != 
				CRegion::SECURTIY_SAFE )
			{
				pPlayer->SendNotifyMessage( CStringReading::LoadString(21,4) );
				return;
			}

			const CGUID& SessionID		= CSessionFactory::CreateSession( 1, 20 );
			CSession* pSession	= const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( pSession && pSession->Start() )
			{
				long SellerPlugID	= CSessionFactory::CreatePlug( CSessionFactory::PT_PERSONAL_SHOP_SELLER, TYPE_PLAYER, pPlayer->GetExID() );
				if( CSessionFactory::InsertPlug(SessionID, SellerPlugID) )
				{
					//pPlayer->SetCurrentProgress( CPlayer::PROGRESS_OPEN_STALL );
					pPlayer->SetMoveable(FALSE);

					pPlayer->GetPlayerDepot()->OnLockRequest(NULL);

					CMessage msg( MSG_S2C_PALYERSHOP_CREATE );
					msg.Add( SessionID );
					msg.Add( SellerPlugID );

					msg.SendToPlayer( pPlayer->GetExID() );

					pPlayer->SetPersonalShopFlag(SessionID, SellerPlugID);
				}
			}

		}
		break;
	case MSG_C2S_PLAYERSHOP_GOODSPRICE:
		{				
		}
		break;
	case MSG_C2S_PLAYERSHOP_OPENFORBUSINESS://玩家请求开始摆摊
		{
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_OPEN_STALL );
			pPlayer->ProduceEvent(ET_OpenShop);
			pPlayer->ProduceEvent(ET_EventBreak);
			CGUID SessionID;
			DWORD dwGoodsAmount=0;
			pMsg->GetGUID(SessionID);				//Session ID
			long PlugID = pMsg->GetLong();		//会话者 ID
			LONG lNum=pMsg->GetLong();			//几类物品要摆摊

			//个人商店的上架栏位更新为15个
			if(lNum<=0 || lNum>15)
				return;
			CGoods* pGoods=NULL;
			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			CPersonalShopSeller* pSellerPlug=NULL;

			if( pSession )
			{
				pSellerPlug = dynamic_cast<CPersonalShopSeller*>( pSession->QueryPlugByID(PlugID) );
				if( pSellerPlug && pSellerPlug->GetOwnerID() == pPlayer->GetExID() )
				{
					if( pSellerPlug->DoesShopOpenedForBusiness() == FALSE )
					{	
						pPlayer->ClearPersonalShopGoodsData();
						for(int i=0;i<lNum;i++)
						{
							WORD wNum=pMsg->GetWord();//影响多少个格子
							if(wNum==0 || wNum>50)
							{
								pSellerPlug->ClearData();
								return;
							}
							LONG lGoodsId=0,lContainerId=0,lPos=0;	
							DWORD dwNum=0,dwPrice=0;
							WORD wPriceType=0;
							DWORD dwSellType = 0;
							DWORD dwGroupNum = 0;
							DWORD dwOneGroupCnt = 0;

							lGoodsId=pMsg->GetLong();//物品编号
							CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(lGoodsId);
							if(!pProperty)
							{
								pSellerPlug->ClearData();
								return;
							}

							//如果物品是叠加物品,则不能存在已经设置的价格
							if(pProperty->GetAddonPropertyValue(GAP_MAXOVERLAP,1)>1)
							{
								if(pSellerPlug->GoodsPriceExist(lGoodsId))
								{
									pSellerPlug->ClearData();
									return;
								}
							}							

							dwSellType = pMsg->GetDWord();				//物品出售的方式（分为两种：0、单个贩卖；1、成组贩卖）
							//贩卖方式只能设置为0或1
							if ( dwSellType != 0 && dwSellType != 1 )
							{
								pSellerPlug->ClearData();
								return;
							}
							dwOneGroupCnt = pMsg->GetDWord();	//物品的单组个数( 单个贩卖时设定为常数1 )
							dwGroupNum = pMsg->GetDWord();		//物品的组数
							//!如果出售方式为 单个贩卖，判断单组个数设定是否正确
							//if ( dwSellType == CPersonalShopSeller::ST_SINGLE_SELL )
							//{
							//	if ( dwOneGroupCnt != 1 )
							//	{
							//		pSellerPlug->ClearData();
							//		return;
							//	}
							//}

							//物品数量判断
							DWORD dwAllGoodsAmount = pPlayer->GetGoodsAmountByOriginName( CGoodsFactory::QueryGoodsOriginalName( lGoodsId ) );
							//上架物品数量应小于玩家拥有总数
							if(  dwAllGoodsAmount < dwGroupNum * dwOneGroupCnt )
							{
								pSellerPlug->ClearData();
								return;
							}

							wPriceType = pMsg->GetWord();		//物品出售的货币类型（分为两种：0、金币；1、位面精华）
							//货币类型只能设置为0或1
							if ( wPriceType != 0 && wPriceType != 1 )
							{
								pSellerPlug->ClearData();
								return;
							}

							//不允许位面精华的货币类型为位面精华
							if ( (lGoodsId == CGlobeSetup::GetSetup()->dwWeimianGoodsId) && (wPriceType == 1) )
							{
								pSellerPlug->ClearData();
								return;
							}
							dwPrice=pMsg->GetDWord();//价格
							if ( dwPrice == 0 )
							{
								pSellerPlug->ClearData();
								return;
							}

#ifdef _DEBUG
							PutLogInfo("\r\n=============================== Start Save Goods Info ===============================\r\n");

							SYSTEMTIME stTime;
							GetLocalTime(&stTime);
							char str[256];
							_snprintf(str, 256, "%04d-%02d-%02d %02d:%02d:%02d\r\n\r\n", 
								stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
							PutLogInfo(str);
							_snprintf(str, 256, "影响格子：%d物品ID：--%d贩卖方式：--%d价格类型：--%d价格：--%d组数：--%d每组个数：--%d\r\n\r\n", 
								wNum, lGoodsId, dwSellType, wPriceType, dwPrice, dwGroupNum,dwOneGroupCnt);
							PutLogInfo(str);
							PutLogInfo("\r\n");
							PutLogInfo("\r\n=============================== End Save Goods Info ===============================\r\n");
#endif
							
							CGoodsShadowContainer::tagGoodsShadow* pTagGoodsShadow=NULL;
							for(WORD j=0;j<wNum;j++)
							{								
								lContainerId=pMsg->GetLong();
								lPos=pMsg->GetLong();
								dwNum=pMsg->GetDWord();//物品数量

#ifdef _DEBUG
								PutLogInfo("\r\n=============================== Start Save Every Goods Info ===============================\r\n");
								char str[128];
								_snprintf(str, 128, "背包ID：%d背包位置：--%d数量：--%d\r\n\r\n", 
									lContainerId, lPos, dwNum);
								PutLogInfo(str);
								PutLogInfo("\r\n");
								PutLogInfo("================================ End Save Every Goods Info ================================\r\n");
#endif

								dwGoodsAmount+=dwNum;
								
								pGoods=pPlayer->FindGoods(lContainerId,lPos);
								//找不到该物品
								if(!pGoods)
								{
									pSellerPlug->ClearData();
									char* strRes=CStringReading::LoadString(21,9);
									if(strRes)
										pPlayer->SendNotifyMessage(strRes);
									return;
								}
								//物品ID匹配的判断
								if(pGoods->GetBasePropertiesIndex()!=lGoodsId)
								{
									pSellerPlug->ClearData();
									return;
								}
								//冻结物品不能摆摊
								if(GoodsExManage::GetInstance().IsFrost(pGoods))
									return;
								//绑定物品不能摆摊
								if(GoodsExManage::GetInstance().IsBind(pGoods))
									return;
								//物品数量错误
								if(pGoods->GetAmount()<dwNum || dwNum==0)
								{
									pSellerPlug->ClearData();
									return;
								}
								//防止同一位置物品重复添加
								if(pPlayer->IsInPersonalShop(pGoods))
								{
									pSellerPlug->ClearData();
									return;
								}

								if( ( pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
									CGoodsBaseProperties::PA_CANNOT_TRADE_AND_STORE ) == CGoodsBaseProperties::PA_CANNOT_TRADE_AND_STORE )
								{
									//
									pSellerPlug->ClearData();
									char* strRes=CStringReading::LoadString(21,12);
									if(strRes)
										pPlayer->SendNotifyMessage(strRes);
									return;
								}

								pPlayer->AddPersonalShopGoodsData(pGoods);
								
								CGoodsShadowContainer::tagGoodsShadow tShadow;
								tShadow.lOriginalContainerType=400;
								tShadow.OriginalContainerID=pPlayer->GetExID();
								tShadow.lOriginalContainerExtendID=lContainerId;
								tShadow.dwOriginalGoodsPosition=lPos;
								tShadow.dwGoodsAmount=dwNum;
								tShadow.guGoodsID=pGoods->GetExID();
								tShadow.dwGoodsBasePropertiesIndex=lGoodsId;
								pSellerPlug->AddShadowTag(&tShadow);
								
								//设置上架商品价格信息
								if ( !pSellerPlug->SetGoodsPrice(pGoods->GetExID(),pGoods->GetBasePropertiesIndex(),
									dwSellType,wPriceType,dwGroupNum,dwOneGroupCnt,dwPrice) )
								{
									return;
								}
								
							}								
						}
						if(pSellerPlug->OpenForBusiness())
						{							
							CMessage msg( MSG_S2C_PLAYERSHOP_OPENFORBUSINESS );
							msg.Add( pPlayer->GetExID() );
							msg.Add( SessionID );
							msg.Add( PlugID );
							msg.SendToAround( pPlayer );

							//向周围玩家发送商店名称信息
							CMessage msgName( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
							msgName.Add( SessionID );
							msgName.Add( pPlayer->GetExID() );
							msgName.Add( (BYTE)1 );
							if ( strcmp( pSellerPlug->GetShopName() , "")  != 0 )
							{
								msgName.Add( pSellerPlug->GetShopName() );
							}
							else
							{
								//默认的商店名称为“个人商店”
								msgName.Add( CStringReading::LoadString(21,18) );
							}
							msgName.SendToAround( pPlayer );

							char strFile[64];
							_snprintf(strFile, 64, "scripts/game/open.script");
							stRunScript st;
							st.pszFileName = strFile;
							st.srcShape=dynamic_cast<CPlayer*>(pPlayer);		
							st.pRegion = dynamic_cast<CRegion*>(pPlayer->GetFather());
							st.desShape = NULL;
							st.guUsedItemID = CGUID::GUID_INVALID;
							RunScript(&st, (char*)GetGame()->GetScriptFileData(strFile));

						}
					}
				}
			}			
		}
		break;
	case MSG_C2S_PLAYERSHOP_CLOSEDOWN:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( pSession )
			{
				CPersonalShopSeller* pSellerPlug = dynamic_cast<CPersonalShopSeller*>( pSession->QueryPlugByID(PlugID) );
				if( pSellerPlug && pSellerPlug->GetOwnerID() == pPlayer->GetExID() )
				{
					if( pSellerPlug->DoesShopOpenedForBusiness() )
					{
						if( pSellerPlug->CloseDown() )
						{
							pPlayer->ClearPersonalShopGoodsData();
							pSession->PlugExit(PlugID);
							CMessage msg( MSG_S2C_PLAYERSHOP_CLOSEDOWN );
							msg.Add( pPlayer->GetExID() );
							msg.Add( SessionID );
							
							msg.SendToAround( pPlayer );

							char strFile[64];
							_snprintf(strFile, 64, "scripts/game/close.script");
							stRunScript st;
							st.pszFileName = strFile;
							st.srcShape=dynamic_cast<CPlayer*>(pPlayer);		
							st.pRegion = dynamic_cast<CRegion*>(pPlayer->GetFather());
							st.desShape = NULL;
							st.guUsedItemID = CGUID::GUID_INVALID;
							RunScript(&st, (char*)GetGame()->GetScriptFileData(strFile));
						}
					}
				}
			}
		}
		break;

	case MSG_C2S_PLAYERSHOP_LOOKGOODS:
		{
			if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_NONE ) 
			{
				return;
			}

			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			CGUID SellerID;
			pMsg->GetGUID(SellerID);

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( pSession && pSession->IsSessionAvailable() )
			{
				long BuyerPlugID = CSessionFactory::CreatePlug( 
					CSessionFactory::PT_PERSONAL_SHOP_BUYER, pPlayer->GetType(), pPlayer->GetExID() );

				if( CSessionFactory::InsertPlug(SessionID, BuyerPlugID) )
				{
					pPlayer->SetCurrentProgress( CPlayer::PROGRESS_SHOPPING );
					CPersonalShopBuyer* pBuyerPlug = dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(BuyerPlugID) );
					if( pBuyerPlug )
					{
						CPersonalShopSeller* pSellerPlug = pBuyerPlug->GetSeller();
						if( pSellerPlug )
						{
							vector<BYTE> vOut;
							pSellerPlug->GetGoodsList( vOut );
							if(vOut.size())
							{
								CMessage msg( MSG_S2C_PLAYERSHOP_LOOKGOODS );							
								msg.Add( SessionID );
								msg.Add(SellerID);
								msg.Add( BuyerPlugID );
								msg.AddEx( &vOut[0], static_cast<LONG>( sizeof(BYTE) * vOut.size() ) );
								msg.SendToPlayer( pPlayer->GetExID() );
							}

							//买家打开商店查看商品时，同时发送留言板信息
							CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
							BYTE bMsgType = 0;
							vector<string>& vecTradeInfo = pSellerPlug->GetTradeInfo();

							msg.Add( SessionID );
							msg.Add( pSellerPlug->GetOwnerID() );
							//买家在查看商店时，如果店主没有设置商店名称，则显示为默认。此处的信息类别一定设置
							bMsgType = bMsgType | 0x01;
							if ( strcmp( pSellerPlug->GetShopInfo() , "") != 0  )
							{
								bMsgType = bMsgType | 0x02;
							}
							if ( !vecTradeInfo.empty() )
							{
								bMsgType = bMsgType | 0x04;
							}
							msg.Add( bMsgType );
							if ( strcmp( pSellerPlug->GetShopName() , "")  != 0 )
							{
								msg.Add( pSellerPlug->GetShopName() );
							}
							else
							{
								msg.Add( CStringReading::LoadString(21,18) );
							}
							if ( strcmp( pSellerPlug->GetShopInfo() , "") != 0 )
							{
								msg.Add( pSellerPlug->GetShopInfo() );
							}
							if ( !vecTradeInfo.empty() )
							{
								msg.Add( (BYTE)vecTradeInfo.size() );
								for ( int i = 0; i < vecTradeInfo.size(); ++i )
								{
									msg.Add( vecTradeInfo.at(i).c_str() );
								}
							}
							msg.SendToPlayer( pPlayer->GetExID() );
						}
					}
				}
				else
				{
					pPlayer->SendNotifyMessage( CStringReading::LoadString(21,6) );
					
				}
			}
		}
		break;

	case MSG_C2S_PLAYERSHOP_QUIT:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();
			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( pSession && pSession->IsSessionAvailable() )
			{
				CPlug* pPlug = pSession->QueryPlugByID( PlugID );
				if( pPlug && pPlug->GetOwnerID() == pPlayer->GetExID() )
				{
					pPlug->Exit();
				}
			}
		}
		break;

	case MSG_C2S_PLAYERSHOP_CLOSE:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();
			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( pSession && pSession->IsSessionAvailable() )
			{
				CPersonalShopSeller* pSellerPlug = dynamic_cast<CPersonalShopSeller*>( pSession->QueryPlugByID(PlugID) );
				if( pSellerPlug && pSellerPlug->GetOwnerID() == pPlayer->GetExID() )
				{
					pSession->End();
					pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );
					pPlayer->SetMoveable();

					char strFile[64];
					_snprintf(strFile, 64, "scripts/game/close.script");
					stRunScript st;
					st.pszFileName = strFile;
					st.srcShape=dynamic_cast<CPlayer*>(pPlayer);		
					st.pRegion = dynamic_cast<CRegion*>(pPlayer->GetFather());
					st.desShape = NULL;
					st.guUsedItemID = CGUID::GUID_INVALID;
					RunScript(&st, (char*)GetGame()->GetScriptFileData(strFile));
				}
			}

			pPlayer->SetPersonalShopFlag(NULL_GUID, 0);
		}
		break;
	case MSG_C2S_PLAYERSHOP_REMOVE:
		{			
			LONG lGoodsId=pMsg->GetLong();
			LONG lSourceContainerType				= pMsg->GetLong();
			CGUID SourceContainerId;
			pMsg->GetGUID(SourceContainerId);

			LONG lSourceContainerExtendID			= pMsg->GetLong();
			LONG lSourceContainerPosition			= pMsg->GetLong();
			LONG lDestinationContainerType			= pMsg->GetLong();

			CGUID DestinationContainerID;
			pMsg->GetGUID(DestinationContainerID);
			LONG lDestinationContainerExtendID		= pMsg->GetLong();
			LONG lDestinationContainerPosition	= pMsg->GetLong();

			LONG lObjectType						= pMsg->GetLong();
			CGUID goodsGuid;
			pMsg ->GetGUID(goodsGuid);
			CGUID guidRet=goodsGuid;
			LONG lAmount							= pMsg->GetLong();
			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SourceContainerId));
			if( pSession )
			{
				CPlug* pPlug = pSession->QueryPlugByID( lSourceContainerExtendID >> 8 );
				if( pPlug )
				{
					CGoodsShadowContainer* pContainer = dynamic_cast<CGoodsShadowContainer*>(
						pPlug->GetContainer(lSourceContainerExtendID & 0x000000ff) );
					CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug->GetOwner() );
					if( pContainer && pPlayer && pPlayer->GetType() == lDestinationContainerType &&
						pPlayer->GetExID() == DestinationContainerID )
					{
						DWORD dwMaxStack=CGoodsFactory::QueryGoodsMaxStackNumber(lGoodsId);
						if(dwMaxStack>1)
						{
							//根据GOODSID清除
							pContainer->RemoveShadowByGoodsIndex(lGoodsId);
						}
						else
						{
							//根据GUID清除
							pContainer->RemoveShadowByGoodsGuid(goodsGuid);
						}
						CMessage msg(MSG_S2C_PLAYERSHOP_REMOVE);
						msg.Add(lGoodsId);
						msg.Add(guidRet);
						msg.SendToPlayer(pPlayer->GetExID());
					}
				}
			}
		}
		break;
	case MSG_C2S_PLAYERSHOP_BUYRELIVEITEM:
		{
		}
		break;
	case MSG_C2S_PLAYERSHOP_BUYITEM:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long lPlugID	= pMsg->GetLong();
			long lGoodsNum	= pMsg->GetLong();

			//个人商店的上架栏位更新为15个
			if(lGoodsNum<=0 || lGoodsNum>15)
				return;


			CPersonalShopSeller::VecBuyInfo *pVecBuyInfo = new CPersonalShopSeller::VecBuyInfo;
			pVecBuyInfo->clear();
			for(long i=0;i<lGoodsNum;i++)
			{
				CPersonalShopBuyer::tagBuyInfo stBuyInfo;
				stBuyInfo.lId	= pMsg->GetLong();

				//唯一检测	
				CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(stBuyInfo.lId);
				if(!pProperty)
					return;
				if(pProperty->IsHasAddonPropertie(GAP_UNIQUE))
				{					
					if(!pPlayer->IsGoodsUnique(CGoodsFactory::QueryGoodsOriginalName(stBuyInfo.lId)))
					{
						char* strMsg=CStringReading::LoadString(20,49);
						if(strMsg)
						{
							CMessage msg(MSG_S2C_OTHER_GAMEMESSAGE);
							msg.Add(strMsg);
							msg.Add((long)0);
							msg.Add(CGUID::GUID_INVALID);
							msg.SendToPlayer(pPlayer->GetExID());
						}
						return;
					}
				}	

				pMsg->GetGUID(stBuyInfo.guid);
				stBuyInfo.dwNum	= pMsg->GetDWord();				
				if(stBuyInfo.dwNum==0)
					return;
				//确保购买消息中guid唯一
				for(size_t j=0;j<pVecBuyInfo->size();j++)
				{
					if(stBuyInfo.guid==(*pVecBuyInfo)[j].guid)
					{
						return;
					}
				}
				pVecBuyInfo->push_back(stBuyInfo);
			}

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if( !pSession || !pSession->IsSessionAvailable() )
				break;

			CPersonalShopBuyer* pBuyerPlug = dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
			if( !pBuyerPlug || pBuyerPlug->GetOwnerID() != pPlayer->GetExID() )
				break;

			CPersonalShopSeller* pSellerPlug = pBuyerPlug->GetSeller();
			if( !pSellerPlug )
				break;

			CPlayer *pSeller = GetGame()->FindPlayer(pSellerPlug->GetOwnerID());
			if( !pSeller )
				break;

			if(pSeller->IsDied())
			{
				char* strRes=CStringReading::LoadStringA(21,11);
				if(strRes)
				{
					pPlayer->SendNotifyMessage(strRes);
				}
				break;
			}

			if ( pSeller->GetCurrentProgress() != CPlayer::PROGRESS_OPEN_STALL )
			{
				break;
			}

			pSellerPlug->AddMapBuying(lPlugID, pVecBuyInfo);
			DWORD dwTotalMoney = 0;
			DWORD dwTotalWeimian = 0;
			BOOL eRes = pSellerPlug->CheckCondition(lPlugID, dwTotalMoney, dwTotalWeimian);

			if( CPersonalShopSeller::CR_WAIT_FINISH == eRes )
			{
				pSellerPlug->PurchaseEx(lPlugID, dwTotalMoney, 0, dwTotalWeimian);
			}
			pSellerPlug->DelMapBuying(lPlugID);
		}
		break;

	case MSG_C2S_PLAYERSHOP_MSGBOARDSET:
		{

			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long SellerPlugID = pMsg->GetLong();
			BYTE	bMsgType = pMsg->GetByte();

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if ( !pSession ) return;
			CPersonalShopSeller* pSellerPlug = dynamic_cast<CPersonalShopSeller*>( pSession->QueryPlugByID(SellerPlugID) );

			if ( pSellerPlug )
			{
				if ( bMsgType& 0x1 )//第一位为1，表明设置商店名称
				{
					char szName[256];
					pMsg->GetStr( szName, 256 );

					if( strlen(szName) <= 16 )//16个字符，8个汉字
					{
						string strShopName = szName;

						CWordsFilter* pFilter = CWordsFilter::GetInstance();
						if( pFilter )
						{
							bool bCheck = pFilter->Check( strShopName, true, true );
							if ( !bCheck )
							{
								pPlayer->SendNotifyMessage( CStringReading::LoadString(21,15) );
								return;
							}
						}
						pSellerPlug->SetShopName( const_cast<CHAR*>( strShopName.c_str() ) );

						if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_OPEN_STALL ) 
						{
							return;
						}

						CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
						msg.Add( SessionID );
						msg.Add( pPlayer->GetExID() );
						msg.Add( (BYTE)1 );
						msg.Add( pSellerPlug->GetShopName() );

						//群发设置商店名称
						msg.SendToAround( pPlayer );
					}
					else 
					{
						pPlayer->SendNotifyMessage( CStringReading::LoadString(21,5) );
					}
				}
				else if ( bMsgType & 0x2 )//第二位为1，表明设置商店简介
				{
					char szShopInfo[256];
					pMsg->GetStr( szShopInfo, 256 );

					if( strlen(szShopInfo) <= 100 )//100个字符
					{
						string strShopInfo = szShopInfo;
						CWordsFilter* pFilter = CWordsFilter::GetInstance();
						if( pFilter )
						{
							bool bCheck = pFilter->Check( strShopInfo, true, true );
							if ( !bCheck )
							{
								pPlayer->SendNotifyMessage( CStringReading::LoadString(21,15) );
								return;
							}
						}
						pSellerPlug->SetShopInfo( strShopInfo );

						CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
						msg.Add( SessionID );
						msg.Add( pPlayer->GetExID() );
						msg.Add( (BYTE)2 );
						msg.Add( pSellerPlug->GetShopInfo() );

						//群发设置店主推介
						list<LONG>::iterator plugIter = pSession->GetPlugList().begin();
						for( ; plugIter != pSession->GetPlugList().end(); ++plugIter )
						{
							CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*plugIter) );
							if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER )
							{
								msg.SendToPlayer(pPlug->GetOwnerID());
							}
						}
					}
					else 
					{
						pPlayer->SendNotifyMessage( CStringReading::LoadString(21,13) );//配置中更新此提示语(店主推介必须少于50个汉字。)
					}
				}
				else if ( bMsgType & 0x4 )//第二位为1，表明设置交易信息
				{
					//此处由服务端在每次交易完成时更新，客户端不主动请求。
				}
			}
		}
		break;
	case MSG_C2S_PLAYERSHOP_MSGBOARDCHAT:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long lPlugID = pMsg->GetLong();
			char szChatInfo[256];	
			pMsg->GetStr( szChatInfo, 256 );

			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
			if ( pSession && pSession->IsSessionAvailable()  )
			{
				CPlug* pPlug = pSession->QueryPlugByID(lPlugID);
				//无效的PlugID
				if ( pPlug == NULL )
				{
					return;
				}

				//买家发送留言消息时，指定的PlugID为自身的PlugID
				if ( pPlayer->GetExID() == pPlug->GetOwnerID() )
				{
					CPersonalShopSeller* pSeller = (dynamic_cast<CPersonalShopBuyer*>(pPlug))->GetSeller();
					long lSellPlugID = pSeller->Get32ID();

					CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDCHAT );
					msg.Add( SessionID );
					msg.Add( lPlugID );
					msg.Add( pPlug->GetOwnerID() );
					msg.Add( szChatInfo );
					msg.SendToPlayer( pSeller->GetOwnerID() );
				}
				//卖家回复留言，指定的PlugID为买家的PlugID
				else
				{
					CPersonalShopBuyer* pBuyer = dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
					CPersonalShopSeller* pSeller = pBuyer->GetSeller();
					CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDCHAT );
					msg.Add( SessionID );
					msg.Add( lPlugID );
					msg.Add( pSeller->GetOwnerID() );
					msg.Add( szChatInfo );
					msg.SendToPlayer( pBuyer->GetOwnerID() );
				}
			}
		}
		break;

	default:
		break;
	}
}