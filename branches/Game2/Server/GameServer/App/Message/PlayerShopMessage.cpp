#include "StdAfx.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../GoodsExManage.h"
#include "../BillApp/BillReLive.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../Session/CSession.h"
#include "../Session/CSessionFactory.h"
#include "../Session/CPersonalShopSeller.h"
#include "../Session/CPersonalShopBuyer.h"
#include "../Container/CGoodsShadowContainer.h"
#include "../../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnPlayerShopMessage(CMessage* pMsg)
{
	if(pMsg == NULL) return;

	CPlayer* pPlayer = pMsg->GetPlayer();
	if(pPlayer == NULL) return;

	switch(pMsg->GetType())
	{
		// 请求开店
	case MSG_C2S_PLAYERSHOP_CREATE:
		{			
			if(pPlayer->IsDied()) return;

			if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_NONE )
			{
				// 发送消息给当前玩家.
				pPlayer->SendNotifyMessage(AppFrame::GetText("GS_PERSONALSHOP_3"));
				return;
			}

			if(pMsg->GetRegion() == NULL)	return;


			//判断是否在安全区
			//if(pMsg->GetRegion()->GetSecurity(pPlayer->GetTileX(), pPlayer->GetTileY()) != 
			//	CRegion::SECURTIY_SAFE)
			//{
			//	pPlayer->SendNotifyMessage(AppFrame::GetText("GS_PERSONALSHOP_4"));
			//	return;
			//}

			const CGUID& SessionID = CSessionFactory::CreateSession(1, 20);
			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
			if(pSession && pSession->Start())
			{
				long SellerPlugID = CSessionFactory::CreatePlug(CSessionFactory::PT_PERSONAL_SHOP_SELLER, TYPE_PLAYER, pPlayer->GetExID());
				if(CSessionFactory::InsertPlug(SessionID, SellerPlugID))
				{
					pPlayer->SetCurrentProgress(CPlayer::PROGRESS_OPEN_STALL);
					pPlayer->SetMoveable(FALSE);

					pPlayer->GetPlayerDepot()->OnLockRequest(NULL);

					CMessage msg( MSG_S2C_PALYERSHOP_CREATE );
					msg.Add(SessionID);
					msg.Add(SellerPlugID);

					msg.SendToPlayer( pPlayer->GetExID() );
				}
			}
		}
		break;

		//玩家请求开始摆摊
	case MSG_C2S_PLAYERSHOP_OPENFORBUSINESS:
		{	
			pPlayer->ProduceEvent(ET_OpenShop);
			pPlayer->ProduceEvent(ET_EventBreak);
			CGUID SessionID;
			DWORD dwGoodsAmount=0;
			pMsg->GetGUID(SessionID);			//Session ID
			long PlugID = pMsg->GetLong();		//会话者 ID
			LONG lNum=pMsg->GetLong();			//几类物品要摆摊

			//个人商店的上架栏位更新为15个
			if(lNum<=0 || lNum>15)
				return;
			CGoods* pGoods=NULL;
			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
			CPersonalShopSeller* pSellerPlug=NULL;

			if(pSession)
			{
				pSellerPlug = dynamic_cast<CPersonalShopSeller*>(pSession->QueryPlugByID(PlugID));
				if(pSellerPlug && pSellerPlug->GetOwnerID() == pPlayer->GetExID())
				{
					if(pSellerPlug->DoesShopOpenedForBusiness() == FALSE)
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
							CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(lGoodsId);
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
							if (dwSellType != 0 && dwSellType != 1)
							{
								pSellerPlug->ClearData();
								return;
							}
							dwOneGroupCnt = pMsg->GetDWord();	//物品的单组个数( 单个贩卖时设定为常数1 )
							dwGroupNum = pMsg->GetDWord();		//物品的组数
							//!如果出售方式为 单个贩卖，判断单组个数设定是否正确
							if (dwSellType == CPersonalShopSeller::ST_SINGLE_SELL)
							{
								if (dwOneGroupCnt != 1)
								{
									pSellerPlug->ClearData();
									return;
								}
							}

							//物品数量判断
							DWORD dwAllGoodsAmount = pPlayer->GetGoodsAmountByOriginName( GoodsSetup::QueryGoodsOriginalName( lGoodsId ) );
							//上架物品数量应小于玩家拥有总数
							if(dwAllGoodsAmount < dwGroupNum*dwOneGroupCnt)
							{
								pSellerPlug->ClearData();
								return;
							}

							wPriceType = pMsg->GetWord();		//物品出售的货币类型（分为两种：0、金币；1、位面精华）
							//货币类型只能设置为0或1
							if (wPriceType != 0 && wPriceType != 1)
							{
								pSellerPlug->ClearData();
								return;
							}

							//不允许位面精华的货币类型为位面精华
							if ((lGoodsId == GlobalSetup::GetSetup()->dwWeimianGoodsId) && (wPriceType == 1) )
							{
								pSellerPlug->ClearData();
								return;
							}
							dwPrice=pMsg->GetDWord();//价格
							if (dwPrice == 0)
							{
								pSellerPlug->ClearData();
								return;
							}

#ifdef _DEBUG
							Log4c::Trace(ROOT_MODULE,"=============================== Start Save Goods Info ===============================");

							SYSTEMTIME stTime;
							GetLocalTime(&stTime);
							Log4c::Trace(ROOT_MODULE, "%04d-%02d-%02d %02d:%02d:%02d\r\n\r\n", 
								stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
							Log4c::Trace(ROOT_MODULE, "影响格子：%d物品ID：--%d贩卖方式：--%d价格类型：--%d价格：--%d组数：--%d每组个数：--%d\r\n\r\n", 
								wNum, lGoodsId, dwSellType, wPriceType, dwPrice, dwGroupNum,dwOneGroupCnt);
							Log4c::Trace(ROOT_MODULE,"=============================== End Save Goods Info ===============================");
#endif
							
							CGoodsShadowContainer::tagGoodsShadow* pTagGoodsShadow=NULL;
							for(WORD j=0;j<wNum;j++)
							{								
								lContainerId=pMsg->GetLong();
								lPos=pMsg->GetLong();
								dwNum=pMsg->GetDWord();//物品数量

#ifdef _DEBUG
								Log4c::Trace(ROOT_MODULE,"=============================== Start Save Every Goods Info ===============================");
								Log4c::Trace(ROOT_MODULE, "背包ID：%d背包位置：--%d数量：--%d\r\n\r\n", lContainerId, lPos, dwNum);
								Log4c::Trace(ROOT_MODULE,"================================ End Save Every Goods Info ================================\r\n");
#endif

								dwGoodsAmount+=dwNum;
								
								pGoods=pPlayer->FindGoods(lContainerId,lPos);
								//找不到该物品
								if(!pGoods)
								{
									pSellerPlug->ClearData();
									const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_9");
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

								if((pGoods->GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) &
									PA_CANNOT_TRADE_AND_STORE) == PA_CANNOT_TRADE_AND_STORE)
								{
									// 含有不能摆摊的物品
									pSellerPlug->ClearData();
									const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_12");
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
									(WORD)dwSellType,wPriceType,dwGroupNum,dwOneGroupCnt,dwPrice) )
								{
									return;
								}
							}								
						}
						if(pSellerPlug->OpenForBusiness())
						{							
							CMessage msg(MSG_S2C_PLAYERSHOP_OPENFORBUSINESS);
							msg.Add(pPlayer->GetExID());
							msg.Add(SessionID);
							msg.Add(PlugID);
							msg.SendToAround(pPlayer);

							//向周围玩家发送商店名称信息
							//CMessage msgName(MSG_S2C_PLAYERSHOP_MSGBOARDGET);
							//msgName.Add(SessionID);
							//msgName.Add(pPlayer->GetExID());
							//msgName.Add((BYTE)1);
							//if (strcmp(pSellerPlug->GetShopName(), "") != 0)
							//{
							//	msgName.Add( pSellerPlug->GetShopName() );
							//}
							//else
							//{
							//	//默认的商店名称为“个人商店”
							//	msgName.Add(AppFrame::GetText("GS_PERSONALSHOP_18"));
							//}
							//msgName.SendToAround(pPlayer);
						}
					}
				}
			}			
		}
		break;

		//玩家关闭个人商店
	case MSG_C2S_PLAYERSHOP_CLOSEDOWN:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();

			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
			if( pSession )
			{
				CPersonalShopSeller* pSellerPlug = dynamic_cast<CPersonalShopSeller*>(pSession->QueryPlugByID(PlugID));
				if(pSellerPlug && pSellerPlug->GetOwnerID() == pPlayer->GetExID())
				{
					if(pSellerPlug->DoesShopOpenedForBusiness())
					{
						if(pSellerPlug->CloseDown())
						{
							pPlayer->ClearPersonalShopGoodsData();
							pSession->PlugExit(PlugID);

							CMessage msg(MSG_S2C_PLAYERSHOP_CLOSEDOWN);
							msg.Add(pPlayer->GetExID());
							msg.Add(SessionID);
							msg.SendToAround(pPlayer);
						}
					}
				}
			}
		}
		break;

		//查看商品
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

			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
			if(pSession && pSession->IsSessionAvailable())
			{
				long BuyerPlugID = CSessionFactory::CreatePlug( 
					CSessionFactory::PT_PERSONAL_SHOP_BUYER, pPlayer->GetType(), pPlayer->GetExID());

				if(CSessionFactory::InsertPlug(SessionID, BuyerPlugID))
				{
					pPlayer->SetCurrentProgress(CPlayer::PROGRESS_SHOPPING);
					CPersonalShopBuyer* pBuyerPlug = dynamic_cast<CPersonalShopBuyer*>(pSession->QueryPlugByID(BuyerPlugID));
					if(pBuyerPlug)
					{
						CPersonalShopSeller* pSellerPlug = pBuyerPlug->GetSeller();
						if(pSellerPlug)
						{
							vector<BYTE> vOut;
							pSellerPlug->GetGoodsList(vOut);
							if(vOut.size())
							{
								CMessage msg(MSG_S2C_PLAYERSHOP_LOOKGOODS);							
								msg.Add(SessionID);
								msg.Add(SellerID);
								msg.Add(BuyerPlugID);
								msg.AddEx(&vOut[0], static_cast<LONG>(sizeof(BYTE)*vOut.size()));
								msg.SendToPlayer(pPlayer->GetExID());
							}

							//买家打开商店查看商品时，同时发送留言板信息
							//CMessage msg(MSG_S2C_PLAYERSHOP_MSGBOARDGET);
							//BYTE bMsgType = 0;
							//vector<string>& vecTradeInfo = pSellerPlug->GetTradeInfo();

							//msg.Add(SessionID);
							//msg.Add(pSellerPlug->GetOwnerID());
							////买家在查看商店时，如果店主没有设置商店名称，则显示为默认。此处的信息类别一定设置
							//bMsgType = bMsgType | 0x01;
							//if (strcmp(pSellerPlug->GetShopInfo(), "") != 0)
							//{
							//	bMsgType = bMsgType | 0x02;
							//}
							//if (!vecTradeInfo.empty())
							//{
							//	bMsgType = bMsgType | 0x04;
							//}
							//msg.Add(bMsgType);
							//if (strcmp( pSellerPlug->GetShopName(), "")  != 0)
							//{
							//	msg.Add(pSellerPlug->GetShopName());
							//}
							//else
							//{
							//	msg.Add(AppFrame::GetText("GS_PERSONALSHOP_18"));
							//}
							//if (strcmp(pSellerPlug->GetShopInfo(), "") != 0)
							//{
							//	msg.Add(pSellerPlug->GetShopInfo());
							//}
							//if (!vecTradeInfo.empty())
							//{
							//	msg.Add((BYTE)vecTradeInfo.size());
							//	for (int i = 0; i<(int)vecTradeInfo.size(); ++i)
							//	{
							//		msg.Add(vecTradeInfo.at(i).c_str());
							//	}
							//}
							//msg.SendToPlayer(pPlayer->GetExID());
						}
					}
				}
				else
				{
					// 该商店的客人过多，暂时无法察看。
					pPlayer->SendNotifyMessage(AppFrame::GetText("GS_PERSONALSHOP_6"));
				}
			}
		}
		break;

		// 购买者离开个人商店
	case MSG_C2S_PLAYERSHOP_QUIT:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long PlugID = pMsg->GetLong();
			CSession* pSession = const_cast<CSession*>(CSessionFactory::QuerySession(SessionID));
			if( pSession && pSession->IsSessionAvailable() )
			{
				CPlug* pPlug = pSession->QueryPlugByID(PlugID);
				if(pPlug && pPlug->GetOwnerID() == pPlayer->GetExID())
				{
					pPlug->Exit();
				}
			}
		}
		break;

		//关闭个人商店
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
					pPlayer->SetMoveable();
				}
			}
		}
		break;
		
		// 购买商品
	case MSG_C2S_PLAYERSHOP_BUYITEM:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long lPlugID = pMsg->GetLong();
			long lGoodsNum = pMsg->GetLong();

			//个人商店的上架栏位更新为15个
			if(lGoodsNum<=0 || lGoodsNum>15)
				return;
			CPersonalShopSeller::VecBuyInfo *pVecBuyInfo = new CPersonalShopSeller::VecBuyInfo;
			pVecBuyInfo->clear();
			for(long i=0;i<lGoodsNum;i++)
			{
				CPersonalShopBuyer::tagBuyInfo stBuyInfo;
				stBuyInfo.lId	= pMsg->GetLong();
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
				const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_11");
				if(strRes)
				{
					pPlayer->SendNotifyMessage(strRes);
				}
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

	//	//设置留言板信息(包括设置商店名称、店主推荐)
	//case MSG_C2S_PLAYERSHOP_MSGBOARDSET:
	//	{
	//		if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_OPEN_STALL ) 
	//		{
	//			return;
	//		}

	//		CGUID SessionID;
	//		pMsg->GetGUID(SessionID);
	//		long SellerPlugID = pMsg->GetLong();
	//		BYTE	bMsgType = pMsg->GetByte();

	//		CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
	//		if ( !pSession ) return;
	//		CPersonalShopSeller* pSellerPlug = dynamic_cast<CPersonalShopSeller*>( pSession->QueryPlugByID(SellerPlugID) );

	//		if ( pSellerPlug )
	//		{
	//			if ( bMsgType& 0x1 )//第一位为1，表明设置商店名称
	//			{
	//				char szName[256];
	//				pMsg->GetStr( szName, 256 );

	//				if( strlen(szName) <= 16 )//16个字符，8个汉字
	//				{
	//					string strShopName = szName;

	//					CWordsFilter* pFilter = CWordsFilter::GetInstance();
	//					if( pFilter )
	//					{
	//						bool bCheck = pFilter->Check( strShopName, true, true );
	//						if ( !bCheck )
	//						{
	//							pPlayer->SendNotifyMessage( AppFrame::GetText("GS_PERSONALSHOP_15") );
	//							return;
	//						}
	//					}
	//					pSellerPlug->SetShopName( const_cast<CHAR*>( strShopName.c_str() ) );

	//					CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
	//					msg.Add( SessionID );
	//					msg.Add( pPlayer->GetExID() );
	//					msg.Add( (BYTE)1 );
	//					msg.Add( pSellerPlug->GetShopName() );

	//					//群发设置商店名称
	//					msg.SendToAround( pPlayer );
	//				}
	//				else 
	//				{
	//					pPlayer->SendNotifyMessage( AppFrame::GetText("GS_PERSONALSHOP_5") );
	//				}
	//			}
	//			else if ( bMsgType & 0x2 )//第二位为1，表明设置商店简介
	//			{
	//				char szShopInfo[256];
	//				pMsg->GetStr( szShopInfo, 256 );

	//				if( strlen(szShopInfo) <= 100 )//100个字符
	//				{
	//					string strShopInfo = szShopInfo;
	//					CWordsFilter* pFilter = CWordsFilter::GetInstance();
	//					if( pFilter )
	//					{
	//						bool bCheck = pFilter->Check( strShopInfo, true, true );
	//						if ( !bCheck )
	//						{
	//							pPlayer->SendNotifyMessage( AppFrame::GetText("GS_PERSONALSHOP_15") );
	//							return;
	//						}
	//					}
	//					pSellerPlug->SetShopInfo( strShopInfo );

	//					CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
	//					msg.Add( SessionID );
	//					msg.Add( pPlayer->GetExID() );
	//					msg.Add( (BYTE)2 );
	//					msg.Add( pSellerPlug->GetShopInfo() );

	//					//群发设置店主推介
	//					list<LONG>::iterator plugIter = pSession->GetPlugList().begin();
	//					for( ; plugIter != pSession->GetPlugList().end(); ++plugIter )
	//					{
	//						CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*plugIter) );
	//						if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER )
	//						{
	//							msg.SendToPlayer(pPlug->GetOwnerID());
	//						}
	//					}
	//				}
	//				else 
	//				{
	//					pPlayer->SendNotifyMessage( AppFrame::GetText("GS_PERSONALSHOP_13") );//配置中更新此提示语(店主推介必须少于50个汉字。)
	//				}
	//			}
	//			else if ( bMsgType & 0x4 )//第二位为1，表明设置交易信息
	//			{
	//				//此处由服务端在每次交易完成时更新，客户端不主动请求。
	//			}
	//		}
	//	}
	//	break;

	//	//客户端发送的留言板中的聊天信息
	//case MSG_C2S_PLAYERSHOP_MSGBOARDCHAT:
	//	{
	//		CGUID SessionID;
	//		pMsg->GetGUID(SessionID);
	//		long lPlugID = pMsg->GetLong();
	//		char szChatInfo[256];	
	//		pMsg->GetStr( szChatInfo, 256 );

	//		CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
	//		if ( pSession && pSession->IsSessionAvailable()  )
	//		{
	//			CPlug* pPlug = pSession->QueryPlugByID(lPlugID);
	//			//无效的PlugID
	//			if ( pPlug == NULL )
	//			{
	//				return;
	//			}

	//			//买家发送留言消息时，指定的PlugID为自身的PlugID
	//			if ( pPlayer->GetExID() == pPlug->GetOwnerID() )
	//			{
	//				CPersonalShopSeller* pSeller = (dynamic_cast<CPersonalShopBuyer*>(pPlug))->GetSeller();
	//				long lSellPlugID = pSeller->Get32ID();

	//				CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDCHAT );
	//				msg.Add( SessionID );
	//				msg.Add( lPlugID );
	//				msg.Add( pPlug->GetOwnerID() );
	//				msg.Add( szChatInfo );
	//				msg.SendToPlayer( pSeller->GetOwnerID() );
	//			}
	//			//卖家回复留言，指定的PlugID为买家的PlugID
	//			else
	//			{
	//				CPersonalShopBuyer* pBuyer = dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
	//				CPersonalShopSeller* pSeller = pBuyer->GetSeller();
	//				CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDCHAT );
	//				msg.Add( SessionID );
	//				msg.Add( lPlugID );
	//				msg.Add( pSeller->GetOwnerID() );
	//				msg.Add( szChatInfo );
	//				msg.SendToPlayer( pBuyer->GetOwnerID() );
	//			}
	//		}
	//	}
	//	break;

	default:
		break;
	}
}