#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\..\worldserver\game.h"
#include "..\..\appworld\player.h"
#include "..\..\appworld\session\csessionfactory.h"
#include "..\..\appworld\session\csession.h"
#include "../../Server/setup/GlobeSetup.h"
#include "../../Server/setup/LogSystem.h"
#include "..\..\appworld\Organizingsystem\OrganizingCtrl.h"
#include "..\..\appworld\Organizingsystem\WSTeam.h"



void OnGSTeamMessage(CMessage * pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_TEAM_QUERY_JOIN:
		GetOrganiCtrl()->OnQueryJoin(pMsg);
		break;
	case MSG_S2W_TEAM_QUERY_JOIN_BYNAME:
		{
			char szAimName[256] = {0};
			pMsg->GetStr(szAimName, 256);

			const CGUID& InviteeID	= GetGame() -> GetPlayerIDByName(szAimName);
			CGUID CaptainID;
			pMsg -> GetGUID(CaptainID);
			if(NULL_GUID == InviteeID)
			{
				GetOrganiCtrl()->SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimNotOnlineOrBeing));
				return;
			}

			CPlayer* pInvitee = GetGame() -> GetMapPlayer( InviteeID );
			CPlayer* pCaptain = GetGame() -> GetMapPlayer( CaptainID );

			GetOrganiCtrl()->OnQueryJoinByName(pInvitee, pCaptain);
		}	
		break;
	case MSG_S2W_TEAM_ANSWER_JOIN:
		GetOrganiCtrl()->OnAnswerJoin(pMsg);
		break;
	case MSG_S2W_TEAM_CHANGE_LEADER:
		GetOrganiCtrl()->OnChangLeader(pMsg);
		break;
	case MSG_S2W_TEAM_KICK_PLAYER:
		GetOrganiCtrl()->OnKickPlayer(pMsg);
		break;
	case MSG_S2W_TEAM_SetGoods:
		GetOrganiCtrl()->OnSetGoods(pMsg);
		break;
	case MSG_S2W_TEAM_RECRUIT:
		GetOrganiCtrl()->OnRecruit(pMsg);
		break;
	case MSG_S2W_TEAM_BeRecruited:
		GetOrganiCtrl()->OnBeRecruited(pMsg);
		break;
	case MSG_S2W_TEAM_CHAT:
		GetOrganiCtrl()->OnChat(pMsg);
		break;
	case MSG_S2W_TEAM_MemberAllIdioinfo:
		GetOrganiCtrl()->OnTeamMemberAllIdioinfo(pMsg);
		break;
	case MSG_S2W_TEAM_MemberPos:
		GetOrganiCtrl()->OnTeamMemberPos(pMsg);
		break;
	case MSG_S2W_TEAM_SwapRgnID:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_SwapRgnID);
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_RequestSwapRgnID:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_GetSrcSwapRgnID);
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_AddRgnID:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_AddRgnID);
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_DelRgnID:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_DelRgnID);
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_ClearRgnID:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_ClearRgnID);
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_GetRgnNum: //! 获取一个玩家的队伍副本数量
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_GetRgnNum);
				pMsg->Add(pMsg->GetSocketID());
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_ReturnGetRgnNum: //! 返回一个玩家的队伍副本数量
		{
			LONG lSocket = pMsg->GetLong();

			pMsg->SetType(MSG_W2S_TEAM_ReturnGetRgnNum);
			pMsg->SendToSocket(lSocket);
		}
		break;
	case MSG_S2W_TEAM_GetRgnGuid: //! 获取一个玩家的队伍副本GUID by Index
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_GetRgnGuid);
				pMsg->Add(pMsg->GetSocketID());
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_ReturnGetRgnGuid: //! 返回一个玩家的队伍副本GUID
		{
			LONG lSocket = pMsg->GetLong();

			pMsg->SetType(MSG_W2S_TEAM_ReturnGetRgnGuid);
			pMsg->SendToSocket(lSocket);
		}
		break;
	case MSG_S2W_TEAM_GetRgnTemplateID: //! 获取一个玩家的队伍副本模板ID by Index
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL != pPlayer)
			{
				pMsg->SetType(MSG_W2S_TEAM_GetRgnTemplateID);
				pMsg->Add(pMsg->GetSocketID());
				pMsg->SendToSocket(pPlayer->GetGsSocket());
			}
		}
		break;
	case MSG_S2W_TEAM_ReturnGetRgnTemplateID: //! 返回一个玩家的队伍副本模板ID
		{
			LONG lSocket = pMsg->GetLong();

			pMsg->SetType(MSG_W2S_TEAM_ReturnGetRgnTemplateID);
			pMsg->SendToSocket(lSocket);
		}
		break;
	case MSG_S2W_TEAM_INLVLDIFFERENCE:
		{
			CGUID ScriptGuid, TeamGuid;
			pMsg->GetGUID(ScriptGuid);
			pMsg->GetGUID(TeamGuid);
			LONG lSpace = pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			WSTeam *pTeam = (WSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamGuid));

			BOOL bRe = FALSE;
			if (NULL != pTeam)
			{
				bRe = pTeam->InLvlDifference(lSpace);
			}

			CMessage msg(MSG_W2S_TEAM_INLVLDIFFERENCE);
			msg.Add(ScriptGuid);
			msg.Add(szRet);
			msg.Add((LONG)bRe);
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
	case MSG_S2W_TEAM_INLVLSPACE:
		{
			CGUID ScriptGuid, TeamGuid;
			pMsg->GetGUID(ScriptGuid);
			pMsg->GetGUID(TeamGuid);
			LONG lMin = pMsg->GetLong();
			LONG lMax = pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			WSTeam *pTeam = (WSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamGuid));

			BOOL bRe = FALSE;
			if (NULL != pTeam)
			{
				bRe = pTeam->InLvlSpace(lMin, lMax);
			}

			CMessage msg(MSG_W2S_TEAM_INLVLSPACE);
			msg.Add(ScriptGuid);
			msg.Add(szRet);
			msg.Add((LONG)bRe);
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
	case MSG_S2W_TEAM_Recommend_ByName:
		{
			char szAimName[128] = {0};
			pMsg->GetStr(szAimName, 128);
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pAim = GetGame()->GetMapPlayer(szAimName);
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if(NULL == pAim || NULL == pPlayer)
				return;

			if( pAim->GetHideFlag() == 1 && !GetGame()->PlayerIsGM(pPlayer) ) // 隐匿状态
			{
				GetOrganiCtrl()->SendInfoToOneClient(PlayerGuid, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimHideName));
				return;
			}

			if (NULL_GUID != pAim->GetTeamID())
			{
				GetOrganiCtrl()->SendInfoToOneClient(PlayerGuid, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimHadTeam));
				return;
			}
			
			WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID());
			if (NULL == pTeam)
				return;

			if(PlayerGuid == pTeam->GetMasterID())
			{
				GetOrganiCtrl()->OnQueryJoinByName(pPlayer, pAim);
				return;
			}

			CPlayer *pMaster = GetGame()->GetMapPlayer(pTeam->GetMasterID());
			if (NULL == pMaster)
				return;

			CMessage msg(MSG_W2S_TEAM_Recommend_ByName);
			msg.Add(szAimName);
			msg.Add(pPlayer->GetName());
			msg.Add(pMaster->GetExID());
			msg.SendToSocket(pMaster->GetGsSocket());
		}
		break;
	default:
		break;
	}
}