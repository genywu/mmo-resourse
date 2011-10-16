#include "stdafx.h"
#include "Message.h"
#include "clientRegion.h"
#include "player.h"
#include "Monster.h"
#include "../Goods/Goods.h"
#include "Build.h"
#include "Effect.h"
#include "CollectionGoods.h"
#include "Citygate.h"
#include "../../EngineExtend/WeatherManager.h"
#include "Skills/SkillXml.h"
#include "Skills/SkillAI.h"
#include "other/RegionList.h"
#include "PetSystem/Pet.h"
//#include "PetSystem/CFairyPet.h"
//#include "PetSystem/CFighterPet.h"
//#include "PetSystem/CSummonPet.h"
//#include "PetSystem/CTrap.h"
//#include "PetSystem/OrnaPet.h"
#include "../Other/FaceActionList.h"
#include "../GameClient/GameControl.h"
#include "Other/AudioList.h"
#include "MainPlayerHand.h"
#include "../GameClient/Game.h"
#include "Other/GlobeSetup.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*
* 功能: 响应其他消息
* 摘要: -
* 参数: pMsg - 服务器发送的消息
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.10.18 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中增加了对加入场景的对象进行是否Alpha渐变的处理;
*	2008.10.27 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中增加了采集物对象设定显示模型为同步显示(修正第一次进游戏,由于采集物可随机设定等级,而造成了无法显示模型问题);
*	2008.12.10 - lpf
*		在MSG_S2C_RGN_CHANGEWEATHER消息中,增加天气处理时的Billboard Y轴数据传入;
*	2009.02.20 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中,"是否已经存在该SHAPE"代码处理处,在重设MoveShape坐标时,寻找其身上特效11114,并同时更新它的坐标,修正这样刷新后,特效和采集物模型可能不在同一处的问题;
*	2009.03.03 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中,修改了新增物品模型等级设定,修正了第一次进入场景时,无法看见掉在地上的物品模型的等级效果;
*	2009.03.03 - lpf
*		在MSG_S2C_RGN_CHANGE消息中,增加了场景是否可以改变装备的属性设定;
*	2009.03.20 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中,增加了"该玩家处于贩卖状态则需要播放坐下动作"的处理;
*	2009.04.03 - lpf
*		在MSG_S2C_RGN_ADDSHAPE消息中,调整了"该玩家处于贩卖状态则需要播放坐下动作"的处理,因为该状态下,玩家已经处于摆摊状态,所以直接播放坐在地上的动作,而不通过仪态动作来播放坐下的过程;
*	2009.11.13 - lpf
*		在MSG_S2C_RGN_CHANGE消息中,增加了对当前场景是否为PK场景标志的读取,并对场景对象做了对应的设定;
*/
void CClientRegion::OnMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2C_RGN_ENTER:
		{			
		}
		break;

	case MSG_S2C_RGN_ADDSHAPE:
		{
			long lType = pMsg->GetLong();
			CGUID ID,guID;
			pMsg->GetGUID(ID);
#ifdef _RUNSTACKINFO_
			char str[1024] = "";
			_snprintf(str,1024,"Add Shape Object(Type:%d) Start",lType);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif

			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);			
            CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			CBaseObject* pObj = 0;
			// 是否已经存在该SHAPE
			pObj = FindChildObject(lType, ID);
			if(pObj)
			{
				/////////////////////////////////////////////////////////////start
				//test add or delete shape,(author:wangqiao)
				char pszTestInfo[1024]="";
				char pszGUID[64]="";
				pObj->GetExID().tostring(pszGUID);
				//_snprintf(pszTestInfo,1024,"when Add the Shape,it exist(type:%d,id:%s,name:%s)(mx:%d,my:%d)",
				//	pObj->GetType(),pszGUID,pObj->GetName(),
				//	GetGame()->GetMainPlayer()->GetTileX(),GetGame()->GetMainPlayer()->GetTileY());
				//PutStringToFile("AddDelShapeTest",pszTestInfo);
				////////////////////////////////////////////////////////////end
				bool isSelf = false;
				switch(lType)
				{
				case TYPE_PET:
					{
						pMsg->GetLong();				//重新接收宠物类型
						isSelf = (pMsg->GetChar()>0);	//重新接收是否是自己的
					}
					break;
				}

				pObj->DecodeFromDataBlock(setReadDB, isSelf);

				CShape* pShape = dynamic_cast<CShape*>(pObj);
				if(pShape)
				{
					pShape->SetIsDeleted(false);
					CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pShape);
					if(pMoveShape)
					{
						pMoveShape->SetDestXY(pMoveShape->GetPosX(),pMoveShape->GetPosY());
						CEffect * pEffect = pMoveShape->FindEffect(11114);
						if (pEffect != NULL)
							pEffect->SetPosXY(pMoveShape->GetPosX(), pMoveShape->GetPosY());
						if (pShape->GetType() == TYPE_PET)	//重新激活自己的宠物
						{
							CPet *pPet = dynamic_cast<CPet*>(pShape);
							if (isSelf)
							{
								pMainPlayer->SetPetFighting(pPet);
								pShape->GetAnimInfo()->GetActionInfo()->dwCurAction = MAKEFOURCC('B','O','R','N');
								pShape->GetAnimInfo()->GetActionInfo()->dwCurActionLoopCount = 0;
								pShape->GetAnimInfo()->GetActionInfo()->dwCurActionStartTime = GetCurTickCount();
								pShape->GetAnimInfo()->GetActionInfo()->bCurActionLooped = false;
								pPet->SetInBornState(true);
								pPet->EnterRegion(isSelf);
							}
						}
					}
				}
				break;
			}

			// 新加入
			CShape* pShape = NULL;
			char bSelf = 0;
			switch(lType)
			{
			case TYPE_PLAYER:
				pShape = new CPlayer;

				if (CGlobeSetup::GetSetup()->dwPlayerAlphaFadeTimeLength != 0)
				{
					pShape->SetIsNeedAlphaFade(true);
					pShape->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwPlayerAlphaFadeTimeLength);
					pShape->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
				}
				break;

			case TYPE_MONSTER:
				{
					pShape = new CMonster;

					if (CGlobeSetup::GetSetup()->dwMonsterAlphaFadeTimeLength != 0)
					{
						pShape->SetIsNeedAlphaFade(true);
						pShape->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwMonsterAlphaFadeTimeLength);
						pShape->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
					}
				}
				break;

			case TYPE_GOODS:
				pShape = new CGoods;
				bSelf = 0;
				pShape->SetExID(ID);
				if (CGlobeSetup::GetSetup()->dwGoodsAlphaFadeTimeLength != 0)
				{
					pShape->SetIsNeedAlphaFade(true);
					pShape->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwGoodsAlphaFadeTimeLength);
					pShape->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
				}
				break;

			case TYPE_BUILD:
				pShape = new CBuild;
				break;
			case TYPE_COLLECTION:
				{
					pShape = new CCollectionGoods;	

					if (CGlobeSetup::GetSetup()->dwCollectionAlphaFadeTimeLength != 0)
					{
						pShape->SetIsNeedAlphaFade(true);
						pShape->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwCollectionAlphaFadeTimeLength);
						pShape->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
					}
				}break;

			case TYPE_CITYGATE:
				{
					pShape = new CCityGate;

					if (CGlobeSetup::GetSetup()->dwCollectionAlphaFadeTimeLength != 0)
					{
						pShape->SetIsNeedAlphaFade(true);
						pShape->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwMonsterAlphaFadeTimeLength);
						pShape->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
					}
				}break;
			case TYPE_PET:    //AddBy: nikai  
				{
					long lType = setReadDB.GetLongFromByteArray();
					bSelf = setReadDB.GetCharFromByteArray();		//是否是自己的
					switch (lType)
					{
					case PET_TYPE_SERVANT:
						if (bSelf)
						{
							pShape = pMainPlayer->GetPet(ID);
                            pMainPlayer->SetPetFighting((CPet*)pShape);
						}
						else
							pShape = new CPet;
						break;
					default:
						break;
					}
				}
				break;
			}

			if( pShape )
			{
				pShape->SetAppearTime( GetGame()->GetAITick() );
				if(bSelf>0)
					pShape->DecodeFromDataBlock( setReadDB, true);
				else
					pShape->DecodeFromDataBlock( setReadDB, false);
				CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
				if (lType == TYPE_PET)   //AddBy:nikai
				{
					CPet *pPet = dynamic_cast<CPet*>(pShape);
					pShape->GetAnimInfo()->GetActionInfo()->dwCurAction = MAKEFOURCC('B','O','R','N');
					pShape->GetAnimInfo()->GetActionInfo()->bCurActionLooped = false;
					pPet->EnterRegion(bSelf>0);
				}

				/////////////////////////////////////////////////////////////start
				//test add or delete shape,(author:wangqiao)
				char pszTestInfo[1024]="";
				char pszGUID[64]="";
				pShape->GetExID().tostring(pszGUID);
				//_snprintf(pszTestInfo,1024,"Add the Shape Message(type:%d,id:%s,name:%s,x:%d,y:%d)(mx:%d,my:%d)",
				//	pShape->GetType(),pszGUID,pShape->GetName(),pShape->GetTileX(),pShape->GetTileY(),
				//	GetGame()->GetMainPlayer()->GetTileX(),GetGame()->GetMainPlayer()->GetTileY());
				//PutStringToFile("AddDelShapeTest",pszTestInfo);
				////////////////////////////////////////////////////////////end


				assert( pShape->GetPosX() >=0 && pShape->GetPosX() <=GetWidth() &&
					pShape->GetPosY() >=0 && pShape->GetPosY() <= GetHeight() );

				//pShape->SetDisplayModel();
				//pShape->SetDisplayModelGroup();

				AddObject( pShape );

				//--------------07.10.16刘科----------
				if(pShape->GetType()==TYPE_MONSTER)
				{
					CMonster* mShape = dynamic_cast<CMonster*>(pShape);
					if(mShape)
					{
						// 收集该NPC相关任务提示信息
						if(mShape->GeteNpcType() == NT_Normal)
						{
							pMainPlayer->CollectionQuestClewInfo(mShape);
						}
						mShape->SetBlock(mShape->GetTileX(), mShape->GetTileY(), mShape->GetBlockType());
					}
				}
				//------------------------------------

				// 如果是采集物就根据它是否是玩家可采的采集物加上特效
				if (pShape->GetType()==TYPE_COLLECTION)
				{
					CQuestQueue* pQueueList = pMainPlayer->GetRoleQueue();
					CCollectionGoods* pCollectGoods = dynamic_cast<CCollectionGoods*>(pShape);
					if (pMainPlayer!=NULL)
					{
						if (pCollectGoods->GetOccuID()==pMainPlayer->GetAssOccu())
						{
							pShape->AddEffect(11114,pShape->GetPosX(),pShape->GetPosY(),pShape->GetHeight(),true);
						}
						else if (pCollectGoods->GetOccuID()==4&&pQueueList!=NULL)
						{
							const vector<tagQuestGoods>* pQustList = pCollectGoods->GetRoleInfo();
							if (pQustList!=NULL)
							{
								vector<tagQuestGoods>::const_iterator QustIter = pQustList->begin();
								for (;QustIter!=pQustList->end();QustIter++)
								{
									if (pQueueList->GetRecordByID(QustIter->nQuestId)!=NULL)
									{
										pShape->AddEffect(11114,pShape->GetPosX(),pShape->GetPosY(),pShape->GetHeight(),true);
										break;
									}
								}
							}
						}
					}
				}

				if(pShape->GetType()==TYPE_MONSTER||pShape->GetType()==TYPE_PLAYER)
				{
					CMoveShape* mShape = dynamic_cast<CMoveShape*>(pShape);
					mShape->UseCurSkill();		// 解码后施放当前技能
				}
				if(pShape->GetType()==TYPE_GOODS)
				{
					CGoods* pTempGoods=dynamic_cast<CGoods*>(pShape);

					// 金钱的GraphicsID
					//pShape->m_cAnimInfo.GetActionInfo()->dwCurAction = MAKEFOURCC('g','r','n','d');
					if(pTempGoods->GetProperty() && pTempGoods->GetProperty()->dwType==GBT_MONEY)
					{						
						long lGraphicsID = CGoodsList::GetMoneyGraphics(pTempGoods->GetNum());
						pTempGoods->SetGraphicsID(lGraphicsID);
					}

					//初始化闪光特效
					pTempGoods->IniFlashEffect();
				}

				//设置状态，以免出现穿越时空的情况
				if( pShape->GetAction() == CShape::ACT_WALK || pShape->GetAction() == CShape::ACT_RUN )
				{
					pShape->SetAction(CShape::ACT_STAND);
				}

				if (pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER)
				{
					extern bool g_bForceUpdate;
					g_bForceUpdate = true;
					CPlayer *pPlayer=(CPlayer*)pShape;
					if(pPlayer->IsInDieAwayState())	// 如果处于死亡和假死状态		
					{
						pShape->SetAction(CShape::ACT_ALREADYDIED);
					}
					else if(pShape->GetType()!=TYPE_PLAYER||!pPlayer->IsEmotionShowing())
					{	
						pShape->SetAction(pShape->GetAction());
					}
				}

				if( pShape->GetType() == TYPE_PLAYER )
				{
					if(pMainPlayer->IsTeammate(pShape->GetExID()))
					{
						//GetGame()->GetCGuiEx()->GetTeamPageEx()->Set3DIconShape(pShape->GetExID(),pShape);
					}
					// 新加的PLAYER已被选中
					//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
					//if(pObjPage && pShape->GetExID() == pObjPage->GetObjGUID())
					//{
					//	GetGame()->GetGameControl()->SetSelectedShape(pShape);
					//}
					CPlayer* pPlayer = (CPlayer*)pShape;

					//判断是否处于争夺城战中,加一个一特效
					if(pPlayer->GetIsContendState() == true)
					{
						pPlayer->AddEffect(2000000,pPlayer->GetPosX(),pPlayer->GetPosY(),pPlayer->GetHeight(),true);
					}
					if(pPlayer->GetCityWarDiedState() == true)
					{
						pPlayer->AddEffect(2000001,pPlayer->GetPosX(),pPlayer->GetPosY(),pPlayer->GetHeight(),true);
					}

					pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON);
					pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WING);
					pPlayer->UpdateSpecialEquipment();					
					pPlayer->UpdateRideShow();

					//检查场景上宠物，并添加给各自的主人
					itBaseObject it;
					for (it = m_listObject.begin(); it != m_listObject.end(); ++it)
					{
						CBaseObject* pObject = (*it);
						if (pObject->GetType() == TYPE_PET)
						{
							CPet *pPet = (CPet*)pObject;
							if (pPet->GetMasterGUID() == pPlayer->GetExID())
							{
								pPet->EnterRegion(false);
							}
						}
					}

					// 该玩家处于贩卖状态则需要播放坐下动作
					if (pPlayer->IsPlayerShopOpened())
					{
						pPlayer->SetAction(801);
						pPlayer->GetAnimInfo()->GetActionInfo()->bInterpolation = FALSE;
						pPlayer->GetAnimInfo()->m_tActionInfo.bCurActionLooped  = TRUE;
					}
				}

				if(pShape->GetType() == TYPE_CITYGATE)
				{
					if(pShape->GetState()==CShape::STATE_PEACE||pShape->GetAction()==CShape::ACT_OPEN)
					{
						pShape->SetAction(CShape::ACT_ALREADYOPEN);
					}else if(pShape->GetState()==CShape::STATE_FIGHT||pShape->GetAction()==CShape::ACT_IDLE)
					{
						pShape->SetAction(CShape::ACT_STAND);
					}
				}

				pShape->OnActionChange();

				if (pShape->GetType() == TYPE_COLLECTION)
				{
					pShape->SetDisplayModel(false);
					pShape->SetDisplayModelGroup();
				}else if (pShape->GetType() == TYPE_GOODS)
				{
					//add by 彭谦:场景增加物品的时候显示等级信息,这里结构不是很好
					//			  现在暂时只能这样加,望增加物品这块代码整理的时候
					//            能把同类别的物品(例如GOODS)的操作统一到一起...
					CGoods* pTempGoods=dynamic_cast<CGoods*>(pShape);
					pTempGoods->SetDisplayModel(false);
					if (pTempGoods->GetDisplayModel())
						pTempGoods->GetDisplayModel()->ShowAllGroupbyLevel(pTempGoods->GetEffectVal(GAP_WEAPON_LEVEL, 1), false);
				}else 
				{
					pShape->SetDisplayModel(); 
					pShape->SetDisplayModelGroup();
				}
			}
			char bGMCreate = pMsg->GetChar();	// 是否是GM制造的

#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Add Shape Object End");
#endif
		}
		break;

	case MSG_S2C_RGN_DELSHAPE:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);

			//跨服务器移动产生的特效(移动前，别人)
			long lUseGoods = pMsg->GetLong();
			if (lUseGoods == 1 && lType == TYPE_PLAYER && ID != GetGame()->GetMainPlayer()->GetExID() ) 
			{
#pragma message("-------- 这添加跨服务器移动产生的特效 --------")
			}

			// 因为服务器在摆摊的玩家下线时不对周围玩家发送消息，所以只能客户端在这里自己判断下并关闭打开的商店页面
			//CPlayerShopPageEx* pPlayerShopPage = GetGame()->GetCGuiEx()->GetPlayerShopPageEx();
			//if (pPlayerShopPage!=NULL&&pPlayerShopPage->IsOpen()
			//	&&lType==TYPE_PLAYER&&ID==pPlayerShopPage->GetMasterID())
			//{
			//	pPlayerShopPage->Close();
			//}

			CShape* pShape = (CShape*)FindChildObject(lType, ID);
			if( pShape)
			{
				/////////////////////////////////////////////////////////////start
				//test add or delete shape,(author:wangqiao)
				char pszTestInfo[1024]="";
				char pszGUID[64]="";
				pShape->GetExID().tostring(pszGUID);
				//_snprintf(pszTestInfo,1024,"Delete the Shape Message(type:%d,id:%s,name:%s,x:%d,y:%d)(mx:%d,my:%d)",
				///	pShape->GetType(),pszGUID,pShape->GetName(),pShape->GetTileX(),pShape->GetTileY(),
				//	GetGame()->GetMainPlayer()->GetTileX(),GetGame()->GetMainPlayer()->GetTileY());
				//PutStringToFile("AddDelShapeTest",pszTestInfo);
				////////////////////////////////////////////////////////////end
				//CLookPageEx *pPage = GetGame()->GetCGuiEx()->GetLookPageEx();
				//if(pShape==pPage->GetSelectPlayer()&&pPage->IsOpen()) //如果是正在查看对象
				//{
				//pPage->Close();  //关闭界面 
				//}
				// 删除阻挡
				if(lType == TYPE_MONSTER)
					pShape->SetBlock(pShape->GetTileX(),pShape-> GetTileY(), CRegion::BLOCK_NO);

				// 主玩家的宠物，场景列表中删除
				if (pShape->GetType() == TYPE_PET && ((CPet*)pShape)->IsMainPlayerPet())
				{
					RemoveObject(pShape);
				}
				else if( pShape != GetGame()->GetMainPlayer())
				{
					pShape->SetIsDeleted(true);
				}

				// 删除鼠标指向对象
				if( pShape==GetGame()->GetGameControl()->GetMouseShape() )
					GetGame()->GetGameControl()->SetMouseShape( NULL );
				// 删除当前被锁定的攻击目标
				if( pShape==GetGame()->GetGameControl()->GetCurAttractShape() )
					GetGame()->GetGameControl()->SetCurAttractShape( NULL );
				// 如果要删除的对象是当前选择的
                if ( pShape == GetGame()->GetGameControl()->GetSelectedShape() )
                {
                    FireUIEvent("ObjectPage", "Closed");
                }
				//删除宠物
				if(lType == TYPE_PET)
				{
					CPet	*pPet = (CPet*)pShape;
					CPlayer *pMaster = pPet->GetMaster();
					if (pMaster)
					{
						//删除前先保存技能设置
						pMaster->SavePetSkillSetting();
						//执行宠物删除操作
						//pMaster->RemovePet(pPet->GetExID());
						//效验当前宠物相关界面打开或关闭
						pMaster->ValidatePetInterface();
					}
				}
			}
			else
			{
				/////////////////////////////////////////////////////////////start
				//test add or delete shape,(author:wangqiao)
				char pszTestInfo[1024]="";
				char pszGUID[64]="";
				ID.tostring(pszGUID);
				//_snprintf(pszTestInfo,1024,"When delete the Shape Message,not exist.(type:%d,id:%s)(mx:%d,my:%d)",
				//	lType,pszGUID,
				//	GetGame()->GetMainPlayer()->GetTileX(),GetGame()->GetMainPlayer()->GetTileY());
				//PutStringToFile("AddDelShapeTest",pszTestInfo);
				////////////////////////////////////////////////////////////end
			}
		}
		break;

	case MSG_S2C_RGN_CHANGE:
		{
			long lType		= pMsg->GetLong();
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			long lUseGoods = pMsg->GetLong();

			//添加玩家离开的动画
			if (lUseGoods) 
			{
#pragma message("-------- 这添加玩家离开的动画 --------")
			}

			CPlayer * pPlayer = GetGame()->GetMainPlayer();
			if(!pPlayer)
				return;
			// 玩家自己
			if( pPlayer->GetExID() == PlayerID )
			{
				CGUID RegionID;
				pMsg->GetGUID(RegionID);
				long lRegionID = pMsg->GetLong();
				long lPlayerX	= pMsg->GetLong();
				long lPlayerY	= pMsg->GetLong();
				long lPlayerDir	= pMsg->GetLong();
				char strRegionName[ 256 ] = "";
				pMsg->GetStr( strRegionName ,256);
				long lTaxRate = pMsg->GetLong();
				eRWT eWarRegionType = (eRWT)pMsg->GetByte();//得到战斗场景类型
				eRST eSpaceType=(eRST)pMsg->GetByte();//是否是水下场景
				bool bChangeEquip = pMsg->GetByte()==0?false:true;
				long lResourceID = pMsg->GetLong();
				float fExpScale = pMsg->GetFloat();
				eRPKT ePKRegionType = (eRPKT)pMsg->GetByte();

				// 在组队中，更新队友信息
				//if(pPlayer->GetTeamID() != CGUID::GUID_INVALID)
				//	GetGame()->GetCGuiEx()->GetTeamPageEx()->Set3DIconShape();
				//GetGame()->GetCGuiEx()->GetCObjectPageEx()->Close();
                FireUIEvent("ObjectPage", "Closed");
				GetGame()->GetGameControl()->SetMouseShape( NULL );
				GetGame()->GetGameControl()->SetSelectedShape( NULL );
				GetGame()->GetGameControl()->SetCurAttractShape( NULL );
				GetGame()->GetGameControl()->SetSendedMoveMsgCount(0);
				GetGame()->GetGameControl()->SetSendedUseSkillMsgCount(0);
				//GetGame()->GetCGuiEx()->GetWorldMapPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetMailPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetReadMailPageEx()->Close();
				//CMessageBoxPageEx* pMessPage = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
				//if (pMessPage!=NULL&&pMessPage->IsOpen())
				//{
				//	pMessPage->Clear();
				//	pMessPage->Close();
				//}
				// 如果玩家背包（包括子背包）是打开的，此时要将背包全部关闭
				// 因为当玩家切换服务器时，服务器会将玩家身上的物品重新编码到客户端，此时如果背包是打开的，
				// 就会使背包中存放的物品地址（内存中的地址）错误，因此要关闭背包，当背包再次打开时就会刷新所有物品的地址。
				CMainPlayerHand* pMainPlayerHand = 0;//GetGame()->GetCGuiEx()->GetPMainPlayerHand();
				if (pMainPlayerHand!=NULL)
				{
					if (pMainPlayerHand->GetPGoodsOfMainPlayerHand()!=NULL)
					{
						pMainPlayerHand->GetPGoodsOfMainPlayerHand()->SetHaveShadowState(false);
					}
					pMainPlayerHand->ClearMainPlayerHand();
				}

				//CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
				//if (pItemPage!=NULL)
				//{
				//for (int i = 0;i<5;i++)
				//{
				//		pItemPage->ClearPackData(i);
				//		pItemPage->CloseSubPack(i);
				//	}
				//	pItemPage->Close();
				//}

				//GetGame()->GetCGuiEx()->GetPassWordBoxPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetTextBoxPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->Close();
				//CNPCShopPageEx* pNPCShop = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
				//if (pNPCShop!=NULL&&pNPCShop->IsOpen())
				//{
				//	pNPCShop->Close();
				//}
				//CCardMutifunPageEx* pMutiCardPage = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
				//if (pMutiCardPage!=NULL&&pMutiCardPage->IsOpen())
				//{
				//	pMutiCardPage->Close();
				//}

				assert( lPlayerX >=0 &&	lPlayerY >=0 );

				//////////////////////////获得分线信息////////////////////////////////////
				//long lDataSize = pMsg->GetLong();
				//if(lDataSize==0)
				//{
				//	LPBYTE	pData = new BYTE[ lDataSize ];
				//	pMsg->GetEx(pData,lDataSize);
				//	long lPos = 0;
				//	GetGame()->GetCGuiEx()->GetSelectLinePageEx()->DecordFromByteArray(pData,lPos);
				//	 //释放消息数据块
				//	SAFE_DELETE_ARRAY( pData );
				//}
				DBReadSet setReadDB;
				pMsg->GetDBReadSet(setReadDB);
				//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->SetCurRegionID(lRegionID);
				//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->DecodeFromDataBlock(setReadDB);
				//根据分线设置界面上的分线按钮
				//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->SetSelectBotton();


				// 王峰：关闭之前打开的界面
				//GetGame()->GetCGuiEx()->GetComposePageEx()->Close();
				//GetGame()->GetCGuiEx()->GetDecomposePageEx()->Close();
				//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->Close();
				//GetGame()->GetCGuiEx()->GetEquipMagicPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->Close();
				//GetGame()->GetCGuiEx()->GetDepotPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetFriendsPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetFactionPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetFactionListPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetUnionPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetTopMsgPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetLookPageEx()->Close();
				//GetGame()->GetCGuiEx()->GetMadalPageEx()->Close();

				//关闭怪物出现音效
				GetGame()->GetAudioList()->SetOppearSoundEnble(false);
				//}

				////如果和NPC交易框未关闭，则关闭
				//if(GetGameClient()->GetInterface()->
				//	FindWindow(GetGameClient()->GetInterface()->GetNPCTradePage()))
				//{
				//	CMessage msg(MSG_OTHER_SENDTALKBOXSELECT);
				//	msg.Add( (long)-1 );	// -1 表示退出 0-100表示选择的选项
				//	msg.Send();

				//	GetGameClient()->GetInterface()->GetNPCTradePage()->Repeat();
				//	GetGameClient()->GetInterface()->PopWindow(GetGameClient()->GetInterface()->GetNPCTradePage());
				//}

				////如果和玩家交易框未关闭，则关闭
				//if(GetGameClient()->GetInterface()->
				//	FindWindow(GetGameClient()->GetInterface()->GetPlayerTradePage()))
				//{
				//	CMessage msg( MSG_PLAYER_QUESTSWAPITEM_CANCEL );
				//	msg.Add( GetGameClient()->GetMainPlayer()->GetID() );
				//	msg.Send();
				//}

				////如果和玩家交易框未关闭，则关闭
				//if(GetGameClient()->GetInterface()->
				//	FindWindow(GetGameClient()->GetInterface()->GetDepotPage()))
				//{
				//	CMessage msg( MSG_OTHER_SENDTALKBOXSELECT );
				//	msg.Add( (long)-1 );	// -1 表示退出 0-100表示选择的选项
				//	msg.Send();

				//	GetGameClient()->GetInterface()->GetNPCTradePage()->Repeat();
				//	GetGameClient()->GetInterface()->PopWindow(GetGameClient()->GetInterface()->GetNPCTradePage());
				//}

				GetGame()->QuestEnterRegion(lRegionID,RegionID,lResourceID,strRegionName, lPlayerX, lPlayerY, lPlayerDir,lUseGoods);
				CClientRegion * pRegion = GetGame()->GetRegion();
				if (pRegion)
				{
					pRegion->SetTaxRate(lTaxRate);
					pRegion->SetWarType(eWarRegionType);
					pRegion->SetExpScale(fExpScale);
					pRegion->SetSpaceType(eSpaceType);
					pRegion->SetChangeEquip(bChangeEquip);
					pRegion->SetPKRegionType(ePKRegionType);

					//当主角成功进入场景
					pRegion->OnMainPlayerEnter();
					//GetGame()->GetCGuiEx()->OnPlayerMoving(1,1);
				}
				// **************************场景的据点相关信息（09.9.29刘科）*********************
				BYTE	bOpenFlag = pMsg->GetByte();
				long	lPointNum = pMsg->GetLong();
				char	szPointName[256] = "";
				if(pRegion)
				{
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetIsOpenSPoint(bOpenFlag>0);
					CClientRegion::tagSPointInfo stSPInfo;
					for ( long num = 0; num < lPointNum; ++num)
					{
						stSPInfo.strPointName = pMsg->GetStr(szPointName,256);
						stSPInfo.lPosX		= pMsg->GetLong();
						stSPInfo.lPosY		= pMsg->GetLong();
						stSPInfo.bState		= (BYTE)pMsg->GetLong();
						pRegion->AddSPointInfo(stSPInfo);
					}
				}
				// ********************************************************************************
			}
			else	// 其他玩家
			{
				CShape* pShape = (CShape*)FindChildObject( lType , PlayerID );
				if (pShape)
				{
					pShape->SetIsDeleted(true);
					//判断该玩家是否是当前正在查看的对象
					//CLookPageEx *pViwePage = GetGame()->GetCGuiEx()->GetLookPageEx();
					//if(pViwePage && pViwePage->GetSelectPlayer())
					//{
					//获得当前正在查看的playerID
					//	CGUID ViwePlayerID = pViwePage->GetSelectPlayer()->GetExID();
					//	if(PlayerID==ViwePlayerID)
					//GetGame()->GetCGuiEx()->GetLookPageEx()->Close();
					//}
				}
			}
			//// 更新坐标
			//CMainBarPageEx *pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			//if(pPage)
			//{
			//	pPage->UpdataCoordinate();
			//	pPage->UpdataPlaceName();
			//}
		}
		break;
		// 改变当前场景的名字
	case MSG_S2C_RGN_CHANGENAME:
		{
			long lRgnType = pMsg->GetLong();
			CGUID RgnID;
			pMsg->GetGUID(RgnID);
			long lRegionID = pMsg->GetLong();
			long lResourceID = pMsg->GetLong();
			char strRegionName[ 256 ] = "";
			pMsg->GetStr( strRegionName ,256);
			if(lRgnType == GetGame()->GetRegion()->GetRegionType()
				&& RgnID == GetGame()->GetRegion()->GetExID()
				&& lResourceID == GetGame()->GetRegion()->GetResourceID())
			{
				GetGame()->GetRegion()->SetName(strRegionName);
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdataPlaceName();
			}
		}
		break;
		// 更新场景中采集物的特效
	case MSG_S2C_RGN_UPDATEEFFECT:
		{
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			list<CBaseObject*>* pBaseObjectList = GetObjectList();
			list<CBaseObject*>::iterator ObjectIter = pBaseObjectList->begin();
			for (;ObjectIter!=pBaseObjectList->end();ObjectIter++)
			{
				if ((*ObjectIter)->GetType()==TYPE_COLLECTION)
				{
					CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
					CQuestQueue* pQueueList = pMainPlayer->GetRoleQueue();

					bool bPlayEffect = false;
					DWORD dwOccuID = ((CCollectionGoods*)(*ObjectIter))->GetOccuID();

					if (dwOccuID!=0&&dwOccuID==pMainPlayer->GetAssOccu())
					{
						bPlayEffect = true;
					}
					const vector<tagQuestGoods>* pQustList = ((CCollectionGoods*)(*ObjectIter))->GetRoleInfo();
					if (pQustList!=NULL)
					{
						vector<tagQuestGoods>::const_iterator QustIter = pQustList->begin();
						for (;QustIter!=pQustList->end();QustIter++)
						{
							if (QustIter->nQuestId!=-1&&QustIter->nQuestId!=0)
							{
								if ((((CCollectionGoods*)(*ObjectIter))->GetOccuID()==DOccu_Other&&pQueueList!=NULL&&
									pQueueList->GetRecordByID(QustIter->nQuestId)!=NULL))
								{
									bPlayEffect = true;
									break;
								}
							}
							else if (QustIter->nQuestId==-1)
							{
								bPlayEffect = true;
								break;
							}
						}
					}

					if (bPlayEffect)
					{
						((CShape*)(*ObjectIter))->AddEffect(11114,((CShape*)(*ObjectIter))->GetPosX(),((CShape*)(*ObjectIter))->GetPosY(),((CShape*)(*ObjectIter))->GetHeight(),true);
					}
					else
						((CShape*)(*ObjectIter))->DelEffect(11114);
				}
			}
		}
		break;

		// 切换服务器
	case MSG_S2C_RGN_CHANGESERVER:
		{
			//获取验证码
			long lChangeServerCode = pMsg->GetLong();
			GetGame()->SetChangeServerCode(lChangeServerCode);
			char strIP[256];
			pMsg->GetStr(strIP,256);
			DWORD dwPort = pMsg->GetDWord();

			CGUID PlayerID = GetGame()->GetMainPlayer()->GetExID();

			FireUIEvent("ObjectPage", "Closed");
			GetGame()->GetGameControl()->SetMouseShape( NULL );
			GetGame()->GetGameControl()->SetSelectedShape( NULL );
			GetGame()->GetGameControl()->SetCurAttractShape( NULL );
			GetGame()->GetGameControl()->SetSendedMoveMsgCount(0);
			//GetGame()->GetCGuiEx()->GetWorldMapPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetMailPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetReadMailPageEx()->Close();

			//CMessageBoxPageEx* pMessPage = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
			//if (pMessPage!=NULL&&pMessPage->IsOpen())
			//{
			//	pMessPage->Clear();
			//	pMessPage->Close();
			//}
			//重置反外挂系统
			GetGame()->ResetCheatSystem();
			// 如果玩家背包（包括子背包）是打开的，此时要将背包全部关闭
			// 因为当玩家切换服务器时，服务器会将玩家身上的物品重新编码到客户端，此时如果背包是打开的，
			// 就会使背包中存放的物品地址（内存中的地址）错误，因此要关闭背包，当背包再次打开时就会刷新所有物品的地址。
			CMainPlayerHand* pMainPlayerHand = 0;//GetGame()->GetCGuiEx()->GetPMainPlayerHand();
			if (pMainPlayerHand!=NULL)
			{
				if (pMainPlayerHand->GetPGoodsOfMainPlayerHand()!=NULL)
				{
					pMainPlayerHand->GetPGoodsOfMainPlayerHand()->SetHaveShadowState(false);
				}
				pMainPlayerHand->ClearMainPlayerHand();
			}
			/*CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
			if (pItemPage!=NULL)
			{
			for (int i = 0;i<5;i++)
			{
			pItemPage->ClearPackData(i);
			pItemPage->CloseSubPack(i);
			}
			pItemPage->Close();
			}
			CNPCShopPageEx* pNPCShop = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			if (pNPCShop!=NULL&&pNPCShop->IsOpen())
			{
			pNPCShop->Close();
			}
			CCardMutifunPageEx* pMutiCardPage = GetGame()->GetCGuiEx()->GetCardMutifunPageEx();
			if (pMutiCardPage!=NULL&&pMutiCardPage->IsOpen())
			{
			pMutiCardPage->Close();
			}*/

			// 王峰：关闭之前打开的界面
			//GetGame()->GetCGuiEx()->GetComposePageEx()->Close();
			//GetGame()->GetCGuiEx()->GetDecomposePageEx()->Close();
			//GetGame()->GetCGuiEx()->GetEquipUpgradePageEx()->Close();
			//GetGame()->GetCGuiEx()->GetEquipMagicPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetMagicDecomposePageEx()->Close();
			//GetGame()->GetCGuiEx()->GetDepotPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetFriendsPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetFactionListPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetUnionPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetTopMsgPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetLookPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetMadalPageEx()->Close();
			//GetGame()->GetCGuiEx()->GetCharacterPageEx()->Close();

			CClient* pTemptClient = GetGame()->GetNetClient();
			CClient* pNetClient = GetGame()->ConnectGameServer(strIP, dwPort,PlayerID);
			//连接GameServer成功
			if(pNetClient != NULL)
			{
				GetGame()->SetNetClient(pNetClient);
				CMessage::SetClient(pNetClient);

				//在模拟测试网络延时，不删除。避免多线程操作非法
				extern bool g_bDebug;
				//if( !g_bDebug )
				//{
				pTemptClient->Exit();
				pTemptClient->Close();
				SAFE_DELETE(pTemptClient);
				//}
			}
			else
			{
				GetGame()->SetNetClient(NULL);
				CMessage::SetClient(NULL);

				//连接GameServer失败
				CMessage msg(MSG_C2S_OTHER_CONNECTGAMESERVER);
				msg.Add((long)0);
				msg.Send(pTemptClient);

				//在模拟测试网络延时，不删除。避免多线程操作非法
				//extern bool g_bDebug;
				//if( !g_bDebug )
				//{
				pTemptClient->Close();
				SAFE_DELETE(pTemptClient);
				//}

				//GetGame()->GetInterface()->GetMsgBox()->Run("连接游戏服务器失败,请重新进入游戏!");
				GetGame()->SetGameState(GS_LOGIN);
			}
			//// 更新坐标
			//CMainBarPageEx *pPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			//if(pPage)
			//{
			//	pPage->UpdataCoordinate();
			//	pPage->UpdataPlaceName();
			//}
			//GetGame()->GetCGuiEx()->OnPlayerMoving(1,1);
		}
		break;

		// 改变天气
	case MSG_S2C_RGN_CHANGEWEATHER:
		{
			GetGame()->GetWeatherManager()->EnableRain(false);
			GetGame()->GetWeatherManager()->EnableSnow(false);
			GetGame()->GetWeatherManager()->EnableFog(false);
			GetGame()->GetWeatherManager()->EnableFlutter(false);
			//GetGame()->GetWeatherManager()->SetEnvFogColor(D3DXCOLOR(0.5, 0.5, 0.5, 1));

			long lNum = pMsg->GetLong();
			for (int i=0; i<lNum; i++)
			{
				long lWeatherIndex = pMsg->GetLong();
				DWORD dwFogColor = pMsg->GetDWord();

				char str[MAX_PATH];
				utility::File file;
				switch(lWeatherIndex/100)
				{
				case 1:
					{
						sprintf(str, "data/weather/rain/%d", lWeatherIndex);

						file.Open(str);
						char *buf = new char[file.GetLength()+1];
						memcpy(buf, file.GetData(), file.GetLength());
						buf[file.GetLength()] = 0;
						file.Close();

						GetGame()->GetWeatherManager()->ModifyRainParameter(buf);
						GetGame()->GetWeatherManager()->EnableRain();

						SAFE_DELETE(buf);
					}
					break;
				case 2:
					{
						sprintf(str, "data/weather/snow/%d", lWeatherIndex);

						file.Open(str);
						char *buf = new char[file.GetLength()+1];
						memcpy(buf, file.GetData(), file.GetLength());
						buf[file.GetLength()] = 0;
						file.Close();

						GetGame()->GetWeatherManager()->ModifySnowParameter(buf);
						GetGame()->GetWeatherManager()->EnableSnow();

						SAFE_DELETE(buf);
					}
					break;
				case 3:
					{
						sprintf(str, "data/weather/fog/%d", lWeatherIndex);

						file.Open(str);
						char *buf = new char[file.GetLength()+1];
						memcpy(buf, file.GetData(), file.GetLength());
						buf[file.GetLength()] = 0;
						file.Close();

						GetGame()->GetWeatherManager()->ModifyFogParameter(buf);
						GetGame()->GetWeatherManager()->EnableFog();

						SAFE_DELETE(buf);
					}
					break;
				case 4:
					{
						//sprintf(str, "data/weather/envfog/%d", lWeatherIndex);

						//file.Open(str);
						//char *buf = new char[file.GetLength()+1];
						//memcpy(buf, file.GetData(), file.GetLength());
						//buf[file.GetLength()] = 0;
						//file.Close();

						//GetGame()->GetWeatherManager()->ModifyEnvFogParameter(buf);
						////GetGame()->GetWeatherManager()->EnableEnvFog();

						//SAFE_DELETE(buf);

						//GetGame()->GetWeatherManager()->SetEnvFogColor(dwFogColor);
						//GetGame()->UpdateSetup();
					}
					break;
				case 5:
					{
						sprintf(str, "data/weather/Flutter/%d", lWeatherIndex);

						file.Open(str);
						char * buf = new char[file.GetLength() + 1];
						memcpy(buf, file.GetData(), file.GetLength());
						buf[file.GetLength()] = 0;
						file.Close();

						GetGame()->GetWeatherManager()->ModifyFlutterParameter(buf);

						D3DXMATRIX matBillboard, matBillboardY, matWorld;
						D3DXVECTOR3 * vView = GetGame()->GetRegion()->GetCamera()->GetView();
						matBillboard = *GetGame()->GetRegion()->GetCamera()->GetBillboard();
						matBillboardY = *GetGame()->GetRegion()->GetCamera()->GetBillboardY();
						D3DXMatrixTranslation(&matWorld, vView->x, vView->y, vView->z);

						GetGame()->GetWeatherManager()->SetWorldMatrix(&matWorld);
						GetGame()->GetWeatherManager()->SetBillboard(&matBillboard, &matBillboardY);
						GetGame()->GetWeatherManager()->EnableFlutter();

						SAFE_DELETE(buf);
					}
					break;
				}
			}
		}
		break;

		// 播放动作
	case MSG_S2C_RGN_PLAYACTION:
		{
			long lType	= pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);

			CShape* pShape = (CShape*)FindChildObject(lType, ID);
			if (pShape)
			{
				WORD wAction = pMsg->GetWord();
				pShape->SetAction(wAction);

				if (wAction>=CShape::ACT_ATTACK)
					pShape->SetState(CShape::STATE_FIGHT);
			}
		}
		break;

		// 播放声音
	case MSG_S2C_RGN_PLAYSOUND:
		{
			char strFile[MAX_PATH]="";
			pMsg->GetStr(strFile, MAX_PATH);
			GetGame()->GetAudioList()->Play(strFile, false, true);
		}
		break;

		// 播放3D声音
	case MSG_S2C_RGN_PLAY3DSOUND:
		{
			char strFile[MAX_PATH]="";
			pMsg->GetStr(strFile, MAX_PATH);
			long lLoop = pMsg->GetLong();    //循环次数
			long lPosX = pMsg->GetLong();    //坐标X
			long lPosY = pMsg->GetLong();    //坐标Y
			long lPosH = pMsg->GetLong();    //坐标Z
			if(lLoop==0)
			{
				GetGame()->GetAudioList()->Stop3DSound(strFile,-1);  //lLoop=0时停止播放该音效
			}else{
				GetGame()->GetAudioList()->Play3DSound(strFile,lLoop,(float)lPosY,(float)lPosH,(float)lPosX); 
			}
		}
		break;
		//播放音乐
	case MSG_S2C_RGN_PLAYMUSIC:
		{
			char strFile[MAX_PATH]="";
			pMsg->GetStr(strFile, MAX_PATH);
			long lLoop = pMsg->GetLong();    //循环次数
			//释放先前的音乐
			GetGame()->GetAudioList()->ReleaseMusic(GetGame()->GetAudioList()->GetBackgroundMusic());
			map<long,CGame::tagRegion>::iterator RegionDataIter=GetGame()->GetRegionSetup()->find(
				GetGame()->GetRegion()->GetResourceID());
			if(RegionDataIter!=GetGame()->GetRegionSetup()->end())
			{
				RegionDataIter->second.bgMusicPlaying=false;	
			}
			GetGame()->GetAudioList()->FadeInMusic(strFile,8000,lLoop); 
			GetGame()->GetAudioList()->SetBackgroundMusic(strFile);
		}
		break;
		//停止播放音乐
	case MSG_S2C_RGN_STOPMUSIC:
		{
			//释放先前的音乐
			GetGame()->GetAudioList()->ReleaseMusic(GetGame()->GetAudioList()->GetBackgroundMusic());
			map<long,CGame::tagRegion>::iterator RegionDataIter=GetGame()->GetRegionSetup()->find(
				GetGame()->GetRegion()->GetResourceID());
			if(RegionDataIter!=GetGame()->GetRegionSetup()->end())
			{
				if(!RegionDataIter->second.bgMusicPlaying)
				{
					//新场景背景音乐渐入
					RegionDataIter->second.bgMusicPlaying=true;	
					const char *filename=RegionDataIter->second.strBgMusic.c_str();
					GetGame()->GetAudioList()->FadeInMusic(filename,8000,0);
					GetGame()->GetAudioList()->SetBackgroundMusic(filename);
				}
			}
		}
		break;

		// 播放特效
	case MSG_S2C_RGN_PLAYEFFECT:
		{
			BYTE bInRegion = pMsg->GetByte();
			long lEffectID = pMsg->GetLong();
			long lType = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);

			float fx = pMsg->GetFloat();
			float fy = pMsg->GetFloat();
			long lPos = pMsg->GetLong();
			if(bInRegion)
			{
				PlayEffect(lEffectID, fx, fy,0.f,-1.0f,false);
			}
			else
			{
				CShape* pShape = (CShape*)FindChildObject(lType,guid);
				if (pShape)
				{
					CShape::eLOCATOR ePos = CShape::LOCATOR_FOOTCENTER;
					switch(lPos)
					{
					case 0: ePos = CShape::LOCATOR_WIND; break;
					case 1: ePos = CShape::LOCATOR_HEAD; break;
					case 2: ePos = CShape::LOCATOR_FOOTCENTER; break;
					case 3: ePos = CShape::LOCATOR_LEFTHAND; break;
					case 4: ePos = CShape::LOCATOR_RIGHTHAND; break;
					case 5: ePos = CShape::LOCATOR_LEFTFOOT; break;
					case 6: ePos = CShape::LOCATOR_RIGHTFOOT; break;
					default: ePos = CShape::LOCATOR_FOOTCENTER; break;
					}
					pShape->AddEffectLocator(lEffectID,pShape,ePos,false,0.f,0,false);
				}
			}
		}
		break;
	case MSG_S2C_RGN_ANSWER_CREATE:
		{
			long rgnType = pMsg->GetLong();
			long templateRgnID = pMsg->GetLong();
			long lResourceID = pMsg->GetLong();
			CGUID rgnID,ownerID;
			pMsg->GetGUID(rgnID);
			pMsg->GetGUID(ownerID);
		}
		break;
	case MSG_S2C_RGN_ANSWER_DELETE:
		{

		}
		break;
		// 改变据点状态
	case MSG_S2C_RGN_STRONGPOINT_STATE_CHANGE:
		{
			string	strName = "";
			char	szName[256] = "";
			long	lState = 0;
			long lRgnID = pMsg->GetLong();
			CGUID rgnID;
			pMsg->GetGUID(rgnID);
			if (rgnID == GetExID())
			{
				strName = pMsg->GetStr(szName,256);
				lState	= pMsg->GetLong();
				ChangeSPointState(strName.c_str(),(BYTE)lState);
			}
		}
		break;
		//case MSG_S2C_RGN_CLEARDUPRGNPLAYER:
		//	{
		//		//副本即将关闭通知
		//		BYTE bType = pMsg->GetByte();
		//		DWORD dwTime = pMsg->GetDWord();
		//		CTimeTextPageEx *pPage =  GetGame()->GetCGuiEx()->GetTimeTextPageEx();
		//        if(pPage==NULL)
		//			return;
		//		pPage->SetBoxType(CTimeTextPageEx::TIME_TEXTBOX);
		//		if(bType==RGN_CLEARDUPRGNPLAYER)
		//		{
		//              pPage->SetTimeBoxOpen(2,dwTime);
		//		}else if(bType==FCM_KICK_PLAYER)
		//		{
		//              pPage->SetTimeBoxOpen(3,dwTime);
		//		}


		//	}
		//	break;
	}
}
