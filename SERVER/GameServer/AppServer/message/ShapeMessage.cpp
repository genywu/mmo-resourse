
#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\area.h"
#include "..\ServerRegion.h"
#include "..\player.h"
#include "..\Monster.h"
#include "../../../setup/GlobeSetup.h"
#include "../../GameServer/GameManager.h"
#include "../RgnManager.h"
#include "..\SMClient.h"

// 响应其他消息
void CMoveShape::OnShapeMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_SHAPE_CHANGELINEDRGN:
		{
			long linedID = pMsg->GetLong();
			long templateID = pMsg->GetLong();
			CGUID rgnID;
			pMsg->GetGUID(rgnID);
			CGUID playerID;
			pMsg->GetGUID(playerID);
			long lX = pMsg->GetLong();
			long lY = pMsg->GetLong();
			CPlayer* player = GetGame()->FindPlayer(playerID);
			if(player)
			{
				// 判断是否是分线场景的TemplateID
				map<long, vector<long>>::iterator linedId = GameManager::GetInstance()->GetRgnManager()->GetLinedRgnIdByLinedIdMap().find(linedID);
				if(linedId != GameManager::GetInstance()->GetRgnManager()->GetLinedRgnIdByLinedIdMap().end())
				{
					vector<long>::iterator templateIdItr;
					// 判断玩家当前场景ID跟消息是否一致
					templateIdItr = std::find(linedId->second.begin(), linedId->second.end(), player->GetRegionID());
					if(templateIdItr == linedId->second.end()) // 未找到该场景ID
					{
						char szGuid[128];
						playerID.tostring(szGuid);
						AddLogText("切换分线场景时:在场景%d上的玩家%s想切换到场景%d非法!", player->GetRegionID(), szGuid, templateID);
						return;
					}

					templateIdItr = std::find(linedId->second.begin(), linedId->second.end(), templateID);
					if(templateIdItr != linedId->second.end())
						player->ChangeRegion(RGN_NORMAL, rgnID, lX, lY, -1, templateID);
				}
			}
		}
		break;
	// 改变玩家方向
	case MSG_C2S_SHAPE_CHANGEDIR:
		{
			BYTE lDir = pMsg->GetByte();
			float fDir = pMsg->GetFloat();
			pMsg->GetPlayer()->SetDirEx(fDir);

			pMsg->SetType(MSG_S2C_SHAPE_CHANGEDIR);
			pMsg->Add(pMsg->GetPlayer()->GetType());
			pMsg->Add(pMsg->GetPlayer()->GetExID());
			pMsg->SendToNearestPlayer(this,true,pMsg->GetPlayer());
		}
		break;

	case MSG_C2S_SHAPE_SETPOS:	// 瞬移
		{
			if (CGlobeSetup::GetSetup()->bAllowClientChangePos == false)
				break;;

			long lType = pMsg->GetLong();
			CGUID guid; pMsg -> GetGUID(guid);
			CShape* pShape = (CShape*)pMsg->GetRegion()->FindChildObject(lType, guid);
			if( pShape )
			{
				long x = pMsg->GetLong();
				long y = pMsg->GetLong();
				
				pMsg->SetType(MSG_S2C_SHAPE_SETPOS);
				pShape->SetPosXY( x+0.5f, y+0.5f );
				pMsg->SendToAround(pShape);
			}
		}
		break;
	case MSG_C2S_SHAPE_MOVE:
		{			
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMoveShape::OnShapeMessage(MSG_C2S_SHAPE_MOVE) Start.");
#endif
			float fCurX = pMsg->GetFloat();
			float fCurY = pMsg->GetFloat();
			float fDestX = pMsg->GetFloat();
			float fDestY = pMsg->GetFloat();
#ifdef _RUNSTACKINFO1_
			char pszStatckInfo[1024]="";
			_snprintf(pszStatckInfo,1024,"CurX:%f,CurY:%f,DestX:%f,DestY:%f",fCurX,fCurY,fDestX,fDestY);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
			CSMClient::GetSM()->SendWatchResult(GetName(),eGGC_Move);
			// 处理行走请求
			OnQuestMoveStep(fCurX,fCurY,fDestX,fDestY);
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMoveShape::OnShapeMessage(MSG_C2S_SHAPE_MOVE) End.");
#endif
		}
		break;
	case MSG_C2S_SHAPE_STOPMOVE:
		{
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMoveShape::OnShapeMessage(MSG_C2S_SHAPE_STOPMOVE) Start.");
#endif
			float fCurX = pMsg->GetFloat();
			float fCurY = pMsg->GetFloat();
			short nDir = pMsg->GetByte();
			OnQuestStopMove(fCurX,fCurY,nDir);
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMoveShape::OnShapeMessage(MSG_C2S_SHAPE_STOPMOVE) End.");
#endif
		}
		break;
		//改变移动模式
	case MSG_C2S_SHAPE_CHANGEMOVEMODE:
		{
			bool bRun = pMsg->GetChar()==0?false:true;
			pMsg -> GetPlayer()->SetIsRun(bRun);

			if (pMsg->GetPlayer()->GetIsRide())
			{
				if(!GetIsRun())
					pMsg -> GetPlayer()->SetSpeed(CGlobeSetup::GetSetup()->fPlayerWalkSpeed);
				else
					pMsg -> GetPlayer()->SetSpeed(CGlobeSetup::GetSetup()->fPlayerRunSpeed);

			}
			else
			{
				if(!GetIsRun())
					pMsg -> GetPlayer()->SetSpeed(CGlobeSetup::GetSetup()->fRidePlayerWalkSpeed);
				else
					pMsg -> GetPlayer()->SetSpeed(CGlobeSetup::GetSetup()->fRidePlayerRunSpeed);
			}


			pMsg -> GetPlayer()->SendChangedSpeedToClients();
		}
		break;
	// 客户端主动请求shape数据
	case MSG_C2S_SHAPE_QUESTINFO:
		{
		}
		break;
	case MSG_C2S_SHAPE_EMOTION:
		{
			LONG lType		= pMsg -> GetLong();
			CGUID guid; pMsg -> GetGUID(guid);
			DWORD dwEmotion	= pMsg -> GetDWord();

			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( pPlayer -> GetType() == lType &&
				pPlayer -> GetExID() == guid )
			{
				pPlayer -> PerformEmotion( dwEmotion );
			}
		}
		break;
	case MSG_C2S_SHAPE_SETSELECTEDSHAPE:
		{
			//设定当前选择的目标
			long lType = pMsg->GetLong();
			CGUID guid = NULL_GUID;
			pMsg->GetGUID(guid);
			CPlayer *pPlayer = pMsg ->GetPlayer();
			if (pPlayer)
			{
				CServerRegion *pRegion = dynamic_cast<CServerRegion*>(pPlayer->GetFather());
				if(pRegion)
				{
					long lOldType = 0;
					CGUID OldGuid;
					pPlayer->GetTarget(OldGuid,lOldType);
					if(OldGuid != guid)
					{
						CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(lOldType,OldGuid));
						if(pMoveShape)
							pMoveShape->RemoveSelectSelfPlayer(pPlayer->GetExID());
						pPlayer->SetTarget(guid,lType);	
						if(guid != NULL_GUID)
						{
							pMoveShape = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(lType,guid));
							if(pMoveShape)
							{
								pMoveShape->AddSelectSelfPlayer(pPlayer->GetExID());
								set<string>	setDirtyAttr;
								setDirtyAttr.insert("C_MaxHp");
								setDirtyAttr.insert("Hp");
								//更新属性
								pMoveShape->UpdateHPMPToClient(setDirtyAttr);
							}
						}
					}
				}
			}
		}
		break;
	case MSG_C2S_SHAPE_GETTSELECTEDSHAPE:
		{
			// 获得当前选择的目标
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (pPlayer)
			{
				long lType = 0;
				CGUID guid = NULL_GUID;
				pPlayer->GetTarget(guid,lType);


				CServerRegion* pServerRegion = (CServerRegion*)(pPlayer->GetFather());
				if(NULL == pServerRegion || guid == NULL_GUID)	
					return ;			

					CBaseObject *pObj = pServerRegion->FindChildObject(lType,guid);		
					CMoveShape *pMoveShape = dynamic_cast<CMoveShape*>(pObj);
					if (pMoveShape)
					{	
						long lTargetType = 0;
						CGUID guidTarget = NULL_GUID;
						pMoveShape->GetTarget(guidTarget,lTargetType);
						
						if (guid != NULL_GUID)
						{
							CMessage msg(MSG_S2C_SHAPE_GETTSELECTEDSHAPE);
							msg.Add(lTargetType);
							msg.Add(guidTarget);
							msg.SendToPlayer(pPlayer->GetExID());
						}		
					}		
			}
		}
		break;

	}
}

//发送变化的速度值到客户端
void CMoveShape::SendChangedSpeedToClients()
{
	//if(m_bSpeedLvlChanged)
	{
		m_bSpeedLvlChanged = false;
		CMessage msg(MSG_S2C_SHAPE_CHANGEMOVEMODE);
		msg.Add(GetType());
		msg.Add(GetExID());
		//移动模式
		msg.Add(static_cast<char>(GetIsRun()==false?0:1));
		//当前速度
		msg.Add( GetSpeed());
		//变化比率
		msg.Add(GetSpeedChangedRatio());
		msg.SendToAround(this);
	}
}
