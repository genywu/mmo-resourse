#include "StdAfx.h"

#include "../Player.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../RgnManager.h"
#include "../PhaseSystem/PhaseManager.h"
//#include "../Script/Script.h"
//#include "../Script/VariableList.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../Session/CSessionFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// [2007-7-19 add]
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 响应WorldServer组队消息
void OnWSTeamMessage(CMessage *pMsg);
//! 响应Client组队消息
void OnClientTeamMessage(CMessage *pMsg);


//-------------------------------------------------------------------------------------------------------------
void OnWSTeamMessage(CMessage *pMsg){
	switch(pMsg->GetType())
	{
	
	case MSG_W2S_TEAM_ToPlayerInfo:
		{
			CGUID PlayerID; pMsg -> GetGUID(PlayerID);
			char szInfo[1024] = {0};
			pMsg->GetStr(szInfo, 1023);

			CPlayer* pPlayer = GetGame() -> FindPlayer( PlayerID );
			if (NULL == pPlayer) return;
			pPlayer -> SendNotifyMessage( szInfo );
		}
		break;
	case MSG_W2S_TEAM_DataInit:
		{
			GetOrganiCtrl()->GetTeamData(pMsg);
		}
		break;
	case MSG_W2S_TEAM_QUERY_JOIN:
		{
			CGUID InvitedID; pMsg -> GetGUID(InvitedID);
			CPlayer *pInvited = GetGame() -> FindPlayer( InvitedID );
			if(NULL != pInvited)
			{
				if(!pInvited->IsDied())
				{
					pMsg->SetType(MSG_S2C_TEAM_QUERY_JOIN);
					pMsg->SendToPlayer(InvitedID);
				}
				else
				{
					CGUID CaptainID; pMsg -> GetGUID(CaptainID);

					CMessage msg(MSG_S2W_TEAM_ANSWER_JOIN);
					msg.Add(InvitedID);
					msg.Add(CaptainID);
					msg.Add((BYTE)4);
					msg.Send(false);
				}
			}
		}
		break;
	case MSG_W2S_TEAM_CREATE:
		{
			GetOrganiCtrl()->CreateTeam(pMsg);
		}
		break;
	case MSG_W2S_TEAM_PlayerJoin:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL != pTeam) pTeam->DoJoin(pMsg);
		}
		break;
	case MSG_W2S_TEAM_MemberExit:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			CGUID PlayerID; pMsg -> GetGUID(PlayerID);
            CPlayer* player = GetGame()->FindPlayer( PlayerID );
            if( player == NULL )
                break;
            /// 玩家离开队伍,通知相位功能
            PHASEMAN.OnPhaseOper(player->GetPhaseID(),player,eLeave);
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL != pTeam) pTeam->Exit(PlayerID);
		}
		break;
	case MSG_W2S_TEAM_MastterChange:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			CGUID PlayerID; pMsg -> GetGUID(PlayerID);
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL != pTeam) pTeam->Demise(NULL_GUID, PlayerID);
		}
		break;
	case MSG_W2S_TEAM_Disband:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			GetOrganiCtrl()->DisbandTeam(TeamID);
		}
		break;
	case MSG_W2S_TEAM_GoodsSetChange:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			long eSet = pMsg->GetLong();
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL != pTeam) pTeam->SetGoodsAllot(eSet);
		}
		break;
	case MSG_W2S_TEAM_SetupInit:
		{
			GetOrganiCtrl()->OnTeamSetupInit(pMsg);
		}
		break;
    case MSG_W2S_TEAM_ServerChange:
        {
            CGUID TeamID; pMsg -> GetGUID(TeamID);
            CGUID PlayerID; pMsg -> GetGUID(PlayerID);
            GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
            if(NULL != pTeam) 
            {
                pTeam->PlayerLeaveHere(PlayerID);
                pTeam->MemberMapChange(PlayerID);
            }
        }
        break;
	case MSG_W2S_TEAM_MemberStateChange:
		{
			CGUID TeamID; pMsg -> GetGUID(TeamID);
			CGUID PlayerID; pMsg -> GetGUID(PlayerID);
			long lPlayerState = pMsg->GetLong();
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL != pTeam) pTeam->MemberStateChange(PlayerID, lPlayerState);
		}
		break;
	case MSG_W2S_TEAM_RECRUIT:
		{
			CGUID PlayerID; pMsg -> GetGUID(PlayerID);
			long lFlag = pMsg->GetLong();
			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerID);
			if (NULL == pPlayer)
			{
				break;
			}

			if(0 != lFlag)//打开招募
			{
				char szRecruitStr[MAX_RECRUIT_TEXT_SIZE] = {0};
				char szRecruitPassword[MAX_RECRUIT_PWD_SIZE] = {0};
				pMsg->GetStr(szRecruitStr, MAX_RECRUIT_TEXT_SIZE);
				pMsg->GetStr(szRecruitPassword, MAX_RECRUIT_PWD_SIZE);

				/*CState* pState = new CTeamState( szRecruitStr, szRecruitPassword );
				if( pState && pState -> Begin( pPlayer, pPlayer ) )
				{
					pPlayer -> GetStatesList().push_back( pState );
				}
				else
				{
					SAFE_DELETE( pState );
				}*/
				pPlayer->SetRecruitState(true, szRecruitStr, szRecruitPassword);
				GSTeam *pTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID()));
				if(NULL != pTeam)
				{
					pTeam->UpdateLeaderRecruitedNum();
				}
			}
			else
			{
				/*vector<CState*>::iterator ite = pPlayer -> GetStatesList().begin();
				for (;ite != pPlayer -> GetStatesList().end(); ++ite)
				{
					CState *pState = (*ite);
					if (STATE_TYPE_STANDARD == pState->GetType() && STATE_TEAM == pState->GetID())
					{
						SAFE_DELETE( pState );
						pPlayer->GetStatesList().erase(ite);
						break;
					}
				}*/
				pPlayer->SetRecruitState(false, NULL, NULL);
			}
		}
		break;
	case MSG_W2S_TEAM_CHAT:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				CGUID SenderGuid;
				pMsg->GetGUID(PlayerGuid);
				char szTxt[512] = {0};
				pMsg->GetStr(szTxt, 511);

				CMessage msg(MSG_S2C_TEAM_CHAT);
				msg.Add(SenderGuid);
				msg.Add(szTxt);
				msg.SendToPlayer(PlayerGuid);
			}
		}
		break;
	case MSG_W2S_TEAM_MemberAllIdioinfo:
		{
			tagMemberIdioinfo MemberIdioinfo;
			pMsg->GetEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
			CGUID guid, ReceiverGuid;
			pMsg->GetGUID(guid);
			pMsg->GetGUID(ReceiverGuid);

			CMessage msg(MSG_S2C_TEAM_MemberAllIdioinfo);
			msg.Add(guid);
			msg.Add(1L);
			msg.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));

			msg.SendToPlayer(ReceiverGuid);
		}
		break;
	case MSG_W2S_TEAM_MemberPos:
		{
			LONG lDataSize = pMsg->GetLong();
			if(0 == lDataSize)
				return;
			BYTE *pData = (BYTE*)M_ALLOC(sizeof(BYTE)*lDataSize);
			pMsg->GetEx(pData, lDataSize);

			CMessage msg(MSG_S2C_TEAM_MemberPos);
			msg.Add((LONG)lDataSize);
			msg.AddEx(pData, lDataSize);

			LONG lReceiverNum = pMsg->GetLong();
			for (LONG i = 0; i < lReceiverNum; ++i)
			{
				CGUID ReceiverGuid;
				pMsg->GetGUID(ReceiverGuid);
				msg.SendToPlayer(ReceiverGuid);
			}

			M_FREE(pData, sizeof(BYTE)*lDataSize);
		}
		break;
	case MSG_W2S_TEAM_LEVELCHANGE:
		{
			CGUID TeamID, PlayerID;
			pMsg->GetGUID(TeamID);
			pMsg->GetGUID(PlayerID);
			LONG lLevel = pMsg->GetLong();

			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL == pTeam)
				break;

			CMessage msg(MSG_S2C_TEAM_UpdateIdioinfo);
			msg.Add(PlayerID);
			msg.Add((LONG)eMIS_Level);
			msg.Add(lLevel);

			pTeam->SendToAllMember(&msg);
		}
		break;
	case MSG_W2S_TEAM_OCCUCHANGE:
		{
			CGUID TeamID, PlayerID;
			pMsg->GetGUID(TeamID);
			pMsg->GetGUID(PlayerID);
			LONG lOccu = pMsg->GetLong();

			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
			if(NULL == pTeam)
				break;

			CMessage msg(MSG_S2C_TEAM_UpdateIdioinfo);
			msg.Add(PlayerID);
			msg.Add((LONG)eMIS_Occupation);
			msg.Add(lOccu);

			pTeam->SendToAllMember(&msg);
		}
		break;
	case MSG_W2S_TEAM_SwapRgnID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CPlayer *pNewMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pNewMaster)
			{
				LONG lDataSize = pMsg->GetLong();
				vector<BYTE> vData(lDataSize);
				pMsg->GetEx(&vData[0], lDataSize);
				LONG lPos = 0;
				GameManager::GetInstance()->GetRgnManager()->RgnGuidSwapFromArray(pNewMaster->GetExID(), &vData[0], lPos);
			}	
		}
		break;
	case MSG_W2S_TEAM_GetSrcSwapRgnID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerID);
			if(NULL == pPlayer)
				return;

			vector<BYTE> vRgnGuidData;
			GameManager::GetInstance()->GetRgnManager()->RgnGuidCode(PlayerID, vRgnGuidData);

			if(vRgnGuidData.size() > sizeof(LONG))//! 判断是否有需要交换的数据
			{
				CGUID AimGuid;
				pMsg->GetGUID(AimGuid);

				CPlayer *pAim = GetGame()->FindPlayer(AimGuid);
				if(NULL != pAim)
				{
					LONG lPos = 0;
					GameManager::GetInstance()->GetRgnManager()->RgnGuidSwapFromArray(AimGuid, &vRgnGuidData[0], lPos);
				}
				else
				{
					CMessage msgSwap(MSG_S2W_TEAM_SwapRgnID);
					msgSwap.Add(AimGuid);
					msgSwap.Add((LONG)vRgnGuidData.size());
					msgSwap.AddEx(&vRgnGuidData[0], (long)vRgnGuidData.size());
					msgSwap.Send(false);
				}
			}
			

		}
		break;
	case MSG_W2S_TEAM_AddRgnID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
			{
				LONG lTemplateID = pMsg->GetLong();
				CGUID RgnGuid;
				pMsg->GetGUID(RgnGuid);
				GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(PlayerID, lTemplateID, RgnGuid, FALSE);
			}
		}
		break;
	case MSG_W2S_TEAM_DelRgnID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
			{
				LONG lTemplateID = pMsg->GetLong();
				GameManager::GetInstance()->GetRgnManager()->DelTeamRgnGUID(PlayerID, lTemplateID);
			}
		}
		break;
	case MSG_W2S_TEAM_ClearRgnID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
			{
				GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(PlayerID);
			}
		}
		break;
	case MSG_W2S_TEAM_GetRgnNum:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);
			LONG lSocketID = pMsg->GetLong();

			CMessage msg(MSG_S2W_TEAM_ReturnGetRgnNum);
			msg.Add(lSocketID);
			msg.Add(ScriptID);
			msg.Add(szRet);

			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
			{
				map<LONG, CGUID>* rgnIdMap = GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidMap(PlayerID);
				if(rgnIdMap)
					msg.Add((LONG)rgnIdMap->size());
				else
					msg.Add((LONG)0);
			}
			else
				msg.Add(0L);

			msg.Send(false);
		}
		break;
	case MSG_W2S_TEAM_ReturnGetRgnNum:
		{
			LONG lSocketID	= pMsg->GetLong();
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			LONG lNum		= pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			//取消挂起的脚本
			//CVariableList* stackVarList = CScript::GetStackVarListInst();
			//if(stackVarList)
			//{
			//	stackVarList->Release();
			//	stackVarList->AddVar(szRet, lNum);
			//}
			//ContinueRunScript(ScriptID, stackVarList);
			Script *script = GetInst( ScriptSys ).GetScript( ScriptID );
			if( script != NULL )
			{
				script->PushRetValue( lNum );
				GetInst( ScriptSys ).ResumeScript( script, 1 );
			}

		}
		break;
	case MSG_W2S_TEAM_GetRgnGuid:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			LONG lIndex = pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);
			LONG lSocketID = pMsg->GetLong();

			CMessage msg(MSG_S2W_TEAM_ReturnGetRgnNum);
			msg.Add(lSocketID);
			msg.Add(ScriptID);
			msg.Add(szRet);

			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
				msg.Add(GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidByIndex(PlayerID, lIndex));
			else
				msg.Add(NULL_GUID);

			msg.Send(false);
		}
		break;
	case MSG_W2S_TEAM_ReturnGetRgnGuid:
		{
			LONG lSocketID	= pMsg->GetLong();
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			CGUID RgnGuid;
			pMsg->GetGUID(RgnGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			//取消挂起的脚本
			//CVariableList* stackVarList = CScript::GetStackVarListInst();
			//if(stackVarList)
			//{
			//	stackVarList->Release();
			//	char szGuid[128] = {0};
			//	RgnGuid.tostring(szGuid);
			//	stackVarList->AddVar(szRet, szGuid);
			//}
			//ContinueRunScript(ScriptID, stackVarList);
			Script *script = GetInst( ScriptSys ).GetScript( ScriptID );
			if( script != NULL )
			{
				script->PushRetValue( RgnGuid );
				GetInst( ScriptSys ).ResumeScript( script, 1 );
			}

		}
		break;		
	case MSG_W2S_TEAM_GetRgnTemplateID:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			char szRet[128] = {0};
			LONG lIndex = pMsg->GetLong();
			pMsg->GetStr(szRet, 128);
			LONG lSocketID = pMsg->GetLong();

			CMessage msg(MSG_S2W_TEAM_ReturnGetRgnNum);
			msg.Add(lSocketID);
			msg.Add(ScriptID);
			msg.Add(szRet);

			CPlayer *pMaster = GetGame()->FindPlayer(PlayerID);
			if(NULL != pMaster)
				msg.Add(GameManager::GetInstance()->GetRgnManager()->GetTeamRgnTemplateIdByIndex(pMaster->GetExID(), lIndex));
			else
				msg.Add(0L);

			msg.Send(false);
		}
		break;
	case MSG_W2S_TEAM_ReturnGetRgnTemplateID:
		{
			LONG lSocketID	= pMsg->GetLong();
			CGUID ScriptID;
			pMsg->GetGUID(ScriptID);
			LONG lNum		= pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			//取消挂起的脚本
			//CVariableList* stackVarList = CScript::GetStackVarListInst();
			//if(stackVarList)
			//{
			//	stackVarList->Release();
			//	stackVarList->AddVar(szRet, lNum);
			//}
			//ContinueRunScript(ScriptID, stackVarList);
			Script *script = GetInst( ScriptSys ).GetScript( ScriptID );
			if( script != NULL )
			{
				script->PushRetValue( lNum );
				GetInst( ScriptSys ).ResumeScript( script, 1 );
			}

		}
		break;
	case MSG_W2S_TEAM_INLVLDIFFERENCE:
		{
			CGUID ScriptGuid;
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);
			LONG lRet = pMsg->GetLong();

			// 取消挂起的脚本
			//CVariableList* stackVarList = CScript::GetStackVarListInst();
			//if(stackVarList)
			//{
			//	stackVarList->Release();
			//	stackVarList->AddVar(szRet, lRet);
			//}
			//ContinueRunScript(ScriptGuid, stackVarList);
			Script *script = GetInst( ScriptSys ).GetScript( ScriptGuid );
			if( script != NULL )
			{
				script->PushRetValue( lRet );
				GetInst( ScriptSys ).ResumeScript( script, 1 );
			}

		}
		break;

	case MSG_W2S_TEAM_INLVLSPACE:
		{
			CGUID ScriptGuid;
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);
			LONG lRet = pMsg->GetLong();

			// 取消挂起的脚本
			//CVariableList* stackVarList = CScript::GetStackVarListInst();
			//if(stackVarList)
			//{
			//	stackVarList->Release();
			//	stackVarList->AddVar(szRet, lRet);
			//}
			//ContinueRunScript(ScriptGuid, stackVarList);
			Script *script = GetInst( ScriptSys ).GetScript( ScriptGuid );
			if( script != NULL )
			{
				script->PushRetValue( lRet );
				GetInst( ScriptSys ).ResumeScript( script, 1 );
			}

		}
		break;
	case MSG_W2S_TEAM_Recommend_ByName:
		{
			char szName[128] = {0};
			pMsg->GetStr(szName, 128);//! 跳过目标名字
			pMsg->GetStr(szName, 128);//! 跳过发送者名字
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pMsg->SetType(MSG_S2C_TEAM_Recommend_ByName);
			pMsg->SendToPlayer(PlayerGuid, false);
		}
		break;
	default:
		break;
	}
}


//-------------------------------------------------------------------------------------------------------------
void OnClientTeamMessage(CMessage *pMsg)
{
	if(!GSTeam::CanTeamOpt(pMsg->GetPlayer()))
	{
		return;
	}

	CPlayer *pPlayer = pMsg->GetPlayer();
	if (NULL == pPlayer)
	{
		return;
	}
	if (pPlayer->IsDied())
	{
		pPlayer->SendNotifyMessage(AppFrame::GetText("69"));
		return;
	}

	switch(pMsg->GetType())
	{
	case MSG_C2S_TEAM_QUERY_JOIN:
		{
			CGUID InviteeID;// 被邀请人
			pMsg -> GetGUID(InviteeID);
			if(InviteeID == pMsg->GetPlayerID())
				return;
			CGUID CaptainID;// 邀请人
			pMsg -> GetGUID(CaptainID);

			CPlayer *pInvitee = GetGame()->FindPlayer(InviteeID);
			if(NULL != pInvitee)
				if(pInvitee->IsDied())
				{
					pMsg->GetPlayer()->SendNotifyMessage(AppFrame::GetText("68"));
					return;
				}

			if(CaptainID != pMsg->GetPlayerID())
				return;

			CPlayer *pCaptain = GetGame()->FindPlayer(CaptainID);
			if(pCaptain && pInvitee)
			{
				CServerRegion* pCapRgn = dynamic_cast<CServerRegion*>(pCaptain->GetFather());
				if(pCapRgn && pCapRgn->GetStrongPointFlag() == true) // 邀请人在开启据点功能的场景
				{
					CServerRegion* pInvRgn = dynamic_cast<CServerRegion*>(pInvitee->GetFather());
					// 判断双方是否在同一个场景
					if(pInvRgn == pCapRgn)
					{
						// 判断双方是否不同阵营
						if(pInvitee->GetCountry() % 2 != pCaptain->GetCountry() % 2)
						{
							pMsg->GetPlayer()->SendNotifyMessage(AppFrame::GetText("71"));
							return;
						}
					}

				}
			}
			
			pMsg->SetType(MSG_S2W_TEAM_QUERY_JOIN);
			pMsg->Send();
		}
		break;
	case MSG_C2S_TEAM_QUERY_JOIN_BYNAME:
		{
			char szAimName[256] = {0};
			pMsg->GetStr(szAimName, 255);
			if(0 == stricmp(szAimName, pMsg->GetPlayer()->GetName()))
				return;
			CGUID CaptainID;
			pMsg -> GetGUID(CaptainID);

			CPlayer *pInvitee = GetGame()->FindPlayer(szAimName);
			if(NULL != pInvitee)
				if(pInvitee->IsDied())
				{
					pMsg->GetPlayer()->SendNotifyMessage(AppFrame::GetText("68"));
					return;
				}

			if(CaptainID != pMsg->GetPlayerID())
				return;

			CPlayer *pCaptain = GetGame()->FindPlayer(CaptainID);
			if(pCaptain && pInvitee)
			{
				CServerRegion* pCapRgn = dynamic_cast<CServerRegion*>(pCaptain->GetFather());
				if(pCapRgn && pCapRgn->GetStrongPointFlag() == true) // 邀请人在开启据点功能的场景
				{
					CServerRegion* pInvRgn = dynamic_cast<CServerRegion*>(pInvitee->GetFather());
					// 判断双方是否在同一个场景
					if(pInvRgn == pCapRgn)
					{
						// 判断双方是否不同阵营
						if(pInvitee->GetCountry() % 2 != pCaptain->GetCountry() % 2)
						{
							pMsg->GetPlayer()->SendNotifyMessage(AppFrame::GetText("71"));
							return;
						}
					}

				}
			}

			pMsg->SetType(MSG_S2W_TEAM_QUERY_JOIN_BYNAME);
			pMsg->Send();
		}
		break;
	case MSG_C2S_TEAM_ANSWER_JOIN:
		{
			pMsg->SetType(MSG_S2W_TEAM_ANSWER_JOIN);
			pMsg->Send();
		}
		break;
	case MSG_C2S_TEAM_CHANGE_LEADER:
		{
			CMessage msg(MSG_S2W_TEAM_CHANGE_LEADER);
			CGUID newLeader;
			pMsg -> GetGUID(newLeader);
			msg.Add(newLeader);
			msg.Add(pMsg -> GetPlayerID());
			msg.Send(false);
		}
		break;
	case MSG_C2S_TEAM_KICK_PLAYER:
		{
			CMessage msg(MSG_S2W_TEAM_KICK_PLAYER);
			CGUID aimPlayer;
			pMsg -> GetGUID(aimPlayer);
			msg.Add(aimPlayer);
			msg.Add(pMsg -> GetPlayerID());
			msg.Send(false);
		}
		break;
	case MSG_C2S_TEAM_SetGoods:
		{
			pMsg->SetType(MSG_S2W_TEAM_SetGoods);
			CMessage msg(MSG_S2W_TEAM_SetGoods);
			msg.Add(pMsg->GetPlayerID());
			msg.Add(pMsg -> GetLong());
			msg.Send(false);
		}
		break;
	case MSG_C2S_TEAM_CHAT://队聊
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL == pPlayer)
			{
				return;
			}
			GSTeam *pTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID()));
			if (NULL == pTeam)
			{
				return;
			}
			CHAR szMsg[512] = {0};
			pMsg -> GetStr( szMsg, 511);
			pTeam->TeamChat(pPlayer, szMsg);
		}
		break;
	case MSG_C2S_TEAM_RECRUIT://招募控制
		{
			const CGUID& PlayerID = pMsg -> GetPlayerID();
			long lFlag = pMsg->GetLong();

			CMessage msg(MSG_S2W_TEAM_RECRUIT);
			msg.Add(PlayerID);
			msg.Add(lFlag);

			if(1 == lFlag)//打开招募
			{
				char szRecruitStr[MAX_RECRUIT_TEXT_SIZE] = {0};
				char szRecruitPassword[MAX_RECRUIT_PWD_SIZE] = {0};
				pMsg->GetStr(szRecruitStr, MAX_RECRUIT_TEXT_SIZE);
				pMsg->GetStr(szRecruitPassword, MAX_RECRUIT_PWD_SIZE);

				/*if (0 == strlen(szRecruitStr))
				{
					return;
				}*/

				msg.Add(szRecruitStr);
				msg.Add(szRecruitPassword);
			}
			msg.Send(false);
		}
		break;
	case MSG_C2S_TEAM_BeRecruited://应征
		{
			const CGUID& PlayerID = pMsg -> GetPlayerID();
			CGUID TeamMastterID; pMsg -> GetGUID(TeamMastterID);

			if(PlayerID == TeamMastterID)
				return;
			
			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerID);
			CPlayer *pTeamMastter = GetGame()->FindPlayer(TeamMastterID);
			if (NULL == pPlayer || NULL_GUID != pPlayer->GetTeamID() || NULL == pTeamMastter)
			{
				return;
			}

			if (!pTeamMastter->GetRecruitState().bIsRecruiting)
			{
				return;
			}

			if(0 != strlen(pTeamMastter->GetRecruitState().szPassword))
			{
				char szSendPassword[MAX_RECRUIT_PWD_SIZE] = {0};
				pMsg->GetStr(szSendPassword, MAX_RECRUIT_PWD_SIZE);
				if( 0 != strcmp(pTeamMastter->GetRecruitState().szPassword, szSendPassword))
				{
					const char* pPasswordErr = AppFrame::GetText("60");
					if (NULL != pPasswordErr)
					{
						pPlayer->SendNotifyMessage(pPasswordErr);
					}
					return;
				}
			}

			CServerRegion* pMasterRgn = dynamic_cast<CServerRegion*>(pTeamMastter->GetFather());
			if(pMasterRgn && pMasterRgn->GetStrongPointFlag() == true) // 招募人在开启据点功能的场景
			{
				CServerRegion* pInvRgn = dynamic_cast<CServerRegion*>(pPlayer->GetFather());
				// 判断双方是否在同一个场景
				if(pInvRgn == pMasterRgn)
				{
					// 判断双方是否不同阵营
					if(pPlayer->GetCountry() % 2 != pTeamMastter->GetCountry() % 2)
					{
						pMsg->GetPlayer()->SendNotifyMessage(AppFrame::GetText("71"));
						return;
					}
				}
			}
			

			CMessage msg(MSG_S2W_TEAM_BeRecruited);
			msg.Add(PlayerID);
			msg.Add(TeamMastterID);

			msg.Send(false);
		}
		break;

	case MSG_C2S_TEAM_Recommend_ByName://! 推荐玩家
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				if (NULL_GUID != pPlayer->GetTeamID())
				{
					char szAimName[128] = {0};
					pMsg->GetStr(szAimName, 128);

					CMessage msg(MSG_S2W_TEAM_Recommend_ByName);
					msg.Add(szAimName);
					msg.Add(pPlayer->GetExID());

					msg.Send(false);
				}
			}
		}
		break;
	default:
		break;
	}
}



