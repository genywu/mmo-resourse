#include "stdafx.h"
#include "../../../Net/Message.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../ClientRegion.h"
#include "../MainPlayerHand.h"
#include "../../../GameClient/ClientApp/Skills/SkillListXml.h"
#include "../../../GameClient/ClientApp/Other/AudioList.h"
#include "../../../GameClient/ClientApp/Depot.h"
#include "../GameClient/Game.h"

#include "../../../UI/Windows/ShopCityDef.h"
#define	PROMPT_Power_lower_limit	20

/*
 * 功能: 响应Goods消息
 * 摘要: -
 * 参数: pMsg - 服务器发送的消息
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.04.01 - lpf
 *		在MSG_S2C_VIEWEQUIP消息中将GetHairPic()修改为SetHairPic();
 *	2008.04.01 - lpf
 *		在MSG_S2C_VIEWEQUIP消息中将pClonePlayer->SetDisplayHeadPiece(pPlayer->GetDisplayHeadPiece())修改为pClonePlayer->SetShowFashion(pPlayer->GetShowFashion())
 */
void OnGoodsMessage(CMessage *pMsg)
{
	switch(pMsg->GetType()) 
	{
	case MSG_S2C_PICKGOOD://玩家从场景中捡起一个物品的结果返回
		{
			CGUID PlayerGUID;
			pMsg->GetGUID(PlayerGUID);
			CPlayer* pPlayer = (CPlayer*)GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerGUID);
			if(pPlayer!=NULL&&PlayerGUID!=GetGame()->GetMainPlayer()->GetExID())
			{
				pPlayer->SetAction(CShape::ACT_PICK);
			}
		}
		break;
	case MSG_S2C_EXCHANGEHOTKEY://玩家交换热键的结果返回消息
		{
 			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();
			int extend = 0;
			int pos = 0;
			CPlayer* pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer && nIndex >= 0 && nIndex < 24)
			{
				pPlayer->SetIsCanSetHotKey(true);
				if(result==HOTKEY_SET_SUCCEED)
				{
					DWORD dwGoodIndex = pMsg->GetDWord();
					/////////////////////////////////////////////////
					// zhaohang  2010/5/19 
					// 恢复技能快捷键设置
					//CMainPlayerHand* pHand = 0;
					CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
					CGoods* pHandPGoods = pHand->GetPGoodsOfMainPlayerHand();
					////////////////////////////////
					if (pHandPGoods!=NULL)
					{
						extend = pHand->GetTGoodsOfMainPlayerHand()->lType;
						pos = pHand->GetTGoodsOfMainPlayerHand()->lPos;
						pHandPGoods->SetHaveShadowState(false);
						
					}
					///////////////////////////////////
					DWORD dwHotKey = pPlayer->GetHotKey(nIndex);
					if (dwHotKey!=0)
					{	
						//点击的快捷栏位有物品图标
						if ((dwHotKey & 0x80000000)==0)
						{
							if ((pHandPGoods!=NULL&&dwHotKey!=pHandPGoods->GetIndex())||pHand->GetSkillOfMainPlayerHand()!=0||pHand->GetGoodsIndexOfMainPlayerHand()!=0)
							{
								pHand->SetMainPlayerHandByGoodsIndex(dwHotKey,CMainPlayerHand::PlaceType_Fastfield);
							}else
							{
								pHand->ClearMainPlayerHand();
							}
						}
						//点击的快捷栏位有技能图标
						else
						{
							DWORD wSkillID = dwHotKey&0x7fffffff;
							CPlayer::tagSkill *pSkill = pPlayer->GetSkill(wSkillID);
							if(pSkill)
							{
								/***********************************************************************/
								/* zhaohang fix 2010-9-27*/
								/***********************************************************************/
								// 技能图标类型（灰度/正常）
								WORD wType = CPicList::PT_SKILL_ICON_GRAY;
								// 职业符合，使用彩色图标
								if(CSkillListXml::OccuJudgeStudy(wSkillID,pPlayer->GetOccupation()))
									wType = CPicList::PT_SKILL_ICON;
								//WORD wType = CPicList::PT_SKILL_ICON;
								pHand->SetMainPlayerHandBySkill(wSkillID,wType,CMainPlayerHand::PlaceType_Fastfield);
							}
							else
								pHand->SetMainPlayerHandBySkill(0);
						}		
					}
					else
					{
						pHand->ClearMainPlayerHand();
					}
					pPlayer->SetHotKey(nIndex,dwGoodIndex);
					
				}
			}
		}
		break;
	case MSG_S2C_CLEARHOTKEY://玩家清除一个热键设置的结果返回消息
		{
			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();
			if(result==HOTKEY_CLEAR_SUCCEED && nIndex>=0 && nIndex<24)
			{
				int extend = 0;
				int pos = 0;
				CPlayer* pPlayer = GetGame()->GetMainPlayer();
				CMainPlayerHand* pHand = 0;
				DWORD dwHotKey = pPlayer->GetHotKey(nIndex);
				if (pHand!=NULL&&dwHotKey!=0)
				{
					if ((dwHotKey & 0x80000000)==0)
					{
						pHand->SetMainPlayerHandByGoodsIndex(pPlayer->GetHotKey(nIndex),CMainPlayerHand::PlaceType_Fastfield);
					}
					else
					{
						DWORD wSkillID = dwHotKey&0x7fffffff;
						CPlayer::tagSkill *pSkill = pPlayer->GetSkill(wSkillID);
						if(pSkill)
						{
							/***********************************************************************/
							/* zhaohang fix */
							/***********************************************************************/
							// 技能图标类型（灰度/正常）
							WORD wType = CPicList::PT_SKILL_ICON_GRAY;
							// 职业符合，使用彩色图标
							if(CSkillListXml::OccuJudgeStudy(wSkillID,pPlayer->GetOccupation()))
								wType = CPicList::PT_SKILL_ICON;
							//WORD wType = CPicList::PT_SKILL_ICON;
							pHand->SetMainPlayerHandBySkill(wSkillID,wType,CMainPlayerHand::PlaceType_Fastfield);
						}
						else
							pHand->SetMainPlayerHandBySkill(0);
					}
				}
				pPlayer->SetHotKey(nIndex,0);
				
			}
		}
		break;
	case MSG_S2C_SETHOTKEY://玩家设置一个热键设置的结果返回消息
		{
			int result=pMsg->GetChar();
			int nIndex=pMsg->GetByte();
			DWORD dwVal=pMsg->GetDWord();
			if(nIndex>=0 && nIndex<24)
			{
				GetGame()->GetMainPlayer()->SetIsCanSetHotKey(true);
				GetGame()->GetMainPlayer()->SetHotKey(nIndex,dwVal);
			}
		}
		break;
	case MSG_S2C_DROPMONEY://玩家扔金钱的结果返回消息
		{
			int result=pMsg->GetChar();
			if(result==GOODS_DROP_SUCCEED)
			{
				DWORD lNum=pMsg->GetDWord();
				if(lNum>=1 && lNum<=GetGame()->GetMainPlayer()->GetMoney())
				{
					GetGame()->GetMainPlayer()->SetMoney(GetGame()->GetMainPlayer()->GetMoney() - lNum);
				}
			}
		}
		break;
	case MSG_S2C_PICKMONEY://玩家拾起金钱的结果返回消息
		{
			int result=pMsg->GetChar();
			if(result==GOODS_PICK_SUCCEED)
			{
				DWORD lNum=pMsg->GetDWord();//拾起的数量

				//金钱数量的
				if(lNum>=0 && GetGame()->GetMainPlayer()->GetMoney()+lNum<=MAX_MONEY)
				{
					GetGame()->GetMainPlayer()->SetMoney(GetGame()->GetMainPlayer()->GetMoney()+lNum);
				}
			}
		}
		break;
	case MSG_S2C_EQUIPDAMAGE://物品因耐久值完全损坏
		{
			int nPos=pMsg->GetByte();
			CGoods *pEquip=GetGame()->GetMainPlayer()->GetEquip(nPos);
			if(pEquip)
			{
				SAFE_DELETE(pEquip);
				GetGame()->GetMainPlayer()->SetEquip(nPos,NULL);
			}
		}
		break;
	case MSG_S2C_SET_YUANBAO:
		{
			DWORD dwBalance=pMsg->GetDWord();
			GetGame()->GetMainPlayer()->SetYuanBao(dwBalance);
			FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
		}
		break;
		// 设置点卷
	case MSG_S2C_SET_TICKET:
		{
			DWORD dwTicket=pMsg->GetDWord();
			GetGame()->GetMainPlayer()->SetJiFen(dwTicket);
			FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
		}
		break;
	case MSG_S2C_EQUIPDURUPDATE://物品耐久值的更新通知
		{
			int nPos=pMsg->GetByte();
			DWORD dwVal=pMsg->GetDWord();
			CGoods *pEquip=GetGame()->GetMainPlayer()->GetEquip(nPos);	
			
			if(pEquip)
			{
				long lMaxDurValue = pEquip->GetEffectVal(GAP_GOODS_MAX_DURABILITY,1);
				int nPercent = 100;
				if(lMaxDurValue >0)
					nPercent = dwVal*100/lMaxDurValue;
				
				pEquip->SetEffectVal(GAP_GOODS_MAX_DURABILITY,2,dwVal);
			}
		}
		break;
	case MSG_S2C_EQUIP_REFINE_UPDATE://物品炼度值的更新通知
		{
			BYTE nPos=pMsg->GetByte();
			DWORD dwVal=pMsg->GetDWord();
			CGoods *pEquip = GetGame()->GetMainPlayer()->GetEquip(nPos);	

			if(pEquip)
			{
				pEquip->SetEffectVal(GAP_REFINE,1,dwVal);

				// 物品炼度满时加提示信息
				long val1 = pEquip->GetEffectVal(GAP_REFINE,2);
				long val2 = pEquip->GetEffectVal(GAP_REFINE,1);
				if (val1==val2)
				{
					char str[64];
					if (pEquip->GetEquipType()==EC_E_RIDER)
					{
						sprintf(str,AppFrame::GetText("Goods_470"),pEquip->GetName());
					}else
						sprintf(str,AppFrame::GetText("Goods_215"),pEquip->GetName());
				}
			}
		}
		break;

	case MSG_S2C_DELETEEQUIP://删除一个特定位置的装备
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			int nPos=pMsg->GetByte();
			if(PlayerID == GetGame()->GetMainPlayer()->GetExID())
			{
				CPlayer* pPlayer = GetGame()->GetMainPlayer();
				CGoods *pEquip=pPlayer->GetEquip(nPos);
				if(pEquip)
				{
					SAFE_DELETE(pEquip);
					pPlayer->SetEquip(nPos,NULL);
				}
			}
			else
			{
				CPlayer* pPlayer = (CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER, PlayerID);
				if(pPlayer)
				{
					pPlayer->SetEquipIndex(nPos,0);
					pPlayer->SetEquipLevel(nPos,0);
				}
			}
		}
		break;

	case MSG_S2C_DELETEGOODS://删除背包中指定位置指定数量的物品
		{
			BYTE lPos=pMsg->GetByte();//要删除的物品的左上角位置
			BYTE lNum=pMsg->GetByte();//要删除数量
			list<CPlayer::tagGoods>::iterator GoodsIter=GetGame()->GetMainPlayer()->GetGoodsList()->begin();
			for(;GoodsIter!=GetGame()->GetMainPlayer()->GetGoodsList()->end();GoodsIter++)
			{
				if(GoodsIter->lPos==lPos)
				{
					int nXSize=1;//GoodsIter->pGoods->GetBaseProperty()->lX_Size;
					int nYSize=1;//GoodsIter->pGoods->GetBaseProperty()->lY_Size;
					int nTemPos=GoodsIter->lPos;
					for(int i=0;i<nYSize;i++)
					{
						/*for(int j=0;j<nXSize;j++)
						{
							CPlayer::tagGoods *pGoodsTag=GetGame()->GetInterface()->GetItemsPage()->GetGoodsData(nTemPos+j);
							if(pGoodsTag)
							{
								pGoodsTag->lNum-=lNum;
							}
						}*/
						nTemPos+=8;
					}
					GoodsIter->lNum-=lNum;
					break;
				}
			}
		}
		break;
	case MSG_S2C_DELETEALLGOODS://删除背包中指定位置的全部物品
		{
			BYTE lPos=pMsg->GetByte();
			list<CPlayer::tagGoods>::iterator GoodsIter=GetGame()->GetMainPlayer()->GetGoodsList()->begin();
			for(;GoodsIter!=GetGame()->GetMainPlayer()->GetGoodsList()->end();GoodsIter++)
			{
				if(GoodsIter->lPos==lPos)
				{
					//GetGame()->GetInterface()->GetItemsPage()->ClearGoodData(lPos,
					//	1/*GoodsIter->pGoods->GetBaseProperty()->lX_Size*/,
					//	1/*GoodsIter->pGoods->GetBaseProperty()->lY_Size*/);
					SAFE_DELETE(GoodsIter->pGoods);
					GetGame()->GetMainPlayer()->GetGoodsList()->erase(GoodsIter);
					break;
				}
			}
		}
		break;
	case MSG_S2C_VIEWEQUIP://gameserver返回查看玩家装备结果
		{
			BYTE lSize=0;
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			BYTE lHeadPic=pMsg->GetByte();
			BYTE lFacePic=pMsg->GetByte();

			lSize=pMsg->GetByte();
			CPlayer* pPlayer=(CPlayer*)(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID));
			if(pPlayer)
			{
				//该玩家装备了的物品的解码
				for(int i=0;i<lSize;i++)
				{
					BYTE EquipPos=pMsg->GetByte();
					CGoods* pEquip=new CGoods;
					if(pEquip)
					{
						BYTE ByteData[102400];
						pMsg->GetEx(ByteData,102400);
						long lPos = 0;
						pEquip->DecordFromByteArray(ByteData,lPos);
						//GetGame()->GetCGuiEx()->GetLookPageEx()->AddEquip(pEquip,EquipPos);
					}
				}
				BYTE lOccuLvl = pMsg->GetByte();               //当前职业等级
				BYTE lConst = pMsg->GetByte();                 //星座
				eOccupation eOccuSkillID = pPlayer->GetOccupation();   
				pPlayer->SetOccuLvl(eOccuSkillID,lOccuLvl);
			//	pPlayer->SetConst(lConst);
				//GetGame()->GetCGuiEx()->GetLookPageEx()->Open();
			}
		}
		break;
	case MSG_S2C_OPENGOODSUPGRADE://gameserver通知客户端打开道具升级界面
		{
			long lSessionID = pMsg->GetLong();
			long lPlugID = pMsg->GetLong();
			//GetGame()->GetInterface()->GetEquipUpgradePage()->SetSessionID(lSessionID, lPlugID);
			//GetGame()->GetInterface()->GetEquipUpgradePage()->Open();
		}
		break;	
	case MSG_S2C_GOODSUPGRADE_UPGRADE://物品升级的相关计算
		{
			CGoods* pUpgradeGoods=NULL/*GetGame()->GetInterface()->GetEquipUpgradePage()->GetGoods()*/;
			if(pUpgradeGoods)
			{
				DWORD dwSize=pMsg->GetDWord();
				long lPos=0;
				BYTE *pBytes=new BYTE[dwSize];
				pMsg->GetEx(pBytes,dwSize);
				pUpgradeGoods->DecordFromByteArray(pBytes,lPos);
				delete pBytes;
			}
		}
		break;
	case MSG_S2C_GOODS_UPDATEPROPERTY:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CGUID guGoodsID;
			pMsg->GetGUID(guGoodsID);

			CPlayer* pPlayer=(CPlayer*)(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,PlayerID));
			if(pPlayer)
			{
				CGoods* pGoods = pPlayer->GetGoodsByGUID(guGoodsID);
				if(pGoods)
				{
					long lDataSize = pMsg->GetLong();
					BYTE* pData = new BYTE[lDataSize];
					pMsg->GetEx(pData, lDataSize);
					long lPos = 0;
					pGoods->DecordFromByteArray(pData,lPos,false);
					delete [] pData;
				}
			}
		}
		break;
	case MSG_S2C_OPENGOODSENCHASE://打开卡片镶嵌功能页面
		{
			CGUID sessionID;
			BYTE bPageType = pMsg->GetByte();
			pMsg->GetGUID(sessionID);
			long playerID = pMsg->GetLong();
			/*CCardMutifunPageEx* pPage = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			if (pPage!=NULL&&!pPage->IsOpen())
			{
				if (pPage->GetNpcID()==CGUID::GUID_INVALID)
				{
					pPage->SetNpcID(GetGame()->GetLastTouchNpcID());
				}
				pPage->OpenPage(sessionID,playerID,bPageType);
			}*/
		}
		break;
	case MSG_S2C_ENCHASE_ARRANGE:
		{
			//CCardFunctionPageEx* pFunPage = GetGame()->GetCGuiEx()->GetCardFunctionPageEx();
			/*CCardMutifunPageEx*	 pMutiPage = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			if (pMutiPage!=NULL)
			{			
				BYTE bEnchaseOrRemove = pMsg->GetByte();  /// 判断是镶嵌还是拆卡操作后的刷新
				BYTE bCardNum = pMsg->GetByte();
				vector<CGUID> temp;
				CGUID guid;
				for (BYTE i = 0;i<bCardNum;i++)
				{
					pMsg->GetGUID(guid);
					temp.push_back(guid);
				}

				/// 如果是镶嵌操作就直接更新卡片组的GUID，如果是拆卡就比对卡片的GUID，并保留有效的卡片然后刷新界面显示
				pMutiPage->UpdateGoodsInfor(bEnchaseOrRemove,temp);
				pMutiPage->UpdatePage();			
			}*/
		}
        break;
	case MSG_S2C_ENCHASE_OPEN_HOLE:
		{
			BYTE holeNum = pMsg->GetByte();
			/*CCardMutifunPageEx* page = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			if (page!=NULL)
			{
				if (holeNum>=0&&holeNum<page->GetTargetGoods()->GetEffectVal(CGoods::GAP_HOLE_NUM,2))
				{
					page->StilettoGoods(holeNum+1);
					page->UpdatePage();
					GetGame()->GetCGuiEx()->GetEffectUIManager()->AddGroup( 108 );
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-01.wav");
				}
			}*/
		}
		break;
	case MSG_S2C_ENCHASE_CLOSE_HOLE:
		{
			BYTE holeNum = pMsg->GetByte();
			BYTE holeIndex = pMsg->GetByte();
			//CCardMutifunPageEx* page = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			//if (page!=NULL)
			//{
			//	if (holeNum>=0&&holeNum<page->GetTargetGoods()->GetEffectVal(CGoods::GAP_HOLE_NUM,2))
			//	{
			//		page->JamGoods(holeIndex,holeNum);
			//		page->UpdateAppointHole(holeIndex);
			//		//GetGame()->GetCGuiEx()->GetEffectUIManager()->AddGroup( 109 );
			//		GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-01.wav");
			//	}
			//}
		}
		break;
	case MSG_S2C_ENCHASE_ENCHASECARD:
		{
			//CCardMutifunPageEx* page = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			//if (page!=NULL)
			//{
			//	page->EnchaseCards();
			//	//page->UpdatePage();
			//	GetGame()->GetCGuiEx()->GetEffectUIManager()->AddGroup( 110 );
			//	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-10.wav"); 
			//}
		}
		break;
	case MSG_S2C_ENCHASE_REMOVECARD:
		{
			BYTE result = pMsg->GetByte();
			BYTE holeIndex = pMsg->GetByte();
			if (result!=0)
			{
				/*CCardMutifunPageEx* page = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
				if (page!=NULL)
				{
					page->RemoveCard(holeIndex);
					page->RemoveAssGoodsFromPage();
					page->UpdateAssGoodsShow();
					GetGame()->GetCGuiEx()->GetEffectUIManager()->AddGroup( 111 );
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-01.wav");
				}*/
			}else
			{
				//失败后的提示（还未处理）
			}
		}
		break;
	// 打开容器
	case MSG_S2C_GOODS_OPEN_CONTAINER:
		{
			char szText[4096];
			pMsg->GetStr(szText, 4096);

			//GetGame()->GetInterface()->GetContainerPage()->Open();
			//GetGame()->GetInterface()->GetContainerPage()->SetContainerType(szText);
		}
		break;


	// 王峰：处理窗口的打开操作
	case MSG_S2C_GOODS_OPEN_WINDOWS:
		{
			static long enumPreWndType = -1;
			//static	CGuiExPage* pPrePage = NULL;

			//
			long enumWndType		= pMsg->GetLong();
			long lPlayerMoveRange	= pMsg->GetLong();
			long lOpenedTileX		= pMsg->GetLong();
			long lOpenedTileY		= pMsg->GetLong();

			// 如果前后开启的窗口相同则忽略
			if( enumWndType == enumPreWndType )
			{
			}
			// 关闭上个窗口
			else
			{
				/*if( pPrePage )
					pPrePage->Close();*/
			}

			//
			switch( enumWndType )
			{
				// 分解
				case eGWT_DisassembleWindow:
					{
						//
						enumPreWndType = enumWndType;
						//pPrePage = GetGame()->GetCGuiEx()->GetDecomposePageEx();
						//GetGame()->GetCGuiEx()->GetDecomposePageEx()->Open();

						//GetGame()->GetCGuiEx()->GetDecomposePageEx()->
						//	SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );
					}
					break;
				// 普通合成
				case eGWT_PrescriptionWindow:
				// 魔化合成
//				case eGWT_EvilSynthesizeWindow:
				// 绑定
				//case eGWT_BindWindow:
				// 冻结
				//case eGWT_BindAndFrostWindow:
					{
						//
						enumPreWndType = enumWndType;
						//pPrePage = GetGame()->GetCGuiEx()->GetComposePageEx();

						////
						//GetGame()->GetCGuiEx()->GetComposePageEx()->Open();

						//GetGame()->GetCGuiEx()->GetComposePageEx()->
						//	SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );
					}
					break;
				// 强化
				case eGWT_UpgradeWindow:
					{
						//
						enumPreWndType = enumWndType;
						//pPrePage = GetGame()->GetCGuiEx()->GetEquipUpgradePageEx();

						////
						//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->Open();

						//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->
						//	SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );
					}
					break;
				// 魔化
				case eGWT_PlusEvilWindow:
					{
						/*if(GetGame()->GetCGuiEx()->GetEnchantEquipmentPageEx()->IsOpen())
						{
		                    GetGame()->GetCGuiEx()->GetEnchantEquipmentPageEx()->Close();
						}
			            GetGame()->GetCGuiEx()->GetEnchantEquipmentPageEx()->Open();
						
						GetGame()->GetCGuiEx()->GetEnchantEquipmentPageEx()->
							SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );*/
					}
					break;
				//重置魔化类型窗口
				case eGWT_ResetElementTypeWindow:
					{
						/*if(GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->IsOpen())
						{
						   GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->Close();
						}
			            GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->Open();
						GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->SetOpenType(0);
						GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->
							SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );*/
					}
					break;
				//! 重置魔化相性窗口
				case eGWT_ResetElementXiangXingWindow:
					{
						/*if(GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->IsOpen())
						{
						   GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->Close();
						}
						GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->Open();
						GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->SetOpenType(1);
						GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->
							SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );*/
					}
					break;
				// 魔化拆解
				//case eGWT_EvilBackoutWindow:
				//	{
				//		//
				//		enumPreWndType = enumWndType;
				//		pPrePage = GetGame()->GetCGuiEx()->GetMagicDecomposePageEx();

				//		//
				//		GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->Open();

				//		GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->
				//			SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );
				//	}
				//	break;
				//// 仓库
				case eGWT_DepotWindow:
					{
						//
						//enumPreWndType = enumWndType;
						//pPrePage = GetGame()->GetCGuiEx()->GetDepotPageEx();

						////
						//GetGame()->GetCGuiEx()->GetDepotPageEx()->Open();

						//GetGame()->GetCGuiEx()->GetDepotPageEx()->
						//	SetPlayerMoveRange( lOpenedTileX, lOpenedTileY, lPlayerMoveRange );
					}
					break;


				default:
					break;
			}
		}
		break;

	// 王峰：Goods 分解的最终结果
	case MSG_S2C_GOODS_RE_DISASSEMBLE_DO:
		{
			//GetGame()->GetCGuiEx()->GetDecomposePageEx()->RecvDecomposeMsg( pMsg );
		}
		break;
	// 王峰：Goods 合成的结果
	case MSG_S2C_GOODS_RE_SYNTHESIZE_DO:
		{
			//GetGame()->GetCGuiEx()->GetComposePageEx()->RecvNormalComposeMsg( pMsg );
		}
		break;
	// 王峰：Goods 绑定的结果
	case MSG_S2C_GOODS_BIND:
		{
			//GetGame()->GetCGuiEx()->GetComposePageEx()->RecvBindMsg( pMsg );
		}
		break;
	// 王峰：Goods 冻结的结果
	case MSG_S2C_GOODS_FROST:
		{
			//GetGame()->GetCGuiEx()->GetComposePageEx()->RecvFreezeMsg( pMsg );
		}
		break;
	// 王峰：Goods 充能的结果
	case MSG_S2C_GOODS_RE_REINFORCE_POWER:
		{
			//CMainBarPageEx*	pMainBarPageEx = GetGame()->GetCGuiEx()->GetMainBarPageEx();

			//// 检测 成功失败
			//if( eGTC_Succeed == pMsg->GetLong() )
			//{
			//	// 相关 PLYAER 的GUID
			//	CGUID guidPlayer;
			//	pMsg->GetGUID( guidPlayer );

			//	// 冲能强度
			//	long lPower = pMsg->GetLong();

			//	// 如果是自己则刷新自己装备，并弹出成功提示
			//	if( guidPlayer == GetGame()->GetMainPlayer()->GetExID() )
			//	{
			//		if( CGoods* pgoodsEquip = GetGame()->GetMainPlayer()->GetEquip( CPlayer::EQUIP_WEAPON ) )
			//			pgoodsEquip->SetEffectVal( CGoods::GAP_POWER_INTENSITY, 1, lPower );

			//		// 提示
			//		GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 200 ), 0xff00ffff );
			//		//播放成功音效
			//		CPlayer *pMainPlayer=GetGame()->GetMainPlayer();
			//		GetGame()->GetAudioList()->Play3DSound("SOUNDS\\interfaces\\i-11.wav",1,pMainPlayer->GetPosY(),pMainPlayer->GetHeight(),pMainPlayer->GetPosX());
			//	} 
			//	// 调用彭谦的接口处理动画效果
			//	CPlayer *pPlayerPowerAdd = dynamic_cast<CPlayer*>( GetGame()->GetRegion()->FindChildObject( TYPE_PLAYER, guidPlayer ) );
			//	if( pPlayerPowerAdd )
			//	{
			//		pPlayerPowerAdd->PlayEnergyEffect( lPower );
			//	}
			//}
			//else
			//{
			//	// 处理错误
			//	switch( pMsg->GetLong() )
			//	{
			//		case eGTC_NoArmInHand:
			//			{
			//				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 201 ), 0xff00ffff );
			//			}
			//			break;

			//		case eGTC_NoReinforceArm:
			//			{
			//				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 202 ), 0xff00ffff );
			//			}
			//			break;

			//		case eGTC_LevelError:
			//			{
			//				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 203 ), 0xff00ffff );
			//			}
			//			break;

			//		case eGTC_ArmIsFull:
			//			{
			//				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 204 ), 0xff00ffff );
			//			}
			//			break;

			//		default:
			//			break;
			//	}
			//	//播放失败音效
			//	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
			//}
		}
		break;

	// 王峰：能量剩余点数提醒
	case MSG_S2C_GOODS_POWER_WARN:
		{
			// 相关 PLYAER 的GUID
			CGUID guidPlayer;
			pMsg->GetGUID( guidPlayer );

			// 冲能强度
			long lPower = pMsg->GetLong();

			// 更新自己的剩余能量
			long lLeavingPower = pMsg->GetLong();

			if( GetGame()->GetMainPlayer()->GetExID() == guidPlayer )
			{
				if( CGoods* pgoodsEquip = GetGame()->GetMainPlayer()->GetEquip( CPlayer::EQUIP_WEAPON ) )
					pgoodsEquip->SetEffectVal( GAP_CURR_POWER, 1, lLeavingPower );

				// 提示信息
				//CMainBarPageEx*	pMainBarPageEx = GetGame()->GetCGuiEx()->GetMainBarPageEx();

				// 能量用完提醒
				//if( lLeavingPower <= 0 )
				//{
				//	pMainBarPageEx->AddText( eNOTIFYPOS_CENTER, CStringReading::LoadText( 208, 213 ), 0xffff0000 );
				//}
				//// 能量即将用完提醒
				//else if( lLeavingPower <= PROMPT_Power_lower_limit )
				//{
				//	TCHAR	strPowerPrompt[256];
				//	wsprintf( strPowerPrompt, CStringReading::LoadText( 208, 205 ), lLeavingPower );

				//	pMainBarPageEx->AddOtherInfo( strPowerPrompt );
				//}
			}

			// 调用特效接口
			if( lLeavingPower > 0 )
			{
				CPlayer *pPlayerPowerAdd = dynamic_cast<CPlayer*>( GetGame()->GetRegion()->FindChildObject( TYPE_PLAYER, guidPlayer ) );
				if( pPlayerPowerAdd )
				{
					pPlayerPowerAdd->PlayEnergyEffect( lPower );
					GetGame()->GetAudioList()->Play3DSound("SOUNDS\\interfaces\\i-34.wav",1,pPlayerPowerAdd->GetPosY(),pPlayerPowerAdd->GetHeight(),pPlayerPowerAdd->GetPosX());
				}
			}
		}
		break;

	// 王峰：装备强化预览的结果
	case MSG_S2C_GOODS_RE_UPGRADE_VIEW:
		{
			//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->RecvPreviewMsg( pMsg );
		}
		break;
	// 王峰：装备强化的结果
	case MSG_S2C_GOODS_RE_UPGRADE_DO:
		{
			//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->RecvUpgradeMsg( pMsg );
		}
		break;
	// 王峰：装备魔化预览的结果
	//case MSG_S2C_GOODS_RE_PLUG_EVIL_VIEW:
	//	{
	//		GetGame()->GetCGuiEx()->GetEquipMagicPageEx()->RecvPreviewMsg( pMsg );
	//	}
	//	break;
	// 王峰：装备魔化的结果
	case MSG_S2C_GOODS_RE_PLUG_EVIL_DO:
		{
			//GetGame()->GetCGuiEx()->GetEnchantEquipmentPageEx()->RecvMagicMsg( pMsg ); 
		}
		break;
	// 王峰：装备魔化合成的结果
	//case MSG_S2C_GOODS_RE_STUFF_SYNTHESIZE:
	//	{
	//		GetGame()->GetCGuiEx()->GetComposePageEx()->RecvMagicComposeMsg( pMsg );
	//	}
	//	break;
	// 王峰：装备魔化拆解的预览
	//case MSG_S2C_GOODS_RE_EVIL_BACKOUT_VIEW:
	//	{
	//		GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->RecvPreviewMsg( pMsg );
	//	}
	//// 王峰：装备魔化拆解的结果
	//case MSG_S2C_GOODS_RE_EVIL_BACKOUT:
	//	{
	//		GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->RecvDecomposeMsg( pMsg );
	//	}
	//	break;

	// 王峰：打开卷轴制作
	case MSG_S2C_MAKE_BEGIN:
		{
			//GetGame()->GetCGuiEx()->GetMakeScrollPageEx()->RecvOpenMsg( pMsg );
		}
		break;
	// 王峰：卷轴制作结果
	case MSG_S2C_MAKE_RESULT:
		{
			//GetGame()->GetCGuiEx()->GetMakeScrollPageEx()->RecvMakeMsg( pMsg );
		}
		break;


	// 王峰：仓库 回复客户端密码输入的结果
	case MSG_S2C_DEPOT_RE_INPUT_PWD:
		{
			CDepot::Inst()->RecvEnterMsg( pMsg );
		}
		break;
	// 王峰：仓库 回复修改密码的结果
	case MSG_S2C_DEPOT_RE_CHANGE_PWD:
		{
			CDepot::Inst()->RecvSetPWMsg( pMsg );
		}
		break;
	// 王峰：仓库 购买子背包的结果
	case MSG_S2C_DEPOT_RE_BUY_SUBPACK:
		{
			CDepot::Inst()->RecvBuyItemMsg( pMsg );
		}
		break;
	// 王峰：仓库 冻结
	case MSG_S2C_DEPOT_FROSTED:
		{
			CDepot::Inst()->RecvFreezeMsg( pMsg );
		}
		break;
	// 王峰：仓库 关闭
	case MSG_S2C_DEPOT_RE_CLOSE:
		{
			//GetGame()->GetCGuiEx()->GetDepotPageEx()->Close();
		}
		break;

	// 王峰：通用物品操作容器 打开
	case MSG_S2C_GOODS_AllPurpose_Open:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvOpenMsg( pMsg );
		}
		break;

	// 王峰：通用物品操作容器 关闭
	case MSG_S2C_GOODS_AllPurpose_Close:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvCloseMsg( pMsg );
		}
		break;

	// 王峰：通用物品操作容器 回复放入物品
	case MSG_S2C_GOODS_AllPurpose_RePush:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvPushMsg( pMsg );
		}
		break;

	// 王峰：通用物品操作容器 回复取出物品
	case MSG_S2C_GOODS_AllPurpose_RePop:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvPopMsg( pMsg );
		}
		break;

	// 王峰：通用物品操作容器 更新说明文本
	case MSG_S2C_GOODS_AllPurpose_UpdateTxt:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvDescMsg( pMsg );
		}
		break;
		// 王峰：通用物品操作容器 更新金币
	case MSG_S2C_GOODS_AllPurpose_UpdateGold:
		{
			//GetGame()->GetCGuiEx()->GetGeneralGoodsPageEx()->RecvGoldMsg( pMsg );
		}
		break;

	case MSG_S2C_MEDAL_ADD_NOTIFY:
		{
			long GoodsIndex = pMsg->GetLong();
			//GetGame()->GetCGuiEx()->GetMadalPageEx()->AddMedal(GoodsIndex); 
			CPlayer *pPlayer=GetGame()->GetMainPlayer();
			if(NULL == pPlayer)
				  return;
			pPlayer->AddMedal(GoodsIndex); 
			//刷新勋章列表的显示
			//GetGame()->GetCGuiEx()->GetCharacterPageEx()->UpdateMedalList();
			//GetGame()->GetCGuiEx()->GetMadalPageEx()->OpenGetTip(GoodsIndex);  //勋章获得提示
		}
		break;
	case MSG_S2C_MEDAL_DEL_NOTIFY:
		{
			long GoodsIndex = pMsg->GetLong();
			CPlayer *pPlayer=GetGame()->GetMainPlayer();
			if(NULL == pPlayer)
			         return;
			pPlayer->DeleteMedal(GoodsIndex); 
			//刷新勋章列表的显示
			//GetGame()->GetCGuiEx()->GetCharacterPageEx()->UpdateMedalList(); 
		}
		break;
	case MSG_S2C_GOODS_UPDATE_SCRIPTDESC:      // AddBy X.Y 2009.9.18
		{
			CGUID GoodsGuid;
			char strActiveInfor[512] = "";
			pMsg->GetGUID(GoodsGuid);
			pMsg->GetStr(strActiveInfor,512);
			CPlayer *pPlayer=GetGame()->GetMainPlayer();
			CGoods*pGoods = pPlayer->GetGoodsByGUID(GoodsGuid);
			// 找场景里面的物品
			if( ! pGoods )
			{
				pGoods = dynamic_cast<CGoods*>( GetGame()->GetRegion()->FindChildObject( TYPE_GOODS, GoodsGuid ) );
			}
			if (pGoods!=NULL)
			{
				pGoods->SetManufacturerName(strActiveInfor);
			}
		}
		break;
	case MSG_C2S_GOODS_RE_RESET_ELEMENT_TYPE:
		{
			//GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->RecvMagicMsg( pMsg ); 
		}
		break;
	case MSG_C2S_GOODS_RE_RESET_XIANG_XING:
		{
			//GetGame()->GetCGuiEx()->GetChangeEnchantPageEx()->RecvMagicMsg( pMsg ); 
		}
		break;
	}
}