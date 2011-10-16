// 增值商店，只允许购买增值道具，不允许卖，不允许修理，而且无税收。
// bugs

#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"

#include "..\player.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"
#include "..\Message Packaging\CS2CContainerObjectAmountChange.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Goods\IncrementShopManager.h"
#include "..\Listener\CGoodsRepairListener.h"
#include "..\Listener\CGoodsRepairPriceListener.h"

#include "../../../../public/StrUnifyInput.h"

#include "..\..\..\setup\IncrementShopList.h"
#include "..\..\..\setup\playerlist.h"
#include "..\..\..\setup\globesetup.h"
#include "../../../setup/logsystem.h"
#include "..\serverregion.h"
#include "../Listener/CSeekGoodsListener.h"
#include "../container/CTestContainer.h"



bool LogicLogInterface::logT521_snda_url_send_log(CPlayer* pPlayer,const char* szOrderId,int nTotalAmount,
												  const char* szGoodsInfo)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(521,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),
		szOrderId,pPlayer->GetAccount(),nTotalAmount,szGoodsInfo);	
}

bool LogicLogInterface::logT522_snda_ticket_consume_log(CPlayer* pPlayer,const char* szOrderId,const char* szSendTime,
														const char* szGoodsInfo,const char* payErrorMsg,int retcode)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) 
		return true;
	if(NULL == m_pLogClient)
		return false;
	if(NULL == pPlayer)
		return false;
	string strTime;
	GetCurrTimeString(strTime);
	return m_pLogClient->SendLogToServer(522,strTime.c_str(),&(pPlayer->GetExID()),pPlayer->GetName(),
		szOrderId,pPlayer->GetAccount(),szSendTime,szGoodsInfo,payErrorMsg,retcode);	
}

// 取消购买
VOID CancelIncrementTrading( CPlayer* pPlayer )
{
	pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );
	pPlayer->SetMoveable();
	CMessage msg( MSG_S2C_INCREMENTSHOP_CLOSE );
	msg.SendToPlayer( pPlayer->GetExID() );
}
//处理赠送购买
bool IncPrsentProcessing(CPlayer* pPlayer, vector<tagSGoods> vtgSGoods, DWORD dwTradeMode, long &lTotalAmount, CGUID &MailGuid)
{
	CMail *pMail = GameManager::GetInstance()->GetMailManager()->GetMail(MailGuid);
	if (pPlayer->GetYuanbao()<lTotalAmount)
	{
		list<CGUID> lMailID;
		lMailID.push_back(MailGuid);
		GameManager::GetInstance()->GetMailManager()->ReleaseMail(lMailID);
		GameManager::GetInstance()->GetMailManager()->SendMailFailedToClient(pPlayer->GetExID(),MAIL_SENDING_FAILED_INVALID_GOODS);
		return false;
	}
	if (pMail)
	{
		vector<CGoods*> vGoods;
		vector<tagSGoods>::iterator it=vtgSGoods.begin();
		for (;it!=vtgSGoods.end();++it)
		{
			//创建要购买的物品VECTOR			
			CGoodsFactory::CreateGoods((*it).lIndex,(*it).lNum, vGoods,30);
		}
		pMail->AddSGoodsToContainer(vGoods);
		GameManager::GetInstance()->GetMailManager()->SendPMail(MailGuid,pPlayer,1);		
		return true;
	}
	return false;
}

// 处理购买
bool IncShopProcessing( CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwNum, DWORD dwTradeMode, long lTotalAmount )
{
	if(!pPlayer)
		return false;

	vector<CGoods*> vGoods;
	//创建要购买的物品VECTOR
	CGoodsFactory::CreateGoods( dwGoodsIndex, dwNum, vGoods,31 );
	if( !vGoods.empty() )
	{	
		CTestContainer testContainer;
		testContainer.Init(pPlayer);
		vector<tagTestResult> vecResult;
		BOOL bSucced=testContainer.AddTest(vGoods,&vecResult);		

		if(bSucced)
		{
			//全部物品可以放置			
			if(vecResult.size() == vGoods.size())
			{
				BOOL bLog = FALSE;
				DWORD dwGoodsId = 0;
				CGUID goodsGuid = NULL_GUID;
				string strGoodsName = "";				
				DWORD dwRemainWeimain =  0;
				DWORD dwRemainTicket = 0;

				for(int i=0;i<vecResult.size();i++)
				{
					CVolumeLimitGoodsContainer* pVolumeContainer=
						dynamic_cast<CVolumeLimitGoodsContainer*>(pPlayer->FindContainer(vecResult[i].dwContainerId));
					if(pVolumeContainer)
					{
						if(vecResult[i].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
						{
							//添加新物品							
							CS2CContainerObjectMove comMsg;
							if(pVolumeContainer->Add(vecResult[i].dwPos, vGoods[i], NULL, &comMsg) )
							{
								pPlayer->OnGetGoods(dwGoodsIndex,vGoods[i]);
								dwGoodsId=vGoods[i]->GetBasePropertiesIndex();
								goodsGuid=vGoods[i]->GetExID();
								strGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);								
								dwRemainWeimain =  pPlayer->GetGoodsAmountByOriginName( 
									CGoodsFactory::QueryGoodsOriginalName( 
									CGlobeSetup::GetSetup()->dwWeimianGoodsId) );
								DWORD dwRemainTicket=pPlayer->GetTicket();
								if(dwTradeMode==CIncrementShopList::TM_TICKET)
								{
									dwRemainTicket-=lTotalAmount;
								}
								else if ( dwTradeMode == CIncrementShopList::TM_WEIMIAN )
								{
									dwRemainWeimain -= lTotalAmount;
								}
								
								bLog = TRUE;
								comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
								comMsg.SetDestinationContainerExtendID( vecResult[i].dwContainerId );
								vGoods[i]->SerializeForOldClient( &comMsg.GetObjectStream() );
								comMsg.Send( pPlayer->GetExID() );
							}
						}
						else if(vecResult[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
						{
							//改变物品数量
							 dwGoodsId=vGoods[i]->GetBasePropertiesIndex();
							 goodsGuid=vGoods[i]->GetExID();
							 strGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);							
							 dwRemainWeimain =  pPlayer->GetGoodsAmountByOriginName( 
								CGoodsFactory::QueryGoodsOriginalName( 
								CGlobeSetup::GetSetup()->dwWeimianGoodsId) );
							 dwRemainTicket=pPlayer->GetTicket();
							if(dwTradeMode==CIncrementShopList::TM_TICKET)
							{
								dwRemainTicket-=lTotalAmount;
							}
							else if ( dwTradeMode == CIncrementShopList::TM_WEIMIAN )
							{
								dwRemainWeimain -= lTotalAmount;
							}

							if(pVolumeContainer->Add(vecResult[i].dwPos, vGoods[i], NULL) )
							{
								pPlayer->OnGetGoods(dwGoodsId);
								bLog = TRUE;
								//GetGameLogInterface()->logT501_Ticket_consume_log(pPlayer,dwGoodsId,goodsGuid,strGoodsName,lTotalAmount,dwGoodsNum,dwRemainWeimain,dwRemainTicket,dwTradeMode);
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
								}								
							}
						}
					}
				}
				if (bLog)
				{
					GetGameLogInterface()->logT501_Ticket_consume_log(pPlayer,dwGoodsId,goodsGuid,strGoodsName.c_str(),lTotalAmount,
						dwNum,dwRemainWeimain,dwRemainTicket,dwTradeMode);
				}
			

				//添加最近购买记录
				pPlayer->Add2IncShopCur10(  dwTradeMode, dwGoodsIndex );
				//添加个人购买详细记录
				pPlayer->Add2IncTradeList( dwTradeMode, dwGoodsIndex, dwNum, lTotalAmount );
				//消费星钻后
				if (dwTradeMode == CIncrementShopList::TM_YUANBAO)
				{
					pPlayer->OnCostYuanBao(lTotalAmount);					
				}
				if (dwTradeMode == CIncrementShopList::TM_YUANBAO && CIncrementShopList::IsReturnDianGuan(dwGoodsIndex))
				{
					//消费星钻返还点券
					float fReturnRatio = 0.0;
					long lReturnNum = CIncrementShopList::GetReturnNum(lTotalAmount,fReturnRatio);
					if (lReturnNum>0)
					{
						long lNum=pPlayer->GetTicket() + lReturnNum;
						pPlayer->SetCostYuanBao(lTotalAmount);
						pPlayer->SetTicket(lNum);	
						GetGameLogInterface()->logT513_Ticket_gain_log(pPlayer,lReturnNum,fReturnRatio);
					}
								

					const char* strScript = CGlobeSetup::GetSetup()->pszPlayerUseDiamondScript;
					if(strScript)
					{
						stRunScript sScript;
						sScript.pszFileName = (char*)strScript;
						sScript.desShape = pPlayer;
						sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
						sScript.srcShape = pPlayer;
						RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript)));
					}
				}
				


				//更新个人限量购买记录
				long lAllowBuyNum = CIncrementShopList::GetLimitNumByID( dwTradeMode, dwGoodsIndex );
				if ( lAllowBuyNum != -1 )
				{
					CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
					if ( !pIncGoodsList ) return false;
					CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwGoodsIndex );
					if ( !pProperties ) return false;
					CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwTradeMode );
					CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
					CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( dwGoodsIndex );

					pPlayer->AddLimitGoodsRecord( dwGoodsIndex, dwNum/itInc->second.dwGoodsNum );
					DWORD dwAlreadyBuyNum = pPlayer->GetLimitGoodsBuyNum( dwGoodsIndex );

					CMessage msg(MSG_S2C_INCREMENTSHOP_UPDATA);
					msg.Add( (BYTE)0 );												//刷新标示（0:个人限购1:限量购买）
					msg.Add( (BYTE)dwTradeMode );									//商店类型

					if ( itInc != mapIncGoodsList.end() )
					{
						msg.Add( itInc->second.dwGoodsType );						//物品分类
						assert( dwGoodsIndex == itInc->second.dwGoodsIndex );
						msg.Add( itInc->second.dwGoodsIndex );						//物编ID
					}
					msg.Add( DWORD(lAllowBuyNum - dwAlreadyBuyNum) );				//剩余可购买数量
					msg.SendToPlayer( pPlayer->GetExID() );
				}
				return true;
			}
			else
			{
				//放置结果SIZE 与 物品SIZE不一致
				for( size_t i = 0; i < vGoods.size(); i ++ )
				{
#ifdef _GOODSLOG1_
					if(vGoods[i])
					{
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						vGoods[i]->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:IncShopProcessing][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							vGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vGoods[i]->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
					}					
#endif
					CGoodsFactory::GarbageCollect( &vGoods[i] ,44);
				}
				vGoods.clear();
				return false;
			}
		}
		else
		{
			//测试失败
			for( size_t i = 0; i < vGoods.size(); i ++ )
			{
#ifdef _GOODSLOG1_
				if(vGoods[i])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					vGoods[i]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:IncShopProcessing:测试失败][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						vGoods[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(vGoods[i]->GetBasePropertiesIndex()),srcGoodsGUID,vGoods[i]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}				
#endif
				CGoodsFactory::GarbageCollect( &vGoods[i] ,44);
			}
			vGoods.clear();
			return false;
		}
	}
	return false;
}

//购买
void ProcessIncrementTrading( CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwBuyNum, DWORD dwTradeMode, DWORD dwFenHongFlag )
{
	if(!pPlayer)
		return;

	CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
	if ( !pIncGoodsList ) return;
	CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwTradeMode );
	CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
	CIncrementShopList::ITR_INCSHOP it = mapIncGoodsList.find( dwGoodsIndex );
	assert( it->first == dwGoodsIndex );

	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwGoodsIndex );
	if ( !pProperties ) return;

	long lTotalAmount(0);					// 总消耗元宝/点券金额

	//计算
	switch( dwTradeMode )
	{
	case CIncrementShopList::TM_YUANBAO:
		{									
			// 余额检测由计费中心处理
			lTotalAmount = it->second.dwPrice * dwBuyNum * it->second.dwDiscount / 10;

			
			if(GetGame()->GetSetup()->lUseBillingServer==BILL_CONNECT_XM)
			{
				CMessage msg(MSG_S2B_ACCOUNT_LOCK);
				msg.Add((BYTE)eICT_NORMAL_BUY_ITEM);		//消费类型
				msg.Add(pPlayer->GetExID());				//player guid
				msg.Add(pPlayer->GetAccount());				//player cdkey
				msg.Add(NULL_GUID);							//attach guid
				msg.Add((DWORD)1);
				msg.Add(dwGoodsIndex);						//goods id
				msg.Add(dwBuyNum);							//goods num
				msg.Add(lTotalAmount);						//money sum
				msg.Add(dwTradeMode);
				msg.SendToBS();
			}
			else if(GetGame()->GetSetup()->lUseBillingServer==BILL_CONNECT_SNDA)
			{
				if(GetGame()->IsSDOAValid())
				{
					vector<CGoods*> vTestGoods;
					//创建要购买的物品VECTOR
					CGoodsFactory::CreateGoods( dwGoodsIndex,it->second.dwGoodsNum * dwBuyNum, vTestGoods,278 );
					if( !vTestGoods.empty() )
					{	
						CTestContainer testContainer;
						testContainer.Init(pPlayer);
						vector<tagTestResult> vecResult;
						BOOL bSucced=testContainer.AddTest(vTestGoods,&vecResult);	
						for(size_t i=0;i<vTestGoods.size();i++)
						{
							MP_DELETE(vTestGoods[i]);
						}
						vTestGoods.clear();
						if(!bSucced)
						{
							char* strRes=CStringReading::LoadString(22,3);
							if(strRes)
								pPlayer->SendNotifyMessage(strRes);									
							return;
						}
					}
					else
						return;
					/////////////
					ISDOAOrderHandler* pOrderHandler=GetGame()->GetSDOAOrderHandler();
					ULONG orderObj=pOrderHandler->CreateOrderInfo(6);
					if(orderObj==0)
						return;
					char szOrderId[33];
					char szData[128];
					char szURL[2048];
					char szGoodsInfoEx[512];
					ZeroMemory(szOrderId,33);
					ZeroMemory(szData,128);
					ZeroMemory(szURL,2048);
					ZeroMemory(szGoodsInfoEx,512);
					//生成订单唯一编号
					pOrderHandler->GetUniqueId(szOrderId);
					//填写订单内容
					pOrderHandler->SetOrderInfoValue(orderObj,"orderId",szOrderId);
					pOrderHandler->SetOrderInfoValue(orderObj,"userId",pPlayer->GetAccount());
					pOrderHandler->SetOrderInfoValue(orderObj,"roleName","");
					//payInfo
					_snprintf(szData, 128, "3|%d",lTotalAmount);
					pOrderHandler->SetOrderInfoValue(orderObj,"payInfo",szData);
					//feelInfo							
					pOrderHandler->SetOrderInfoValue(orderObj,"feeInfo","");
					//goodsNum
					pOrderHandler->SetOrderInfoValue(orderObj,"goodsNum","1");
					//goodsInfoEx
					ZeroMemory(szGoodsInfoEx,512);
					_snprintf(szGoodsInfoEx, 512, "%d,%s,%d,%d",dwGoodsIndex,pProperties->GetName(),dwBuyNum,lTotalAmount);
					pOrderHandler->SetOrderInfoValue(orderObj,"goodsInfoEx",szGoodsInfoEx);
					//notifyAddrType
					pOrderHandler->SetOrderInfoValue(orderObj,"notifyAddrType","3");
					//notifyAddr
					pOrderHandler->SetOrderInfoValue(orderObj,"notifyAddr","");
					//postBackUrl
					pOrderHandler->SetOrderInfoValue(orderObj,"postBackUrl","");
					//callTime
					ZeroMemory(szData,128);
					SYSTEMTIME stTime;
					GetLocalTime(&stTime);
					/*
					_snprintf(szData,"%d-%02d-%02d %02d:%02d:%02d",stTime.wYear,stTime.wMonth,stTime.wDay,
					stTime.wHour,stTime.wMinute,stTime.wSecond);
					pOrderHandler->SetOrderInfoValue(orderObj,"callTime",szData);*/
					//sessionId
					pOrderHandler->SetOrderInfoValue(orderObj,"sessionId","0");
					//indication
					if( dwFenHongFlag==1 ) // 参与分红
						pOrderHandler->SetOrderInfoValue(orderObj,"indication","164");
					else
						pOrderHandler->SetOrderInfoValue(orderObj,"indication","0");
					//responseType
					pOrderHandler->SetOrderInfoValue(orderObj,"responseType","1");
					//remark
					pOrderHandler->SetOrderInfoValue(orderObj,"remark","");

					pOrderHandler->SetOrderInfoValue(orderObj,"reserveInt","0");
					pOrderHandler->SetOrderInfoValue(orderObj,"reserveString","");

					//生成签名URL
					pOrderHandler->BuildOrderUrl(orderObj,szURL);
					CGoodsFactory::PushPendingOrder(szOrderId,pPlayer->GetExID());
					//PutDebugString(szURL);

					//URL发送给客户端
					CMessage msg(MSG_S2C_OTHER_BILLURL);
					msg.Add(szURL);
					msg.SendToPlayer(pPlayer->GetExID());
					//销毁order对象
					pOrderHandler->DestroyOrderInfo(orderObj);
					AddLogText("logT521_snda_url_send_log");
					GetGameLogInterface()->logT521_snda_url_send_log(pPlayer,szOrderId,lTotalAmount,szGoodsInfoEx);
				}
			}					
		}
		break;
	case CIncrementShopList::TM_TICKET:
		{
			if(it->second.dwPrice==0)
				return;
			//计算总价
			lTotalAmount = it->second.dwPrice * dwBuyNum * it->second.dwDiscount / 10;
			if( pPlayer->GetTicket() < lTotalAmount)
			{
				char strTemp[256] = "";
				char* strRes=CStringReading::LoadString(22,20);
				_snprintf( strTemp, 256, strRes, lTotalAmount);
				if( strTemp && strRes )
					pPlayer->SendNotifyMessage(strTemp);
				return;
			}

			dwBuyNum*=it->second.dwGoodsNum;
			bool bConfirm = IncShopProcessing( pPlayer, it->second.dwGoodsIndex, dwBuyNum, dwTradeMode, lTotalAmount );
			if( bConfirm )
			{
				// 扣除点券
				if( lTotalAmount )
					pPlayer->SetTicket( pPlayer->GetTicket() - lTotalAmount );
			}
			else
			{
				char* strRes=CStringReading::LoadString(22,3);
				if(strRes)
					pPlayer->SendNotifyMessage(strRes);
			}
		}
		break;
	case CIncrementShopList::TM_WEIMIAN:
		{
			lTotalAmount = it->second.dwPrice * dwBuyNum * it->second.dwDiscount / 10;
			if ( lTotalAmount < 0 ) return;
			//检测位面是否足够
			if ( pPlayer->GetGoodsAmountByOriginName( 
				CGoodsFactory::QueryGoodsOriginalName( 
				CGlobeSetup::GetSetup()->dwWeimianGoodsId ) )  < lTotalAmount )
			{
				//位面精华不足,无法购买
				char* strRes=CStringReading::LoadString(22,23);
				if(strRes)
					pPlayer->SendNotifyMessage(strRes);
				return;
			}

			//玩家物品添加检测，位面移除检测
			vector<CGoods*> vTestGoods;
			CGoodsFactory::CreateGoods( dwGoodsIndex,it->second.dwGoodsNum *dwBuyNum, vTestGoods,599 );
			vector<tagDelTestResult> vecDelResult;
			if( !vTestGoods.empty() )
			{	
				CTestContainer testContainer;
				testContainer.Init(pPlayer);
				vector<tagTestArg_ID_Num> vecDelGoodsForBuyer;
				tagTestArg_ID_Num DelTest;
				DelTest._dwGoodsID = CGlobeSetup::GetSetup()->dwWeimianGoodsId;//180
				DelTest._dwNum = lTotalAmount;
				vecDelGoodsForBuyer.push_back( DelTest );
				BOOL bDelSucced = testContainer.DelTest( vecDelGoodsForBuyer, &vecDelResult );
				BOOL bAddSucced = testContainer.AddTest(vTestGoods);
				for(size_t i=0;i<vTestGoods.size();i++)
				{
					MP_DELETE(vTestGoods[i]);
				}
				vTestGoods.clear();
				if ( !bDelSucced ) return;
				if( !bAddSucced )
				{
					char* strRes=CStringReading::LoadString(22,3);
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);									
					return;
				}
			}

			//买家所需支付的位面精华移除操作
			for ( int i = 0; i < vecDelResult.size(); ++i )
			{
				CVolumeLimitGoodsContainer* pVolumeContainer =
					dynamic_cast<CVolumeLimitGoodsContainer*>( pPlayer->FindContainer(vecDelResult[i].dwContainerId) );
				if(pVolumeContainer)
				{
					if( vecDelResult[i].eOperType == CS2CContainerObjectMove::OT_DELETE_OBJECT)
					{
						//删除
						CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecDelResult[i].dwPos );
						CGoods* pGoods = 
							dynamic_cast<CGoods*>( pVolumeContainer->Remove( goodsGuid ) );

						//发送删除物品的消息
						CS2CContainerObjectMove comMsg;
						comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						comMsg.SetSourceContainer(TYPE_PLAYER,pPlayer->GetExID(),
							vecDelResult[i].dwPos );
						comMsg.SetSourceObject( pGoods->GetType(),pGoods->GetExID() );
						comMsg.SetSourceObjectAmount( pGoods->GetAmount() );
						comMsg.SetSourceContainerExtendID( vecDelResult[i].dwContainerId );
						comMsg.Send( pPlayer->GetExID() );
						if ( pGoods )
						{
							CGoodsFactory::GarbageCollect(&pGoods,222);
							GetGameLogInterface()->logT504_weimian_log( pPlayer, vecDelResult[i].dwNum, 1, 1 );
						}
						else
						{
							return;
						}
					}
					else if ( vecDelResult[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT )
					{
						//修改数量
						CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecDelResult[i].dwPos );
						CGoods* pGoods =pVolumeContainer->GetGoods( vecDelResult[i].dwPos );
						LONG lGoodsNum = pGoods->GetAmount() - vecDelResult[i].dwNum;
						pGoods->SetAmount( lGoodsNum );
						if ( pGoods )
						{
							CS2CContainerObjectAmountChange coacMsg;
							coacMsg.SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), 
								vecDelResult[i].dwPos );
							coacMsg.SetSourceContainerExtendID( vecDelResult[i].dwContainerId );
							coacMsg.SetObject( pGoods->GetType(), pGoods->GetExID() );
							coacMsg.SetObjectAmount( lGoodsNum );
							coacMsg.Send( pPlayer->GetExID() );
							GetGameLogInterface()->logT504_weimian_log( pPlayer, vecDelResult[i].dwNum, 1, 2 );
						}
					}
				}
			}

			dwBuyNum*=it->second.dwGoodsNum;
			bool bConfirm = IncShopProcessing( pPlayer, it->second.dwGoodsIndex, dwBuyNum, dwTradeMode, lTotalAmount );
		}
		break;
	default:
		{
			char* strRes=CStringReading::LoadString(22,21);
			if(strRes)
				pPlayer->SendNotifyMessage(strRes);
			return;
		}
		break;
	}
}

int SNDACALLBACK MsgHandle(int msgType,int nMsgFrom, int nMsgSeq, ISDOAMsg* req) 
{
	PutStringToFile("MsgHandleCallback","Function Start");
	//根据类型处理回调信息,具体参照RecvMsgCallback示例及ISDOAMsg接口
	static int nCallTimes=0;

	switch(msgType)
	{
	case SDOA_ITEM_SEND_REQUEST:
		{	
			tagSDOA_ITEM_SEND_REQUEST stSdo_Item_Send_Request;
			//orderId
			stSdo_Item_Send_Request.szOrderId=req->GetValue("orderId");

			if(!stSdo_Item_Send_Request.szOrderId)
			{				
					return 0;
			}
			//payStatus
			stSdo_Item_Send_Request.szPayStatus=req->GetValue("payStatus");
			if(!stSdo_Item_Send_Request.szPayStatus)
				return 0;			

			//sendTimes
			stSdo_Item_Send_Request.szSendTimes=req->GetValue("sendTimes");
			if (!stSdo_Item_Send_Request.szSendTimes)
			{
				stSdo_Item_Send_Request.szSendTimes = "";
			}

			//callTime
			stSdo_Item_Send_Request.szCallTime=req->GetValue("callTime");
			if (!stSdo_Item_Send_Request.szCallTime)
			{
				stSdo_Item_Send_Request.szCallTime = "";
			}

			//userId
			stSdo_Item_Send_Request.szUserId=req->GetValue("userId");
			if (!stSdo_Item_Send_Request.szUserId)
			{
				stSdo_Item_Send_Request.szUserId = "";
			}


			//roleName
			stSdo_Item_Send_Request.szRoleName=req->GetValue("roleName");
			if (!stSdo_Item_Send_Request.szRoleName)
			{
				stSdo_Item_Send_Request.szRoleName = "";
			}


			//goodsNum
			stSdo_Item_Send_Request.szGoodsNum=req->GetValue("goodsNum");
			if (!stSdo_Item_Send_Request.szGoodsNum)
			{
				stSdo_Item_Send_Request.szGoodsNum = "";
			}

			//goodsInfoEx
			stSdo_Item_Send_Request.szGoodsInfoEx=req->GetValue("goodsInfoEx");
			if (!stSdo_Item_Send_Request.szGoodsInfoEx)
			{
				stSdo_Item_Send_Request.szGoodsInfoEx = "";
			}

			//indication
			stSdo_Item_Send_Request.szIndication=req->GetValue("indication");	
			if (!stSdo_Item_Send_Request.szIndication)
			{
				stSdo_Item_Send_Request.szIndication = "";
			}

			//payErrorMsg
			stSdo_Item_Send_Request.payErrorMsg=req->GetValue("payErrorMsg");
			if (!stSdo_Item_Send_Request.payErrorMsg)
			{
				stSdo_Item_Send_Request.payErrorMsg = "";
			}

			stSdo_Item_Send_Request.nMsgType = msgType;
			stSdo_Item_Send_Request.nMsgFrom = nMsgFrom;
			stSdo_Item_Send_Request.nMsgSeq = nMsgSeq;
			GetGame()->OnReceiveSdoItemRequest(stSdo_Item_Send_Request);
#ifdef _ASCYN_DEBUG
			char szOutPut[512];
			_snprintf(szOutPut,512,"MsgHandle====OnReceiveSdoItemRequest msgtype:%d;msgform:%d,msgseq:%d,szOrderId:%s",stSdo_Item_Send_Request.nMsgType,stSdo_Item_Send_Request.nMsgFrom,stSdo_Item_Send_Request.nMsgSeq,stSdo_Item_Send_Request.szOrderId);
			PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
		}
		break;
	}
	PutStringToFile("MsgHandleCallback","Function Start");
	return 0; 
}

void OnIncrementShopMessage(CMessage* pMsg)
{
	CPlayer* pPlayer = pMsg->GetPlayer();
	if( pPlayer == NULL )
	{
		return;
	}

	//玩家死亡，关闭交易
	if( pPlayer->IsDied() )
	{
		CancelIncrementTrading( pPlayer );
		char* strRes=CStringReading::LoadString(22,16);
		if(strRes)
			pPlayer->SendNotifyMessage( strRes );
		return;
	}

	CServerRegion* pRegion = pMsg->GetRegion();
	if( pRegion == NULL )
	{
		CancelIncrementTrading( pPlayer );
		return;
	}

	switch(pMsg->GetType()) 
	{
	case MSG_C2S_INCREMENTSHOP_GETLOG_REQUEST:
		{
			CIncrementShopList::VEC_BUYLIST vecBuyList = pPlayer->GetIncTradeList();
			CMessage msg( MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE );
			msg.Add( (long)vecBuyList.size() );
			CIncrementShopList::RITR_BUYLIST itBuylist = vecBuyList.rbegin();
			for ( ; itBuylist != vecBuyList.rend(); ++itBuylist )
			{
				msg.Add( itBuylist->strBuyTime.c_str() );
				msg.Add( itBuylist->dwPriceType );
				msg.Add( itBuylist->dwPriceNum );
				msg.Add( itBuylist->dwBuyNum );
				msg.Add( itBuylist->dwGoodsIdx );
			}
			msg.SendToPlayer( pPlayer->GetExID() );
		}
		break;

	case MSG_C2S_INCREMENTSHOP_AFFICHE_REQUEST:
		{
			CMessage msg(MSG_S2C_INCREMENTSHOP_AFFICHE_RESPONSE);
			msg.Add( (long)CIncrementShopList::GetAffiche()->length() );
			msg.AddEx( (void*)CIncrementShopList::GetAffiche()->data(), (long)CIncrementShopList::GetAffiche()->length() );
			msg.SendToPlayer(pPlayer->GetExID());
		}
		break;

	case MSG_C2S_INCREMENTSHOP_OPEN:		// 打开增值商店
		{
			if( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_NONE )
			{
				CMessage msg(MSG_S2C_INCREMENTSHOP_OPEN);
				msg.Add((BYTE)0);			// 失败
				msg.SendToPlayer(pPlayer->GetExID());
				return;
			}

			pPlayer->ProduceEvent(ET_EventBreak);

			CIncrementShopList::MAP_TOP10* pTop10List = CIncrementShopList::GetTop10List();
			CIncrementShopList::MAP_NEWGOODS* pNewGoodsList = CIncrementShopList::GetNewGoodsList();
			CIncrementShopList::MAP_GUIDEGOODS* pGuideGoodsList = CIncrementShopList::GetGuideGoodsList();
			CIncrementShopList::MAP_FILTERGOODS* pFilterListOfWShop = CIncrementShopList::GetFilterListOfWShop();
			CIncrementShopList::MAP_FILTERGOODS* pFilterListOfXShop = CIncrementShopList::GetFilterListOfXShop();
			CIncrementShopList::MAP_FILTERGOODS* pFilterListOfTShop = CIncrementShopList::GetFilterListOfTShop();
			CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
			CIncrementShopList::MAP_INCCLASSGOODS* pIncClassGoodsList = CIncrementShopList::GetIncClassGoods();
			CIncrementShopList::MAP_INCSHOP* pMapIncShopList = CIncrementShopList::GetMapIncShopList();
			CIncrementShopList::MAP_INCMAP* pMapIndexList = CIncrementShopList::GetMapIndexList();

			if ( !pTop10List || !pNewGoodsList || !pGuideGoodsList || !pFilterListOfWShop
				|| !pFilterListOfXShop|| !pFilterListOfTShop || !pIncGoodsList 
				|| !pIncClassGoodsList || !pMapIncShopList || !pMapIndexList)
			{
				CMessage msg(MSG_S2C_INCREMENTSHOP_OPEN);
				msg.Add((BYTE)0);			// 失败
				msg.SendToPlayer(pPlayer->GetExID());
				return;
			}

			// 当前为商店活动，不能移动。
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_INCREMENT );
			pPlayer->SetMoveable( FALSE );

			CMessage msg(MSG_S2C_INCREMENTSHOP_OPEN);
			msg.Add((BYTE)1);				// 成功

			BYTE byteFlag = pPlayer->GetIncShopFlag();
			msg.Add(byteFlag);

			// 玩家登陆时发送所有商品信息
			if( byteFlag )
			{
				long lNum = pTop10List->size();
				assert( lNum == 3 );
				msg.Add( (BYTE)lNum );//商店种类，目前为三种商店（位面商店 & 星钻商店 & 点券商店）
				for ( int i = 0; i < lNum; ++i )
				{
					DWORD dwShopType = i;
					CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwShopType );
					CIncrementShopList::ITR_INCCLASSGOODS itIncClassGoods = pIncClassGoodsList->find( dwShopType );
					CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
					CIncrementShopList::MAP_CLASSGOODS& mapIncClassGoods = itIncClassGoods->second;

					//详细商品列表
					msg.Add( (long)mapIncGoodsList.size() );
					CIncrementShopList::ITR_CLASSGOODS iterIncGoods = mapIncClassGoods.begin();
					for ( ; iterIncGoods != mapIncClassGoods.end(); ++iterIncGoods )
					{
						CIncrementShopList::VEC_GOODSIDX& vecGoodsIdx = iterIncGoods->second;
						CIncrementShopList::ITR_GOODSIDX iter = vecGoodsIdx.begin();
						for ( ; iter != vecGoodsIdx.end(); ++iter )
						{
							CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iter );
							if(!pProperties) 
								return;

							CIncrementShopList::ITR_INCSHOP iterIncGoods;
							iterIncGoods = mapIncGoodsList.find( *iter );
							if ( iterIncGoods == mapIncGoodsList.end() )
								return;
									msg.Add( iterIncGoods->second.dwGoodsType );
									msg.Add( iterIncGoods->second.dwGoodsIndex );
									msg.Add( iterIncGoods->second.dwPrice );
									msg.Add( iterIncGoods->second.dwGoodsNum );
									msg.Add( (BYTE)iterIncGoods->second.dwDiscount );
									msg.Add( iterIncGoods->second.strTimeLimit.c_str() );
									if ( iterIncGoods->second.lQuantityLimit != -1 )
									{
										DWORD dwWorldAlreadyBuyNum = CIncrementShopManager::GetWorldLimitNumByGoodsID( *iter );
										DWORD dwNum = iterIncGoods->second.lQuantityLimit - dwWorldAlreadyBuyNum;
										msg.Add( dwNum );
									}
									else
									{
										msg.Add( iterIncGoods->second.lQuantityLimit );
									}
									if ( iterIncGoods->second.lBuyLimit != -1 )
									{
										DWORD dwAlreadyBuyNum = pPlayer->GetLimitGoodsBuyNum( *iter );
										DWORD dwNum = iterIncGoods->second.lBuyLimit - dwAlreadyBuyNum;
										msg.Add( dwNum );
									}
									else
									{
										msg.Add( iterIncGoods->second.lBuyLimit );
									}


							if (dwShopType == CIncrementShopList::TM_YUANBAO)
							{
								//分红配置
								CIncrementShopList::ITR_MAPLIST it = pMapIndexList->find(*iter);
								if (it != pMapIndexList->end())
								{
									msg.Add((BYTE)CIncrementShopList::PM_PROFIT);
								}
								else
								{
									msg.Add((BYTE)CIncrementShopList::PM_NORMAL);
								}
							}
							else
							{
								msg.Add((BYTE)CIncrementShopList::PM_NORMAL);
							}
						}
					}

					//热销商品
					CIncrementShopList::ITR_TOP10 itTop10 = pTop10List->find( dwShopType );
					CIncrementShopList::VEC_GOODSIDX& vecTop10GoodsIdxList = itTop10->second;
					msg.Add( (long)vecTop10GoodsIdxList.size() );
					CIncrementShopList::ITR_GOODSIDX iterTop10GoodsIdx = vecTop10GoodsIdxList.begin();
					for ( ; iterTop10GoodsIdx != vecTop10GoodsIdxList.end(); ++iterTop10GoodsIdx )
					{
						CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterTop10GoodsIdx );
						CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterTop10GoodsIdx );
						if ( pProperties && itInc != mapIncGoodsList.end() )
						{
							assert( *iterTop10GoodsIdx == itInc->second.dwGoodsIndex );
							msg.Add( itInc->second.dwGoodsType );
							msg.Add( itInc->second.dwGoodsIndex );
						}
						else
						{
							return;
						}
					}

					//新品推荐
					CIncrementShopList::ITR_NEWGOODS itNewGoods = pNewGoodsList->find( dwShopType );
					CIncrementShopList::VEC_GOODSIDX& vecNewGoodsIdxList = itNewGoods->second;
					msg.Add( (long)vecNewGoodsIdxList.size() );
					CIncrementShopList::ITR_GOODSIDX iterNewGoodsIdx = vecNewGoodsIdxList.begin();
					for ( ; iterNewGoodsIdx != vecNewGoodsIdxList.end(); ++iterNewGoodsIdx )
					{
						CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterNewGoodsIdx );
						CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterNewGoodsIdx );
						if ( pProperties && itInc != mapIncGoodsList.end() )
						{
							assert( *iterNewGoodsIdx == itInc->second.dwGoodsIndex );
							msg.Add( itInc->second.dwGoodsType );
							msg.Add( itInc->second.dwGoodsIndex );
						}
						else
						{
							return;
						}
					}

					//导购商品列表
					CIncrementShopList::ITR_GUIDEGOODSLIST itGuideGoods = pGuideGoodsList->find( dwShopType );
					CIncrementShopList::MAP_TYPEGOODS& mapTypeGoods = itGuideGoods->second;
					msg.Add( (long)mapTypeGoods.size() );
					CIncrementShopList::ITR_TYPEGOODS iterType = mapTypeGoods.begin();
					for ( ; iterType != mapTypeGoods.end(); ++iterType )
					{
						string strGuideType = iterType->first;
						msg.Add( strGuideType.c_str() );
						CIncrementShopList::VEC_GOODSIDX& vecGuideGoodsIdxList = iterType->second;
						msg.Add( (long)vecGuideGoodsIdxList.size() );
						CIncrementShopList::ITR_GOODSIDX iterGuideGoodsIdxList = vecGuideGoodsIdxList.begin();
						for ( ; iterGuideGoodsIdxList != vecGuideGoodsIdxList.end(); ++iterGuideGoodsIdxList )
						{
							CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterGuideGoodsIdxList );
							CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterGuideGoodsIdxList );
							if ( pProperties && itInc != mapIncGoodsList.end() )
							{
								assert( *iterGuideGoodsIdxList == itInc->second.dwGoodsIndex );
								msg.Add( itInc->second.dwGoodsType );
								msg.Add( itInc->second.dwGoodsIndex );
							}
							else
							{
								return;
							}
						}
					}

					//筛选商品列表
					if ( dwShopType == CIncrementShopList::TM_WEIMIAN )
					{
						msg.Add( (long)pFilterListOfWShop->size() );//物品种类
						CIncrementShopList::ITR_FILTERTGOODSLIST iterFilter = pFilterListOfWShop->begin();
						for ( ; iterFilter != pFilterListOfWShop->end(); ++iterFilter )
						{
							DWORD dwGoodsType = iterFilter->first;
							CIncrementShopList::MAP_TYPEGOODS& mapTypeGoods = iterFilter->second;
							msg.Add( (long)mapTypeGoods.size() );
							CIncrementShopList::ITR_TYPEGOODS itFilterGoods = mapTypeGoods.begin();
							for ( ; itFilterGoods != mapTypeGoods.end(); ++itFilterGoods )
							{					
								string strFilterType = itFilterGoods->first;
								msg.Add( strFilterType.c_str() );
								CIncrementShopList::VEC_GOODSIDX& vecFilterGoodsIdxList = itFilterGoods->second;
								msg.Add( (long)vecFilterGoodsIdxList.size() );
								CIncrementShopList::ITR_GOODSIDX iterFilterGoodsIdxList = vecFilterGoodsIdxList.begin();
								for ( ; iterFilterGoodsIdxList != vecFilterGoodsIdxList.end(); ++iterFilterGoodsIdxList )
								{
									CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterFilterGoodsIdxList );
									CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterFilterGoodsIdxList );
									if ( pProperties && itInc != mapIncGoodsList.end() )
									{
										assert( *iterFilterGoodsIdxList == itInc->second.dwGoodsIndex );
										msg.Add( itInc->second.dwGoodsType );
										msg.Add( itInc->second.dwGoodsIndex );
									}
									else
									{
										return;
									}
								}
							}
						}
					}
					else if ( dwShopType == CIncrementShopList::TM_YUANBAO )
					{
						msg.Add( (long)pFilterListOfXShop->size() );//物品种类
						CIncrementShopList::ITR_FILTERTGOODSLIST iterFilter = pFilterListOfXShop->begin();
						for ( ; iterFilter != pFilterListOfXShop->end(); ++iterFilter )
						{
							DWORD dwGoodsType = iterFilter->first;
							CIncrementShopList::MAP_TYPEGOODS& mapTypeGoods = iterFilter->second;
							msg.Add( (long)mapTypeGoods.size() );
							CIncrementShopList::ITR_TYPEGOODS itFilterGoods = mapTypeGoods.begin();
							for ( ; itFilterGoods != mapTypeGoods.end(); ++itFilterGoods )
							{
								string strFilterType = itFilterGoods->first;
								msg.Add( strFilterType.c_str() );
								CIncrementShopList::VEC_GOODSIDX& vecFilterGoodsIdxList = itFilterGoods->second;
								msg.Add( (long)vecFilterGoodsIdxList.size() );
								CIncrementShopList::ITR_GOODSIDX iterFilterGoodsIdxList = vecFilterGoodsIdxList.begin();
								for ( ; iterFilterGoodsIdxList != vecFilterGoodsIdxList.end(); ++iterFilterGoodsIdxList )
								{
									CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterFilterGoodsIdxList );
									CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterFilterGoodsIdxList );
									if ( pProperties && itInc != mapIncGoodsList.end() )
									{
										assert( *iterFilterGoodsIdxList == itInc->second.dwGoodsIndex );
										msg.Add( itInc->second.dwGoodsType );
										msg.Add( itInc->second.dwGoodsIndex );
									}
									else
									{
										return;
									}
								}
							}
						}
					}
					else if ( dwShopType == CIncrementShopList::TM_TICKET )
					{
						msg.Add( (long)pFilterListOfTShop->size() );//物品种类
						CIncrementShopList::ITR_FILTERTGOODSLIST iterFilter = pFilterListOfTShop->begin();
						for ( ; iterFilter != pFilterListOfTShop->end(); ++iterFilter )
						{
							DWORD dwGoodsType = iterFilter->first;
							CIncrementShopList::MAP_TYPEGOODS& mapTypeGoods = iterFilter->second;
							msg.Add( (long)mapTypeGoods.size() );
							CIncrementShopList::ITR_TYPEGOODS itFilterGoods = mapTypeGoods.begin();
							for ( ; itFilterGoods != mapTypeGoods.end(); ++itFilterGoods )
							{
								string strFilterType = itFilterGoods->first;
								msg.Add( strFilterType.c_str() );
								CIncrementShopList::VEC_GOODSIDX& vecFilterGoodsIdxList = itFilterGoods->second;
								msg.Add( (long)vecFilterGoodsIdxList.size() );
								CIncrementShopList::ITR_GOODSIDX iterFilterGoodsIdxList = vecFilterGoodsIdxList.begin();
								for ( ; iterFilterGoodsIdxList != vecFilterGoodsIdxList.end(); ++iterFilterGoodsIdxList )
								{	
									CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iterFilterGoodsIdxList );
									CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iterFilterGoodsIdxList );
									if ( pProperties && itInc != mapIncGoodsList.end() )
									{
										assert( *iterFilterGoodsIdxList == itInc->second.dwGoodsIndex );
										msg.Add( itInc->second.dwGoodsType );
										msg.Add( itInc->second.dwGoodsIndex );
									}
									else
									{
										return;
									}
								}
							}
						}
					}
				}
				pPlayer->SetIncShopFlag();
			}

			//商店最近购买列表
			long lListSize = (long)pPlayer->GetIncShopCur10().size();
			msg.Add( (BYTE)lListSize );
			CPlayer::IncShopCur10Iter iterCur10 = pPlayer->GetIncShopCur10().begin();
			for ( ; iterCur10 != pPlayer->GetIncShopCur10().end(); ++iterCur10 )
			{	
				DWORD dwShopType = iterCur10->first;
				msg.Add( (BYTE)dwShopType );
				list<DWORD>&  lstCur10 = iterCur10->second;
				msg.Add( (long)lstCur10.size() );
				list<DWORD>::iterator iter = lstCur10.begin();
				for ( ; iter != lstCur10.end(); ++iter )
				{
					CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( *iter );
					CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwShopType );
					CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
					CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( *iter );
					if ( pProperties && itInc != mapIncGoodsList.end() )
					{
						msg.Add( itInc->second.dwGoodsType );
						msg.Add( itInc->second.dwGoodsIndex );
					}
					else 
					{
						msg.Add( (DWORD)0);
						msg.Add( (DWORD)0);
					}
				}
			}
			//返还参数数据
			CIncrementShopList::tagReturnParam *ptgReturnParam = CIncrementShopList::GetReturnParam();

			list<string>::iterator it = ptgReturnParam->lSGoodsName.begin();

			msg.Add(ptgReturnParam->fNoramlRatio);
			msg.Add(ptgReturnParam->fSpecialRatio);
			msg.Add((char*)ptgReturnParam->strStartTime.c_str());
			msg.Add((char*)ptgReturnParam->strEndTime.c_str());
			msg.Add(long(ptgReturnParam->lSGoodsName.size()));
			for (; it!=ptgReturnParam->lSGoodsName.end(); ++it)
			{
				msg.Add((char*)(*it).c_str());
			}

			
			msg.SendToPlayer(pPlayer->GetExID());
		}
		break;

	// 王峰：请求一个物品信息
	case MSG_C2S_INCREMENTSHOP_GET_INFO:
		{	
			// 物品ID
			DWORD dwGoodsIndex	= pMsg->GetDWord();

			// 获取门店表
			CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
			if( pIncGoodsList )
			{
				// 获取 星钻门店
				CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( CIncrementShopList::TM_YUANBAO );
				if ( itIncGoodsList != pIncGoodsList->end() )
				{
					// 找到物品
					CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
					CIncrementShopList::ITR_INCSHOP it = mapIncGoodsList.find( dwGoodsIndex );
					if( it != mapIncGoodsList.end() )
					{
						// 当前为商店活动，不能移动。
						pPlayer->SetCurrentProgress( CPlayer::PROGRESS_INCREMENT );

						// 发消息
						CIncrementShopList::IncrementShopElem&	tsGoods = it->second;

						CMessage msg( MSG_S2C_INCREMENTSHOP_RE_INFO );
						msg.Add( tsGoods.dwGoodsType );
						msg.Add( tsGoods.dwGoodsIndex );
						msg.Add( tsGoods.dwPrice );
						msg.Add( tsGoods.dwGoodsNum );
						msg.Add( tsGoods.dwDiscount );
						msg.Add( tsGoods.lQuantityLimit );
						msg.Add( tsGoods.lBuyLimit );
						msg.SendToPlayer( pPlayer->GetExID() );
					}
				}
			}
		}

	case MSG_C2S_INCREMENTSHOP_BUY:			// 玩家请求在商店中购买一个物品
		{
			DWORD dwGoodsIndex	= pMsg->GetDWord();	// 购买的物品ID
			DWORD dwNum			= pMsg->GetDWord();	// 准备购买的数量，如果没有数量则直接返回。
			DWORD dwTradeMode	= pMsg->GetDWord();	// 交易方式（元宝、点券、位面） 和商店类型一致

			DWORD dwQuestFlag = CIncrementShopList::IsProfitGoods(dwGoodsIndex);				
			
			if( dwNum == 0 ) return;
			if( dwTradeMode > CIncrementShopList::TM_TICKET ) return;			

			if ( pPlayer->GetCurrentProgress() != CPlayer::PROGRESS_INCREMENT )
			{
				CancelIncrementTrading( pPlayer );
				return;
			}

			CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
			CIncrementShopList::ITR_INCSHOP it;
			if ( !pIncGoodsList )
			{
				//提示获取商品列表失败，退出。
				char* strRes=CStringReading::LoadString(22,17);
				if(strRes)
					pPlayer->SendNotifyMessage(strRes);
				CancelIncrementTrading( pPlayer );
				return;
			}
			else
			{
				CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwTradeMode );
				if ( itIncGoodsList == pIncGoodsList->end() )
					return;
				CIncrementShopList::MAP_INCSHOP& mapIncGoodsList = itIncGoodsList->second;
				it = mapIncGoodsList.find( dwGoodsIndex );
				if( it == mapIncGoodsList.end() )
				{
					char* strRes=CStringReading::LoadString(22,18);
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
					return;
				}
			}
	
			CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwGoodsIndex );
			if( !pProperties )
			{
				char* strRes=CStringReading::LoadString(22,19);
				if(strRes)
					pPlayer->SendNotifyMessage(strRes);
				return;
			}

			//唯一检测	
			if(pProperties->IsHasAddonPropertie(GAP_UNIQUE))
			{			
				if(dwNum>1)
				{
					char* strMsg=CStringReading::LoadString(20,54);
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
				if(!pPlayer->IsGoodsUnique(CGoodsFactory::QueryGoodsOriginalName(dwGoodsIndex)))
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

			// 首先确定数量的有效性
			if( pProperties->GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT )
			{
				// 装备类道具数量必须为1
				//dwNum = 1;
			}

			//限时，限量，限购判断
			//限时判断
			std::string strTimeLimit = CIncrementShopList::GetLimitTimeByID( dwTradeMode, dwGoodsIndex );
			if ( strTimeLimit != "" )
			{
				if ( !CIncrementShopList::TimeLimited( strTimeLimit ) )
				{
					char* strRes=CStringReading::LoadString(22,32);
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
					CancelIncrementTrading( pPlayer );
					return;
				}
			}

			//个人限量判断(设置为-1表明此商品没有限量设置)
			long lAllowBuyNum = CIncrementShopList::GetLimitNumByID( dwTradeMode, dwGoodsIndex );
			if ( lAllowBuyNum != -1 )
			{						
				DWORD dwAlreadyBuyNum = pPlayer->GetLimitGoodsBuyNum( dwGoodsIndex );
				if ( (dwAlreadyBuyNum+dwNum) > lAllowBuyNum )
				{
					char* strRes=CStringReading::LoadString(22,34);
					if(strRes)
						pPlayer->SendNotifyMessage(strRes);
					CancelIncrementTrading( pPlayer );
					return;
				}
			}

			//限购判断(设置为-1表明此商品没有限量设置)
			long lWorldAllowBuyNum = CIncrementShopList::GetWorldBuyLimitNum( dwTradeMode, dwGoodsIndex );
			if ( lWorldAllowBuyNum != -1 )
			{
				CMessage msg( MSG_S2W_OTHER_LIMIT_GOODS_REQUEST );
				msg.Add( pPlayer->GetExID() );
				msg.Add( dwGoodsIndex );
				msg.Add( dwNum );
				msg.Add( dwTradeMode );
				msg.Add( lWorldAllowBuyNum );
				msg.Send();
			}
			else
			{
				ProcessIncrementTrading( pPlayer, dwGoodsIndex, dwNum, dwTradeMode, dwQuestFlag );
			}
		}
		break;
	case MSG_C2S_INCREMENTSHOP_CLOSE:
		//##客户端告诉服务器需要关闭商店。
		{
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );
			pPlayer->SetMoveable();			
		}
		break;
	}
}

void SDOSendResponse(int nMsgType,int nMsgForm,int nMsgSeq, list<tagValue> lstValue)
{
	
#ifdef _ASCYN_DEBUG
	char szOutPut[512];
	_snprintf(szOutPut,512,"SDOSendResponse(int nMsgType,int nMsgForm,int nMsgSeq, list<tagValue> lstValue)--SDO返回值大小:%d,nMsgType:%d,nMsgForm:%d,nMsgSeq:%d",lstValue.size(),nMsgType,nMsgForm,nMsgSeq);
	PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif

	ISDOAMsg* pRes = sdoaCreateMsg();
	list<tagValue>::iterator it=lstValue.begin();
	for (;it!=lstValue.end();++it)
	{
		pRes->SetValue((*it).strName.c_str(),(*it).strValue.c_str());
#ifdef _ASCYN_DEBUG
		_snprintf(szOutPut,512,"SDOSendResponse(int nMsgType,int nMsgForm,int nMsgSeq, list<tagValue> lstValue)--SDO设置名字:%s,值:%s",(*it).strName.c_str(),(*it).strValue.c_str());
		PutStringToFile("SDO_ITEM_SEND_RESPONSE",szOutPut);
#endif
	}
	GetGame()->GetSDOAOrderHandler()->SendResponse(nMsgType,nMsgForm,nMsgSeq,pRes);
	pRes->Release();

}