#include "stdafx.h"
#include "../ClientRegion.h"
#include "../Player.h"
#include "../Goods/Goods.h"
//#include "Message.h"
#include "../../Game.h"
//#include "Other/Chat.h"
#include "../../GameControl.h"
#include "Other/AudioList.h"
#include "../../Public/Common/Public.h"
//#include "../../Public/Common/StrType.h"
#include "../TradeSystem/PlayerShop.h"
#include "../../../UI/GamePage/ChatList/ChatListPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 * 功能: 响应玩家开店消息
 * 摘要: -
 * 参数: pMsg - 服务器发送的消息
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 */

void OnPlayerShopMessage(CMessage* pMsg)
{
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	if (!pMainPlayer) return;

	PlayerShop& playerShop = GetPlayerShop();
	
	switch(pMsg->GetType()) 
	{
		// 打开个人商店
	case MSG_S2C_PALYERSHOP_CREATE:
		{
			CGUID sessionID;
			pMsg->GetGUID(sessionID);
			long plugID = pMsg->GetLong();
			CGUID masterID = pMainPlayer->GetExID();

			pMainPlayer->SetPlayerShopSessionID(sessionID);
			pMainPlayer->SetPlayerShopPlugID(plugID);
			
			// 打开个人商店
			playerShop.SetCurShopState(PlayerShop::SET_SHOP);
			playerShop.SetShopPara(sessionID, plugID, masterID);
			FireUIEvent("PlayerShop", "Opened");
		}
		break;

		// 开始摆摊
	case MSG_S2C_PLAYERSHOP_OPENFORBUSINESS:
		{
			CGUID PlayerID,SessionID;
			pMsg->GetGUID(PlayerID);
			pMsg->GetGUID(SessionID);
			long lPlugID = pMsg->GetLong();
			CPlayer *pPlayer=(CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID);
			if(pPlayer==NULL)
				break;

			GetGame()->GetGameControl()->AutoMoveStop();
			GetGame()->GetGameControl()->AutoAttactStop();
			GetGame()->GetGameControl()->StopMove();
			pPlayer->SetPlayerShopOpen(true);				
			pPlayer->SetPlayerShopSessionID(SessionID);
			pPlayer->SetPlayerShopPlugID(lPlugID);
			if (pPlayer==GetGame()->GetMainPlayer()) 
			{		
				playerShop.SetCurShopState(PlayerShop::OPEN_SHOP);
				//playerShop->OpenShop(SessionID,lPlugID,PlayerID);

				// 开店时,让玩家播放坐下的仪态动作
				//if (pPlayer->GetActionIndexOfActionAnim() != 0 ||
				//	(pPlayer->GetActionIndexOfActionAnim() == 0 && !pPlayer->GetPlayStateOfActionAnim()))
				//{
				//	CMessage msg(MSG_C2S_OTHER_FACEACTION);
				//	msg.Add((unsigned long)1);
				//	msg.Add((long)0);
				//	msg.Add(NULL_GUID);
				//	msg.Send();
				//}

				// 更新操作界面按钮状态
				FireUIEvent("PlayerShop", "UpdateOperBtn");
				playerShop.UpdataPlayerShopGoodsList();
			}
		}
		break;

		// 看其他玩家的个人商店页面
	case MSG_S2C_PLAYERSHOP_LOOKGOODS:
		{
			CGUID SessionID;
			CGUID MasterID;		//店主的GUID
			pMsg->GetGUID(SessionID);
			pMsg->GetGUID(MasterID);
			long lBuyerPlugID = pMsg->GetLong();
			pMainPlayer->SetPlayerShopSessionID(SessionID);
			pMainPlayer->SetPlayerShopPlugID(lBuyerPlugID);				
			BYTE ByteData[102400];
			long lBytePos = 0;
			pMsg->GetEx(ByteData,102400);
			DWORD dwNumber = _GetDwordFromByteArray(ByteData,lBytePos);
			for(size_t i=0;i<dwNumber;i++)
			{
				CGoods *pGoods = new CGoods();
				pGoods->DecordFromByteArray(ByteData,lBytePos,true);
				DWORD dwDealType = _GetDwordFromByteArray(ByteData,lBytePos);
				DWORD dwPriceType = _GetDwordFromByteArray(ByteData,lBytePos);
				DWORD dwTeamNum = _GetDwordFromByteArray(ByteData,lBytePos);
				DWORD dwGoodsNumPerTeam = _GetDwordFromByteArray(ByteData,lBytePos);
				DWORD dwPrice = _GetDwordFromByteArray(ByteData,lBytePos);
				playerShop.AddPlayerShopGoods(pGoods,dwTeamNum,dwPriceType,dwPrice,dwDealType,dwGoodsNumPerTeam);
			}
            GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-30.wav");

			playerShop.SetCurShopState(PlayerShop::SHOPPING_SHOP);
			playerShop.SetShopPara(SessionID,lBuyerPlugID,MasterID);

			FireUIEvent("PlayerShop", "Opened");
		}
		break;

		// 关闭商店
	case MSG_S2C_PLAYERSHOP_CLOSE:
		{
			CGUID PlayerID,SessionID;
			pMsg->GetGUID(PlayerID);
			pMsg->GetGUID(SessionID);
			CPlayer *pPlayer=(CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID);
			if(pMainPlayer==NULL)break;
			if (pPlayer!=NULL)
			{
				pPlayer->SetPlayerShopOpen(false);

				// 开店取消时,让玩家播放站立动作
				pPlayer->SetAction(CMoveShape::ACT_STAND);
			}

			if (SessionID == playerShop.GetSessionID())
			{
                GetChatListPage().AddChatWords(AppFrame::GetText("Shop_7"),//店主停止摆摊
                    0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				playerShop.CloseShop();
				FireUIEvent("PlayerShop", "Closed");
			}
		}
		break;

		// 店主取消摆摊
	case MSG_S2C_PLAYERSHOP_CLOSEDOWN:
		{
			CGUID PlayerID,SessionID;
			pMsg->GetGUID(PlayerID);
			pMsg->GetGUID(SessionID);
			CPlayer *pPlayer = (CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID);
			if(pPlayer==NULL) break;
			pPlayer->SetPlayerShopOpen(false);

			if(PlayerID==GetGame()->GetMainPlayer()->GetExID()
				&&SessionID==playerShop.GetSessionID()
				&&playerShop.GetCurShopState()==PlayerShop::OPEN_SHOP)
			{
				playerShop.SetbSetTrue(false);
				playerShop.SetCurShopState(PlayerShop::SET_SHOP);
				playerShop.CancelAllTrade();
				//playerShop.Open();
			}
			else if (playerShop.GetSessionID()==SessionID
				&&playerShop.GetCurShopState()==PlayerShop::SHOPPING_SHOP)
			{
				playerShop.CloseShop();
			}

			GetChatListPage().AddChatWords(AppFrame::GetText("Shop_7")  //店主停止摆摊
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		}
		break;

		// 退出个人商店
	case MSG_S2C_PLAYERSHOP_QUIT:
		{
			CGUID SessionID;
			pMsg->GetGUID(SessionID);

			if (/*playerShop.IsOpen()&&*/playerShop.GetCurShopState()==PlayerShop::SHOPPING_SHOP)
			{
				CPlayer* pPlayer = (CPlayer*)GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,playerShop.GetMasterID());
				if (pPlayer!=NULL&&pMainPlayer->Distance(pPlayer)>6)
				{
					GetChatListPage().AddChatWords(AppFrame::GetText("Shop_9"), //你离店太远了
                        0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				}
			}

			if (playerShop.GetSessionID()==SessionID
				&&playerShop.GetCurShopState()==PlayerShop::SHOPPING_SHOP)
			{
				playerShop.CloseShop();
			}
		}
		break;

		// 给买家
	case MSG_S2C_PLAYERSHOP_TOBUYER:
		{
			DWORD lGoodsIndex = 0;
			long lExtendID = -1;
			long lRetNum = 0;
			CGUID guid;
			DWORD dwPos = 0;
			DWORD dwMoney = 0;
			DWORD dwYuanBao = 0;
			DWORD dwOPNumber = pMsg->GetDWord();		/// 操作过程数
			for(size_t i=0;i<dwOPNumber;i++)
			{
				CGoods *pGoods = NULL;
				BYTE bType = pMsg->GetByte();
				if (bType==OT_NEW_OBJECT)
				{
					pGoods = new CGoods();			
					BYTE ByteData[102400];
					long lBytePos = 0;
					pMsg->GetEx(ByteData,102400);
					pGoods->DecordFromByteArray(ByteData,lBytePos,true);
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					lGoodsIndex = pGoods->GetIndex();
					CGoods ShowGoods(*pGoods);
					pMainPlayer->AddGoods(pGoods,dwPos,lExtendID);

					// 新加的物品是放在背包里面的，同时新物品不是来源于背包，更新任务相关信息
					if(IS_PACKETCONTAINER(lExtendID))
					{
						// 自动装备该物品
						pMainPlayer->AutoEquip(&ShowGoods,0,lExtendID,dwPos);
					}

					//如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
					if (pGoods!=NULL&&pGoods->GetGoodsBaseType()==GBT_CONSUMABLE&&(lExtendID==PEI_PACKET||
						(lExtendID>=PEI_PACK1&&lExtendID<=PEI_PACK5)))
					{
						pMainPlayer->AddShortCutGoods(lGoodsIndex);
						for(int i = 0;i<PACK_NUM;i++)
						{
							list<CPlayer::tagGoods>* GoodsList = pMainPlayer->GetGoodsList(i);
							list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
							for(;GoodsIter!=GoodsList->end();GoodsIter++)
							{
								if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==pGoods->GetEffectVal(GAP_COOLDOWN,2))
								{
									pMainPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
								}
							}
						}
					}
				}
				else if (bType==OT_CHANGE_AMOUNT)
				{
					pMsg->GetGUID(guid);
					lRetNum = pMsg->GetDWord();
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					pGoods = pMainPlayer->GetGoodsByGUID(guid);
					if (pGoods!=NULL)
					{
						lGoodsIndex = pGoods->GetIndex();
						pMainPlayer->ChangeObjectAmount(lExtendID,TYPE_GOODS,guid,lRetNum);
					}
				}
				else if (bType==OT_DELETE_OBJECT)
				{
					pMsg->GetGUID(guid);
					lRetNum = pMsg->GetLong();	
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					pGoods = pMainPlayer->RemoveGoods(guid,guid,dwPos,lExtendID,lRetNum);
					lGoodsIndex = pGoods->GetIndex();
					SAFE_DELETE(pGoods);
					//把此物品的物编号从冷却列表中删除
					if (pMainPlayer->GetGoodsAmount(lGoodsIndex)==0&&pMainPlayer->GetGoodsUseState(lGoodsIndex))
					{
						pMainPlayer->SetGoodsCoolDownStartTime((WORD)lGoodsIndex,0);
					}
				}
				if(i==0&&pGoods!=NULL)
				{
					GetGame()->GetAudioList()->PlayEquipmentSound(pGoods->GetProperty()->dwIndex);
				}
				// 物品获取时的特效
				//if (pGoods!=NULL&&pGoods->GetProperty()!=NULL)
				//{
				//	pItemPage->AddPickGoodsEffect(pGoods->GetIndex());
				//}

				//更新快捷栏上的显示
				//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
				//if (pPage!=NULL)
				//{
				//	DWORD dwGoodsIndex = 0;
				//	for (int i = 0;i<24;i++)
				//	{
				//		dwGoodsIndex = pMainPlayer->GetHotKey(i);
				//		if (dwGoodsIndex!=0)
				//		{
				//			if (lGoodsIndex==dwGoodsIndex)
				//			{
				//				pPage->UpdataHotComp(i);
				//			}
				//		}
				//	}	
				//}
			}
			dwMoney = pMsg->GetDWord();
			dwYuanBao = pMsg->GetDWord();
			pMainPlayer->SetMoney(dwMoney);
			pMainPlayer->SetYuanBao(dwYuanBao);

			//if (pItemPage&&pItemPage->GetPackState())
			//{
			//	char strValue[32];
			//	sprintf(strValue,"%d",dwMoney);
			//	pItemPage->UpdataYBMoney("Gold",strValue);
			//	sprintf(strValue,"%d",dwYuanBao);
			//	pItemPage->UpdataYBMoney("Gem",strValue);

			playerShop.CloseShop();
			CMessage msg(MSG_C2S_PLAYERSHOP_LOOKGOODS);
			msg.Add(playerShop.GetSessionID());
			msg.Add(playerShop.GetMasterID());
			msg.Send();
		}
		break;

		// 给卖家
	case MSG_S2C_PLAYERSHOP_TOSELLER:
		{
			long lExtendID = -1;
			DWORD dwPos = 0;
			int iAddOrSub = -1;
			long lRetNum = -1;
			DWORD dwMoney = 0;
			DWORD dwYuanBao = 0;
			DWORD dwWeiMian = 0;
			CGUID guid;
			DWORD lGoodsIndex = 0;

			//所有物品的操作过程数
			DWORD dwOPNumber = pMsg->GetDWord();
			CGoods* pGoods = NULL;
			for (size_t i = 0;i<dwOPNumber;i++)
			{
				BYTE bType = pMsg->GetByte();

				if (bType==OT_NEW_OBJECT)
				{
					pGoods = new CGoods();			
					BYTE ByteData[102400];
					long lBytePos = 0;
					pMsg->GetEx(ByteData,102400);
					pGoods->DecordFromByteArray(ByteData,lBytePos,true);
					lRetNum = pGoods->GetNum();
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					pMainPlayer->AddGoods(pGoods,dwPos,lExtendID);
					lGoodsIndex = pGoods->GetIndex();
					//如果新增物品是消耗性物品，就要将它加入玩家的消耗物品列表中
					if (pGoods!=NULL&&pGoods->GetGoodsBaseType()==GBT_CONSUMABLE&&(lExtendID==PEI_PACKET||
						(lExtendID>=PEI_PACK1&&lExtendID<=PEI_PACK5)))
					{
						pMainPlayer->AddShortCutGoods(pGoods->GetIndex());
						for(int i = 0;i<PACK_NUM;i++)
						{
							list<CPlayer::tagGoods>* GoodsList = pMainPlayer->GetGoodsList(i);
							list<CPlayer::tagGoods>::iterator GoodsIter = GoodsList->begin();
							for(;GoodsIter!=GoodsList->end();GoodsIter++)
							{
								if(GoodsIter->pGoods->GetEffectVal(GAP_COOLDOWN,2)==pGoods->GetEffectVal(GAP_COOLDOWN,2))
								{
									pMainPlayer->AddShortCutGoods(GoodsIter->pGoods->GetIndex());
								}
							}
						}
					}
					// 物品获取时的特效
					//if (pGoods->GetProperty()!=NULL)
					//{
					//	pItemPage->AddPickGoodsEffect(pGoods->GetIndex());
					//}
				}
				else if (bType==OT_DELETE_OBJECT)
				{
					pMsg->GetGUID(guid);
					lRetNum = pMsg->GetLong();	
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					pGoods = pMainPlayer->GetGoodsByGUID(guid);
					if (pGoods!=NULL)
					{
						lGoodsIndex = pGoods->GetIndex();
						if(i==0)
						{
							GetGame()->GetAudioList()->PlayEquipmentSound(lGoodsIndex);
						}
					}
					pGoods = pMainPlayer->RemoveGoods(guid,guid,dwPos,lExtendID,lRetNum);
					SAFE_DELETE(pGoods);
				}
				else if (bType==OT_CHANGE_AMOUNT)
				{
					pMsg->GetGUID(guid);
					iAddOrSub = pMsg->GetByte();
					lRetNum = pMsg->GetLong();	
					lExtendID = pMsg->GetLong();
					dwPos = pMsg->GetDWord();
					pGoods = pMainPlayer->GetGoodsByGUID(guid);
					if (pGoods!=NULL)
					{
						lGoodsIndex = pGoods->GetIndex();
						if(i==0)
						{
							GetGame()->GetAudioList()->PlayEquipmentSound(lGoodsIndex);
						}
					}
					// 零是减少一是增加
					if (iAddOrSub==0)
					{
						pMainPlayer->ChangeObjectAmount(lExtendID,TYPE_GOODS,guid,pGoods->GetNum()-lRetNum);
					}else if (iAddOrSub==1)
					{
						pMainPlayer->ChangeObjectAmount(lExtendID,TYPE_GOODS,guid,pGoods->GetNum()+lRetNum);
					}
				}

				//更新快捷栏上的显示
				//CMainBarPageEx* pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
				//if (pPage!=NULL)
				//{
				//	DWORD dwGoodsIndex = 0;
				//	for (int i = 0;i<24;i++)
				//	{
				//		dwGoodsIndex = pMainPlayer->GetHotKey(i);
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
				if (pMainPlayer->GetGoodsAmount(lGoodsIndex)==0&&pMainPlayer->GetGoodsUseState(lGoodsIndex))
				{
					pMainPlayer->SetGoodsCoolDownStartTime((WORD)lGoodsIndex,0);
				}
				if (bType!=OT_NEW_OBJECT)
				{
					playerShop.UpdateMyShopList(guid,lGoodsIndex,lExtendID,dwPos,lRetNum,iAddOrSub);
				}				
			}
			dwMoney = pMsg->GetDWord();
			dwYuanBao = pMsg->GetDWord();
			pMainPlayer->SetMoney(dwMoney);
			pMainPlayer->SetYuanBao(dwYuanBao);

			//@todo 背包更新货币显示
			//if (pItemPage&&pItemPage->GetPackState())
			//{
			//	char strValue[32];
			//	sprintf(strValue,"%d",dwMoney);
			//	pItemPage->UpdataYBMoney("Gold",strValue);
			//	sprintf(strValue,"%d",dwYuanBao);
			//	pItemPage->UpdataYBMoney("Gem",strValue);
			//}

			dwMoney = pMsg->GetDWord();				//此次交易所得的金钱数
			dwYuanBao = pMsg->GetDWord();				//此次交易所得的元宝数
			dwWeiMian = pMsg->GetDWord();				//此次交易所得的位面精华数

			//pShopPage->UpdateIncomeGolds(dwMoney,dwWeiMian);	//即时更新卖家的收入
			FireUIEvent("PlayerShop", "UpdateMoneyInfo");
			if (playerShop.GetCurGoodsNum()==0)
			{
				FireUIEvent("PlayerShop", "Closed");
			}
		}
		break;

		//逛商店的玩家页面的刷新
	case MSG_S2C_PLAYERSHOP_UPDATE:
		{				
			if (playerShop.GetCurShopState()==PlayerShop::SHOPPING_SHOP)
			{
				playerShop.ClearPlayerShopList();
			}
			bool bIsThatGoods = false;
			BYTE ByteData[102400];
			long lBytePos = 0;
			pMsg->GetEx(ByteData,102400);
			ulong goodsNum = _GetDwordFromByteArray(ByteData,lBytePos);
			for(size_t i=0; i<goodsNum; ++i)
			{
				CGoods* pGoods = new CGoods();
				pGoods->DecordFromByteArray(ByteData, lBytePos, true);

				ulong tradeType = _GetDwordFromByteArray(ByteData,lBytePos);
				ulong priceType = _GetDwordFromByteArray(ByteData,lBytePos);
				ulong groupNum = _GetDwordFromByteArray(ByteData,lBytePos);
				ulong oneGroupNum = _GetDwordFromByteArray(ByteData,lBytePos);
				ulong price = _GetDwordFromByteArray(ByteData,lBytePos);

				//if (pDescPage!=NULL&&pDescPage->IsShow()&&pDescPage->GetShowGoods()!=NULL
				//	&&pDescPage->GetShowGoods()->GetExID()==pGoods->GetExID())
				//{
				//	bIsThatGoods = true;
				//}

				if (playerShop.GetCurShopState()==PlayerShop::SHOPPING_SHOP)
				{
					playerShop.AddPlayerShopGoods(pGoods,groupNum,priceType,price,tradeType,oneGroupNum);
				}
				else if (playerShop.GetCurShopState()==PlayerShop::OPEN_SHOP)
				{
					playerShop.UpdateSellGoodsNum(pGoods->GetExID(), pGoods->GetIndex(), groupNum);
					SAFE_DELETE(pGoods);
				}
			}

			FireUIEvent("PlayerShop", "UpdateOperBtn");
			FireUIEvent("PlayerShop", "UpdateMoneyInfo");
			playerShop.UpdataPlayerShopGoodsList();

			//if (pDescPage->IsShow())
			//{
			//	pDescPage->SetIsShow(bIsThatGoods);
			//}
		}
		break;

	//	/// 留言板中的设置商店名称、店主推荐、系统信息等
	//case MSG_S2C_PLAYERSHOP_MSGBOARDGET:
	//	{
	//		CGUID PlayerID,SessionID;
	//		pMsg->GetGUID(SessionID);
	//		pMsg->GetGUID(PlayerID);
	//		BYTE bInforType = pMsg->GetByte();
	//		CPlayer *pPlayer=(CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID);
	//		
	//		/// 设置商店名字
	//		if (bInforType&0x1)		
	//		{
	//			char strShopName[64] = "";
	//			pMsg->GetStr(strShopName,64);
	//			if (pPlayer!=NULL)
	//			{
	//				pPlayer->SetPlayerShopName(strShopName);
	//			}
	//			
	//			//if (pShopPage->GetSessionID()==SessionID&&pMainPlayer->GetExID()!=PlayerID)
	//			{
	//			//	pShopPage->SetShopName(strShopName);
	//			}
	//		}
	//		/// 设置店主推荐
	//		if (bInforType&0x2)
	//		{
	//			char strShopRecommend[256] = "";
	//			pMsg->GetStr(strShopRecommend,256);
	//			//if (pShopPage->GetSessionID()==SessionID&&pMainPlayer->GetExID()!=PlayerID)
	//			{
	//				//pShopPage->SetShopRecommend(strShopRecommend);
	//			}
	//		}
	//		/// 系统信息
	//		if (bInforType&0x4)
	//		{
	//			BYTE dwNum = pMsg->GetByte();
	//			for (BYTE count = 0;count<dwNum;count++)
	//			{
	//				char strChatAllInfor[512] = "";
	//				pMsg->GetStr(strChatAllInfor,512);
	//				//pShopPage->AddChatItem(SessionID,strChatAllInfor);
	//			}
	//		}
	//	}
	//	break;
	//	/// 留言板中的聊天内容
	//case MSG_S2C_PLAYERSHOP_MSGBOARDCHAT:
	//	{
	//		CGUID PlayerID,SessionID;
	//		long lPlayerPlug = 0;
	//		pMsg->GetGUID(SessionID);
	//		lPlayerPlug = pMsg->GetLong();
	//		pMsg->GetGUID(PlayerID);
	//		char strChatAllInfor[512] = "";
 //			pMsg->GetStr(strChatAllInfor,512);

	//		//pShopPage->AddChatItem(SessionID,strChatAllInfor,lPlayerPlug,PlayerID);
	//	}
	//	break;
	default:
		break;
	}
}