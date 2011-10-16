#include "stdafx.h"

#include "../../../Net/Message.h"

#include "../Player.h"
#include "../ClientRegion.h"
#include "../Goods/Goods.h"
#include "../MainPlayerHand.h"
#include "../Other/AudioList.h"
#include "../TradeSystem/PlayerTrade.h"
#include "../../GameClient/Game.h"

extern void UpdataPlayerTradeItem(int index,DWORD dwWhose);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

VOID OnContainerMessage( CMessage *pMsg )
{	
	CPlayer* pMainPlayer = pMsg -> GetPlayer();
	CClientRegion *pRegion = (CClientRegion *)pMsg->GetRegion();
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	
	if( pMainPlayer == NULL )
	{
		return;
	}
    long lMsgType = pMsg -> GetType();
	switch( lMsgType )
	{
	case MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE:	// 服务器通知客户端物品事件：物品的数量改变
		{
            CGUID SourceContainerID;
            LONG lSourceContainerType = pMsg->GetLong();
            pMsg->GetGUID(SourceContainerID);
            LONG lSourceContainerExtendID  = pMsg->GetLong();
            DWORD dwSourceContainerPosition = pMsg->GetDWord();
            LONG lObjectType = pMsg->GetLong();
            CGUID guID;
            pMsg->GetGUID(guID);
            DWORD dwAmount=pMsg->GetDWord();

            CGoods* pGoods = NULL;
            pGoods = pMainPlayer->GetGoodsByGUID(guID);
            if(pGoods==NULL&&lSourceContainerExtendID!=PEI_WALLET&&lSourceContainerExtendID!=PEI_SILVERWALLET)
                return;
            if(pGoods)
            {
                GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetIndex());
            }
            if(lSourceContainerExtendID==PEI_WALLET||lSourceContainerExtendID==PEI_SILVERWALLET)
            {
                GetGame()->GetAudioList()->Play2DSound("SOUNDS\\equipments\\00018.wav");
            }
            // 只对主角直接起效
            if( lSourceContainerType==TYPE_PLAYER && pMainPlayer->GetExID() == SourceContainerID)
            {
                if (lSourceContainerExtendID==PEI_PACKET||(lSourceContainerExtendID<=PEI_PACK5&&lSourceContainerExtendID>=PEI_PACK1))
                {
                    if (dwAmount > (DWORD)pGoods->GetNum())
                    {
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_5"),pGoods->GetName(),dwAmount-pGoods->GetNum());
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);
                    }
                    else if (pGoods->GetProperty()->dwType!=GBT_CONSUMABLE && dwAmount < (DWORD)pGoods->GetNum())
                    { 
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_19"),pGoods->GetName(),pGoods->GetNum()-dwAmount);
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);
                    }
                }else if ((lSourceContainerExtendID==PEI_WALLET ))
                {
                    if (dwAmount > GetGame()->GetMainPlayer()->GetMoney())
                    {
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_3"), dwAmount - GetGame()->GetMainPlayer()->GetMoney());
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str,0xfffff600);
                    }
                    else if (GetGame()->GetMainPlayer()->GetMoney()>dwAmount)
                    {
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_14"),GetGame()->GetMainPlayer()->GetMoney() - dwAmount);
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);
                    }
                }else if ((lSourceContainerExtendID==PEI_SILVERWALLET ))
                {
                    if (dwAmount > GetGame()->GetMainPlayer()->GetSilverMoney())
                    {   
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_17"), dwAmount - GetGame()->GetMainPlayer()->GetSilverMoney());
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str,0xfffff600);
                        
                    }
                    else if (GetGame()->GetMainPlayer()->GetSilverMoney()>dwAmount)
                    {
                        char str[64];
                        sprintf(str,AppFrame::GetText("Package_18"),GetGame()->GetMainPlayer()->GetSilverMoney() - dwAmount);
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);  
                    }
                }
                else if (lSourceContainerExtendID==PEI_YUANBAO)
                {
                    if (dwAmount > GetGame()->GetMainPlayer()->GetYuanBao())
                    {
                        
                        char str[256];
                        sprintf(str,AppFrame::GetText("Package_4"), dwAmount - GetGame()->GetMainPlayer()->GetYuanBao());
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);  
                    }
                    else if (dwAmount < GetGame()->GetMainPlayer()->GetYuanBao())
                    {
                        char str[256];
                        sprintf(str,AppFrame::GetText("Package_15"),  GetGame()->GetMainPlayer()->GetYuanBao() - dwAmount);
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);
                    }
                }
                //CGoods* pGoods = NULL;
                DWORD oldGoodsNum = 0;
                if (lSourceContainerExtendID!=PEI_WALLET&&lSourceContainerExtendID!=PEI_SILVERWALLET&&lSourceContainerExtendID!=PEI_YUANBAO)
                {
                    /*pGoods = pMainPlayer->GetGoodsByGUID(guID);*/
                    //if(pGoods==NULL)
                    //	return;
                    oldGoodsNum = pGoods->GetNum();		//此数量是为了在后面判定是否是物品在被消耗
                    ///////////////物品获取时的特效//////////////
                    if (pGoods->GetProperty()!=NULL&&dwAmount>oldGoodsNum)
                    {
                        //pItemPage->AddPickGoodsEffect(pGoods->GetIndex());
                    }
                }
                pMainPlayer->ChangeObjectAmount(lSourceContainerExtendID, lObjectType, guID, dwAmount);

                if (lSourceContainerExtendID!=PEI_WALLET&&lSourceContainerExtendID!=PEI_SILVERWALLET&&lSourceContainerExtendID!=PEI_YUANBAO)
                {
                    // 刷新制作页面的材料显示
                    //CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
                    //if (pFacturePage!=NULL&&pFacturePage->IsOpen())
                    //{	
                    //    pFacturePage->UpdateMaterial();
                    //    pFacturePage->CalculateTotalNum();
					//    pFacturePage->UpdateFactureTree();
					//}
					//更新快捷栏上的物品显示
					//CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
					//if (pMainBarPage!=NULL)
					//{
					//    DWORD dwGoodsIndex = 0;
					//    for (int i = 0;i<24;i++)
					//    {
					//        dwGoodsIndex = pMainPlayer->GetHotKey(i);
					//        if (dwGoodsIndex!=0)
					//        {
					//            if (pGoods->GetIndex()==dwGoodsIndex)
					//            {
					//                pMainBarPage->UpdataHotComp(i);
					//            }	
					//        }
					//    }
					//}
				}
				//pMainPlayer->ChangeObjectAmount(lSourceContainerExtendID, lObjectType, guID, dwAmount);
				if (pHand->GetPGoodsOfMainPlayerHand()!=NULL)
				{
					if (guID==pHand->GetPGoodsOfMainPlayerHand()->GetExID())
					{
						pHand->GetTGoodsOfMainPlayerHand()->lNum = dwAmount;
					}
				}
			}
		}
		break;
	case MSG_S2C_CONTAINER_OBJECT_MOVE:	// 服务器通知客户端物品事件：移动，删除，新建
		{
			BYTE opType = pMsg->GetByte();
			if( opType== OT_ROLL_BACK )
			{
				break;
			}

			CGUID guSourceObjectGUID;
			CGUID guDestinationObjectGUID;
			CGUID SourceContainerID,DestinationContainerID;
			// 源
			LONG lSourceContainerType=pMsg->GetLong();
			pMsg->GetGUID(SourceContainerID);
			LONG lSourceContainerExtendID=pMsg->GetLong();
			DWORD dwSourceContainerPosition=pMsg->GetDWord();
			//目标
			LONG lDestinationContainerType=pMsg->GetLong();
			pMsg->GetGUID(DestinationContainerID);
			LONG lDestinationContainerExtendID=pMsg->GetLong();
			DWORD dwDestinationContainerPosition=pMsg->GetDWord();

			LONG lSourceObjectType = pMsg->GetLong();
			pMsg->GetGUID(guSourceObjectGUID);
			LONG lDestinationObjectType = pMsg->GetLong();
			pMsg->GetGUID(guDestinationObjectGUID);
			DWORD dwSourceAmount = pMsg->GetDWord();

			// 新加物品
			if( opType==OT_NEW_OBJECT )
			{
				//读取物品序列化的内容

				DWORD dwSize=pMsg->GetDWord();
				if( dwSize==0 )
					break;
				BYTE *pBuff=new BYTE[dwSize];
				pMsg->GetEx(pBuff,dwSize);
				CGoods *pGoods=new CGoods;

				LONG lPos=0;
				if (pGoods!=NULL)
				{
					pGoods->DecordFromByteArray(pBuff,lPos);
				}
				SAFEDELETEARRAY(pBuff);

				// 玩家得到物品
				if( lDestinationContainerType==TYPE_PLAYER )
				{
					// 查找目标
					CPlayer* pPlayer = dynamic_cast<CPlayer*>(pRegion->FindChildObject(TYPE_PLAYER, DestinationContainerID));
					if (pPlayer == NULL)
						break;
					CGoods ShowGoods(*pGoods);
					pPlayer->AddGoods(pGoods,dwDestinationContainerPosition,lDestinationContainerExtendID);

					// 新加的物品是放在背包里面的，同时新物品不是来源于背包，更新任务相关信息
					if(IS_PACKETCONTAINER(lDestinationContainerExtendID) && !IS_PACKETCONTAINER(lSourceContainerExtendID))
					{
						pPlayer->UpdataQuestByGoods(&ShowGoods);
						// 自动装备该物品
						pPlayer->AutoEquip(&ShowGoods,lSourceContainerExtendID,lDestinationContainerExtendID,dwDestinationContainerPosition);
					}
					// 当物品是金币或者银币时，在加到玩家身上后就要将其删除，避免内存泄漏
					if (lDestinationContainerExtendID==PEI_WALLET||lDestinationContainerExtendID==PEI_SILVERWALLET)
					{
						SAFEDELETE(pGoods);
					}
					//播放物品音效

					GetGame()->GetAudioList()->PlayEquipmentSound(ShowGoods.GetIndex()); 

					if(lSourceContainerExtendID==PEI_WALLET||lSourceContainerExtendID==PEI_SILVERWALLET)
					{
						GetGame()->GetAudioList()->Play2DSound("SOUNDS\\equipments\\00018.wav");
					}
					//如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
					if (/*pGoods!=NULL&&*/ShowGoods.GetGoodsBaseType()==GBT_CONSUMABLE&&(lDestinationContainerExtendID==PEI_PACKET||
						(lDestinationContainerExtendID>=PEI_PACK1&&lDestinationContainerExtendID<=PEI_PACK5)))
					{
						pPlayer->AddShortCutGoods(ShowGoods.GetIndex());
						/// 获取此种物品的冷却类型
						DWORD dwRemainTime = pPlayer->GetGoodsRemainTime((WORD)ShowGoods.GetIndex());
						if (dwRemainTime!=0)
						{
							DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)ShowGoods.GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
							pPlayer->SetGoodsCoolDownStartTime((WORD)ShowGoods.GetIndex(),timeGetTime()-dwTimeElapse);
							pPlayer->SetGoodsUseState((WORD)ShowGoods.GetIndex(),false);
						}
						for(int i = 0;i<PACK_NUM;i++)
						{
							list<CPlayer::tagGoods>* GoodsList = pPlayer->GetGoodsList(i);
							list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
							for(;GoodsIter!=GoodsList->end();GoodsIter++)
							{
								if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==ShowGoods.GetEffectVal(GAP_COOLDOWN,2))
								{
									pPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
									/// 获取此种物品的冷却类型
									DWORD dwRemainTime = pPlayer->GetGoodsRemainTime((WORD)GoodsIter->pGoods->GetIndex());
									if (dwRemainTime!=0)
									{
										DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)GoodsIter->pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
										pPlayer->SetGoodsCoolDownStartTime((WORD)GoodsIter->pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
										pPlayer->SetGoodsUseState((WORD)GoodsIter->pGoods->GetIndex(),false);
									}
								}
							}
						}
					}
					//更新快捷栏上的物品显示
					//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
					//if (pPage!=NULL)
					//{
					//    DWORD dwGoodsIndex = 0;
					//    for (int i = 0;i<24;i++)
					//    {
					//        dwGoodsIndex = pPlayer->GetHotKey(i);
					//        if (dwGoodsIndex!=0)
					//        {
					//            if (/*pGoods!=NULL&&*/ShowGoods.GetIndex()==dwGoodsIndex)
					//            {
					//                pPage->UpdataHotComp(i);								
					//            }
					//        }
					//    }
					//}
					// 刷新制作页面的材料显示
					//CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
					//if (pFacturePage!=NULL&&pFacturePage->IsOpen())
					//{	
					//    pFacturePage->UpdateMaterial();
					//    pFacturePage->CalculateTotalNum();
					//    pFacturePage->UpdateFactureTree();
					//}
					//if( lDestinationContainerExtendID==PEI_PACKET||(lDestinationContainerExtendID<=PEI_PACK5&&lDestinationContainerExtendID>=PEI_PACK1) )
					//{
					//    if (CStringReading::LoadText(200,5)!=NULL)
					//    {
					//        char str[256];
					//        sprintf(str,CStringReading::LoadText(200,5),ShowGoods.GetName(),ShowGoods.GetNum());
					//        GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(str);
					//    }
					//    ///////////////物品获取时的特效//////////////
					//    if (ShowGoods.GetProperty()!=NULL)
					//    {
					//        pItemPage->AddPickGoodsEffect(ShowGoods.GetIndex());	
					//    }
					//}else if (lDestinationContainerExtendID == PEI_WALLET) //金币
					//{
					//    char strValue[64];
					//    if (CStringReading::LoadText(200,3)!=NULL)
					//    {
					//        sprintf(strValue,CStringReading::LoadText(200,3),dwSourceAmount);
					//        GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(strValue);
					//    }
					//}
					//else if (lDestinationContainerExtendID == PEI_SILVERWALLET)
					//{
					//    char strValue[64];
					//    if (CStringReading::LoadText(200,17)!=NULL)
					//    {
					//        sprintf(strValue,CStringReading::LoadText(200,17),dwSourceAmount);
					//    }
					//}
					//else if (lDestinationContainerExtendID == PEI_YUANBAO)	//元宝
					//{
					//    pPlayer->SetYuanBao(dwSourceAmount);
					//}
				}
				// 场景出现物品
				else if( lDestinationContainerType==TYPE_REGION )
				{
					pRegion->AddGoods(pGoods,dwDestinationContainerPosition,lDestinationContainerExtendID);
				}
				// 会话物品
				else if( lDestinationContainerType==TYPE_SESSION )
				{
					//交易
					if(DestinationContainerID == GetInst(PlayerTrade).GetSessionID())
					{
						CPlayer* pPlayer = GetGame()->GetMainPlayer();
						if (lSourceContainerExtendID == PEI_WALLET)			//金币
						{
							if (SourceContainerID!=pPlayer->GetExID())
							{
								if (g_bDebug)
								{
									/// 用于输出发送物品移动的信息
									char strTitle[64] = "";
									strcpy_s(strTitle,"--服务器向客户端发送的交易金钱信息--");
									char strPrintInfor[512] = "";
									char strSessionID[64] = "";
									char strGoodsGUID[64] = "";
									DestinationContainerID.tostring(strSessionID);
									pGoods->GetExID().tostring(strGoodsGUID);
									sprintf_s(strPrintInfor,"%s\n%s%s\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%d\n",
										strTitle,
										"此次交易的SessionID是:",strSessionID,
										"金币原来所在的容器ID:",lSourceContainerExtendID,
										"金币在原来容器中的位置:",dwSourceContainerPosition,
										"金币要移动到的目标容器ID:",lDestinationContainerExtendID,
										"金币要移动到目标容器的位置:",dwDestinationContainerPosition,
										"本次操作的金币的GUID:",strGoodsGUID,
										"本次操作的金币数量:",dwSourceAmount);
									PutStrToFileSaveToDir( "log/GoldsTradeLog", "记录金币的交易过程", strPrintInfor, false );
								}

								//GetInst(PlayerTrade).UpdataTradeInfo(PlayerTrade::PT_Money,dwSourceAmount,PlayerTrade::Trade_Other);
								GetInst(PlayerTrade).SetOtherGold(dwSourceAmount);
                            }
                            break;
                        }
                        else if (lSourceContainerExtendID == PEI_YUANBAO)
                        {
                            if (SourceContainerID!=pPlayer->GetExID())
                               GetInst(PlayerTrade).UpdataTradeInfo(PlayerTrade::PT_YuanBao,dwSourceAmount,PlayerTrade::Trade_Other);
                            break;
                        }
                        else
                        {
                            if (SourceContainerID==GetGame()->GetMainPlayer()->GetExID())
							{
								GetInst(PlayerTrade).AddMyTradeGoods(pGoods,(int)dwDestinationContainerPosition);
								UpdataPlayerTradeItem((int)dwDestinationContainerPosition,PlayerTrade::Trade_Me);
							}
							else
							{
								GetInst(PlayerTrade).AddOtherTradeGoods(pGoods,(int)dwDestinationContainerPosition);
								UpdataPlayerTradeItem((int)dwDestinationContainerPosition,PlayerTrade::Trade_Other);
							}
                            break;
                        }
                    }
                }
                else
                {
                    delete pGoods;
                }
            }
            // 删除物品
            else if( opType==OT_DELETE_OBJECT )
            {
                //读取物品数量				
                DWORD dwDestinationAmount=pMsg->GetDWord();
                CGoods *pGoods=NULL;

                // 删除玩家物品
                if( lSourceContainerType==TYPE_PLAYER )
                {
                    // 查找目标
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pRegion->FindChildObject(TYPE_PLAYER, SourceContainerID));
                    if (pPlayer == NULL)
                        break;

                    pGoods = pPlayer->RemoveGoods( guSourceObjectGUID,guDestinationObjectGUID,dwSourceContainerPosition,lSourceContainerExtendID,dwSourceAmount);
                    // 删除背包里的物品，刷新相关任务
                    if(IS_PACKETCONTAINER(lSourceContainerExtendID))
                    {
                        if(pGoods)
                            pPlayer->UpdataQuestByGoods(pGoods);
                    }
                    //更新快捷栏上的显示
                    //CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
                    //if (pPage!=NULL)
                    //{
                    //    DWORD dwGoodsIndex = 0;
                    //    for (int i = 0;i<24;i++)
                    //    {
                    //        dwGoodsIndex = pMainPlayer->GetHotKey(i);
                    //        if (dwGoodsIndex!=0)
                    //        {
                    //            if (pGoods!=NULL&&pGoods->GetIndex()==dwGoodsIndex)
                    //            {
                    //                pPage->UpdataHotComp(i);
                    //            }
                    //        }
                    //    }	
                    //}
                    //把此物品的物编号从冷却列表中删除
                    if (pGoods!=NULL&&CGoodsList::GetProperty(pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE
                        &&pPlayer->GetGoodsAmount(pGoods->GetIndex())==0&&pPlayer->GetGoodsUseState(pGoods->GetIndex()))
                    {
                        pPlayer->SetGoodsCoolDownStartTime((WORD)pGoods->GetIndex(),0);
                        //pPlayer->SetGoodsUseState(pGoods->GetIndex(),false);
                    }

                    /*CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
                    if (pFacturePage!=NULL&&pFacturePage->IsOpen())
                    {
                    pFacturePage->UpdateMaterial();
                    pFacturePage->CalculateTotalNum();
                    pFacturePage->UpdateFactureTree();
                    }*/
                }
                // 删除地上物品
                else if( lSourceContainerType ==TYPE_REGION )
                {                
                    pGoods=pRegion->RemoveGoods( guSourceObjectGUID,dwSourceContainerPosition,lSourceContainerExtendID,dwSourceAmount);
                }

                if(pGoods)
                {
                    SAFE_DELETE(pGoods);
                }
            }
            // 移动物品
            else if( opType==OT_MOVE_OBJECT )
            {
                if (g_bDebug)
                {
                    /// 用于输出发送物品移动的信息
                    char strTitle[64] = "";
                    //if (GetGame()->GetCGuiEx()->GetItemPageEx()->GetFinishBagState())
                    //{
                    //    strcpy_s(strTitle,"--服务器向客户端发送的（整理）物品移动信息--");
                    //}
                    //else
                    //    strcpy_s(strTitle,"--服务器向客户端发送的物品的普通的移动信息--");
                    char strPrintInfor[512] = "";
                    char strGoodsGUID[64] = "";
                    guSourceObjectGUID.tostring(strGoodsGUID);
                    sprintf_s(strPrintInfor,"%s\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%d\n",
                        strTitle,
                        "物品原来所在的容器ID:",lSourceContainerExtendID,
                        "物品在原来容器中的位置:",dwSourceContainerPosition,
                        "物品要移动到的目标容器ID:",lDestinationContainerExtendID,
                        "物品要移动到目标容器的位置:",dwDestinationContainerPosition,
                        "本次操作的物品的GUID:",strGoodsGUID,
                        "本次操作的物品数量:",dwSourceAmount);
                    PutStrToFileSaveToDir( "log/GoodsMoveLog", "记录物品的移动过程", strPrintInfor, false );
                    //////////////////////////////
                }

                // 读取物品数量
                //DWORD dwSourceAmount=pMsg->GetDWord();
                DWORD dwDestinationAmount=pMsg->GetDWord();
                CGoods *pGoods=NULL;

                // 移走玩家身上物品
                if( lSourceContainerType==TYPE_PLAYER )
                {
                    // 查找目标，只对自己有效
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pRegion->FindChildObject(TYPE_PLAYER, SourceContainerID));
                    if (pPlayer != pMainPlayer)
                        break;
                    pGoods = pPlayer->RemoveGoods( guSourceObjectGUID,guDestinationObjectGUID,dwSourceContainerPosition,lSourceContainerExtendID,dwSourceAmount);
                    // 把物品从背包中移动到其它容器,刷新任务信息
                    if(IS_PACKETCONTAINER(lSourceContainerExtendID) && !IS_PACKETCONTAINER(lDestinationContainerExtendID))
                    {
                        if(pGoods)
                            pPlayer->UpdataQuestByGoods(pGoods);
                    }
                    // 刷新制作页面的材料显示
                    //CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
                    //if (pFacturePage!=NULL&&pFacturePage->IsOpen())
                    //{	
                    //    pFacturePage->UpdateMaterial();
                    //    pFacturePage->CalculateTotalNum();
                    //    pFacturePage->UpdateFactureTree();
                    //}

                    //把此物品的物编号从冷却列表中删除
                    if (pGoods!=NULL&&CGoodsList::GetProperty(pGoods->GetIndex())->BaseProperty.dwType==GBT_CONSUMABLE
                        &&pPlayer->GetGoodsAmount(pGoods->GetIndex())==0&&pPlayer->GetGoodsUseState(pGoods->GetIndex()))
                    {
                        pPlayer->SetGoodsCoolDownStartTime((WORD)pGoods->GetIndex(),0);
                        //pPlayer->SetGoodsUseState(pGoods->GetIndex(),false);
                    }

                    if( lSourceContainerExtendID== PEI_MEDAL )
                    {
                        //pGoods = GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetGoods(dwSourceContainerPosition);
                    }
                    //GetGame()->GetCGuiEx()->GetCharacterPageEx()->SetTitleShow();
                }
                // 从地上捡起
                else if( lSourceContainerType==TYPE_REGION )
                {
                    pGoods=pRegion->RemoveGoods( guSourceObjectGUID,dwSourceContainerPosition,lSourceContainerExtendID,dwSourceAmount);
                }
                if( pGoods==NULL )
                {
                    break;
                }

                // 准备将移出的物品移动到目标地点
                if( guDestinationObjectGUID != CGUID::GUID_INVALID )
                {
                    pGoods->SetExID(guDestinationObjectGUID);
                }
                // 移动到人身上
                if( lDestinationContainerType==TYPE_PLAYER )
                {
                    //CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
                    // 查找目标，这里只对自己起效
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pRegion->FindChildObject(TYPE_PLAYER, DestinationContainerID));
                    if (pPlayer != pMainPlayer)
                        break;

                    // 从地上获得物品
                    if( (lDestinationContainerExtendID==PEI_PACKET||(lDestinationContainerExtendID<=PEI_PACK5&&lDestinationContainerExtendID>=PEI_PACK1)) && lSourceContainerType==TYPE_REGION )
                    {

                        char str[256];
                        if(CGoodsList::GetEffectVal(pGoods->GetIndex(),GAP_MAXOVERLAP,1)>0)
                            sprintf(str,AppFrame::GetText("Package_5"),pGoods->GetName(),pGoods->GetNum());
                        else
                            sprintf(str,AppFrame::GetText("Package_6"),pGoods->GetName());
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddOtherInfo(str);
                        GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetIndex());

                        /////获取物品时的特效/////
                        if (pGoods->GetProperty()!=NULL)
                        {
                            //pItemPage->AddPickGoodsEffect(pGoods->GetIndex());
                        }
                    }
                    // 从地上获得金钱
                    else if( (lDestinationContainerExtendID==PEI_WALLET||lDestinationContainerExtendID==PEI_SILVERWALLET) && lSourceContainerType==TYPE_REGION )
                    {
                        char str[128];
                        if (lDestinationContainerExtendID==PEI_WALLET)
                        {
                            sprintf(str,AppFrame::GetText("Package_3"),dwSourceAmount);
                        }
                        else if (lDestinationContainerExtendID==PEI_SILVERWALLET)
                        {
                            sprintf(str,AppFrame::GetText("Package_17"),dwSourceAmount);
                        }
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddOtherInfo(str);					
                        pGoods->SetNum(dwSourceAmount);
                    }
                    //if (pPlayer->GetMoney() == 0)
                    //	pPlayer->SetguID(lDestinationContainerExtendID,pGoods->GetExID());

                    //读取物品序列化的内容
                    if (pGoods!=NULL)
                    {	
                        /// 判断移动到玩家身上的是否是需要重新编码的物品
                        if ( dwDestinationAmount!=0)
                        {
                            BYTE *pBuff=new BYTE[dwDestinationAmount];
                            pMsg->GetEx(pBuff,dwDestinationAmount);
                            LONG lPos=0;
                            if (pGoods!=NULL)
                            {
                                pGoods->DecordFromByteArray(pBuff,lPos);
                            }
                            SAFEDELETEARRAY(pBuff);
                        }
                    }

                    CGoods ShowGoods(*pGoods);
                    pPlayer->AddGoods( pGoods,dwDestinationContainerPosition,lDestinationContainerExtendID );
                    // 把物品从其它容器移动到背包中,刷新任务信息
                    if(!IS_PACKETCONTAINER(lSourceContainerExtendID) && IS_PACKETCONTAINER(lDestinationContainerExtendID))
                    {
                        pPlayer->UpdataQuestByGoods(&ShowGoods);
                        // 自动装备该物品
                        pPlayer->AutoEquip(&ShowGoods,lSourceContainerExtendID,lDestinationContainerExtendID,dwDestinationContainerPosition);
                    }					
                    ///// 当处于背包整理状态时就继续执行下去。。。直到整理完毕。。
                    //if (pItemPage!=NULL&&pItemPage->GetFinishBagState())
                    //{
                    //	if (pItemPage->CanFinishBag()&&pItemPage->GoodsSort())
                    //	{
                    //		pItemPage->SendMsgForFinishBag();
                    //	}
                    //}

                    //GetGame()->GetCGuiEx()->GetCharacterPageEx()->UpdateCoolIcon();
                    //GetGame()->GetCGuiEx()->GetCharacterPageEx()->UpdateEquipButton();
                    GetGame()->GetAudioList()->PlayEquipmentSound(ShowGoods.GetIndex());

                    // 当物品是金币或者银币时，在加到玩家身上后就要将其删除，避免内存泄漏
                    if (lDestinationContainerExtendID==PEI_WALLET||lDestinationContainerExtendID==PEI_SILVERWALLET)
                    {
                        SAFEDELETE(pGoods);
                    }

                    // 刷新制作页面的材料显示
                    //CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
                    //if (pFacturePage!=NULL&&pFacturePage->IsOpen())
                    //{	
                    //    pFacturePage->UpdateMaterial();
                    //    pFacturePage->CalculateTotalNum();
                    //    pFacturePage->UpdateFactureTree();
                    //}

                    //如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
                    if (ShowGoods.GetGoodsBaseType()==GBT_CONSUMABLE&&(lDestinationContainerExtendID==PEI_PACKET||
                        (lDestinationContainerExtendID>=PEI_PACK1&&lDestinationContainerExtendID<=PEI_PACK5)))
                    {
                        pPlayer->AddShortCutGoods(ShowGoods.GetIndex());
                        /// 获取此种物品的冷却类型
                        DWORD dwRemainTime = pPlayer->GetGoodsRemainTime((WORD)ShowGoods.GetIndex());
                        if (dwRemainTime!=0)
                        {
                            DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)ShowGoods.GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
                            pPlayer->SetGoodsCoolDownStartTime((WORD)ShowGoods.GetIndex(),timeGetTime()-dwTimeElapse);
                            pPlayer->SetGoodsUseState((WORD)ShowGoods.GetIndex(),false);
                        }
                        for(int i = 0;i<PACK_NUM;i++)
                        {
                            list<CPlayer::tagGoods>* GoodsList = pPlayer->GetGoodsList(i);
                            list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
                            for(;GoodsIter!=GoodsList->end();GoodsIter++)
                            {
                                if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==ShowGoods.GetEffectVal(GAP_COOLDOWN,2))
                                {
                                    pPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
                                    /// 获取此种物品的冷却类型
                                    DWORD dwRemainTime = pPlayer->GetGoodsRemainTime((WORD)GoodsIter->pGoods->GetIndex());
                                    if (dwRemainTime!=0)
                                    {
                                        DWORD dwTimeElapse = CGoodsList::GetEffectVal((WORD)GoodsIter->pGoods->GetIndex(),GAP_COOLDOWN,1)-dwRemainTime;
                                        pPlayer->SetGoodsCoolDownStartTime((WORD)GoodsIter->pGoods->GetIndex(),timeGetTime()-dwTimeElapse);
                                        pPlayer->SetGoodsUseState((WORD)GoodsIter->pGoods->GetIndex(),false);
                                    }
                                }
                            }
                        }
                    }

                }
                // 移动到场景上
                else if(lDestinationContainerType==TYPE_REGION )
                {
                    pRegion->AddGoods( pGoods,dwDestinationContainerPosition,lDestinationContainerExtendID );
                }
                else
                {
                    SAFE_DELETE(pGoods);
                }
            }
            else if (opType==OT_SWITCH_OBJECT)
            {
                // 查找消息发起者，这里只对自己起效
                CPlayer* pPlayer = dynamic_cast<CPlayer*>(pRegion->FindChildObject(lSourceContainerType, SourceContainerID));
                if (pPlayer != pMainPlayer)
                    break;

                if(lSourceContainerType!=TYPE_PLAYER)
                {
                    break;
                }

                int sPos = 0;
                int dPos = 0;

                bool bIsMedal = false;
                CGoods *pGoods1 = NULL;
                CGoods *pGoods2 = NULL;
                //判断交换的物品是否是勋章
                if(lSourceContainerExtendID == PEI_MEDAL)
                {
                    bIsMedal = true;
                    //pGoods1 = GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetGoods(dwSourceContainerPosition);
                    CGoods *pMedalOnPlayer = pPlayer->GetEquip(dwDestinationContainerPosition);
                    if(pMedalOnPlayer)
                    {
                        DWORD dwDesGoods = pMedalOnPlayer->GetProperty()->dwIndex; 
                        //pGoods2 = GetGame()->GetCGuiEx()->GetCharacterPageEx()->GetGoods(dwDesGoods);
                    }

                    sPos = -1;
                    dPos = -1;
                }
                else
                {
                    pGoods1=pPlayer->GetGoodsByGUID(guSourceObjectGUID,&sPos);
                    pGoods2=pPlayer->GetGoodsByGUID(guDestinationObjectGUID,&dPos);
                }

                if(pGoods2!=NULL&&pGoods1!=NULL)
                {
                    pPlayer->RemoveGoods( guDestinationObjectGUID,guDestinationObjectGUID,dPos,lDestinationContainerExtendID,pGoods2->GetNum());
                    pPlayer->RemoveGoods(guSourceObjectGUID,guSourceObjectGUID, sPos, lSourceContainerExtendID, pGoods1->GetNum());
                    pPlayer->AddGoods(pGoods1,dwDestinationContainerPosition,lDestinationContainerExtendID);
                    pPlayer->AddGoods(pGoods2,dwSourceContainerPosition,lSourceContainerExtendID);
                    GetGame()->GetAudioList()->PlayEquipmentSound(pGoods1->GetIndex());
                    // 不是勋章
                    if(!bIsMedal)
                    {
                        // 不是背包内的物品交换，刷新任务信息
                        if(!(IS_PACKETCONTAINER(lSourceContainerExtendID) && IS_PACKETCONTAINER(lDestinationContainerExtendID)))
                        {
                            pPlayer->UpdataQuestByGoods(pGoods1);
                            pPlayer->UpdataQuestByGoods(pGoods2);
                        }
                    }
                }
            }
          }
        break;
	case MSG_S2C_CONTAINER_OBJECT_UNLOCK:	//服务器通知客户端各种Session页面对物品的锁定解除的消息
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);
			long lPlugID = pMsg->GetLong();
			long lPos = pMsg->GetLong();
			CGUID GoodsGUID;
			pMsg->GetGUID(GoodsGUID);
			CPlayer* pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer==NULL) 
				return;

			//镶嵌卡片
			//CCardMutifunPageEx* pMutiPage = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			//if (pMutiPage!=NULL&&pMutiPage->IsOpen()&&SessionID==pMutiPage->GetSessionID())
			//{
			//	if (lPos==CCardMutifunPageEx::EEPC_EQUIPMENT)
			//	{
			//		pMutiPage->RemoveGoodsFromPage();
			//		pMutiPage->UpdatePageForGoods();
			//	}else if (lPos>=CCardMutifunPageEx::EEPC_CARD1&&lPos<=CCardMutifunPageEx::EEPC_CARD5)
			//	{
			//		BYTE index = (BYTE)(lPos-CCardMutifunPageEx::EEPC_CARD1);
			//		pMutiPage->RemoveCardFromPage(index);
			//		pMutiPage->UpdatePage();
			//		pMutiPage->UpdateAppointHole(index);
			//	}else if (lPos==CCardMutifunPageEx::EEPC_YUANBAO_ITEM1)
			//	{
			//		pMutiPage->RemoveAssGoodsFromPage();
			//		pMutiPage->UpdateAssGoodsShow();
			//	}
			//	if (pCharacterPage!=NULL)
			//	{
			//		pCharacterPage->UpdataEquip();
			//	}
			//}
			// 交易
			/*else */if (SessionID==GetInst(PlayerTrade).GetSessionID())
			{
				CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
				if(lPlugID == GetInst(PlayerTrade).GetMyPlugID())
				{
					long lExtendID = 0;
					int iPos = -1;
					GetInst(PlayerTrade).RemoveTradeGoods(lPos, PlayerTrade::Trade_Me);

					CGoods* pRemoveGoods = pMainPlayer->GetGoodsByGUID(GoodsGUID,&iPos,&lExtendID);
					if (pRemoveGoods!=NULL)
					{
						// 为移出交易界面的物品解锁
						if (pRemoveGoods->IsLocked())
						{
							pRemoveGoods->UnLock();
						}

						// 当手上有移出的物品图标时，背包中的图标不变亮，否则视为关闭交易界面时移出物品，此时物品在背包中的
						// 图标要变亮
						if (pHand!=NULL&&pRemoveGoods==pHand->GetPGoodsOfMainPlayerHand())
						{
							pRemoveGoods->SetHaveShadowState(true);
						}else
							pRemoveGoods->SetHaveShadowState(false);
					}
				}
				else
				{
					GetInst(PlayerTrade).RemoveTradeGoods(lPos, PlayerTrade::Trade_Other);
				}
			}
		}
		break;

	case MSG_S2C_CONTAINER_CLEAR:	//服务器通知客户端物品事件：清除容器的所有物品
		
		break;		

	}
}