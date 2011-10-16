#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"

#include "..\player.h"
#include "..\Monster.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"

#include "..\Message Packaging\CS2CContainerObjectAmountChange.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "../GoodsExManage.h"
#include "..\business\CBusinessManager.h"
#include "..\Listener\CGoodsRepairListener.h"
#include "..\container\CTestContainer.h"
#include "..\Listener\CGoodsRepairPriceListener.h"

#include "..\..\..\setup\tradelist.h"
#include "..\..\..\setup\playerlist.h"
#include "..\..\..\setup\globesetup.h"
#include "../../../setup/logsystem.h"
#include "..\serverregion.h"

#include "../../../../public/StrUnifyInput.h"
#include "../FBusiness\ShopManager.h"


#define DELGOODS(vecGoods)\
for (int i=0;i<vecGoods.size();i++)\
{\
	CGoodsFactory::GarbageCollect(&vecGoods[i],169);\
}
#define SENDMSGBOX(pPlayer,szText)\
	CMessage msg(MSG_S2C_OTHER_GAMEMESSAGE);\
	msg.Add(szText);\
	msg.Add((long)0);\
	msg.Add(NULL_GUID);\
	msg.SendToPlayer(pPlayer->GetExID());
bool CheckUnique(CPlayer* pPlayer,CGoodsBaseProperties* pProperty,DWORD dwTotalNum)
{
	if(!pPlayer->IsGoodsUnique(pProperty->GetOriginalName())) 
	{
		SENDMSGBOX(pPlayer,CStringReading::LoadString(22,27));
		return false;
	}
	else
	{
		//玩家没有这个物品,但购买数量大于1,返回
		if(dwTotalNum>1)
		{
			SENDMSGBOX(pPlayer,CStringReading::LoadString(22,30));
			return false;
		}
	}
	return true;
}

VOID CancelTrading( CPlayer* pPlayer )
{
	pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_NONE );
	pPlayer -> SetMoveable();
	CMessage msg( MSG_S2C_SHOP_CLOSE );
	msg.SendToPlayer( pPlayer -> GetExID() );
}


void OnShopMessage(CMessage* pMsg)
{
	CPlayer* pPlayer = pMsg -> GetPlayer();
	if( pPlayer == NULL /*|| pPlayer -> GetCurrentProgress() != CPlayer::PROGRESS_SHOPPING*/ )
	{
		return;
	}
	if( pPlayer -> IsDied() )
	{
		CancelTrading( pPlayer );
		pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,1));
		return;
	}

	if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
	{
		return;
	}

	CServerRegion* pRegion = pMsg -> GetRegion();
	if( pRegion == NULL )
	{
		CancelTrading( pPlayer );
		return;
	}

	switch(pMsg->GetType()) 
	{	
	case MSG_C2S_SHOP_BUY:	//玩家请求在商店中购买一个物品		
		{					
			CGUID NpcID; 
			pMsg -> GetGUID(NpcID);
			DWORD dwGoodsId=pMsg->GetDWord();
			//购买的组数
			DWORD dwGoodsNum=pMsg->GetDWord();
			//根据dwContainerId检查是否是拖动物品到背包中
			DWORD dwContainerId=pMsg->GetDWord();
			DWORD dwContainerPos=pMsg->GetDWord();

			if(dwGoodsNum==0)
				return;

			//NPC检查
			CMonster* pNpc = (CMonster*)pRegion->FindAroundObject(pPlayer, TYPE_MONSTER, NpcID);

			if (pNpc)
			{
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					CancelTrading( pPlayer );
					break;
				}
#ifdef _GOODSLOG1_
				char pszLog1[1024]="";							

				_snprintf(pszLog1,1024,"[MSG_C2S_SHOP_BUY][NPC NAME:%s][物品ID:%d][物品名:%s][物品数量:%d][ConID:%d][ConPos:%d]",
					pNpc->GetName(),dwGoodsId,CGoodsFactory::QueryGoodsName(dwGoodsId),dwGoodsNum,dwContainerId,dwContainerPos);
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog1);							
#endif
			}
			else
			{
				//##NPC不存在
				CancelTrading( pPlayer );
				break;
			}

		/*	CServerRegion* pServerRegion=dynamic_cast<CServerRegion*>(pNpc->GetFather());
			if(!pServerRegion)
			{
				CancelTrading( pPlayer );
				break;
			}*/
		//	DWORD dwNpcAreaId=pServerRegion->GetAreaId();

			//物品检查
 			CTradeList::tagTrade *pTrade = CTradeList::GetTradeList(pNpc->GetOriginName());			
			if(!pTrade)
			{				
				CancelTrading( pPlayer );
				return;
			}
			bool bFound=false;
			CTradeList::tagTradeData* pTradeData=NULL;
			for(size_t i=0;i<pTrade->vecGoods.size();i++)
			{
				if(pTrade->vecGoods[i].dwGoodsId == dwGoodsId)
				{
					pTradeData=&pTrade->vecGoods[i];
					bFound=true;
					break;
				}
			}
			CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(dwGoodsId);
			if(!pProperty)
			{
				//消息发过来的物品ID不合法！
				return;
			}

			//声望检查
			if(!bFound || pTradeData==NULL)
				return;
			DWORD dwPrice=0,dwSilver=0,dwTotalPrice=0,dwTotalSilver=0;

			if (pTradeData->dwCreditId!=0 && pTrade->_creditType!=CTradeList::CREDIT_NOT)
			{
				//区域声望商店
				if (pTrade->_creditType==CTradeList::CREDIT_AREA)
				{
					if (pTradeData->dwCreditVal>pPlayer->GetCredit(pTradeData->dwCreditId))
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,29));
						return;
					}
				}
				//组织声望商店
				else if (pTrade->_creditType==CTradeList::CREDIT_ORGAN)
				{
					if(pTradeData->dwCreditVal>pPlayer->GetOrganCredit(pTradeData->dwCreditId))
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,29));
						return;
					}
				}
			}
			
			//物品总的数量，组数*每组个数
			DWORD dwTotalNum=dwGoodsNum*pTradeData->dwGoodsNum;
			CTestContainer testContainer;
			testContainer.Init(pPlayer);
			//bResult为真时要回收物品
			bool bResult=false,bGoodsChange=false;
			//指定位置，一次只能买一个物品
			if (dwContainerId!=0)
			{
				//位置检测
				CVolumeLimitGoodsContainer* pContainer=NULL;
				if(dwContainerId==CS2CContainerObjectMove::PEI_PACKET)
				{
					pContainer=pPlayer->getOriginPack();
				}
				else if(dwContainerId>=CS2CContainerObjectMove::PEI_PACK1 && dwContainerId<=CS2CContainerObjectMove::PEI_PACK5)
				{
					CGoods* pSubpackGoods=pPlayer->getSubpackContainer()->GetSubpack(dwContainerId-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
					if(!pSubpackGoods)
					{
						//没有装备子背包，dwContainerId是错误的
						return;
					}
					pContainer=pPlayer->getSubpackContainer()->GetSubpack(dwContainerId-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
				}
				if(!pContainer)
					return ;	
#ifdef _GOODSLOG1_
					char pszLog2[1024]="";							

				_snprintf(pszLog2,1024,"[MSG_C2S_SHOP_BUY][位置检测通过][NPC NAME:%s]",pNpc->GetName());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog2);							
#endif
				//买一个的数量与配置文件中的数量必然相等
				if (dwTotalNum!=pTradeData->dwGoodsNum)
				{
					return;
				}
				//创建物品
				CGoods* pGoods=CGoodsFactory::CreateGoods(dwGoodsId,32);
				if(!pGoods)
				{
					//创建物品失败
					return;
				}
				//唯一性检查
				if (pGoods->HasAddonProperty(GAP_UNIQUE))
				{
					if(!CheckUnique(pPlayer,pProperty,dwTotalNum))
						return;
				}
				pGoods->SetAmount(dwTotalNum);
				//货币商店
				if(pTrade->tradeType==CTradeList::ETT_MONEY || pTrade->tradeType==CTradeList::ETT_SILVER)
				{
					if(pProperty->GetPrice()==0 && pProperty->GetSilverPrice()==0)
					{
						return;
					}
					//先计算价格
					dwTotalPrice=pGoods->GetAmount()*pProperty->GetPrice();
					dwTotalSilver=pGoods->GetAmount()*pProperty->GetSilverPrice();
	
					DWORD dwLogPrice=dwTotalSilver;
					if(dwTotalPrice>0)
					{
						dwLogPrice=dwTotalPrice;
					}
	
					//金钱不够
					if(dwTotalPrice  > pPlayer->GetMoney())
					{
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 金钱不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
						CGoodsFactory::GarbageCollect(&pGoods,170);					
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,2));
						return;
					}
	
					//银币不够
					if(dwTotalSilver  > pPlayer->GetSilver())
					{
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 银币不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
						CGoodsFactory::GarbageCollect(&pGoods,171);	
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,14));
						return;
					}

#ifdef _GOODSLOG1_
					char pszLog3[1024]="";							

					_snprintf(pszLog3,1024,"[MSG_C2S_SHOP_BUY][金币银币检测通过][NPC NAME:%s]",pNpc->GetName());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszLog3);							
#endif
					CGUID GoodsGuid = pGoods->GetExID();
					string strGoodsName = pGoods->GetName();
					if(pPlayer->AddGoodsWithProtect(pGoods,FALSE))
					{
						pPlayer -> SetMoney( pPlayer -> GetMoney() - dwTotalPrice,24 );
						pPlayer->SetSilver(pPlayer->GetSilver()-dwTotalSilver);	
						//玩家NPC商店买物品的日志记录												
						GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,GoodsGuid,strGoodsName.c_str(),0,dwLogPrice,dwGoodsNum);	
					}
					else
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,4));
						bResult=true;
					}
				}
				//以物换物商店
				else if(pTrade->tradeType==CTradeList::ETT_GOODS)
				{
					vector<tagTestArg_ID_Num>vecDel;
					vector<CTradeList::tagTradeGoods>&vecTrade=pTradeData->_vecTreadGoods;
					tagTestArg_ID_Num tgdelArg;
					//得到要删除物品的列表
					for (int i=0;i<vecTrade.size();i++)
					{
						tgdelArg._dwGoodsID=vecTrade[i].dwGoodsChgId;
						tgdelArg._dwNum=vecTrade[i].dwChgAmount;
						vecDel.push_back(tgdelArg);
					}
					//测试删除
					vector<tagDelTestResult>vecDelResult;
					BOOL bDelSucc=testContainer.DelTest(vecDel,&vecDelResult);
					if (!bDelSucc)
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,28));
						bResult=true;
					}
					CGUID GoodsGuid = pGoods->GetExID();
					//测试成功
					if (bDelSucc)
					{		
						string strGoodsName = pGoods->GetName();
						if(pPlayer->AddGoodsWithProtect(pGoods,FALSE))
						{	
							//删除物品
							for (int i=0;i<vecDelResult.size();i++)
							{
								pPlayer->DelGoodsByTest(&vecDelResult[i]);
							}
							//玩家NPC商店买物品的日志记录												
							GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,GoodsGuid,strGoodsName.c_str(),0,0,dwGoodsNum);
						}
						else
						{
							SENDMSGBOX(pPlayer,CStringReading::LoadString(22,4));
							bResult=true;
						}
					}
					//添加成功
					if (!bResult)
					{
						//记录位面精华消耗日志
						DWORD dwSum=0;
						for (int i=0;i<vecDel.size();i++)
						{
							if (vecDel[i]._dwGoodsID==CGlobeSetup::GetSetup()->dwWeimianGoodsId)
							{
								dwSum+=vecDel[i]._dwNum;
							}
						}
						if(dwSum>0)
						{
							GetGameLogInterface()->logT031_goods_wm_npc(pPlayer,dwGoodsId,GoodsGuid,dwTotalNum,dwSum);
						}	
					}
				}
				
				if(bResult)
				{
#ifdef _GOODSLOG1_
					for (int i=0;i<GainList.size();i++)
					{
						CGoods* pGoods=GainList[i];
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 金钱不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
					}

#endif
					CGoodsFactory::GarbageCollect(&pGoods,168);
					return;
				}
				else
					bGoodsChange=true;
				
			}
			//不指定位置,输入数量购买（一次可以买多个）
			else
			{
				//最大叠加数
				DWORD dwMaxStack=pProperty->GetAddonPropertyValue(GAP_MAXOVERLAP,1);
				if (dwMaxStack==0)
					dwMaxStack=1;
				//通过购买数量和物品最大叠加数判断如果要创建的物品大于背包总格子数，则失败
				DWORD dwMaxGoods=dwTotalNum/dwMaxStack;
				DWORD dwGoodsLeft=dwMaxGoods;
				//叠加后还有剩余物品,加1
				if (dwTotalNum%dwMaxStack>0)
					dwGoodsLeft=dwMaxGoods+1;
				DWORD dwSpaceNum=pPlayer->GetTotalSpaceNum();
				if (dwMaxGoods>dwSpaceNum || dwGoodsLeft>dwSpaceNum)
				{
					//背包空间不满足
					SENDMSGBOX(pPlayer,CStringReading::LoadString(22,4));
					return;
				}
				//创建物品用于测试
				vector<CGoods*>GainList;
				if (dwMaxStack>=dwTotalNum)
				{
					CGoodsFactory::CreateGoods( dwGoodsId, dwTotalNum, GainList ,169);
				}
				//大于叠加数则要创建多个物品
				else
				{
					for (int i=0;i<dwMaxGoods;i++)
					{
						CGoodsFactory::CreateGoods( dwGoodsId, dwMaxStack, GainList ,173);
					}
					if (dwGoodsNum%dwMaxStack>0)
					{
						CGoodsFactory::CreateGoods( dwGoodsId, dwTotalNum%dwMaxStack, GainList ,175);
					}
				}
				//唯一性检查
				for (size_t i=0;i<GainList.size();i++)
				{
					if (GainList[i]->HasAddonProperty(GAP_UNIQUE))
					{
						if(!CheckUnique(pPlayer,pProperty,dwTotalNum))
							return;
					}
				}
				//货币商店
				if(pTrade->tradeType==CTradeList::ETT_MONEY || pTrade->tradeType==CTradeList::ETT_SILVER)
				{
					if(pProperty->GetPrice()==0 && pProperty->GetSilverPrice()==0)
					{
						return;
					}
					//测试金钱,按一组物品计算
					for (int i=0;i<GainList.size();i++)
					{
						dwTotalPrice+= GainList[i]->GetAmount()*pProperty->GetPrice();
						dwTotalSilver+= GainList[i]->GetAmount()*pProperty->GetSilverPrice();
					}
					DWORD dwLogPrice=dwTotalSilver;
					if(dwTotalPrice>0)
					{
						dwLogPrice=dwTotalPrice;
					}
					bool bMoney=false;
					//金币不够
					if(dwTotalPrice  > pPlayer->GetMoney())
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,2));
						bMoney=true;
#ifdef _GOODSLOG1_
						for (int i=0;i<GainList.size();i++)
						{
							CGoods* pGoods=GainList[i];
							char pszGoodsLog[1024]="";			
							char srcGoodsGUID[50]="";
							pGoods->GetExID().tostring(srcGoodsGUID);					
							_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 金钱不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
								pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
						}

#endif
					}
					//银币不够
					if(dwTotalSilver  > pPlayer->GetSilver())
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,14));
						bMoney=true;
#ifdef _GOODSLOG1_
						for (int i=0;i<GainList.size();i++)
						{
							CGoods* pGoods=GainList[i];
							char pszGoodsLog[1024]="";			
							char srcGoodsGUID[50]="";
							pGoods->GetExID().tostring(srcGoodsGUID);					
							_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 银币不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
								pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
						}
#endif
					}
					if (bMoney)
					{
						//添加不成功,回收内存
						DELGOODS(GainList);
						return;
					}
					//测试背包
					vector<tagTestResult>vecAddResult;
					BOOL bAddSuccess=testContainer.AddTest(GainList,&vecAddResult);
					if (!bAddSuccess)
					{
						//背包空间不满足
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,4));
						bResult=true;
					}
					//正式添加到背包
					if (bAddSuccess && vecAddResult.size() == GainList.size())
					{
						for(int i=0;i<vecAddResult.size();i++)
						{
							//添加物品有可能删除GainList[i]指针,先记下来用来记日志
							CGUID goodsguid=GainList[i]->GetExID();
							//添加失败则回收
							if(!pPlayer->AddGoodsByTest(&vecAddResult[i],GainList[i]))
							{
								CGoodsFactory::GarbageCollect(&GainList[i],176);
							}
							else
							{
								GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,goodsguid,
									CGoodsFactory::QueryGoodsName(dwGoodsId),0,dwLogPrice,dwTotalNum);
							}
						}
					}
					else
						bResult=true;
					if (bResult)
					{
#ifdef _GOODSLOG1_
						for (int i=0;i<GainList.size();i++)
						{
							CGoods* pGoods=GainList[i];
							char pszGoodsLog[1024]="";			
							char srcGoodsGUID[50]="";
							pGoods->GetExID().tostring(srcGoodsGUID);					
							_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 添加到背包不成功][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
								pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
						}
#endif
						//添加不成功,回收内存
						DELGOODS(GainList);
					}
					else
					{
						pPlayer -> SetMoney( pPlayer -> GetMoney() - dwTotalPrice,24 );
						pPlayer->SetSilver(pPlayer->GetSilver()-dwTotalSilver);
						bGoodsChange=true;
					}
				}
				//以物换物商店
				else if(pTrade->tradeType==CTradeList::ETT_GOODS)
				{
					vector<tagTestArg_ID_Num>vecDel;
					vector<CTradeList::tagTradeGoods>&vecTrade=pTradeData->_vecTreadGoods;
					tagTestArg_ID_Num tgdelArg;
					DWORD dwWmNum=0;//统计使用位面精华的数量
					//得到要删除物品的列表，按组计算
					for (int i=0;i<vecTrade.size();i++)
					{
						tgdelArg._dwGoodsID=vecTrade[i].dwGoodsChgId;
						tgdelArg._dwNum=vecTrade[i].dwChgAmount * dwGoodsNum;
						if (tgdelArg._dwGoodsID==CGlobeSetup::GetSetup()->dwWeimianGoodsId)
						{
							dwWmNum+=tgdelArg._dwNum;
						}
						vecDel.push_back(tgdelArg);
					}
					vector<tagDelTestResult>vecDelResult;
					BOOL bDelSucc=testContainer.DelTest(vecDel,&vecDelResult);
					if (!bDelSucc)
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,28));
						//添加不成功,回收内存
						DELGOODS(GainList);
						break;
					}
					vector<tagTestResult>vecAddResult;
					BOOL bAddSucc=testContainer.AddTest(GainList,&vecAddResult);
					if (!bAddSucc)
					{
						SENDMSGBOX(pPlayer,CStringReading::LoadString(22,4));
						bResult=true;
					}
					//正式添加到背包
					if (bDelSucc && bAddSucc && vecAddResult.size() == GainList.size())
					{
						//删除物品
						for (int i=0;i<vecDelResult.size();i++)
						{
							if (!pPlayer->DelGoodsByTest(&vecDelResult[i]))	
							{
								bResult=true;
								break;
							}
						}
						//添加获得品
						if (!bResult)
						{
							for(int i=0;i<vecAddResult.size();i++)
							{
								CGUID goodsguid=GainList[i]->GetExID();
								DWORD dwGoodsNum=GainList[i]->GetAmount();
								if (!pPlayer->AddGoodsByTest(&vecAddResult[i],GainList[i]))
								{
									CGoodsFactory::GarbageCollect(&GainList[i],177);
								}
								else
								{
									//玩家NPC商店买物品的日志记录,以物换物金钱为0												
									GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,goodsguid,
									CGoodsFactory::QueryGoodsName(dwGoodsId),0,0,dwTotalNum);
									//添加成功,记录位面精华消耗日志,一组物品要消耗的位面数量
									if (dwWmNum>0)
									{
										GetGameLogInterface()->logT031_goods_wm_npc(pPlayer,dwGoodsId,goodsguid,
										dwGoodsNum,dwWmNum/dwGoodsNum);
									}	
								}
							}//for
						}//if	
					}
					else
						bResult=true;
					if (bResult)
					{
#ifdef _GOODSLOG1_
						for (int i=0;i<GainList.size();i++)
						{
							CGoods* pGoods=GainList[i];
							char pszGoodsLog[1024]="";			
							char srcGoodsGUID[50]="";
							pGoods->GetExID().tostring(srcGoodsGUID);					
							_snprintf(pszGoodsLog,1024,"[物品回收:NPC_SHOP_BUY 添加到背包不成功][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
								pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
							CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
						}
#endif
						//添加不成功,回收内存
						DELGOODS(GainList);
					}
					else
					{
						bGoodsChange=true;	
					}
					
				}	
			}
			if (bGoodsChange)
			{
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPACKGOODS);										
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS1);					
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS2);						
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS3);					
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS4);					
				GetGame()->UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS5);
			}		
		}
		break;
	case MSG_C2S_SHOP_BUYBACK:
		{
			CGUID NpcID; 
			pMsg -> GetGUID(NpcID);
			CGUID goodsID;
			pMsg->GetGUID(goodsID);
			DWORD dwContainerId=pMsg->GetDWord();
			DWORD dwContainerPos=pMsg->GetDWord();						

			CMonster* pNpc = (CMonster*)pRegion->FindAroundObject(pPlayer, TYPE_MONSTER, NpcID);

			if (pNpc)
			{
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					CancelTrading( pPlayer );
					CMessage msg(MSG_S2C_SHOP_BUYBACK);
					msg.Add((BYTE)0);
					msg.Add(goodsID);
					msg.SendToPlayer(pPlayer->GetExID());
					break;
				}
			}
			else
			{
				//##NPC不存在
				CancelTrading( pPlayer );
				CMessage msg(MSG_S2C_SHOP_BUYBACK);
				msg.Add((BYTE)0);
				msg.Add(goodsID);
				msg.SendToPlayer(pPlayer->GetExID());
				break;
			}

			//位置检测
			CVolumeLimitGoodsContainer* pContainer=NULL;
			if(dwContainerId==CS2CContainerObjectMove::PEI_PACKET)
			{
				pContainer=pPlayer->getOriginPack();
			}
			else if(dwContainerId>=CS2CContainerObjectMove::PEI_PACK1 && dwContainerId<=CS2CContainerObjectMove::PEI_PACK5)
			{
				CGoods* pSubpackGoods=pPlayer->getSubpackContainer()->GetSubpack(dwContainerId-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
				if(!pSubpackGoods)
				{
					//没有装备子背包，dwContainerId是错误的
					CMessage msg(MSG_S2C_SHOP_BUYBACK);
					msg.Add((BYTE)0);
					msg.Add(goodsID);
					msg.SendToPlayer(pPlayer->GetExID());
					return;
				}
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(dwContainerId-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			}
			if(!pContainer)
				return;
			DWORD dwPrice=0;
			CGoods* pGoods=pPlayer->FindNpcShopSellGoods(goodsID,&dwPrice);
			if(!pGoods)
			{
				// 根据客户端发过来的GUID在回购LIST中招不到匹配的物品
				CMessage msg(MSG_S2C_SHOP_BUYBACK);
				msg.Add((BYTE)0);
				msg.Add(goodsID);
				msg.SendToPlayer(pPlayer->GetExID());
				return;
			}
			CGoodsFactory::UpdateEquipmentAttr(pGoods);
			DWORD dwTotalPrice=0,dwTotalSilver=0;
			dwTotalSilver=dwPrice;
			/*
			if(pGoods->GetPrice()>0)
			dwTotalPrice=dwPrice;
			if(pGoods->GetSilverPrice()>0)
			dwTotalSilver=dwPrice;	
			*/

			//金钱不够
			/*
			if(dwTotalPrice  > pPlayer->GetMoney())
			{						
			pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,2));
			CMessage msg(MSG_S2C_SHOP_BUYBACK);
			msg.Add((BYTE)0);
			msg.Add(goodsID);
			msg.SendToPlayer(pPlayer->GetExID());
			return;
			}*/
			//银币不够
			if(dwTotalSilver  > pPlayer->GetSilver())
			{								
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,14));
				CMessage msg(MSG_S2C_SHOP_BUYBACK);
				msg.Add((BYTE)0);
				msg.Add(goodsID);
				msg.SendToPlayer(pPlayer->GetExID());
				return;
			}

			CGoods* pExistedGoods = pContainer->GetGoods( dwContainerPos );
			if( pExistedGoods == NULL )
			{
				CS2CContainerObjectMove comMsg;
				DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
				CGUID goodsGuid=pGoods->GetExID();
				const char* strGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);
				DWORD dwGoodsNum=pGoods->GetAmount();
				if( pContainer->Add(dwContainerPos,pGoods, NULL, &comMsg) )
				{
#ifdef _GOODSLOG1_
					char pszGoodsLog[1024]="";						
					char srcGoodsGUID[50]="";
					pGoods->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[操作:SHOP_BUYBACK][玩家名:%s][容器ID:%d][容器位置:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						pPlayer->GetName(),dwContainerId,dwContainerPos,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
					comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
					comMsg.SetDestinationContainerExtendID( dwContainerId);
					pGoods-> SerializeForOldClient( &comMsg.GetObjectStream() );
					comMsg.Send( pPlayer ->GetExID() );

					pPlayer -> SetMoney( pPlayer -> GetMoney() - dwTotalPrice,25 );
					pPlayer->SetSilver(pPlayer->GetSilver()-dwTotalSilver);	

					pPlayer->FetchNpcShopSellGoods(goodsID);

					CMessage msg(MSG_S2C_SHOP_BUYBACK);
					msg.Add((BYTE)1);
					msg.Add(goodsID);
					msg.SendToPlayer(pPlayer->GetExID());

					//玩家NPC商店回购物品的日志记录												
					GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,goodsGuid,strGoodsName,3,dwTotalSilver,dwGoodsNum);																									

				}				
			}
			else
			{
				DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
				DWORD dwMaxStack=pGoods->GetMaxStackNumber();
				DWORD dwGoodsNum=pGoods->GetAmount();				
				CGUID goodsGuid=pGoods->GetExID();
				const char* strGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);				
				if(dwGoodsId==pExistedGoods->GetBasePropertiesIndex() && dwMaxStack>1 && 
					dwGoodsNum+pExistedGoods->GetAmount()<=dwMaxStack)
				{
					pPlayer->FetchNpcShopSellGoods(goodsID);
					if( pContainer->Add(dwContainerPos,pGoods, NULL) )
					{
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";						
						char srcGoodsGUID[50]="";
						goodsID.tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[操作:SHOP_BUYBACK_叠加][玩家名:%s][容器ID:%d][容器位置:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pPlayer->GetName(),dwContainerId,dwContainerPos,dwGoodsId,strGoodsName,srcGoodsGUID,dwGoodsNum);
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
						CGoods* pNewGoods=pContainer->GetGoods(dwContainerPos);
						if(pNewGoods)
						{
							CS2CContainerObjectAmountChange coacMsg;
							coacMsg.SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), 
								dwContainerPos );
							coacMsg.SetSourceContainerExtendID( dwContainerId);
							coacMsg.SetObject( pNewGoods -> GetType(), pNewGoods -> GetExID() );
							coacMsg.SetObjectAmount( pNewGoods -> GetAmount() );
							coacMsg.Send( pPlayer->GetExID());
						}					

						pPlayer -> SetMoney( pPlayer -> GetMoney() - dwTotalPrice,26 );
						pPlayer->SetSilver(pPlayer->GetSilver()-dwTotalSilver);						

						CMessage msg(MSG_S2C_SHOP_BUYBACK);
						msg.Add((BYTE)1);
						msg.Add(goodsID);
						msg.SendToPlayer(pPlayer->GetExID());

						//玩家NPC商店回购物品的日志记录												
						GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,goodsGuid,strGoodsName,3,dwTotalSilver,dwGoodsNum);																								

					}				
				}
			}			
		}
		break;
	case MSG_C2S_BUSINESS_BUY:
		{
			return;
			// 玩家必须已经取得商人身份，且有商业背包
			if( !( pPlayer->IsBusinessMan() && pPlayer->GetBusinessContainer().IsValid() ) )
			{
				return ;
			}

			CGUID NpcID; 
			pMsg->GetGUID( NpcID );
			DWORD dwSpotId = pMsg->GetDWord();
			Business::CBusinessManager &BM = GetInst( Business::CBusinessManager );
			Business::CTradeSpot *pSpot = BM.GetTradeSpot( dwSpotId );

			if( pSpot == NULL )
			{
				return ;
			}
			// 准备购买的数量，如果没有数量则直接返回。
			WORD wAmount = pMsg->GetWord();
			if( wAmount == 0 ) 
			{
				return;
			}

			// 距离判断
			CMonster *pNpc = static_cast<CMonster*>( pRegion->FindAroundObject( pPlayer, TYPE_MONSTER, NpcID ) );
			if( pNpc != NULL )
			{
				if( pNpc->Distance( pPlayer ) > 10 )
				{
					CancelTrading( pPlayer );
					break;
				}
			}
			else
			{
				CancelTrading( pPlayer );
				break;
			}

			vector<LONG> vecBuyGoodsId;
			vector<DWORD> vecGoodsNum;
			vector<DWORD> vecGoodsPrice;
			vecBuyGoodsId.reserve( wAmount );
			vecGoodsNum.reserve( wAmount );
			vecGoodsPrice.reserve( wAmount );

			for( WORD n = 0; n < wAmount; n++ )
			{
				vecBuyGoodsId.push_back( pMsg->GetLong() );
				vecGoodsNum.push_back( pMsg->GetDWord() );
				vecGoodsPrice.push_back( pMsg->GetDWord() );
			}

			// 物品属性及限购额的检查
			for( WORD n = 0; n < wAmount; n++ )
			{

				Business::CBusinessGoods *pGoods = pSpot->GetGoods( vecBuyGoodsId[n] );
				if( pGoods == NULL )
				{
					pPlayer->SendNotifyMessage( "该贸易点没有此商品" );
					return ;
				}

				DWORD dwBuyLimit = pGoods->GetBuyLimit( pPlayer->GetExID() );
				if( dwBuyLimit < vecGoodsNum[n] )
				{
					pPlayer->SendNotifyMessage( "物品超过限购额，不能购买" );
					return;
				}

				// 价格发生变动
				if( pGoods->GetBuyPrice() != vecGoodsPrice[n] )
				{
					pPlayer->SendNotifyMessage( "商品价格发生变化，购买失败" );
					return ;
				}
			}

			struct tagBuyBusinessGoodsInfo
			{
				CGoods *pGoods;	
				DWORD dwPrice;
				DWORD dwPos;

			};
			vector<tagBuyBusinessGoodsInfo> buyGoodsVec;	
			vector<CGoods*> testGoodsVec;

			DWORD dwPrice = 0,dwTotalPrice = 0;
			LONG lTotalTax = 0;		
			DWORD dwTotalCount = 0; 

			// 开始创建物品
			DWORD dwLockGrid = 0,dwMaxStack = 0;
			for( WORD n = 0; n < wAmount; n++ )
			{					
				dwMaxStack = CGoodsFactory::QueryGoodsMaxStackNumber( vecBuyGoodsId[n] );
				CGoodsBaseProperties *pProperties = CGoodsFactory::QueryGoodsBaseProperties( vecBuyGoodsId[n] );
				if( !pProperties )
				{
					continue;
				}

				Business::CBusinessGoods *pBusinessGoods = pSpot->GetGoods( vecBuyGoodsId[n] );
				if( pBusinessGoods == NULL )
				{
					continue;
				}

				if( dwMaxStack > 1 )
				{
					// 总数将会被拆分成几件来创建						
					vector<CGoods*> goodsVec;
					CGoodsFactory::CreateGoods( vecBuyGoodsId[n], vecGoodsNum[n], goodsVec ,33);
					dwLockGrid += goodsVec.size();
					for( size_t i = 0; i < goodsVec.size(); i++ )
					{
						tagBuyBusinessGoodsInfo stInfo;
						stInfo.pGoods = goodsVec[i];						
						stInfo.dwPos = 0;
						stInfo.dwPrice = pBusinessGoods->GetBuyPrice();
						stInfo.pGoods->SetBuyPrice( stInfo.dwPrice );
						buyGoodsVec.push_back( stInfo );
						testGoodsVec.push_back( stInfo.pGoods );
						dwPrice = stInfo.dwPrice * goodsVec[i]->GetAmount();
						dwTotalPrice += dwPrice;

						dwTotalCount += goodsVec[i]->GetAmount();
					}
				}
				else
				{
					dwLockGrid += vecGoodsNum[n];
					for( int i=0; i < vecGoodsNum[n]; i++ )
					{
						CGoods *pGoods = CGoodsFactory::CreateGoods( vecBuyGoodsId[n],34 );
						tagBuyBusinessGoodsInfo stInfo;
						stInfo.pGoods = pGoods;						
						stInfo.dwPos = 0;
						stInfo.dwPrice = pBusinessGoods->GetBuyPrice();
						stInfo.pGoods->SetBuyPrice( stInfo.dwPrice );
						buyGoodsVec.push_back( stInfo );	
						testGoodsVec.push_back( stInfo.pGoods );
						dwTotalPrice += stInfo.dwPrice;
					}

					dwTotalCount += vecGoodsNum[n];
				}
			}

			// 检查玩家是否有钱购买
			if( dwTotalPrice > pPlayer->GetMoney() )
			{
				for( size_t i = 0; i < buyGoodsVec.size(); i++ )
				{
#ifdef _GOODSLOG1_
					if( buyGoodsVec[i].pGoods )
					{
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						buyGoodsVec[i].pGoods->GetExID().tostring( srcGoodsGUID );					
						_snprintf( pszGoodsLog, 1024, "[物品回收:BUSINESS_BUY 金币不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							buyGoodsVec[i].pGoods->GetBasePropertiesIndex(), CGoodsFactory::QueryGoodsName(buyGoodsVec[i].pGoods->GetBasePropertiesIndex()),srcGoodsGUID,buyGoodsVec[i].pGoods->GetAmount());
						CMessage::AsyWriteFile( GetGame()->GetGoodsLogFile(), pszGoodsLog );							
					}					
#endif
					CGoodsFactory::GarbageCollect( &( buyGoodsVec[i].pGoods ), 47 );						
				}
				pPlayer -> SendNotifyMessage( CStringReading::LoadString( 22, 2 ) );
				return;
			}

			Business::CPackage &businessContainer = pPlayer->GetBusinessContainer();

			// 测试是否可以添加到背包
			CTestContainer test_container;
			vector<tagTestResult> test_result;
			test_container.AddContainer( CS2CContainerObjectMove::PEI_BUSSINESPACK, businessContainer.m_pContainer );
			if( dwTotalCount > businessContainer.GetLeftGoodsCount() || 
				!test_container.AddTest( testGoodsVec, &test_result ) ||
				test_result.size() != buyGoodsVec.size() )
			{
				// 添加失败，回收创建的物品
				for( size_t i = 0; i < buyGoodsVec.size(); i++ )
				{
#ifdef _GOODSLOG1_
					if( buyGoodsVec[i].pGoods )
					{
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						buyGoodsVec[i].pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:BUSINESS_BUY 背包空间不够][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							buyGoodsVec[i].pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(buyGoodsVec[i].pGoods->GetBasePropertiesIndex()),srcGoodsGUID,buyGoodsVec[i].pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
					}					
#endif
					CGoodsFactory::GarbageCollect( &(buyGoodsVec[i].pGoods), 48 );
				}
				pPlayer->SendNotifyMessage( CStringReading::LoadString(25,1) );
				return;
			}

			// 更新背包剩余可放置商品数量
			businessContainer.AddGoodsCount( dwTotalCount );
			pPlayer->SetMoney( pPlayer->GetMoney() - dwTotalPrice,27 );	

			// 发送回应消息、放置物品到商业背包
			vector<BYTE> array;
			CMessage msg( MSG_S2C_BUSINESS_BUY );
			msg.Add( (WORD)buyGoodsVec.size() );
			for( size_t i = 0; i < buyGoodsVec.size(); i++ )
			{
				pPlayer->OnGetGoods( buyGoodsVec[i].pGoods->GetBasePropertiesIndex(), buyGoodsVec[i].pGoods );

				// 记录操作日志
				GetGameLogInterface()->logT700_business_npc( pPlayer, buyGoodsVec[i].pGoods, 
					buyGoodsVec[i].pGoods->GetAmount(), 
					buyGoodsVec[i].pGoods->GetBuyPrice() * buyGoodsVec[i].pGoods->GetAmount(),
					1, dwSpotId );

				// 更新到限购额记录中
				Business::CBusinessGoods *pBusinessGoods = pSpot->GetGoods( buyGoodsVec[i].pGoods->GetBasePropertiesIndex() );
				pBusinessGoods->SubBuyLimit( pPlayer->GetExID(), buyGoodsVec[i].pGoods->GetAmount() ); 	
				// 减少库存量
				pBusinessGoods->SetVolume( pBusinessGoods->GetVolume() - buyGoodsVec[i].pGoods->GetAmount() );
				// 影响其他商品库存量
				pSpot->DecreaseGoodsVolume( pBusinessGoods, buyGoodsVec[i].pGoods->GetAmount() );

				long lIndex = 0;
				DWORD dwNewPrice = 0;
				// 更新价格
				dwNewPrice = businessContainer.UpdateGoodsPrice( buyGoodsVec[i].pGoods );
				lIndex = buyGoodsVec[i].pGoods->GetBasePropertiesIndex();

				msg.Add( (BYTE)test_result[i].eOperType );
				if( test_result[i].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT )
				{
					// 将物品放进背包
					businessContainer.m_pContainer->Add( test_result[i].dwPos, buyGoodsVec[i].pGoods, NULL, NULL );					

					// 添加商品信息到消息
					array.clear();
					buyGoodsVec[i].pGoods->SerializeForOldClient( &array );
					msg.Add( (long)array.size() );
					msg.AddEx( &array[0], (long) array.size() );
					msg.Add( test_result[i].dwPos );
				}
				else if( test_result[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT )
				{
					// 增加背包里该物品的叠加数
					CGoods *pGoods = businessContainer.m_pContainer->GetGoods( test_result[i].dwPos );
					pGoods->SetAmount( pGoods->GetAmount() + buyGoodsVec[i].pGoods->GetAmount() );

					// 添加增加的叠加数到消息
					msg.Add( (long) buyGoodsVec[i].pGoods->GetAmount() );
					msg.Add( pGoods->GetExID() );
					// 回收此物品
					CGoodsFactory::GarbageCollect( &buyGoodsVec[i].pGoods, 48 );
				}

				// 附加更新后的物品价格
				msg.Add( (long) lIndex );
				msg.Add( (DWORD) dwNewPrice );
			}
			msg.SendToPlayer( pPlayer->GetExID() );	
			// 设置存档标志
			GetGame()->UpdateSavePlayer( pPlayer, SAVE_DETAIL_ATTRBUSINESSPACK );
		}
		break;
	case MSG_C2S_SHOP_SELL:	//玩家请求卖一个物品到商店
		{			
			CGUID NpcID; 
			pMsg -> GetGUID(NpcID);	//NPC的ID号			
			DWORD dwContianerId=0,dwPos=0;
			dwContianerId=pMsg->GetDWord();
			dwPos=pMsg->GetDWord();	

			//取得容器
			CVolumeLimitGoodsContainer* pContainer=NULL;
			if(dwContianerId==CS2CContainerObjectMove::PEI_PACKET)
			{
				pContainer=pPlayer->getOriginPack();
			}
			else if(dwContianerId>=CS2CContainerObjectMove::PEI_PACK1 && dwContianerId<=CS2CContainerObjectMove::PEI_PACK5)
			{
				CGoods* pSubpackGoods=pPlayer->getSubpackContainer()->GetSubpack(dwContianerId-CS2CContainerObjectMove::PEI_PACK1)->pGoods;
				if(!pSubpackGoods)
				{
					//没有装备子背包，dwContainerId是错误的

					return;
				}
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(dwContianerId-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
			}
			else
			{
				//不支持这个ContainerId
				return;
			}

			if(!pContainer)
				return;
			if(dwPos >= pContainer->GetVolume())
				return;

			CGoods* pGoods=pContainer->GetGoods(dwPos);
			if(!pGoods)
			{
				//该位置上没有物品！
				return;
			}

			CMonster* pNpc	= dynamic_cast<CMonster*>( pRegion ->FindAroundObject(pPlayer, TYPE_MONSTER, NpcID) );
			if( pNpc )
			{
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,5));
					CMessage msg(MSG_S2C_SHOP_SELL);
					msg.Add((BYTE)0);
					msg.Add(pGoods->GetExID());
					msg.SendToPlayer(pPlayer->GetExID());
					CancelTrading( pPlayer );
					break;
				}
			}
			else
			{
				CancelTrading( pPlayer );
				CMessage msg(MSG_S2C_SHOP_SELL);
				msg.Add((BYTE)0);
				msg.Add(pGoods->GetExID());
				msg.SendToPlayer(pPlayer->GetExID());
				break;
			}									


			if (GoodsExManage::GetInstance().IsFrost(pGoods)) //! 如果冻结
			{
				//! 不能与玩家交易（交易、摆摊）
				//! 不能贩卖到商店；
				//! 不能拍卖、寄售（均为计划开放功能，目前暂未）；
				//! 玩家死亡时不会掉落；
				//! 不能进行分解操作；
				//! 不能丢弃。
				char* strRes=CStringReading::LoadString(20,25);
				if(strRes)
				{
					pPlayer->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
					CMessage msg(MSG_S2C_SHOP_SELL);
					msg.Add((BYTE)0);
					msg.Add(pGoods->GetExID());
					msg.SendToPlayer(pPlayer->GetExID());
					return;
				}
			}
			/*
			else if (GoodsExManage::GetInstance().IsBind(pGoods)) //! 如果绑定
			{
			//! 不能与玩家交易（交易、摆摊）
			//! 不能拍卖、寄售（均为计划开放功能，目前暂未）；
			//! 玩家死亡时不会掉落；
			//! 丢弃时提示玩家丢弃该物品，物品将被销毁
			char* strRes=CStringReading::LoadString(20,26);
			if(strRes)
			{
			pPlayer->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
			CMessage msg(MSG_S2C_SHOP_SELL);
			msg.Add((BYTE)0);
			msg.Add(pGoods->GetExID());
			msg.SendToPlayer(pPlayer->GetExID());
			return;
			}
			}*/

			CGUID guid = pGoods->GetExID();
			char szGoodsName[32];
			lstrcpy( szGoodsName, pGoods->GetGoodsName() );

			//##得到单个物品的出售价格
			DWORD dwSilverPrice=0;

			dwSilverPrice=CGoodsFactory::CalculateVendSilverPrice(pGoods);


			if(dwSilverPrice==0 )
			{
				pPlayer -> SendNotifyMessage( CStringReading::LoadString(22,6));
				CMessage msg(MSG_S2C_SHOP_SELL);
				msg.Add((BYTE)0);
				msg.Add(pGoods->GetExID());
				msg.SendToPlayer(pPlayer->GetExID());
			}
			else
			{				
				dwSilverPrice=dwSilverPrice * pGoods->GetAmount();				

				//##判断上限
				DWORD dwMaxMoney=CGoodsFactory::QueryGoodsMaxStackNumber( CGoodsFactory::GetGoldCoinIndex() );
				DWORD dwMaxSilver=CGoodsFactory::QueryGoodsMaxStackNumber(CGoodsFactory::GetSilverIndex());		
				/*
				if(pPlayer->GetMoney()+dwPrice>dwMaxMoney)
				{
				pPlayer -> SendNotifyMessage( CStringReading::LoadString(22,7) );
				CMessage msg(MSG_S2C_SHOP_SELL);
				msg.Add((BYTE)0);
				msg.Add(pGoods->GetExID());
				msg.SendToPlayer(pPlayer->GetExID());

				}*/
				if(pPlayer->GetSilver()+dwSilverPrice>dwMaxSilver)
				{
					pPlayer -> SendNotifyMessage( CStringReading::LoadString(22,15) );
					CMessage msg(MSG_S2C_SHOP_SELL);
					msg.Add((BYTE)0);
					msg.Add(pGoods->GetExID());
					msg.SendToPlayer(pPlayer->GetExID());
				}
				else 
				{	
					CS2CContainerObjectMove comMsg;
					comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );											
					//pContainer->Remove(pGoods,&comMsg);
					CGoods* pRemovedGoods=dynamic_cast<CGoods*>(pContainer->Remove(dwPos,pGoods->GetAmount(),&comMsg));
					if(pRemovedGoods)
					{
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";						
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[操作:SHOP_SELL][玩家名:%s][容器ID:%d][容器位置:%d][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pPlayer->GetName(),dwContianerId,dwPos,pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	

						pPlayer->PushbakNpcShopSellGoods(pGoods,dwSilverPrice);
						pPlayer->SetSilver(pPlayer->GetSilver()+dwSilverPrice);


						comMsg.SetSourceContainerExtendID(dwContianerId);
						comMsg.Send( pPlayer -> GetExID() );

						CMessage msg(MSG_S2C_SHOP_SELL);
						msg.Add((BYTE)1);
						msg.Add(pGoods->GetExID());
						msg.SendToPlayer(pPlayer->GetExID());

						//失去物品触发
						pPlayer->OnLoseGoods(pGoods);
						//玩家卖物品到NPC商店的日志记录
						DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
						CGUID goodsGuid=pGoods->GetExID();
						const char* strGoodsName=CGoodsFactory::QueryGoodsName(dwGoodsId);
						DWORD dwGoodsNum=pGoods->GetAmount();
						GetGameLogInterface()->logT030_goods_log_npc(pPlayer,dwGoodsId,goodsGuid,strGoodsName,1,dwSilverPrice,dwGoodsNum);																									
					}																									
				}
			}//else end				
		}
		break;
	case MSG_C2S_ESSENCE_SHOP_OPEN:
		{
			DWORD dwAreaId=0;
			CServerRegion* pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
			if(pRegion)
				dwAreaId=pRegion->GetAreaId();
			char szNpcName[256];
			//szNpcName="TreasureShop";
			pMsg->GetStr(szNpcName,256);
			if(strcmp(szNpcName,"")==0)
			{
				strcpy_s(szNpcName,256, "TreasureShop");
			}			

			CTradeList::tagTrade *pTrade = CTradeList::GetTradeList(szNpcName);
			if (pTrade)
			{
				pPlayer->ProduceEvent(ET_EventBreak);
				CMessage msg(MSG_S2C_SHOP_OPEN);				
				
				CGUID emptyGuid;
				msg.Add(emptyGuid);
				

				msg.Add(long(pTrade->shopType));//商店类型

				msg.Add(BYTE(pTrade->tradeType));//类型：金钱交易 OR 以物换物
				msg.Add(CGlobeSetup::GetSetup() -> fBasePriceRate);//FLOAT
				msg.Add(CGlobeSetup::GetSetup() -> fTradeInRate);//FLOAT	

				long lNum=pTrade->vecGoods.size();			
				msg.Add(lNum);//物品数量
				if(pTrade->tradeType==CTradeList::ETT_MONEY || pTrade->tradeType==CTradeList::ETT_SILVER)
				{
					for(int i=0;i<pTrade->vecGoods.size();i++)
					{					
						CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties(
							pTrade->vecGoods[i].dwGoodsId);
						if(!pProperties)
						{
							return;
						}


						msg.Add(pTrade->vecGoods[i].dwGoodsId);//物品ID		
						msg.Add(pTrade->vecGoods[i].dwGoodsNum);										
					}
				}
				else if(pTrade->tradeType==CTradeList::ETT_GOODS)
				{
					/*
					struct tagTradeData
					{
					DWORD dwGoodsId;	//物品ID
					DWORD dwGoodsChgId;	//要交换物品的ID
					DWORD dwChgAmount;	//要交换物品数量
					DWORD dwAreaId;		//所需区域ID
					DWORD dwCredit;		//所需区域声望
					DWORD dwCreditConsume;	//声望消耗
					};
					*/
					//for(int i=0;i<pTrade->vecGoods.size();i++)
					//{
					//	msg.Add(pTrade->vecGoods[i].dwGoodsId);//物品ID
					//	msg.Add(pTrade->vecGoods[i].dwGoodsNum);
					//	msg.Add(pTrade->vecGoods[i].dwGoodsChgId);//要交换的物品ID
					//	msg.Add(pTrade->vecGoods[i].dwChgAmount);//需要交换数量
					//	msg.Add(dwAreaId);//需要哪个区域的声望
					//	msg.Add(pTrade->vecGoods[i].dwCredit);//需要该区域声望达到多少级
					//	msg.Add(pTrade->vecGoods[i].dwCreditConsume);//需要消耗多少声望点数
					//}
				}
				msg.SendToPlayer(pPlayer->GetExID());
			}
			else 
			{
				pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_NONE );
				pPlayer -> SetMoveable();
			}							
		}
		break;
	case MSG_C2S_BUSINESS_SELL:	//玩家请求卖一个物品到商店
		{	
			return;
			// 玩家必须已经取得商人身份，且有商业背包
			if( !( pPlayer->IsBusinessMan() && pPlayer->GetBusinessContainer().IsValid() ) )
			{
				return ;
			}

			CGUID NpcID; 
			pMsg->GetGUID( NpcID );
			DWORD dwSpotId = pMsg->GetDWord();
			Business::CBusinessManager &BM = GetInst( Business::CBusinessManager );
			Business::CTradeSpot *pSpot = BM.GetTradeSpot( dwSpotId );
			Business::CPackage &businessContainer = pPlayer->GetBusinessContainer();
			if( pSpot == NULL )
			{
				return ;
			}
			// 出售数量，如果没有数量则直接返回。
			long lSellAmount = pMsg->GetLong();
			if( lSellAmount == 0 ) 
			{
				return;
			}

			// 距离判断
			CMonster *pNpc = static_cast<CMonster*>( pRegion->FindAroundObject( pPlayer, TYPE_MONSTER, NpcID ) );
			if( pNpc != NULL )
			{
				if( pNpc->Distance( pPlayer ) > 10 )
				{
					CancelTrading( pPlayer );
					break;
				}
			}
			else
			{
				CancelTrading( pPlayer );
				break;
			}

			struct tagSellGoods
			{
				CGUID guid;
				long count;
				DWORD price;
				CGoods *pGoods;
				Business::CBusinessGoods *pBusinessGoods;
			};
			typedef std::vector<tagSellGoods> SellGoodsList;
			SellGoodsList sell_goods;

			for( long i = 0; i < lSellAmount; ++ i )
			{
				tagSellGoods goods_info;
				pMsg->GetGUID( goods_info.guid );
				goods_info.count = pMsg->GetLong();
				goods_info.price = pMsg->GetDWord();

				// 从背包获取物品
				CGoods *pGoods = static_cast<CGoods*>( businessContainer.m_pContainer->Find( goods_info.guid ) );
				if( pGoods == NULL )
				{
					pPlayer->SendNotifyMessage( "出售非法物品ID" );
					return ;
				}

				if( pGoods->GetAmount() < goods_info.count )
				{
					pPlayer->SendNotifyMessage( "出售数量超过最大数" );
					return ;
				}

				// 获取商品描述
				Business::CBusinessGoods *pBusinessGoods = pSpot->GetGoods( pGoods->GetBasePropertiesIndex() );
				if( pBusinessGoods == NULL )
				{
					BUSINESS_LOG_ERROR( Business::FMT_STR( "Not found business goods from id [%d]", pGoods->GetBasePropertiesIndex() ) );
					pPlayer->SendNotifyMessage( "此贸易点无此商品" );
					return ;
				}

				// 价格对比
				bool bLocal = pBusinessGoods->GetBaseProperty().Local != 0;
				if(	goods_info.price != pBusinessGoods->GetSellPrice( bLocal ) )
				{
					pPlayer->SendNotifyMessage( "物品价格发生变化，购买失败" );
					return ;
				}

				goods_info.pGoods = pGoods;
				goods_info.pBusinessGoods = pBusinessGoods;
				sell_goods.push_back( goods_info );
			}

			CMessage msg(MSG_S2C_BUSINESS_SELL);
			DBWriteSet db;
			long lProfit = 0;
			long lPos = 0;
			long lCount = 0;
			DWORD dwSellCount = 0;
			msg.GetDBWriteSet( db );
			db.AddToByteArray( (long) sell_goods.size() ); 
			for( SellGoodsList::iterator it = sell_goods.begin(); it != sell_goods.end(); ++ it )
			{
				tagSellGoods &goods_info = *it;

				// 商品买入时的价格
				DWORD dwBuyPrice = goods_info.pGoods->GetBuyPrice();
				// 获取本贸易点此商品价格，如果是本地出售，则为1/2当前卖出价格
				DWORD dwSellPrice = goods_info.price;

				// TODO:计算利润、及获取的商业经验、商业SP
				lProfit += ( (long)dwSellPrice - (long)dwBuyPrice ) * goods_info.count;

				// ...

				// TODO:根据获得的利润值计算是否出现暴跌

				// 更新库存及出售记录
				goods_info.pBusinessGoods->AddSellCount( goods_info.count );
				goods_info.pBusinessGoods->SetVolume( goods_info.pBusinessGoods->GetVolume() + goods_info.count );
				// 影响其他商品库存量
				pSpot->IncreaseGoodsVolume( goods_info.pBusinessGoods, goods_info.count );

				// 记录操作日志
				GetGameLogInterface()->logT700_business_npc( pPlayer, goods_info.pGoods, 
					goods_info.count, dwSellPrice * goods_info.count, 
					2, dwSpotId );

				// 清理物品
				if( goods_info.pGoods->GetAmount() == goods_info.count )
				{
					db.AddToByteArray( (BYTE)CS2CContainerObjectMove::OT_DELETE_OBJECT);
					db.AddToByteArray( goods_info.guid );

					businessContainer.m_pContainer->Remove( goods_info.pGoods );
					pPlayer->OnLoseGoods( goods_info.pGoods );

#ifdef _GOODSLOG1_
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					goods_info.pGoods->GetExID().tostring( srcGoodsGUID );					
					_snprintf( pszGoodsLog, 1024, "[物品回收:BUSINESS_SELL][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						goods_info.pGoods->GetBasePropertiesIndex(), CGoodsFactory::QueryGoodsName( goods_info.pGoods->GetBasePropertiesIndex() ), srcGoodsGUID, goods_info.pGoods->GetAmount() );
					CMessage::AsyWriteFile( GetGame()->GetGoodsLogFile(), pszGoodsLog );							
#endif
					CGoodsFactory::GarbageCollect( &goods_info.pGoods ,49);
					dwSellCount ++;
				}
				else
				{
					goods_info.pGoods->SetAmount( goods_info.pGoods->GetAmount() - goods_info.count );
					db.AddToByteArray( (BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT );
					db.AddToByteArray( goods_info.guid );
					db.AddToByteArray( goods_info.pGoods->GetAmount() );
					dwSellCount ++;
				}

				// 获取金钱
				DWORD dwMoney = dwSellPrice * goods_info.count;
				pPlayer->SetMoney( pPlayer->GetMoney() + dwMoney,28 );
			}

			// 改变背包耐久
			businessContainer.UpdateEndure( lProfit );
			// 更新耐久属性到客户端
			businessContainer.UpdateEndureToClient( pPlayer->GetExID() );
			// 检查背包是否有效，无效则回收
			businessContainer.CheckValid();
			if( dwSellCount > 0 )
			{
				GetGame()->UpdateSavePlayer( pPlayer, SAVE_DETAIL_ATTRBUSINESSPACK );
				msg.SendToPlayer( pPlayer->GetExID() );
			}
		}
		break;
	case MSG_C2S_BUSINESS_INVEST:
		{
			//CGUID NpcID; 
			//pMsg -> GetGUID(NpcID);	//NPC的ID号
			//DWORD dwSpotId=pMsg->GetDWord();
			//DWORD dwInvestNum=pMsg->GetDWord();			
			//CMonster* pNpc	= dynamic_cast<CMonster*>( pRegion ->FindAroundObject(pPlayer, TYPE_MONSTER, NpcID) );
			//if( pNpc )
			//{
			//	if( pNpc -> Distance(pPlayer) > 10 )
			//	{
			//		pPlayer -> SendNotifyMessage(CStringReading::LoadString(25,2) );
			//		CancelTrading( pPlayer );
			//		return;
			//	}
			//}
			//else
			//{
			//	CancelTrading( pPlayer );
			//	return;
			//}
			//if(dwInvestNum>CBusinessManager::GetInvestMaxValue())
			//{
			//	//超过了单次最大投资额
			//	pPlayer->SendNotifyMessage(CStringReading::LoadString(25,3));
			//	return;
			//}
			//CTradeSpot* pSpot=CBusinessManager::GetTradeSpotById(dwSpotId);
			//if(!pSpot)
			//	return;
			//if(pSpot->GetCountryId()!=0)
			//{
			//	pPlayer->SendNotifyMessage(CStringReading::LoadString(25,4));
			//	return;
			//}
			//BOOL bCanInvest=pPlayer->SetInvestRecord(dwSpotId,dwInvestNum);
			//if(!bCanInvest)
			//{
			//	pPlayer->SendNotifyMessage(CStringReading::LoadString(25,5));
			//	return;
			//}
			////更新国家投资额度
			//BYTE btCountryId=pPlayer->GetCountry();
			//pSpot->AddInvestValue(btCountryId,dwInvestNum);

			////更新玩家在区域的声望
			//pPlayer->AddCredit(pSpot->GetTradeAreaId(),dwInvestNum);


		}
		break;
	case MSG_C2S_SHOP_REQUESTREPAIRALLPRICE:
		{
			CGoodsRepairPriceListener lPrice;
			pPlayer ->getOriginPack()->TraversingContainer( &lPrice );
			pPlayer -> m_cEquipment.TraversingContainer( &lPrice );
			pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->TraversingContainer(&lPrice);

			lPrice.m_dwPrice = static_cast<DWORD>( lPrice.m_dwPrice * ( 1 + pRegion -> GetTaxRate() / 100 ) );
			CMessage msg(MSG_S2C_SHOP_REPAIRALLCOST);
			msg.Add(lPrice.m_dwPrice);
			msg.SendToPlayer(pPlayer->GetExID());
		}
		break;
	case MSG_C2S_SHOP_REPAIR: //玩家请求在商店中修理一个物品
		{
			//##首先判断NPC距离
			CGUID NpcID; pMsg -> GetGUID(NpcID);
			CMonster* pNpc	= dynamic_cast<CMonster*>( pRegion -> 
				FindAroundObject(pPlayer, TYPE_MONSTER, NpcID) );
			if( pNpc )
			{
				CTradeList::tagTrade *pTrade = CTradeList::GetTradeList(pNpc->GetOriginName());			
				if(!pTrade)
				{				
					CancelTrading( pPlayer );
					break;
				}
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,5) );
					CancelTrading( pPlayer );
					break;
				}	
			}
			else
			{
				CancelTrading( pPlayer );
				break;
			}

			//##其次，开始修理物品
			LONG lContainerId=pMsg->GetLong();
			LONG lPos=pMsg->GetLong();			


			//根据ID取容器对象
			CGoodsContainer* pContainer = NULL;
			CGoods* pGoods=NULL;
			switch(lContainerId)
			{
			case CS2CContainerObjectMove::PEI_PACKET:
				pContainer = pPlayer -> getOriginPack();
				break;
			case CS2CContainerObjectMove::PEI_EQUIPMENT:
				pContainer = &( pPlayer -> m_cEquipment );
				break;						
				/*			case CS2CContainerObjectMove::PEI_DEPOT:										
				pContainer = &( pPlayer -> m_cDepot);					
				break;	*/			
			case CS2CContainerObjectMove::PEI_PACK:
				pContainer =  pPlayer ->getSubpackContainer();
				break;
			case CS2CContainerObjectMove::PEI_PACK1:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK2:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK3:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK4:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK5:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
				break;
			}
			if( pContainer )
			{
				pGoods=pContainer->GetGoods(lPos);
			}
			/*
			//装备栏中的物品
			else if( lPos >= pPlayer ->getOriginPack()->GetGoodsAmountLimit() && lPos < 
			pPlayer ->getOriginPack()->GetGoodsAmountLimit() + CEquipmentContainer::s_dwEquipmentLimit )
			{
			pGoods = pPlayer -> m_cEquipment.GetGoods( lPos - pPlayer ->getOriginPack()->GetGoodsAmountLimit() );
			}*/
			if(pGoods && pGoods -> CanReparied())
			{
				LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
				DWORD dwPrice = CGoodsFactory::CalculateRepairPrice( pGoods );
				dwPrice = static_cast<DWORD>( dwPrice * ( 1 + pRegion -> GetTaxRate() / 100 ) );
				if( pPlayer ->GetSilver() >= dwPrice )
				{
					if( CGoodsFactory::RepairEquipment(pGoods) )
					{
						//GetGameLogInterface()->logT023_goods_log_repair(pPlayer,pGoods->GetBasePropertiesIndex(),pGoods->GetExID(),1);
						pPlayer ->SetSilver( pPlayer ->GetSilver() - dwPrice );
						if (pContainer==&( pPlayer -> m_cEquipment )&&lCurrentDurability==0)
						{
							DWORD  dwPosition = 0;
							pPlayer->GetEquipmentContainer()->QueryGoodsPosition(pGoods,dwPosition);
							pPlayer->MountEquipEx(pGoods,true);
							pPlayer->UpdateAttribute(TRUE);
							pPlayer->ProduceEvent(ET_EquipMount,
								CS2CContainerObjectMove::PEI_EQUIPMENT,
								dwPosition,
								pGoods->GetGoodsBaseType(), 
								pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),
								(void*)CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
						}
						CMessage msg(MSG_S2C_SHOP_REPAIR);
						msg.Add((char)SHOP_REPAIR_SUCCEED);
						msg.Add(lContainerId);
						msg.Add(lPos);
						msg.Add(dwPrice);
						msg.SendToPlayer( pPlayer -> GetExID() );
						return;
					}
					else
					{
						pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,8));
					}
				}
				else
				{
					pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,9));
				}
			}
		}
		break;
	case MSG_C2S_SHOP_REQUESTREPAIRSINGLEPRICE:
		{
			//##其次，开始修理物品
			LONG lContainerId=pMsg->GetLong();
			LONG lPos=pMsg->GetLong();			


			//根据ID取容器对象
			CGoodsContainer* pContainer = NULL;
			CGoods* pGoods=NULL;
			switch(lContainerId)
			{
			case CS2CContainerObjectMove::PEI_PACKET:
				pContainer = pPlayer -> getOriginPack();
				break;
			case CS2CContainerObjectMove::PEI_EQUIPMENT:
				pContainer = &( pPlayer -> m_cEquipment );
				break;						
				/*			case CS2CContainerObjectMove::PEI_DEPOT:										
				pContainer = &( pPlayer -> m_cDepot);					
				break;	*/			
			case CS2CContainerObjectMove::PEI_PACK:
				pContainer =  pPlayer ->getSubpackContainer();
				break;
			case CS2CContainerObjectMove::PEI_PACK1:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK2:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK3:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK4:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
				break;
			case CS2CContainerObjectMove::PEI_PACK5:
				pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
				break;
			}
			if( pContainer )
			{
				pGoods=pContainer->GetGoods(lPos);
			}
			/*
			//装备栏中的物品
			else if( lPos >= pPlayer ->getOriginPack()->GetGoodsAmountLimit() && lPos < 
			pPlayer ->getOriginPack()->GetGoodsAmountLimit() + CEquipmentContainer::s_dwEquipmentLimit )
			{
			pGoods = pPlayer -> m_cEquipment.GetGoods( lPos - pPlayer ->getOriginPack()->GetGoodsAmountLimit() );
			}*/
			if(pGoods && pGoods -> CanReparied())
			{
				DWORD dwPrice = CGoodsFactory::CalculateRepairPrice( pGoods );
				dwPrice = static_cast<DWORD>( dwPrice * ( 1 + pRegion -> GetTaxRate() / 100 ) );
				CMessage priceMsg(MSG_S2C_SHOP_REPAIRSINGLECOST);
				priceMsg.Add(dwPrice);
				priceMsg.SendToPlayer(pPlayer->GetExID());
			}
		}
		break;
	case MSG_C2S_SHOP_REPAIRALL:	//修理全部物品
		{
			//##首先判断NPC距离
			CGUID NpcID; pMsg -> GetGUID(NpcID);
			CMonster* pNpc	= dynamic_cast<CMonster*>(pRegion->FindAroundObject(pPlayer, TYPE_MONSTER, NpcID));
			if( pNpc )
			{
				CTradeList::tagTrade *pTrade = CTradeList::GetTradeList(pNpc->GetOriginName());			
				if(!pTrade)
				{				
					CancelTrading( pPlayer );
					break;
				}
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					pPlayer -> SendNotifyMessage( CStringReading::LoadString(22,5));
					CancelTrading( pPlayer );
					break;
				}
			}
			else
			{
				CancelTrading( pPlayer );
				break;
			}

			CGoodsRepairPriceListener lPrice;
			pPlayer ->getOriginPack()->TraversingContainer( &lPrice );
			pPlayer -> m_cEquipment.TraversingContainer( &lPrice );
			pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->TraversingContainer(&lPrice);
			pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->TraversingContainer(&lPrice);

			lPrice.m_dwPrice = static_cast<DWORD>( lPrice.m_dwPrice * ( 1 + pRegion -> GetTaxRate() / 100 ) );

			//##如果没有需要修理的物品
			if( lPrice.m_dwNumEquipments == 0 )
			{
				pPlayer -> SendNotifyMessage( CStringReading::LoadString(22,10) );
				break;
			}

			//如果玩家所持金钱满足修理所需费用,则将物品的当前耐久值设置为最大耐久值
			if(pPlayer->GetSilver()>=lPrice.m_dwPrice)
			{
				pPlayer->SetSilver(pPlayer->GetSilver() - lPrice.m_dwPrice);


				CGoodsRepairListener lRepair;
				pPlayer ->getOriginPack()->TraversingContainer( &lRepair );
				//pPlayer -> m_cEquipment.TraversingContainer( &lRepair );
				pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->TraversingContainer(&lRepair);
				pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->TraversingContainer(&lRepair);
				pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->TraversingContainer(&lRepair);
				pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->TraversingContainer(&lRepair);
				pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->TraversingContainer(&lRepair);
				for(size_t i = 0; i<CEquipmentContainer::EC_TOTAL; i++)
				{	
					BOOL bReMount = FALSE;
					CGoods* pGoods = pPlayer->m_cEquipment.GetGoods(i);
					if (pGoods&&pGoods->HasAddonProperty(GAP_GOODS_MAXIMUM_DURABILITY)&&pGoods->GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 )==0)
						bReMount = TRUE;
					if(CGoodsFactory::RepairEquipment(pGoods))
					{
						if (pGoods!=NULL&&bReMount)
						{
							DWORD  dwPosition = 0;
							pPlayer->GetEquipmentContainer()->QueryGoodsPosition(pGoods,dwPosition);
							pPlayer->MountEquipEx(pGoods,true);
							pPlayer->UpdateAttribute(TRUE);
							pPlayer->ProduceEvent(ET_EquipMount,
								CS2CContainerObjectMove::PEI_EQUIPMENT,
								dwPosition,
								pGoods->GetGoodsBaseType(), 
								pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),
								(void*)CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
						}
					}
				}
				CSubpackContainer* pSubpack=pPlayer->getSubpackContainer();	
				
				//GetGameLogInterface()->logT023_goods_log_repair(pPlayer,0,CGUID::GUID_INVALID,2);

				CMessage msg(MSG_S2C_SHOP_REPAIRALL);
				msg.Add((char)SHOP_REPAIR_SUCCEED);
				msg.Add(lPrice.m_dwPrice);
				msg.SendToPlayer(pPlayer -> GetExID());
			}
			else
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,9));
			}
		}
		break;
	case MSG_C2S_SHOP_DURUPDATE://商店查看修理费用时物品当前耐久值的更新(精确值)
		{
			//##首先判断NPC距离
			CGUID NpcID; pMsg -> GetGUID(NpcID);
			CMonster* pNpc	= dynamic_cast<CMonster*>( pMsg -> GetRegion() -> 
				FindAroundObject(pPlayer, TYPE_MONSTER, NpcID) );
			if( pNpc )
			{
				if( pNpc -> Distance(pPlayer) > 10 )
				{
					pPlayer -> SendNotifyMessage(CStringReading::LoadString(22,5) );
					CancelTrading( pPlayer );
					break;
				}
			}
			else
			{
				CancelTrading( pPlayer );
				break;
			}

			//CMessage msg(MSG_S2C_SHOP_DURUPDATE);
			//list<CPlayer::tagGoods>::iterator GoodsIter=pPlayer->GetGoodsList().begin();
			//long lCount=0;
			////装备栏
			//for(int i=0;i<CPlayer::EQUIP_SUM;i++)
			//{
			//	CGoods* pEquip=pPlayer->GetEquip(i);
			//	if(pEquip)
			//	{
			//		DWORD dwVal=pEquip->GetEffectVal(37,2);
			//		if(dwVal!=0 && dwVal!=pEquip->GetLastNotifyDur())						
			//			lCount++;					
			//	}
			//}
			//msg.Add(lCount);

			//
			//for(int i=0;i<CPlayer::EQUIP_SUM;i++)
			//{
			//	CGoods* pEquip=pPlayer->GetEquip(i);
			//	if(pEquip)
			//	{
			//		DWORD dwVal=pEquip->GetEffectVal(37,2);
			//		if(dwVal!=0 && dwVal!=pEquip->GetLastNotifyDur())
			//		{
			//			msg.Add(BYTE(48+i));
			//			msg.Add(dwVal);						
			//		}
			//	}
			//}

			////背包栏
			//lCount=0;
			//for(;GoodsIter!=pPlayer->GetGoodsList().end();GoodsIter++)
			//{
			//	if(GoodsIter->bChange)
			//	{
			//		DWORD dwVal=GoodsIter->pGoods->GetEffectVal(37,2);
			//		if(dwVal!=0 && dwVal!=GoodsIter->pGoods->GetLastNotifyDur())									
			//			lCount++;					
			//	}
			//}
			//msg.Add(lCount);
			//for(;GoodsIter!=pPlayer->GetGoodsList().end();GoodsIter++)
			//{
			//	if(GoodsIter->bChange)
			//	{
			//		DWORD dwVal=GoodsIter->pGoods->GetEffectVal(37,2);
			//		if(dwVal!=0 && dwVal!=GoodsIter->pGoods->GetLastNotifyDur())
			//		{
			//			msg.Add(GoodsIter->lPos);
			//			msg.Add(dwVal);						
			//		}
			//	}
			//}

			////手上物品
			//if(pPlayer->GetHand()->bChange && pPlayer->GetHand()->pGoods)
			//{
			//	DWORD dwVal=pPlayer->GetHand()->pGoods->GetEffectVal(37,2);
			//	if(dwVal!=0 && dwVal!=pPlayer->GetHand()->pGoods->GetLastNotifyDur())
			//	{
			//		msg.Add((BYTE)true);
			//		msg.Add(dwVal);
			//	}
			//	else
			//		msg.Add((BYTE)false);
			//}		
			//msg.SendToPlayer((CMySocket*)(pPlayer->GetSocket()));
		}
		break;
	case MSG_C2S_SHOP_CLOSE:
		//##客户端告诉服务器需要关闭商店。
		{
			pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_NONE );
			pPlayer -> SetMoveable();
		}
		break;
	case MSG_C2S_FBUSINESS_OPER:
		{
			GetInst( FBusiness::CShopManager ).OnMessage( pMsg );
		}
		break;
	}
}