#include "stdafx.h"
#include "Message.h"
#include "ClientRegion.h"
#include "Player.h"
#include "Build.h"
#include "Citygate.h"
#include "Monster.h"
#include "CollectionGoods.h"
#include "../Goods/Goods.h"
#include "../GameClient/Game.h"
#include "Other/DeputyOccuSystemClient.h"
#include "PetSystem/Pet.h"
#include "../GameClient/GameControl.h"
#include "../MsgBoxSys/MsgEventDef.h"
#include "../MsgBoxSys/MsgEventManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void RelivePlayerSelf(MsgEvent& event,eMsgResult UserId);
/*
 * 功能: 响应其他消息
 * 摘要: -
 * 参数: pMsg - 服务器发送的消息
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.21 - lpf
 *		在MSG_S2C_SHAPE_RELIVE消息中增加了对象重新获得生命前渐出处理;
 *	2008.11.18 - lpf
 *		在MSG_S2C_SHAPE_CHANGEMOVEMODE消息中增加了在改变速率的后调用SetAction(),否则速率无法立即更新
 */
void CMoveShape::OnMessage(CMessage* pMsg)
{
	if(!pMsg->GetRegion())
		return;
	switch(pMsg->GetType())
	{
		// 改变玩家方向
	case MSG_S2C_SHAPE_CHANGEDIR:
		{
			BYTE lDir = pMsg->GetByte();
			float fDir = pMsg->GetFloat();
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);

			CMoveShape* pMoveShape =  dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, ID));
			if( pMoveShape )
			{
				if(pMoveShape->GetAction() == CShape::ACT_WALK  ||
					 pMoveShape->GetAction() == CShape::ACT_RUN )
				{
					pMoveShape->SetEndMoveTime(timeGetTime());
					pMoveShape->SetPosXY(pMoveShape->GetDestX(),pMoveShape->GetDestY());
					pMoveShape->SetAction(CShape::ACT_STAND);
				}
				pMoveShape->SetDirEx(fDir);
				// 判断是否是玩家,并处理场景相机位置
				if (GetGame()->GetMainPlayer()->GetExID() == ID)
					GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();
			}
		}
		break;
	case MSG_S2C_SHAPE_CHANGE_PROPERTIES:
		{
			//CCharacterPageEx* pCharacterPage = GetGame()->GetCGuiEx()->GetCharacterPageEx();
			//CItemPageEx*	  pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
			//CMainBarPageEx* pMainBarPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
			//CFacturePageEx* pFacturePage = GetGame()->GetCGuiEx()->GetFacturePageEx();
			//CNPCShopPageEx* pNPCpage = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
			long lType=pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			CShape *pShape = (CShape *)GetGame()->GetRegion()->FindChildObject(lType,ID);
			if (lType == TYPE_PLAYER)
			{
				CPlayer *m_pPlayer=(CPlayer *)pShape;
				if (m_pPlayer)
				{
					long lOldCollectExp = m_pPlayer->GetCollectSkillExp();
					//long lOldCollectLevel = m_pPlayer->GetCollectSkillLevel();
					m_pPlayer->UpdateProperty(pMsg);
					if (m_pPlayer->GetExID()==GetGame()->GetMainPlayer()->GetExID())
					{
						/*if (pCharacterPage!=NULL&&pCharacterPage->IsOpen())
						{
							pCharacterPage->UpdatePlayerInfor();	
						}
						if (pItemPage!=NULL&&pItemPage->IsOpen())
						{
							pItemPage->YBMoneyInit();
							pItemPage->UpdataAllItems();
							pItemPage->UpdataAllSubPack();
						}
						if (pMainBarPage!=NULL)
						{
							pMainBarPage->UpdatePlayerInforHar();
							pMainBarPage->UpdataPlayerExp();
                            pMainBarPage->UpdateSpExpBar();
						}*/
						long lNewCollectExp = m_pPlayer->GetCollectSkillExp();
						long lCollectLevel = m_pPlayer->GetCollectSkillLevel();
						long lCollectExp = 0;
						if (lNewCollectExp>lOldCollectExp)
						{
							lCollectExp = lNewCollectExp-lOldCollectExp;
						}
						//else if(lNewCollectExp<lOldCollectExp)
						//{
						//	lCollectExp = CDeputyOccuSystemClient::GetCollectionNeededExp(lCollectLevel)-lOldCollectExp+lNewCollectExp;
						//}
						if (lCollectExp>0)
						{
							char strExp[64];
                            sprintf(strExp,AppFrame::GetText("Deputy_24")   //您获得了%d采集经验
                                ,lCollectExp);
							//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddOtherInfo(strExp);
						}
						//if (pFacturePage!=NULL&&pFacturePage->IsOpen())
						//{
						//	DWORD dwFactureID = pFacturePage->GetSelectFactureItemsID();
						//	DWORD dwOldFactureExp = pFacturePage->GetFactureExpProgressbar()->GetCurValue();
						//	pFacturePage->UpdateFactureLevelExpProcessBar();
						//	if (dwOldFactureExp!=pFacturePage->GetFactureExpProgressbar()->GetCurValue())
						//	{
						//		const CFacture* pSelectFacture = CDeputyOccuSystemClient::GetFacture(dwFactureID);
						//		if (pSelectFacture!=NULL)
						//		{
						//			//制作经验等信息
						//			char strExp[64];
						//			DWORD dwExp = CDeputyOccuSystemClient::GetFactureExp((LONG)(m_pPlayer->GetFactureSkillLevel()-pSelectFacture->GetFactureLevel()));	
						//			if (dwExp>0)
						//			{
						//				sprintf(strExp,CStringReading::LoadText(205,23),dwExp);
						//				GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddOtherInfo(strExp);
						//			}
						//		}
						//	}
						//	pFacturePage->UpdateFactureTree();
						//	pFacturePage->SetSelectFactureID(dwFactureID);
						//	pFacturePage->UpdateMaterial();
						//}
						//设置商业技能按钮的显示
						//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetOccuButtonVisible();
      //                  long lOccType = (long)m_pPlayer->GetAssOccu();
						////对于新学习的商业技能，设置按钮闪烁
						//if(GetGame()->GetCGuiEx()->GetMainBarPageEx()->GetOuccType()!=lOccType)
						//{
						//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetOccuButtonSpecialState(true,lOccType);
						//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetOcuuType(lOccType);
						//}
					}

                    FireUIEvent("MainRole", "RoleAttr");
				}
			}
			else if (lType == TYPE_MONSTER)
			{
				CMonster* m_pMonster = (CMonster*)pShape;
				if (m_pMonster)
				{
					m_pMonster->UpdateProperty(pMsg);
				}
			}
			else if (lType == TYPE_BUILD||lType == TYPE_CITYGATE)
			{
				CCityGate* m_pCityGate = (CCityGate*)pShape;
				if (m_pCityGate)
				{
                    m_pCityGate->UpdateProperty(pMsg);
				}
			}
			else if (lType == TYPE_PET)
			{
				CPet* pet;
				if (pShape==NULL)
				{
					pet = GetGame()->GetMainPlayer()->GetPet(ID);
				}
				else
					pet = (CPet*)pShape;
				if (pet)
					pet->UpdateProperty(pMsg);
			}
			// 如果更新的Shape是当前选择的对象，更新信息
			//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
			//if(pObjPage && (pObjPage->GetObjShape()==pShape))
			//{
			//	pObjPage->SetObjectHpInfo();
			//	pObjPage->SetObjectPKCP();
			//}
            if ( pShape == GetGame()->GetGameControl()->GetSelectedShape() )
            {
                FireUIEvent("ObjectPage", "ObjectAttr");
            }
		}
		break;
	case MSG_S2C_SHAPE_SETPOS:	// 瞬移
		{
			long	  lType		  = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			float x = pMsg->GetFloat();
			float y = pMsg->GetFloat();
			long lUseGoods = pMsg->GetLong();
            static char Debugstring[256];
			CMoveShape* pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, ID));
			if( pShape )
			{	
				if (pShape->GetType() == TYPE_PET)
				{
					CPet *pPet = (CPet *)pShape;
					if (pPet->IsMainPlayerPet())
					{	
						pPet->ClearFindPath();
					}
				}
				if(pShape->GetTileX() != x || pShape->GetTileY() != y)
				{
					pShape->SetPosXY(x, y);
				}
				CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pShape);
				if (pMoveShape)
				{
					pMoveShape->SetDestXY(pShape->GetPosX(), pShape->GetPosY());
				}
				pShape->SetEndMoveTime(timeGetTime());
                sprintf_s<256>(Debugstring,"玩家:%s 瞬移到 X: %f Y:%f \n",pShape->GetName(),x,y);
                if (pShape == GetGame()->GetMainPlayer())
                {
                    // 判断是否是玩家,并处理场景相机位置和各个弹出界面是否关闭
                    GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();
                    // 更新坐标
                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdataCoordinate();
                    GetGame()->GetRegion()->GetRegionCamera()->SetSmooth(false);
                    GetGame()->GetRegion()->GetRegionCamera()->SetCamera(GetGame()->GetRegion()->GetCamera(),pShape->GetPosX(),pShape->GetPosY());
                    GetGame()->GetRegion()->GetRegionCamera()->SetSmooth(true);
                    //设置更新位置
                    GetGame()->GetGameControl()->SetUpdateXY(x,y);
                    //清除自动寻路的路径
                    GetGame()->GetGameControl()->GetFindPath()->GetPathList()->clear();

                    GetGame()->GetGameControl()->SetSendedMoveMsgCount(0);

                    // @todo 王晓轩
                    CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
                    CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
                    if (pPet)
                    {
                        pPet->UpdateTargetPos(pShape);
                    }
                    sprintf_s<256>(Debugstring,"你被瞬移到 X: %f Y:%f \n", x,y);
                }
                OutputDebugString(Debugstring);
                PutStrToFileSaveToDir("log","MoveDebug",Debugstring);
			}
		}
		break;
	case MSG_S2C_SHAPE_MOVE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);

			CMoveShape* pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_PLAYER, guid));
			if(!pShape)
			{
				pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_MONSTER, guid));
				if(!pShape)
					pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_PET, guid));
			}
			if(pShape)
			{
				long lType =pShape->GetType();
				//不是自己
				if(pShape != GetGame()->GetMainPlayer())
				{
					if (lType== TYPE_PET)
					{
						CPet *pPet =(CPet*)pShape;
						if (pPet->IsMainPlayerPet())
						{
							return;
						}
					}
                    //是否需要继续移动
                    BYTE bStopFlag = pMsg->GetByte();
                    if (bStopFlag)
                    {
                        //停止移动
                        float fStopPosX = pMsg->GetFloat();
                        float fStopPosY = pMsg->GetFloat();
                        pShape->SetPosXY(fStopPosX,fStopPosY);
                        pShape->SetDestXY(fStopPosX,fStopPosY);
                        pShape->SetAction(ACT_STAND);
                    } 
                    else//需要继续移动
                    {
                        float fDestX = pMsg->GetFloat();
                        float fDestY = pMsg->GetFloat();
                        float fx =  pShape->GetPosX();
                        float fy = pShape->GetPosY();

                        DWORD dwCurTime = GetCurTickCount();
                        if(pShape->GetAction() != CShape::ACT_WALK &&
                            pShape->GetAction() != CShape::ACT_RUN)
                        {
                            //如果现在的动作不是行走动作,重置上次的更新时间
                            pShape->SetLastUpdatePosTime(dwCurTime);
                        }
                        //如果目标位置相差太大,则修正位置。
                        /*if(abs(pShape->GetTileX() - fDestX) >=5 ||
                            abs(pShape->GetTileY() - fDestY) >= 5 )
                        {
                            pShape->SetPosXY(fDestX,fDestY);
                            pShape->SetDestXY(fDestX,fDestY);
                            pShape->SetAction(ACT_STAND);
                            break;
                        }*/

                        //如果在行走过程中				
                        WORD action = CShape::ACT_WALK;
                        if(pShape->GetIsRun())
                            action = CShape::ACT_RUN;
                        if(pShape->GetAction() != action)
                            pShape->SetAction(action);

                        float fRadianDir = pShape->GetLineDirF(fx,fy,fDestX,fDestY);
                        //设置新的方向
                        pShape->SetDirEx(fRadianDir);
                        pShape->SetDestXY(fDestX,fDestY);
                        //得出移动时间
                        float fTotalDistance = pShape->RealDistance(fDestX,fDestY);
                        float dwMoveTime = fTotalDistance/pShape->GetSpeed();
                        pShape->SetEndMoveTime(dwCurTime+(DWORD)dwMoveTime);
                        pShape->SetMoveOffset((fDestX-fx)/dwMoveTime, (fDestY-fy)/dwMoveTime);
                    }
				}
				else
				{
					long lCount = GetGame()->GetGameControl()->GetSendedMoveMsgCount();
					if(lCount > 0)
						GetGame()->GetGameControl()->SetSendedMoveMsgCount(lCount-1);
				}
			}
		}
		break;
	// 王峰：怪物脱离战斗效果
	case MSG_S2C_SHAPE_LEAVEFIGHT:
		{
			// Shape 的类型和GUID
			long typeShape = pMsg->GetLong();
			CGUID guidShape;
			pMsg->GetGUID( guidShape );

			// 目的坐标
			long	xDest = pMsg->GetLong();
			long	yDest = pMsg->GetLong();

			// 特效时间
			long	time = pMsg->GetLong();

			// 找到 怪物
			if( CMonster* pShape = dynamic_cast<CMonster*>( pMsg->GetRegion()->FindChildObject( typeShape, guidShape ) ) )
			{
				// 退出战斗原路渐隐返回
				pShape->RunLeaveFightAI( xDest, yDest, time );
			}
		}
		break;
	case MSG_S2C_SHAPE_STOPMOVE:
		{			
			//CGUID guid;
			//pMsg->GetGUID(guid);
			//CMoveShape* pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_PLAYER, guid));
			//if(!pShape)
			//{
			//	pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_MONSTER, guid));
			//	if(!pShape)
			//		pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(TYPE_PET, guid));
			//}
			//if(pShape == GetGame()->GetMainPlayer())
			//	break;
			//if(pShape)
			//{
			//	long lType = pShape->GetType();
			//	long lDestX = pMsg->GetShort();
			//	long lDestY = pMsg->GetShort();
			//	long lDir = pMsg->GetByte();

			//	float fDestX = lDestX+0.5f;
			//	float fDestY = lDestY+0.5f;

			//	bool bFlag = false;
			//	//如果在行走过程中
			//	if(pShape->GetAction() == CShape::ACT_RUN ||
			//		pShape->GetAction()== CShape::ACT_WALK)
			//	{	//如果目标不一样，修正位置
			//		if(pShape->GetDestX() != lDestX ||
			//			pShape->GetDestY() != lDestY )
			//		{
			//			pShape->SetPosXY(fDestX,fDestY);
			//			bFlag = true;
			//		}
			//	}
			//	else
			//	{
			//		if(pShape->GetTileX() != (long)fDestX || pShape->GetTileY() != (long)fDestY)
			//		{
			//			pShape->SetPosXY(fDestX,fDestY);
			//		}
			//		bFlag = true;
			//	}
			//	if(bFlag)
			//	{
			//		pShape->SetAction(CShape::ACT_STAND);					
			//		pShape->SetDir((CShape::eDIR)lDir);
			//		pShape->SetDestXY(fDestX,fDestY);
			//	}
			//}
		}
		break;
		//shape的行走速度发生变化
	case MSG_S2C_SHAPE_CHANGEMOVEMODE:
		{
			long lType = pMsg->GetLong();
			CGUID shapeGuid;
			pMsg->GetGUID(shapeGuid);
			char bRun = pMsg->GetChar();
			//最终速度
			float fSpeed = pMsg->GetFloat();
			float fRatio = pMsg->GetFloat();

			//变化比值
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(lType,shapeGuid));
			if(pMoveShape)
			{
				bool bOldRun = pMoveShape->GetIsRun();
				pMoveShape->SetIsRun(bRun==0?false:true);
				//设置行走速度
				pMoveShape->SetSpeed(fSpeed);
				pMoveShape->SetActionSpeedRate(ACT_WALK,fRatio);
				pMoveShape->SetActionSpeedRate(ACT_RUN,fRatio);

				extern bool g_bForceUpdate;
				g_bForceUpdate = true;
				pMoveShape->SetAction(pMoveShape->GetAction());
				if(pMoveShape == GetGame()->GetMainPlayer())
				{
					if(pMoveShape->GetIsRun() != bOldRun)
					{
						if(!bRun)
						{
							//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER
							//	,CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_CHANGETOWALKMODE),D3DCOLOR_ARGB(255,255,255,0));
						}
						else
						{
							//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER
							//	,CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_CHANGETORUNMODE),D3DCOLOR_ARGB(255,255,255,0));
						}
					}
					//得出移动时间
					float fTotalDistance = pMoveShape->RealDistance(pMoveShape->GetDestX(),pMoveShape->GetDestY());
					DWORD dwMoveTime = (DWORD)(fTotalDistance/pMoveShape->GetSpeed());
					DWORD dwCurTime = GetCurTickCount();
					pMoveShape->SetEndMoveTime(dwCurTime+dwMoveTime);
					if(pMoveShape->GetAction() != CShape::ACT_WALK &&
						pMoveShape->GetAction() != CShape::ACT_RUN)
					{
						//如果现在的动作不是行走动作,重置上次的更新时间
						pMoveShape->SetLastUpdatePosTime(dwCurTime);
					}
				}
			}
		}
		break;
	case MSG_S2C_SHAPE_ATK_DAMAGE:
		{
			// 消息顺序lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			long lTargType = pMsg->GetLong();			// 目标者类型
			CGUID TargetID;
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType,TargetID);
			if(pTargetShape)
			{
				tagAttackEvent st;
				st.cType = AE_DAMAGE;							// 减血伤害

				char cSize = pMsg->GetChar();
				for(int i = 0; i<cSize; i++)
				{
					tagLost stLost;
					stLost.cLostType = pMsg->GetChar();			// 伤害类型
					stLost.lLostPoint = pMsg->GetLong();		// 伤害点数
					st.vtLost.push_back(stLost);
					st.lTotalLostPoint += stLost.lLostPoint;	// 点数合计
				}
				st.lFinalHP = pMsg->GetDWord();					//被攻击方剩余的HP
				st.bBlastAttack = (pMsg->GetChar()==0)?false:true;// 是否爆击

				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else		// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 收到受伤指令
	case MSG_S2C_SHAPE_ATK_BREAK:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
			{
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();
				//如果是玩家，检查身上装备的耐久度
				//GetGame()->GetCGuiEx()->GetEquipEwearPageEx()->CheckEquip();
			}


			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape)
			{
				if(pTargetShape->IsDied())
					break;
				CGUID	MainPlayerGuid = GetGame()->GetMainPlayer()->GetExID();
				if (MainPlayerGuid == TargetID		// 自己受到伤害，宠物还击	
					|| MainPlayerGuid == ActorID )	// 自己发起攻击使目标受伤,宠物协同主人作战
				{
					// @todo 王晓轩
					CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
					CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
					if (pPet && pPet->GetPetFightMode() != PET_FIGHETYPE_DEFEND)
					{
						CMoveShape *pAtkShape = pPet->GetCurAttackShape();
						if (pAtkShape == NULL || (pAtkShape && pAtkShape->IsDied()))
						{
							pPet->SetIsRetaliateUponNextShape(true);
							pPet->PetContAttRecved();
							if(MainPlayerGuid == TargetID)	//宠物还击
							{
								CMoveShape *pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
								pPet->SetCurAttackShape(pActorShape);
								pPet->UpdateTargetPos(pActorShape);
							}
							else if (MainPlayerGuid == ActorID)	//宠物协同主人作战
							{
								pPet->SetCurAttackShape(pTargetShape);
								pPet->UpdateTargetPos(pTargetShape);
							}
						}
					}
				}
				
				if (pTargetShape->GetType() == TYPE_PET)
				{
					CPet *pPet = (CPet*)pTargetShape;
					if (pPet->GetCurAttackShape() == NULL)
					{
						pPet->SetIsRetaliateUponNextShape(true);
						pPet->PetContAttRecved();
					}
				}

				// 加入一个受伤的事件
				tagAttackEvent st;
				st.cType = AE_BREAK;			// 受伤
				st.Guidattracker = ActorID;
				char cSize = pMsg->GetChar();
				for(int i=0; i<cSize; i++)
				{
					tagLost stLost;
					stLost.cLostType = pMsg->GetChar();			// 伤害类型
					stLost.lLostPoint = pMsg->GetLong();		// 伤害点数
					st.vtLost.push_back(stLost);
					st.lTotalLostPoint += stLost.lLostPoint;	// 点数合计
				}

				st.lFinalHP = pMsg->GetDWord();					//被攻击方剩余的HP
				st.bBlastAttack = pMsg->GetChar()==0?false:true;// 是否爆击
				//解码技能类型
				st.dwSkillID = pMsg->GetLong();				// 技能编号
				st.bySkillLevel = pMsg->GetByte();				// 技能等级

				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;

		// 完全防御
	case MSG_S2C_SHAPE_ATK_FULLMISS:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();		// 攻击者的类型
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();		// 目标者类型
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_FULLMISS;
				st.bBlastAttack = pMsg->GetChar()==0?false:true;		// 是否为爆击
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 招架
	case MSG_S2C_SHAPE_ATK_PARRY:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_PARRY;				// 招架
				st.bBlastAttack = pMsg->GetChar()==0?false:true;		// 是否为爆击
				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 格档
	case MSG_S2C_SHAPE_ATK_BLOCK:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();			

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_BLOCK;				// 格档
				char cSize = pMsg->GetChar();
				for(int i=0; i<cSize; i++)
				{
					tagLost stLost;
					stLost.cLostType = pMsg->GetChar();		// 伤害类型
					stLost.lLostPoint = pMsg->GetLong();	// 伤害点数
					st.vtLost.push_back(stLost);
					st.lTotalLostPoint += stLost.lLostPoint;	// 点数合计
				}

				st.lFinalHP = pMsg->GetDWord();					//被攻击方剩余的HP
				st.bBlastAttack = pMsg->GetChar()==0?false:true;// 是否爆击
				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 抵抗
	case MSG_S2C_SHAPE_ATK_RESIST:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|bBlastAttack
			long lActorType = pMsg->GetLong();		// 攻击者的类型
			CGUID ActorID,TargetID;
			pMsg->GetGUID(ActorID);		// 攻击者ID
			long lTargType = pMsg->GetLong();		// 目标者类型
			pMsg->GetGUID(TargetID);;		// 目标者ID

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_RESIST;				// 抵抗
				st.bBlastAttack = pMsg->GetChar()==0?false:true;		// 是否为爆击
				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 反弹
	case MSG_S2C_SHAPE_ATK_REBOUND:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_REBOUND;				// 反弹
				char cSize = pMsg->GetChar();
				for(int i=0; i<cSize; i++)
				{
					tagLost stLost;
					stLost.cLostType = pMsg->GetChar();		// 伤害类型
					stLost.lLostPoint = pMsg->GetLong();	// 伤害点数
					st.vtLost.push_back(stLost);
					st.lTotalLostPoint += stLost.lLostPoint;	// 点数合计
				}
				st.lFinalHP = pMsg->GetDWord();					//被攻击方剩余的HP
				st.bBlastAttack = pMsg->GetChar()==0?false:true;// 是否爆击
				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
	case MSG_S2C_SHAPE_ATK_SUCK:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape )
			{
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_SUCK;					// 吸收
				char cSize = pMsg->GetChar();
				for(int i=0; i<cSize; i++)
				{
					tagLost stLost;
					stLost.cLostType = pMsg->GetChar();		// 伤害类型
					stLost.lLostPoint = pMsg->GetLong();	// 伤害点数
					st.vtLost.push_back(stLost);
					st.lTotalLostPoint += stLost.lLostPoint;	// 点数合计
				}

				st.lFinalHP = pMsg->GetDWord();					//被攻击方剩余的HP
				st.bBlastAttack = pMsg->GetChar()==0?false:true;// 是否爆击
				// 获取受伤的方向
				CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType, ActorID);
				if (pActorShape)
				{
					st.fHurtedDir = CShape::GetLineDirF(pActorShape->GetPosX(), pActorShape->GetPosY(), pTargetShape->GetPosX(), pTargetShape->GetPosY());
				}
				pTargetShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 受到死亡指令
	case MSG_S2C_SHAPE_ATK_DIED:
		{
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);

			// 判断是否是玩家,并处理场景相机位置

			if (GetGame()->GetMainPlayer()->GetExID() == TargetID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType,TargetID);
			if( pTargetShape )
			{
				// 加入一个死亡的事件
				tagAttackEvent st;
				st.Guidattracker = ActorID;
				st.cType = AE_DIED;
				st.lHitNum = -1;
				pTargetShape->GetAttackEventQueue()->push(st);

				// 如果是monster，播放死亡特效
				if (pTargetShape->GetType() == TYPE_MONSTER || pTargetShape->GetType() == TYPE_PET)
				{
					long lGraphicsID = 900000 + pTargetShape->GetGraphicsID();
					char strFile[MAX_PATH];
					sprintf(strFile, "model/effect/%d/model", lGraphicsID);
					bool bExist = GetGame()->IsFileExist(strFile);
					if (bExist)
					{
						GetGame()->GetRegion()->PlayEffect(lGraphicsID, pTargetShape->GetPosX(), pTargetShape->GetPosY());
					}
					// 死亡之后不能再对话
					((CMonster *)pTargetShape)->SetIsCanTalk(0);
				}
				else if(pTargetShape->GetType()==TYPE_BUILD || pTargetShape->GetType()==TYPE_CITYGATE)
				{
					long lGraphicsID = 1000000 + pTargetShape->GetGraphicsID();
					char strFile[MAX_PATH];
					sprintf(strFile, "model/effect/%d/model", lGraphicsID);
					bool bExist = GetGame()->IsFileExist(strFile);
					if (bExist)
					{
						GetGame()->GetRegion()->PlayEffect(lGraphicsID, pTargetShape->GetPosX(), pTargetShape->GetPosY());
					}
				}
				else if(pTargetShape==GetGame()->GetMainPlayer())	// 自己死亡
				{
					//如果玩家有激活宠物 则清除宠物状态
					CPet* pet = GetGame()->GetMainPlayer()->GetActivePet();
					if (pet)
						pet->OnHostDied();
					/***********************************************************************/
					/*  zhaohang 2010-10-12 简单加入复活功能 */
					/***********************************************************************/

					GetInst(MsgEventManager).PushEvent(Msg_YesNo,
						AppFrame::GetText("RELIVE_1"),RelivePlayerSelf);
				}
				else if (pTargetShape->GetType() == TYPE_PET && ((CPet*)pTargetShape)->IsMainPlayerPet())
				{
					((CPet*)pTargetShape)->OnDied();
					CPlayer *pMaster = ((CPet*)pTargetShape)->GetMaster();
					if(pMaster)
						//pMaster->RemovePet(pTargetShape->GetExID()); 
					{}
				}
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lTargType);
				msg.Add(TargetID);
				msg.Send();*/
			}
		}
		break;
		// 丢失
	case MSG_S2C_SHAPE_ATK_LOSE:
		{
			// 丢失者的ID和类型
			long lActorType = pMsg->GetLong();
			CGUID ActorID;
			pMsg->GetGUID(ActorID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == ActorID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pActorShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lActorType,ActorID);
			if( pActorShape )
			{
				// 加入一个丢失的事件
				tagAttackEvent st;
				st.cType = AE_LOST;
				pActorShape->GetAttackEventQueue()->push(st);
			}
			else	// 没有找到这个shape，发消息向服务器请求该shape的数据
			{
				/*CMessage msg(MSG_C2S_SHAPE_QUESTINFO);
				msg.Add(lActorType);
				msg.Add(ActorID);
				msg.Send();*/
			}
		}
		break;

		// 改变状态
	case MSG_S2C_SHAPE_CHANGESTATE:
		{
			long lType = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			long lState = pMsg->GetLong();

			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, guid) );
			if (pMoveShape)
			{
				if (lState==CShape::STATE_FIGHT)
				{
					if (pMoveShape == GetGame()->GetMainPlayer())
					{
						long PetNum = GetGame()->GetMainPlayer()->GetFightPetNum();
						if (PetNum)
						{
							//@todo 王晓轩
							CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
							CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
							if (pPet && pPet->GetCurAttackShape() == NULL)
							{
								pPet->SetIsRetaliateUponNextShape(true);
								pPet->PetContAttRecved();
							}
						}
					}
					if (pMoveShape->GetType() == TYPE_PET)
					{
						CPet *pPet = (CPet*)pMoveShape;
						if (pPet->GetCurAttackShape() == NULL)
						{
							pPet->SetIsRetaliateUponNextShape(true);
							pPet->PetContAttRecved();
						}
					}
				}
				if(pMoveShape->GetState()==lState)
					break;

				WORD wAct = pMoveShape->GetAction();

				// 从和平进入战斗状态
				if (lState==CShape::STATE_FIGHT)
				{
					if (wAct == CShape::ACT_IDLE)
						wAct = CShape::ACT_STAND;
				}
				// 从战斗进入和平状态
				else
				{
					// 恢复到站立状态
					if (wAct != CShape::ACT_WALK &&
						wAct != CShape::ACT_RUN &&
						wAct != CShape::ACT_DIED &&
						wAct != CShape::ACT_ALREADYDIED)
					{
						wAct = CShape::ACT_STAND;
					}
				}

				pMoveShape->SetState((WORD)lState);
				extern bool g_bForceUpdate;
				g_bForceUpdate = true;
				if (pMoveShape->GetType() == TYPE_PET)
				{
					CPet *pPet = (CPet*)pMoveShape;
					if (pPet->IsInBornState())
						return;
				}
				if( pMoveShape->GetState() != CShape::STATE_DIED)
				{
					pMoveShape->SetAction(wAct);
				}
			}
		}
		break;

		// 改变犯罪状态
	case MSG_S2C_SHAPE_SINSTATE:
		{
			CGUID ID;
			pMsg->GetGUID(ID);
			BYTE bState = pMsg->GetByte();

			CPlayer* pPlayer = (CPlayer*)pMsg->GetRegion()->FindChildObject(TYPE_PLAYER, ID);
			if (pPlayer)
			{
				pPlayer->SetSinState(bState!=0);
			}
		}
		break;

		// 强制移动
	case MSG_S2C_SHAPE_FORCEMOVE:
		{
			CGUID ID;
			pMsg->GetGUID(ID);
			long lType = pMsg->GetLong();
			CMoveShape* pShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lType, ID);
			if (pShape)
			{
				float fDestX = pMsg->GetLong()+0.5f;
				float fDestY = pMsg->GetLong()+0.5f;

				DWORD dwCurTime = GetCurTickCount();
				if(pShape->GetAction()==CShape::ACT_RUN ||
					pShape->GetAction()==CShape::ACT_WALK)
				{
					if((long)(pShape->GetDestX()) != pShape->GetTileX() ||
						(long)(pShape->GetDestY()) != pShape->GetTileY() )
					{
						pShape->SetPosXY(pShape->GetDestX(),pShape->GetDestY());
					}
				}
				else
				{
					//如果现在的动作不是行走动作,重置上次的更新时间
					pShape->SetLastUpdatePosTime(dwCurTime);
				}

				float fx =  pShape->GetPosX();
				float fy = pShape->GetPosY();

				float fRadianDir = pShape->GetLineDirF(fx,fy,fDestX,fDestY);
				//设置新的方向
				pShape->SetDirEx(fRadianDir);
				pShape->SetDestXY(fDestX,fDestY);
				//得出移动时间
				float fTotalDistance = pShape->RealDistance(fDestX,fDestY);
				DWORD dwMoveTime = DWORD(fTotalDistance/pShape->GetSpeed());
				pShape->SetEndMoveTime(dwCurTime+dwMoveTime);
				WORD action = CShape::ACT_WALK;
				if(pShape->GetIsRun())
					action = CShape::ACT_RUN;
				if(pShape->GetAction() != action)
					pShape->SetAction(action);
				pShape->SetInForceMove(true);
				if(pShape == GetGame()->GetMainPlayer())
				{
					//设置更新位置
					GetGame()->GetGameControl()->SetUpdateXY(fDestX,fDestY);
				}
			}
		}
		break;

	case MSG_S2C_SHAPE_Refresh:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			WORD wAction = pMsg->GetWord();
			DWORD dwMaxHp = pMsg->GetDWord();
			DWORD dwHp = pMsg->GetDWord();
			CMoveShape* pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, ID));
			if (pShape)
			{
				pShape->SetAction(wAction);
				//pShape->SetMaxHP(wMaxHp);
				pShape->SetHp(dwHp);
			}
		}
		break;

		// 表情
	case MSG_S2C_SHAPE_EMOTION:
		{
			long lType=pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			long lIndex=pMsg->GetLong();
			CMoveShape *pMoveShape=(CMoveShape*)GetGame()->GetRegion()->FindChildObject(lType,guid);
			if(pMoveShape)
			{
				//播放表情
				char strInfo[256]="";
                _snprintf(strInfo,256, AppFrame::GetText("Base_114")    //%s开始表情动作%d
                    ,pMoveShape->GetName(),lIndex);
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM, strInfo, 0xffffff00 );
			}
		}
		break;

		// 复活
	case MSG_S2C_SHAPE_RELIVE:
		{
			long lType = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);

			// 判断是否是玩家,并处理场景相机位置
			if (GetGame()->GetMainPlayer()->GetExID() == ID)
				GetGame()->GetRegion()->GetRegionCamera()->ResumeCamera();

			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, ID));
			if (pMoveShape)
			{
				WORD dwAction = pMsg->GetWord();
				WORD wState = pMsg->GetWord();

				if (pMoveShape->GetIsNeedAlphaFade())
				{
					GetGame()->GetRegion()->GetAlphaFadeObjectList().AddFadeOutObject((CShape *)pMoveShape);
					pMoveShape->SetIsAddAlphaFaded(true);
				}

				pMoveShape->SetState(wState);
				pMoveShape->SetAction(dwAction);
			}
		}
		break;
		// 设置怪物或采集物的任务保护信息
	case MSG_S2C_SHAPE_CHANGE_FIRSTATTACKER:
		{
			CGUID ShapID,CreatorID;
			long lShapType = 0,lCreatorType = 0;
			pMsg->GetGUID(ShapID);			// 任务保护对象
			lShapType = pMsg->GetLong();
			pMsg->GetGUID(CreatorID);	// 创建者ID
			lCreatorType = pMsg->GetLong();
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lShapType, ShapID));
			if (pMoveShape)
			{
				// 怪物
				if(pMoveShape->GetType() == TYPE_MONSTER)
				{
					((CMonster *)pMoveShape)->SetCreatorType(lCreatorType);
					((CMonster *)pMoveShape)->SetCreatorID(CreatorID);
					// 如果要删除的对象是当前选择的
					//CObjectPageEx *pObjPage = GetGame()->GetCGuiEx()->GetCObjectPageEx();
					//if(pObjPage)
					//{
					//	if(TYPE_MONSTER == pObjPage->GetObjType() && ShapID == pObjPage->GetObjGUID())
					//		pObjPage->UpdataObjInfo();
					//}
				}
				// 收集物
				else if(pMoveShape->GetType() == TYPE_COLLECTION)
				{
					((CCollectionGoods *)pMoveShape)->SetCreatorType(lCreatorType);
					((CCollectionGoods *)pMoveShape)->SetCreatorID(CreatorID);
				}
			}
		}
		break;
		// 平滑移位
	case MSG_S2C_SHAPE_SLIPPOS:
		{
			long	  lType		  = pMsg->GetLong();
			CGUID ID;
			pMsg->GetGUID(ID);
			float x = pMsg->GetFloat();
			float y = pMsg->GetFloat();
			float fSpeed = pMsg->GetFloat();
			float fDir = pMsg->GetFloat();
			CMoveShape* pShape = dynamic_cast<CMoveShape*>(pMsg->GetRegion()->FindChildObject(lType, ID));
			if( pShape )
			{
                float fx = pShape->GetPosX();
                float fy = pShape->GetPosY();
				
				DWORD dwCurTime = GetCurTickCount();
				pShape->SetIsSlipState(true);
				pShape->SetSlipSpeed(fSpeed);
				pShape->SetSlipDestPos(x,y);
				pShape->SetSlipLastTime(dwCurTime);
				pShape->SetSlipDir(fDir);
				//得出移动时间
				float fTotalDistance = pShape->RealDistance(x+0.5f,y+0.5f);
				DWORD dwMoveTime = DWORD(fTotalDistance/fSpeed);
				pShape->SetSlipEndMoveTime(dwCurTime+dwMoveTime);
				// 清除之前的移动目标相关信息
				pShape->SetDestXY(pShape->GetPosX(), pShape->GetPosY());
				pShape->SetEndMoveTime(dwCurTime);
                pShape->SetMoveOffset((x-fx)/dwMoveTime, (y-fy)/dwMoveTime);
			}
		}
		break;
		// 加血
	case MSG_S2C_SHAPE_ATK_ADDBLOOD:
		{
			// 消息顺序lActorType|lActorID|lTargType|lTargetID|cSize|cLostType|lLostPoint|lFinalHP|bBlastAttack
			CGUID ActorID,TargetID;
			long lActorType = pMsg->GetLong();
			pMsg->GetGUID(ActorID);
			long lTargType = pMsg->GetLong();
			pMsg->GetGUID(TargetID);
			CMoveShape* pTargetShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(lTargType, TargetID);
			if( pTargetShape)
			{
				if(pTargetShape->IsDied())
					break;
				// 加入一个攻击的事件
				tagAttackEvent st;
				st.cType = AE_CURE;								// 加血
				st.Guidattracker = ActorID;
				st.lTotalLostPoint = pMsg->GetLong();			// 加血点数
				pTargetShape->GetAttackEventQueue()->push(st);
			}
		}
		break;
		// 获得选择目标
	case MSG_S2C_SHAPE_GETTSELECTEDSHAPE:
		{
			long type = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CMoveShape* pMoveShape = (CMoveShape*)pMsg->GetRegion()->FindChildObject(type, guid);
			if(pMoveShape && GetGame()->GetGameControl())
			{
				GetGame()->GetGameControl()->SetSelectedShape(pMoveShape,false);
			}
		}
		break;
	}
}
