#include "StdAfx.h"
#include "../Player.h"
#include "../Session/SessionFactory.h"
#include "../Session/Session.h"
#include "../OrganizingSystem/OrganizingCtrl.h"
#include "../OrganizingSystem/WSTeam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

			const CGUID& InviteeID	= GetGame().GetPlayerIDByName(szAimName);
			CGUID CaptainID;
			pMsg->GetGUID(CaptainID);
			if(NULL_GUID == InviteeID)
			{
				GetOrganiCtrl()->SendInfoToOneClient(CaptainID, AppFrame::GetText("GS_TEAM_AimNotOnlineOrBeing"));
				return;
			}

			CPlayer* pInvitee = GetGame().GetMapPlayer( InviteeID );
			CPlayer* pCaptain = GetGame().GetMapPlayer( CaptainID );

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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
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

			CPlayer *pAim = GetGame().GetMapPlayer(szAimName);
			CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
			if(NULL == pAim || NULL == pPlayer)
				return;

			if( pAim->GetHideFlag() == 1 && !GetGame().PlayerIsGM(pPlayer) ) // 隐匿状态
			{
				GetOrganiCtrl()->SendInfoToOneClient(PlayerGuid, AppFrame::GetText("GS_TEAM_AimHideName"));
				return;
			}

			if (NULL_GUID != pAim->GetTeamID())
			{
				GetOrganiCtrl()->SendInfoToOneClient(PlayerGuid, AppFrame::GetText("GS_TEAM_AimHadTeam"));
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

			CPlayer *pMaster = GetGame().GetMapPlayer(pTeam->GetMasterID());
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

/*
void OnTeamMessage(CMessage * pMsg)
{

	switch(pMsg->GetType())
	{
	case MSG_S2W_TEAM_CREATE:
		{
			//LONG lSessionType = pMsg->GetLong();
			BYTE SessionData[102400];
			pMsg->GetEx(SessionData,102400);
			long lPos = 0;
			CSessionFactory::UnserializeSession( SessionData,lPos);
		}
		break;
	case MSG_S2W_TEAM_DESTORY:
		{
			DWORD dwTeamID = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) );
			if( pSession )
			{
				pSession->End();
			}
			//LONG lSessionID = GetGame() ->getteam
			//CSession* pSession = CSessionFactory::QuerySession( 
		}
		break;
	case MSG_S2W_TEAM_TEAMATE_JOIN:
		{

			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				CHAR szName[256];
				LONG lOwnerType		= pMsg->GetLong();
				LONG lOwnerID		= pMsg->GetLong();
				LONG lOwnerRegion	= pMsg->GetLong();
				pMsg->GetStr( szName, 256 );

				if( pSession->QueryPlugByOwner(lOwnerType, lOwnerID) == NULL )
				{
					LONG lPlugID = CSessionFactory::CreatePlug( CSessionFactory::PT_TEAMATE, lOwnerType, lOwnerID );
					CTeamate* pPlug = dynamic_cast<CTeamate*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID) ) );
					if( pPlug )
					{
						pPlug->SetOwnerRegionID( lOwnerRegion );
						pPlug->SetOwnerName( szName );
						CSessionFactory::InsertPlug( lSessionID, lPlugID );
					}
				}
			}
		}
		break;

	case MSG_S2W_TEAM_TEAMATE_LEAVE:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lOwnerType		= pMsg->GetLong();
				LONG lOwnerID		= pMsg->GetLong();

				CPlug* pPlug = pSession->QueryPlugByOwner( lOwnerType, lOwnerID );
				if( pPlug )
				{
					pPlug->Exit();
				}
			}
		}
		break;

	case MSG_S2W_TEAM_CHANGE_LEAER:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lNewLeaderID = pMsg->GetLong();
				CPlug* pPlug = pSession->QueryPlugByOwner( TYPE_PLAYER, lNewLeaderID );
				if( pPlug )
				{
					pSession->SetLeader( lNewLeaderID );
				}
			}
		}
		break;
	case MSG_S2W_TEAM_KICK_PLAYER:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lPlayerID = pMsg->GetLong();
				pSession->KickPlayer( lPlayerID );
			}
		}
		break;
	case MSG_S2W_TEAM_TEAMATE_CHANGE_REGION:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lOwnerType		= pMsg->GetLong();
				LONG lOwnerID		= pMsg->GetLong();
				LONG lNewRegionID	= pMsg->GetLong();

				CTeamate* pPlug = dynamic_cast<CTeamate*>( pSession->QueryPlugByOwner(lOwnerType, lOwnerID) );
				if( pPlug )
				{
					pPlug->SetOwnerRegionID( lNewRegionID );
				}
			}
		}
		break;

	case MSG_S2W_TEAM_CHANGE_STATE:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lOwnerType		= pMsg->GetLong();
				LONG lOwnerID		= pMsg->GetLong();
				FLOAT fHp			= pMsg->GetFloat();

				CTeamate* pPlug = dynamic_cast<CTeamate*>( pSession->QueryPlugByOwner(lOwnerType, lOwnerID) );
				if( pPlug )
				{
					pSession->OnPlugChangeState( pPlug->GetID(), CTeamate::TS_CHANGE_STATE, reinterpret_cast<UCHAR*>(&fHp) );
				}
			}
		}
		break;
	case MSG_S2W_TEAM_CHAT:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				LONG lOwnerType	= pMsg->GetLong();
				LONG lOwnerID	= pMsg->GetLong();
				CPlug* pPlug	= pSession->QueryPlugByOwner( lOwnerType, lOwnerID );
				if( pPlug )
				{
					CHAR szMsg[512];
					pMsg->GetStr( szMsg, 512 );
					pSession->OnPlugChangeState( pPlug->GetID(), CTeamate::TS_CHAT, reinterpret_cast<UCHAR*>(szMsg) );
				}
			}
		}
		break;
	case MSG_S2W_TEAM_CHANGE_ALLOCATION_SCHEME:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				CTeam::ALLOCATION_SCHEME as = static_cast<CTeam::ALLOCATION_SCHEME>( pMsg->GetLong() );
				if( as != pSession->GetAllocationScheme() &&
					as <= CTeam::AS_SHARE )
				{
					pSession->SetAllocationScheme( as );
				}
			}
		}
		break;
	case MSG_S2W_TEAM_QUEST_SYNCHRONIZE:
		{
			DWORD dwTeamID  = pMsg->GetDWord();
			LONG lSessionID = GetGame().GetTeamSessionID( dwTeamID );
			CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			if( pSession )
			{
				vector<BYTE> vOut;
				pSession->Serialize( &vOut );

				CMessage msg( MSG_W2S_TEAM_SYNCHRONIZE );
				msg.AddEx( &vOut[0], static_cast<LONG>( vOut.size() ) );
				msg.SendToSocket( pMsg->GetSocketID() );
			}
		}
		break;

	case MSG_S2W_TEAM_QUERY_PLAYER_RESULT:
		{
			LONG lPlugID	= pMsg->GetLong();
			LONG lOwnerType	= pMsg->GetLong();
			LONG lOwnerID	= pMsg->GetLong();
			BOOL bResult	= pMsg->GetLong();
			CTeamate* pTeamate = dynamic_cast<CTeamate*>( const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID) ) );
			if( pTeamate )
			{
				if( bResult )
				{
					pTeamate->PlayerStillExisted();
				}
			}
		}
		break;
	}
}
*/