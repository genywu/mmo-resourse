#include "stdafx.h"
#include "Message.h"
#include "Player.h"
#include "Other/BusinessSystem.h"
#include "Clientregion.h"
#include "Other/AudioList.h"
#include "../../Game.h"
#include "../../../Public/Common/BusinessGoodsDef.h"
#include "../../../Public/Common/FBusinessCommon.h"
#include "../../../Public/Common/FBusinessCommon.h"
#include "../../../Public/Common/Public.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnShopMessage(CMessage *pMsg)
{
	CPlayer* pPlayer=GetGame()->GetMainPlayer();
	DWORD dwMsgType = pMsg->GetType();
	switch(dwMsgType) 
	{
	case MSG_S2C_BUSINESS_OPEN:
		{
			//CBusinessShopPageEx *pShopPage = GetGame()->GetCGuiEx()->GetBusinessShopPageEx();
			BYTE bPageType = pMsg->GetByte();
			if (bPageType==Business::BUSINESS_BUYPAGE)
			{
				//if(!pShopPage)	return;
				CGUID NpcGUID = CGUID::GUID_INVALID;
				pMsg->GetGUID(NpcGUID);
				//pShopPage->SetNpcID(NpcGUID);
				//pShopPage->SetCurShopType(Business::BUSINESS_BUYPAGE);
				//pShopPage->SetTradePotID(pMsg->GetDWord());
				//pShopPage->SetNpcShopMsg(pMsg);
				//pShopPage->Open();
			}
			else if (bPageType==Business::BUSINESS_SELLPAGE)
			{
				//if(!pShopPage)	return;
				CGUID NpcGUID = CGUID::GUID_INVALID;
				pMsg->GetGUID(NpcGUID);
				//pShopPage->SetNpcID(NpcGUID);
				//pShopPage->SetCurShopType(Business::BUSINESS_SELLPAGE);
				//pShopPage->SetTradePotID(pMsg->GetDWord());
				//pShopPage->UpdateSpecialNeedGoodsInfor(pMsg);
				//pShopPage->UpdateSellPriceList(pMsg);
				//pShopPage->SetNpcShopMsg(pMsg);
				//pShopPage->Open();
			}
			else if (bPageType==Business::BUSINESS_CHECKPAGE)
			{

			}
			GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-20.wav");
		}
		break;
	case MSG_S2C_SHOP_OPEN:	//gameserver通知客户端打开商店界面
		{			
			//CNPCShopPageEx *pShopPage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			//if(!pShopPage)	return;
			CGUID NpcGUID = CGUID::GUID_INVALID;
			pMsg->GetGUID(NpcGUID);
			//pShopPage->SetNpcID(NpcGUID);
			//pShopPage->SetCurShopType((int)(pMsg->GetLong()));
			//pShopPage->SetCoinType(pMsg->GetByte());
			//pShopPage->SetShopCreditType(pMsg->GetByte());
			//pShopPage->SetBasePriceRate(pMsg->GetFloat());
			//pShopPage->SetTradeInRate(pMsg->GetFloat());
			//pShopPage->SetNpcShopMsg(pMsg);
			//pShopPage->Open();	
			GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-20.wav");
		}
		break;
	case MSG_S2C_SHOP_CLOSE:
		//GetGame()->GetInterface()->GetShopPage()->Close();
		break;
	case MSG_S2C_SHOP_BUYBACK:	// 用于确定回购物品(只适用于普通商店和以物易物商店)是否成功，以便处理购回页面的数据
		{
			//CNPCShopPageEx *pShopPage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
			//if (pShopPage==NULL||pMainPlayer==NULL) break;
			BYTE bSellResult = pMsg->GetByte();
			CGUID GoodsGUID;
			pMsg->GetGUID(GoodsGUID);
			if (bSellResult==1)			//回购成功
			{
				int i = 0;
				deque<CGoods*>& tempBuyBackGoods = pMainPlayer->GetQueueBuyBackGoods();
				deque<CGoods*>::iterator GoodsIter = tempBuyBackGoods.begin();
				for (;GoodsIter!=tempBuyBackGoods.end();GoodsIter++,i++)
				{
					if (GoodsGUID==(*GoodsIter)->GetExID())
					{
						break;
					}
				}
				//pMainPlayer->DelBuyBackGoods(i);
				//pShopPage->UpdateBuyBackPage();
				//pShopPage->UpdateLastSellGoods();
			}else if (bSellResult==0)	//回购失败
			{
				//pShopPage->ClearTempBuyBackGoods();
			}

		}break;
	case MSG_S2C_SHOP_SELL:			// 用于确定卖出物品(只适用于普通商店和以物易物商店)是否成功，以便处理购回页面的数据
		{
			//CNPCShopPageEx *pShopPage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			//if (pShopPage==NULL) break;
			BYTE bSellResult = pMsg->GetByte();
			CGUID GoodsGUID;
			pMsg->GetGUID(GoodsGUID);
			
			if (bSellResult==1)
			{
				//CGoods* pBuyBackGoods = pShopPage->GetTempBuyBackGoods();
				//if (pBuyBackGoods!=NULL)
				//{
				//	pPlayer->AddBuyBackGoods(pBuyBackGoods);
					//pShopPage->UpdateBuyBackPage();
					//pShopPage->UpdateLastSellGoods();
					//pShopPage->ClearTempBuyBackGoods(true);
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\equipments\\00018.wav");
				//}
			}
			else if (bSellResult==0)
			{
				//pShopPage->ClearTempBuyBackGoods(false);
			}
		}break;
	case MSG_S2C_BUSINESS_BUY:	//gameserver返回从跑商NPC中买物品的结果
		{
			WORD wGoodsNum = pMsg->GetWord();
			for (WORD i = 0;i<wGoodsNum;i++)
			{
				BYTE bOpType = pMsg->GetByte();
				if (bOpType==OT_NEW_OBJECT)
				{
					long lDateSize = pMsg->GetLong();	//数据块的大小
					BYTE* GoodInfor = new BYTE[lDateSize];
					memset(GoodInfor,0,lDateSize);
					pMsg->GetEx(GoodInfor,lDateSize);
					long lPos = 0;
					CGoods* pGoods = new CGoods;
					pGoods->DecordFromByteArray(GoodInfor,lPos);
					DWORD dwPos = pMsg->GetDWord();
					pPlayer->AddGoods(pGoods,dwPos,PEI_BUSINESSPACK);
					SAFEDELETEARRAY(GoodInfor);
				}
				else if (bOpType==OT_CHANGE_AMOUNT)
				{
					long lGoodsChangeNum = pMsg->GetLong();
					CGUID guid;
					pMsg->GetGUID(guid);	
					CGoods* pGoods = pPlayer->GetGoodsByGUID(guid);
					if (pGoods!=NULL)
					{
						pPlayer->ChangeObjectAmount(PEI_BUSINESSPACK,TYPE_GOODS,guid,pGoods->GetNum()+lGoodsChangeNum);	
					}
				}

				/// 设置背包中指定物编ID的物品的买入价格
				long lGoodsIndex = pMsg->GetLong();
				DWORD dwGoodsPrice = pMsg->GetDWord();
				long lDimen = pPlayer->GetBussinesPackDimen();
				for (int i = 0;i<lDimen;i++)
				{
					CPlayer::tagGoods* ptgGoods = pPlayer->GetBusinessGoods(i);
					if (ptgGoods!=NULL&&ptgGoods->pGoods!=NULL&&lGoodsIndex==ptgGoods->pGoods->GetIndex())
					{
						ptgGoods->pGoods->GetBaseProperty()->dwBuyPrice = dwGoodsPrice;
					}
				}
			}
		}
		break;
	case MSG_S2C_SHOP_BUY_BAT: //gameserver返回从商店中买物品的结果
		{
			DWORD dwExtendID = 0;
			DWORD dwPos = 0;
			if (dwMsgType==MSG_S2C_BUSINESS_BUY)
			{
				dwExtendID = pMsg->GetDWord();
			}
			WORD wNum = pMsg->GetWord();
			BYTE GoodInfor[102400];
			for (WORD i = 0;i<wNum;i++)
			{
				long pos = 0;
				memset(GoodInfor,0,102400);
				pMsg->GetEx(GoodInfor,102400);
				CGoods* pGoods = new CGoods;
				pGoods->DecordFromByteArray(GoodInfor,pos);
				if (dwMsgType!=MSG_S2C_BUSINESS_BUY)
				{
					dwExtendID = pMsg->GetDWord();
				}
				dwPos = pMsg->GetDWord();
				CGoods ShowGoods(*pGoods);
				pPlayer->AddGoods(pGoods,dwPos,dwExtendID);
				if(i == 0&&pGoods!=NULL)
				{
					GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetProperty()->dwIndex);
				}
				//如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
				if (pGoods!=NULL&&pGoods->GetGoodsBaseType()==GBT_CONSUMABLE&&(dwExtendID==PEI_PACKET||
					(dwExtendID>=PEI_PACK1&&dwExtendID<=PEI_PACK5)))
				{
					pPlayer->AddShortCutGoods(pGoods->GetIndex());
					for(int i = 0;i<PACK_NUM;i++)
					{
						list<CPlayer::tagGoods>* GoodsList = pPlayer->GetGoodsList(i);
						list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
						for(;GoodsIter!=GoodsList->end();GoodsIter++)
						{
							if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==pGoods->GetEffectVal(GAP_COOLDOWN,2))
							{
								pPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
							}
						}
					}
				}
				// 新加的物品是放在背包里面的，同时新物品不是来源于背包，更新任务相关信息
				if(IS_PACKETCONTAINER(dwExtendID))
				{
					// 自动装备该物品
					pPlayer->AutoEquip(&ShowGoods,0,dwExtendID,dwPos);
				}
				///////////物品获取时的特效//////////////
				if (pGoods->GetProperty()!=NULL)
				{
					//GetGame()->GetCGuiEx()->GetItemPageEx()->AddPickGoodsEffect(pGoods->GetIndex());
				}
				//更新快捷栏上的物品显示
				//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
				//if (pPage!=NULL)
				//{
				//	DWORD dwGoodsIndex = 0;
				//	for (int i = 0;i<24;i++)
				//	{
				//		dwGoodsIndex = pPlayer->GetHotKey(i);
				//		if (dwGoodsIndex!=0)
				//		{
				//			if (pGoods!=NULL&&pGoods->GetIndex()==dwGoodsIndex)
				//			{
				//				pPage->UpdataHotComp(i);								
				//			}
				//		}
				//	}
				//}
			}
		}
		break;
	case MSG_S2C_SHOP_GOODSCHANGEGOODS:	//以物易物类商店的消息
		{
			if (pPlayer!=NULL)
			{
				//扣掉玩家身上的一些物品
				DWORD dwGoodsTypeNum = pMsg->GetDWord();	//要扣除物品种类的个数	
				for(size_t i=0;i<dwGoodsTypeNum;i++)
				{
					DWORD dwContainerNum = pMsg->GetDWord();	//每种类型的物品所在的容器的个数
					for(size_t j=0;j<dwContainerNum;j++)
					{
						DWORD dwContainerID = pMsg->GetDWord();	//每个容器的ID
						DWORD dwGoodsPosNum = pMsg->GetDWord();	//要处理的位置个数
						for(size_t k=0;k<dwGoodsPosNum;k++)
						{
							DWORD dwGoodsPos = pMsg->GetDWord();
							DWORD dwGoodsNum = pMsg->GetDWord();
							CGoods* pDelGoods = pPlayer->GetGoodsByPosition(dwContainerID,dwGoodsPos);
							if (pDelGoods!=NULL)
							{
								if (dwGoodsNum==pDelGoods->GetNum())
								{
									pPlayer->RemoveGoods(pDelGoods->GetExID(),CGUID::GUID_INVALID,dwGoodsPos,dwContainerID,dwGoodsNum);
								}else if (pDelGoods->GetNum()> (long)dwGoodsNum)
								{
									CGUID guid = pDelGoods->GetExID();
									pPlayer->ChangeObjectAmount(dwContainerID,TYPE_GOODS,guid,pDelGoods->GetNum()-dwGoodsNum);
								}
							}
						}
					}
				}
				//添加一些物品到玩家身上
				DWORD dwTotalGoodsNum = pMsg->GetDWord();
				DWORD dwDataSize = pMsg->GetDWord();
				BYTE* pData = new BYTE[dwDataSize];
				memset(pData,0,dwDataSize);
				pMsg->GetEx(pData,dwDataSize);
				long pos = 0;
				for (size_t i = 0;i<dwTotalGoodsNum;i++)
				{
					BYTE type = _GetCharFromByteArray(pData,pos);
					if (type==OT_CHANGE_AMOUNT)
					{
						DWORD dwContainerID = _GetDwordFromByteArray(pData,pos);
						DWORD dwGoodsPos = _GetDwordFromByteArray(pData,pos);
						DWORD dwGoodsNum = _GetDwordFromByteArray(pData,pos);
						CGoods* pChangeGoods = pPlayer->GetGoodsByPosition(dwContainerID,dwGoodsPos);
						if (pChangeGoods!=NULL)
						{
							CGUID guid = pChangeGoods->GetExID();
							DWORD dwOldGoodsNum = pChangeGoods->GetNum();
							DWORD dwCurGoodsNum = dwOldGoodsNum+dwGoodsNum;
							pPlayer->ChangeObjectAmount(dwContainerID,TYPE_GOODS,guid,dwCurGoodsNum);
							///////////物品获取时的特效//////////////
							if (pChangeGoods->GetProperty()!=NULL&&dwCurGoodsNum>dwOldGoodsNum)
							{
								//GetGame()->GetCGuiEx()->GetItemPageEx()->AddPickGoodsEffect(pChangeGoods->GetIndex());
							}
							//更新快捷栏上的物品显示
							//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
							//if (pPage!=NULL)
							//{
								DWORD dwGoodsIndex = 0;
								for (int i = 0;i<24;i++)
								{
									dwGoodsIndex = pPlayer->GetHotKey(i);
									if (dwGoodsIndex!=0)
									{
										if (pChangeGoods!=NULL&&pChangeGoods->GetIndex()==dwGoodsIndex)
										{
											//pPage->UpdataHotComp(i);								
										}
									}
								}
							//}
						}
					}else if (type==OT_NEW_OBJECT)
					{
						DWORD dwContainerID = _GetDwordFromByteArray(pData,pos);
						CGoods* pNewGoods = new CGoods;
						pNewGoods->DecordFromByteArray(pData,pos);
						DWORD dwGoodsPos = _GetDwordFromByteArray(pData,pos);
						pPlayer->AddGoods(pNewGoods,dwGoodsPos,dwContainerID);
						//如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
						if (pNewGoods!=NULL&&pNewGoods->GetGoodsBaseType()==GBT_CONSUMABLE&&(dwContainerID==PEI_PACKET||
							(dwContainerID>=PEI_PACK1&&dwContainerID<=PEI_PACK5)))
						{
							pPlayer->AddShortCutGoods(pNewGoods->GetIndex());
							for(int i = 0;i<PACK_NUM;i++)
							{
								list<CPlayer::tagGoods>* GoodsList = pPlayer->GetGoodsList(i);
								list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
								for(;GoodsIter!=GoodsList->end();GoodsIter++)
								{
									if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==pNewGoods->GetEffectVal(GAP_COOLDOWN,2))
									{
										pPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
									}
								}
							}
						}
						///////////物品获取时的特效//////////////
						if (pNewGoods->GetProperty()!=NULL)
						{
							//GetGame()->GetCGuiEx()->GetItemPageEx()->AddPickGoodsEffect(pNewGoods->GetIndex());
						}
						//更新快捷栏上的物品显示
						//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
						//if (pPage!=NULL)
						//{
							DWORD dwGoodsIndex = 0;
							for (int i = 0;i<24;i++)
							{
								dwGoodsIndex = pPlayer->GetHotKey(i);
								if (dwGoodsIndex!=0)
								{
									if (pNewGoods!=NULL&&pNewGoods->GetIndex()==dwGoodsIndex)
									{
										//pPage->UpdataHotComp(i);								
									}
								}
							}
						//}
					}
				}
				SAFEDELETEARRAY(pData);
			}
		}break;
	case MSG_S2C_BUSINESS_SELL:		//gameserver返回卖物品到跑商商店的结果 
		{
			long lSellGoodsNum = pMsg->GetLong();
			for (int i = 0;i<lSellGoodsNum;i++)
			{
				BYTE bOPType = pMsg->GetByte();
				CGUID guid;
				long lExtendID = 0;
				int	 iPos = -1;
				pMsg->GetGUID(guid);
				CGoods* pGoods = pPlayer->GetGoodsByGUID(guid,&iPos,&lExtendID);
				if (pGoods!=NULL)
				{
					if (bOPType==OT_DELETE_OBJECT)
					{
						pPlayer->RemoveGoods(guid,guid,iPos,lExtendID,pGoods->GetNum());
					}
					else if (bOPType==OT_CHANGE_AMOUNT)
					{
						long lGoodsNum = pMsg->GetLong();
						pPlayer->ChangeObjectAmount(lExtendID,TYPE_GOODS,guid,lGoodsNum);
					}
				}
			}
		}break;
	case MSG_S2C_SHOP_SELL_BAT:		//gameserver返回卖物品到商店的结果 
		{
			long lExtendID = 0;
			if (dwMsgType==MSG_S2C_BUSINESS_SELL)
			{
				lExtendID = pMsg->GetDWord();
			}
			WORD wNum = pMsg->GetWord();
			BYTE pByte[102400];
			memset(pByte,0,102400);
			pMsg->GetEx(pByte,102400);
			long pos = 0;
			for (WORD i = 0;i<wNum;i++)
			{
				BYTE type = _GetCharFromByteArray(pByte,pos);
				if (dwMsgType==MSG_S2C_SHOP_SELL_BAT)
				{
					lExtendID = _GetLongFromByteArray(pByte,pos);
				}
				long lPos = _GetLongFromByteArray(pByte,pos);
				CGoods* pGoods = pPlayer->GetGoodsByPosition(lExtendID,lPos);
				if (pGoods)
				{		
					if(i == 0)
					{
						GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetProperty()->dwIndex);
					}
					DWORD lGoodsIndex = pGoods->GetIndex();
					CGUID guID = pGoods->GetExID();
					DWORD dwAmount = 0;
					if(type==OT_DELETE_OBJECT)
					{
						dwAmount = pGoods->GetNum();
						pPlayer->RemoveGoods(guID,guID,lPos,lExtendID,dwAmount);
						SAFE_DELETE(pGoods);
					}else if (type==OT_CHANGE_AMOUNT)
					{
						dwAmount = _GetDwordFromByteArray(pByte,pos);
						pPlayer->RemoveGoods(guID,guID,lPos,lExtendID,pGoods->GetNum()-dwAmount);
						pGoods = NULL;
					}
					//更新快捷栏上的显示
					//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
					//if (pPage!=NULL)
					//{
					//	DWORD dwGoodsIndex = 0;
					//	for (int i = 0;i<24;i++)
					//	{
					//		dwGoodsIndex = pPlayer->GetHotKey(i);
					//		if (dwGoodsIndex!=0)
					//		{
					//			if (lGoodsIndex==dwGoodsIndex)
					//			{
					//				pPage->UpdataHotComp(i);
					//			}
					//		}
					//	}	
					//}
					//把此物品的物编号从冷却列表中删除
					if (pPlayer->GetGoodsAmount(lGoodsIndex)==0
						&&pPlayer->GetGoodsUseState(lGoodsIndex))
					{
						pPlayer->SetGoodsCoolDownStartTime((WORD)lGoodsIndex,0);
						//pPlayer->SetGoodsUseState(lGoodsIndex,true);
						//pPlayer->DelShortCutGoods(lGoodsIndex);
					}
				}
			}

		}
		break;
	case MSG_S2C_INVEST_OPEN:
		{
			CBusinessSystem::tagInvestInfor* ptrInvestInfor = CBusinessSystem::GetInvestInfor();
			if (ptrInvestInfor!=NULL)
			{
				ptrInvestInfor->NpcGUID = GetGame()->GetLastTouchNpcID();
				ptrInvestInfor->dwPointID = pMsg->GetDWord();
			}
			// 弹出投资界面,输入投资数目
			//CNumberBoxPageEx *pBox =  GetGame()->GetCGuiEx()->GetNumberBoxPageEx();
			/*if(pBox!=NULL)
			{
				pBox->Open();
				pBox->SetTextOfBox(CStringReading::LoadText(202,1));
				pBox->SetMaxNum(200000000);
				pBox->SetMinNum(0);
				pBox->SetSpecialToucherType(CNumberBoxPageEx::STType_Business);
			}*/
		}break;
	case MSG_S2C_SHOP_REPAIR:	//gameserver返回在商店中修理一个物品的结果
		{
			int result=pMsg->GetChar();
			
			if(result==SHOP_REPAIR_SUCCEED)
			{						
				long lExtendID = pMsg->GetLong();
				long lPos=pMsg->GetLong();
				DWORD dwPrice=pMsg->GetDWord();
				//pPlayer->SetMoney(pPlayer->GetMoney()-dwPrice);
				CPlayer::tagGoods* pstGoods = NULL;
				//在背包中查找该物品,将当前耐久值设置为最大耐久值
				if (lExtendID==PEI_PACKET)
				{
					if(lPos>=0 && lPos<20)
					{
						pstGoods = pPlayer->GetGoods((WORD)lPos);
						if (pstGoods==NULL) break;
						vector<CGoodsList::tagEffect>::iterator EffectIter=pstGoods->pGoods->GetEffect()->begin();
						for(;EffectIter!=pstGoods->pGoods->GetEffect()->end();EffectIter++)
						{
							if(EffectIter->wType==GAP_GOODS_MAX_DURABILITY)
							{
								EffectIter->dwValue2=EffectIter->dwValue1;
								break;
							}
						}
					}
				}else if (lExtendID<=PEI_PACK5&&lExtendID>=PEI_PACK1)
				{
					WORD diem = pPlayer->GetPackageDimen((WORD)lExtendID-PEI_PACK1);
					if (lPos>=0&&lPos<diem)
					{
						pstGoods = pPlayer->GetSubGoods((WORD)lExtendID-PEI_PACK1,(WORD)lPos);
						if (pstGoods==NULL) break;
						vector<CGoodsList::tagEffect>::iterator EffectIter=pstGoods->pGoods->GetEffect()->begin();
						for(;EffectIter!=pstGoods->pGoods->GetEffect()->end();EffectIter++)
						{
							if(EffectIter->wType==GAP_GOODS_MAX_DURABILITY)
							{
								EffectIter->dwValue2=EffectIter->dwValue1;
								break;
							}
						}
					}
				}
				//在装备栏中查找该物品,将当前耐久值设置为最大耐久值
				else if(lExtendID==PEI_EQUIPMENT)
				{
					CGoods* pGoods=pPlayer->GetEquip(lPos);
					if(pGoods)
					{
						vector<CGoodsList::tagEffect>::iterator iter=pGoods->GetEffect()->begin();
						for(;iter!=pGoods->GetEffect()->end();iter++)
						{
							if(iter->wType==GAP_GOODS_MAX_DURABILITY)
							{
								iter->dwValue2=iter->dwValue1;
								break;
							}
						}
					}
				}
			    GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-22.wav");
				//GetGame()->GetCGuiEx()->GetEquipEwearPageEx()->CheckEquip();
			}
			else if(result==SHOP_REPAIR_MONEYNOTENOUGH)
			{
				//GetGame()->GetInterface()->AddText("你没有足够的金钱修理该物品!",0XFFFF0000);
			}
			else if(result==SHOP_REPAIR_FAILED)
			{
			}
		}
		break;
	case MSG_S2C_SHOP_REPAIRALL: //gameserver修理全部物品的返回结果
		{
			int result=pMsg->GetChar();
			if(result==SHOP_REPAIR_SUCCEED)
			{
				DWORD dwTotalPrice=pMsg->GetDWord();
				//pPlayer->SetMoney(pPlayer->GetMoney() - dwTotalPrice);
				list<CPlayer::tagGoods>::iterator GoodsIter;
				//背包里的物品全修理
				for (int i = 0;i<PACK_NUM;i++)
				{
					GoodsIter=pPlayer->GetGoodsList(i)->begin();
					for(;GoodsIter!=pPlayer->GetGoodsList(i)->end();GoodsIter++)
					{
						vector<CGoodsList::tagEffect>::iterator EffectIter=GoodsIter->pGoods->GetEffect()->begin();
						for(;EffectIter!=GoodsIter->pGoods->GetEffect()->end();EffectIter++)
						{
							if(EffectIter->wType == GAP_GOODS_MAX_DURABILITY&& EffectIter->dwValue2<EffectIter->dwValue1)
							{
								EffectIter->dwValue2=EffectIter->dwValue1;
							}
						}
					}
				}
				//装备栏里的物品全修理
				for(int i=0;i<CPlayer::EQUIP_SUM;i++)
				{
					CGoods* pGoods=pPlayer->GetEquip(i);
					if(pGoods)
					{
						vector<CGoodsList::tagEffect>::iterator EffectIter=pGoods->GetEffect()->begin();
						for(;EffectIter!=pGoods->GetEffect()->end();EffectIter++)
						{
							if(EffectIter->wType==GAP_GOODS_MAX_DURABILITY && EffectIter->dwValue2<EffectIter->dwValue1)
							{
								EffectIter->dwValue2=EffectIter->dwValue1;
							}
						}
					}
				}
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-23.wav");
				//GetGame()->GetCGuiEx()->GetEquipEwearPageEx()->CheckEquip();
			}
			else if(result==SHOP_REPAIR_MONEYNOTENOUGH)
			{
				//GetGame()->GetInterface()->AddText("你没有足够的金钱修理全部物品",0XFFFF0000);
			}
		}
		break;
	case MSG_S2C_SHOP_REPAIRALLCOST:
		{
			DWORD dwPrice = pMsg->GetDWord();
			//CNPCShopPageEx* pNPCShopPage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			//if (pNPCShopPage!=NULL)
			//{
			//	int type = CMessageBoxPageEx::MSG_OK_NO;
			//	char str[64];
			//	if(dwPrice > 0)
			//		sprintf(str,CStringReading::LoadText( 202, 6 ),dwPrice);
			//	else
			//	{
			//		sprintf(str,"%s",CStringReading::LoadText( 202, 9 ));
			//		type = CMessageBoxPageEx::MSG_OK;
			//	}
				//GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( pNPCShopPage, str, type, true );
			//}
		}
		break;
	case MSG_S2C_SHOP_REPAIRSINGLECOST:
		{
			DWORD dwPrice = pMsg->GetDWord();
			//CNPCShopPageEx* pNPCShopPage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			//CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
			/*if (pNPCShopPage!=NULL&&pItemPage!=NULL)
			{
				int type = CMessageBoxPageEx::MSG_OK_NO;
				char str[64];
				if(dwPrice > 0)
				{
					sprintf(str,CStringReading::LoadText( 202, 22 ),dwPrice);
				}
				else
				{
					sprintf(str,"%s",CStringReading::LoadText( 202, 23 ));
					type = CMessageBoxPageEx::MSG_OK;
				}
				GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open( pItemPage, str, type, true );
			}*/
		}break;
	case MSG_S2C_FBUSINESS_OPER:
		{
			//CFamilyBusinessShopPageEx *pShopPage = GetGame()->GetCGuiEx()->GetFamilyBusinessShopPageEx();
			BYTE bPageType = pMsg->GetByte();
			if (bPageType==FBusiness::S2C_SELL_PAGE)
			{
				//if(!pShopPage)	return;
				CGUID NpcGUID = CGUID::GUID_INVALID;
				pMsg->GetGUID(NpcGUID);
				/*pShopPage->SetNpcID(NpcGUID);
				pShopPage->SetCurShopType(FBusiness::S2C_SELL_PAGE);
				pShopPage->SetTradePotID(pMsg->GetDWord());
				pShopPage->SetNpcShopMsg(pMsg);
				pShopPage->Open();*/
			}
			else if (bPageType==FBusiness::S2C_BUY_PAGE)
			{
				//if(!pShopPage)	return;
				CGUID NpcGUID = CGUID::GUID_INVALID;
				pMsg->GetGUID(NpcGUID);
				//pShopPage->SetNpcID(NpcGUID);
				//pShopPage->SetCurShopType(FBusiness::S2C_BUY_PAGE);
				//pShopPage->SetTradePotID(pMsg->GetDWord());
				//pShopPage->UpdateSpecialNeedGoodsInfor(pMsg);
				//pShopPage->UpdateSellPriceList(pMsg);
				/*pShopPage->SetNpcShopMsg(pMsg);
				pShopPage->Open();*/
			}
			else if (bPageType==FBusiness::S2C_PRICE_CHANGED)
			{
				CGUID NpcGUID;
				long  lShopID = 0;
				pMsg->GetGUID(NpcGUID);
				lShopID = pMsg->GetLong();
				/*if (pShopPage->GetNpcID()==NpcGUID&&pShopPage->GetTradePotID()==lShopID)
				{
					CMessageBoxPageEx* pMsgPage = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
					if (pMsgPage)
					{
						pMsgPage->Open(pShopPage,CStringReading::LoadText(202,18),CMessageBoxPageEx::MSG_OK_NO,true);
					}
				}*/
			}
			GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-20.wav");
		}
		break;
	}
}