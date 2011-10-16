#include "StdAfx.h"

#include "../Area.h"
#include "../ServerRegion.h"
#include "../Player.h"
#include "../Pet.h"
#include "../Monster.h"
#include "../RgnManager.h"
#include "../../Net/Message.h"
#include "../../Server/GameManager.h"
#include "../PhaseSystem/PhaseManager.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应其他消息
void CMoveShape::OnShapeMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_SHAPE_CHANGELINEDRGN:
		{

			/*CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = GlobalSetup::GetSetup()->pszChangeLineRgn;
				if(strScript)
				{
					stRunScript sScript;
					sScript.pszFileName = (char*)strScript;
					sScript.desShape = pPlayer;
					sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					sScript.srcShape	= pPlayer;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
				}
			}*/

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
						Log4c::Trace(ROOT_MODULE,FormatText("GS_SHAPEMSG_0", player->GetRegionID(), szGuid, templateID));//切换分线场景时:在场景%d上的玩家%s想切换到场景%d非法!
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

			//取消玩家的争夺状态
			//if(pMsg->GetPlayer()->GetIsContendState() == true && pMsg->GetRegion())
			//{
			//	if(pMsg->GetRegion()->CancelContendByPlayerID(pMsg->GetPlayer()) == true)
			//	{
			//		DWORD dwColor = MAKELONG(MAKEWORD(255,255), MAKEWORD(255,255));
			//		DWORD dwBkColor = MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255));
			//		pMsg -> GetPlayer()->SendNotifyMessage("因为改变方向，争夺状态被打断。",dwColor,dwBkColor);
			//	}

			//}
		}
		break;

	case MSG_C2S_SHAPE_SETPOS:	// 瞬移
		{
			if (GlobalSetup::GetSetup()->bAllowClientChangePos == false)
				break;;

			long lType = pMsg->GetLong();
			CGUID guid; pMsg -> GetGUID(guid);
			CShape* pShape = (CShape*)pMsg->GetRegion()->FindChildObject(lType, guid);
			if( pShape )
			{
				//long x = pMsg->GetLong();
				//long y = pMsg->GetLong();
                float x = pMsg->GetFloat();
                float y = pMsg->GetFloat(); 
                if(PHASEMAN.IsPhaseBlock((CMoveShape*)pShape,x,y))
                    break;
				pMsg->SetType(MSG_S2C_SHAPE_SETPOS);
				//pShape->SetPosXY( x+0.5f, y+0.5f );
                pShape->SetPosXY( x, y );
				pMsg->SendToAround(pShape);

                if( pShape->GetType() == TYPE_PLAYER )
                {
                    CPlayer *player = (CPlayer*) pShape;
                    player->GetMover().OnSetPos( x, y );
                }
			}
		}
		break;
	case MSG_C2S_SHAPE_MOVE:
		{			
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMoveShape::OnShapeMessage(MSG_C2S_SHAPE_MOVE) Start.");
#endif
			//float fCurX = pMsg->GetFloat();
			//float fCurY = pMsg->GetFloat();
			//float fDestX = pMsg->GetFloat();
			//float fDestY = pMsg->GetFloat();
#ifdef _RUNSTACKINFO1_
			//char pszStatckInfo[1024]="";
			//_snprintf(pszStatckInfo,1024,"CurX:%f,CurY:%f,DestX:%f,DestY:%f",fCurX,fCurY,fDestX,fDestY);
			//CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
			//// 处理行走请求
			//OnQuestMoveStep(fCurX,fCurY,fDestX,fDestY);
            float curX = pMsg->GetFloat();
            float curY = pMsg->GetFloat();
            uchar stopFlag = pMsg->GetByte();
            float destX = 0.0f; 
            float destY = 0.0f;
            if( !stopFlag )
            {
                destX = pMsg->GetFloat();
                destY = pMsg->GetFloat();
            }
            if(PHASEMAN.IsPhaseBlock(this,(long)destX,(long)destY))
                break;
            if( GetType() == TYPE_PLAYER )
            {
                CPlayer *player = (CPlayer*) this;
                if( stopFlag )
                {
                    player->GetMover().OnStopMove( curX, curY );
                }
                else
                {
                    player->GetMover().OnMove( curX, curY, destX, destY );
                }
            }
            else if( GetType() == TYPE_PET )
            {
                CPet *pet = (CPet*) this;
                if( stopFlag )
                {
                    pet->GetMover().OnStopMove( curX, curY );
                }
                else
                {
                    pet->GetMover().OnMove( curX, curY, destX, destY );
                }
            }
            else
            {
                assert( "Not implemented" && 0 );
                OnQuestMoveStep(curX, curY, destX, destY);
            }
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
					pMsg -> GetPlayer()->SetSpeed(GlobalSetup::GetSetup()->fPlayerWalkSpeed);
				else
					pMsg -> GetPlayer()->SetSpeed(GlobalSetup::GetSetup()->fPlayerRunSpeed);

			}
			else
			{
				if(!GetIsRun())
					pMsg -> GetPlayer()->SetSpeed(GlobalSetup::GetSetup()->fRidePlayerWalkSpeed);
				else
					pMsg -> GetPlayer()->SetSpeed(GlobalSetup::GetSetup()->fRidePlayerRunSpeed);
			}


			pMsg -> GetPlayer()->SendChangedSpeedToClients();
			//pMsg -> GetPlayer()->CheckMoveSpeedValidate();
		}
		break;
	// 客户端主动请求shape数据
	case MSG_C2S_SHAPE_QUESTINFO:
		{
			//long lType = pMsg->GetLong();
			//CGUID guid; pMsg -> GetGUID(guid);

			//CServerRegion* pRegion = pMsg->GetRegion();
			//if (pRegion)
			//{
			//	CShape* pShape = (CShape*)pRegion->FindChildObject(lType, guid);
			//	if (pShape)
			//	{
			//		vector<BYTE>	array;
			//		if (pShape->GetType()==TYPE_PLAYER)
			//			((CPlayer*)pShape)->AddToByteArray_ForClient( &array, false );
			//		else if (pShape->GetType()==TYPE_GOODS)
			//			((CGoods*)pShape)->SerializeForOldClient( &array, false );
			//		else
			//			pShape->AddToByteArray( &array, false );

			//		CMessage Msg(MSG_S2C_RGN_ADDSHAPE,pShape->GetExID());
			//		Msg.Add( pShape->GetType() );
			//		//if(pShape->GetType()==TYPE_MONSTER)
			//		//{
			//		//	//LONG a=dynamic_cast<CMonster*>(pShape)->GetNpcType();
			//		//	Msg.Add((LONG)dynamic_cast<CMonster*>(pShape)->GetNpcType());
			//		//	Msg.Add(dynamic_cast<CMonster*>(pShape)->GetCanTalk());
			//		//	/////
			//		//	WORD picID=dynamic_cast<CMonster*>(pShape)->GetTitlePicID();
			//		//	Msg.Add(picID);
			//		//	
			//		//	Msg.Add(dynamic_cast<CMonster*>(pShape)->GetTitleColor());
			//		//	const string & str=dynamic_cast<CMonster*>(pShape)->GetTitleName();
			//		//	Msg.Add(( char *)str.c_str());

			//		//	////
			//		//}
			//		Msg.Add( pShape->GetExID() );
			//		Msg.Add( (long)array.size() );
			//		Msg.AddEx( &array[0] , (long)array.size() );
			//		Msg.Add( (char)false );		// 是否出现特效
			//		Msg.SendToPlayer( pMsg->GetPlayer()->GetExID() );
			//	}
			//}
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
