
#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\..\worldserver\game.h"
#include "..\..\appworld\player.h"
#include "..\..\appworld\Organizingsystem\OrganizingCtrl.h"
#include "..\..\appworld\Organizingsystem\Union.h"
#include "..\..\appworld\Organizingsystem\Faction.h"
#include "..\..\appworld\Organizingsystem\organizingparam.h"
#include "../../../setup/LogSystem.h"
#include "..\Country\Country.h"
#include "..\Country\CountryHandler.h"
#include "..\..\appworld\Organizingsystem\WSTeam.h"
#include "../GlobalRgnManager.h"




void OnOrgasysMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2W_ORGSYS_CreateFaction_Reques: //! GS请求WS创建工会（条件检测和条件资源扣除已经完成）
		{
			GetOrganiCtrl()->OnGsCreateFaction(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_UpgradeFaction_Reques:
		{
			GetOrganiCtrl()->OnGsUpgradeFaction(pMsg); 
		}
		break;
	case MSG_S2W_ORGSYS_DisbandFaction_Reques:
		{
			CGUID DelFacGuid;
			pMsg->GetGUID(DelFacGuid);
			GetOrganiCtrl()->OnDelFaction(DelFacGuid, FALSE);
		}
		break;
	case MSG_S2W_Faction_InvitePlayer_Reques:
		{
			GetOrganiCtrl()->OnGsFacInvitePlayer(pMsg);
		}
		break;
	case MSG_S2W_Faction_AddMember:
		{
			GetOrganiCtrl()->OnGsFacAddMember(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_SetJobName_Request:
		{
			GetOrganiCtrl()->OnGsFacSetJobName(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_SetJobPurview_Request:
		{
			GetOrganiCtrl()->OnGsFacSetJobPurview(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_Quit_Request:
		{
			GetOrganiCtrl()->OnGsFacQuit(pMsg);
		}
		break;
	case MSG_S2W_Faction_SetPronounce_Reques:
		{
			GetOrganiCtrl()->OnGsSetPronounce(pMsg);
		}
		break;
	case MSG_S2W_Faction_SetLeaveword_Reques:
		{
			GetOrganiCtrl()->OnGsSetLeaveword(pMsg);
		}
		break;
	case MSG_S2W_Faction_Fire_Reques:
		{
			GetOrganiCtrl()->OnGsFire(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_SetMemberJob_Request:
		{
			GetOrganiCtrl()->OnGsSetMemberJob(pMsg);
		}
		break;
	case MSG_S2W_Faction_SetMemberTitle_Reques:
		{
			GetOrganiCtrl()->OnGsSetMemberTitle(pMsg);
		}
		break;
	case MSG_S2W_Faction_SetNewMaster_Reques:
		{
			GetOrganiCtrl()->OnGsSetNewMaster(pMsg);
		}
		break;
	case MSG_S2W_Faction_CheckRequisition_Reques:
		{
			GetOrganiCtrl()->OnGsCheckRequisition(pMsg);
		}
		break;
	case MSG_S2W_ORGSYS_JoinFac_Request:
		{
			GetOrganiCtrl()->OnGsJoinFac(pMsg);
		}
		break;
	case MSG_S2W_Faction_UpLoadIcon_Reques:
		{
			GetOrganiCtrl()->OnGsUpLoadIcon(pMsg);
		}
		break;
	case MSG_S2W_Faction_ChangeRes_Reques:						//! 修改资源请求
		{
			CGUID FacGuid, ScriptGuid;
			pMsg->GetGUID(FacGuid);
			LONG lDeltaRes = pMsg->GetLong();
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			CMessage msg(MSG_W2S_Faction_ChangeRes_Re);
			msg.Add(ScriptGuid);
			msg.Add(szRet);
	
			CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				if(pFaction->AddRes(lDeltaRes))
				{
					msg.Add(1L);
					msg.SendToSocket(pMsg->GetSocketID(), false);
					return;
				}
			}
			
			msg.Add(0L);
			msg.SendToSocket(pMsg->GetSocketID(), false);
		}
		break;
	case MSG_S2W_Faction_ChangeExp_Reques:						//! 修改经验请求
		{
			CGUID FacGuid, ScriptGuid;
			pMsg->GetGUID(FacGuid);
			LONG lDeltaExp = pMsg->GetLong();
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			CMessage msg(MSG_W2S_Faction_ChangeExp_Re);
			msg.Add(ScriptGuid);
			msg.Add(szRet);

			CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				if(pFaction->AddExp(lDeltaExp))
				{
					msg.Add(1L);
					msg.SendToSocket(pMsg->GetSocketID(), false);
					return;
				}
			}

			msg.Add(0L);
			msg.SendToSocket(pMsg->GetSocketID(), false);
		}
		break;
	case MSG_S2W_Faction_ChangeMemberContribute_Reques:			//! 修改成员贡献请求
		{
			CGUID FacGuid, PlayerGuid, ScriptGuid;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(PlayerGuid);
			LONG lDeltaContribute = pMsg->GetLong();
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			CMessage msg(MSG_W2S_Faction_ChangeMemberContribute_Re);
			msg.Add(ScriptGuid);
			msg.Add(szRet);

			CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				if(pFaction->AddMemberContribute(PlayerGuid, lDeltaContribute))
				{
					msg.Add(1L);
					msg.SendToSocket(pMsg->GetSocketID(), false);
					return;
				}
			}

			msg.Add(0L);
			msg.SendToSocket(pMsg->GetSocketID(), false);
		}
		break;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2W_UNION_Request_Create: //! 创建联盟请求
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL == pPlayer) return;

			char szNewUnionName[MAXSIZE_FactionName] = {0};
			pMsg->GetStr(szNewUnionName, MAXSIZE_FactionName);

			GetOrganiCtrl()->OnGsCreateUnion(pPlayer, szNewUnionName);
		}
		break;
	case MSG_S2W_UNION_Request_Disband:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL != pPlayer)
				GetOrganiCtrl()->OnGsDisbandUnion(pPlayer);
		}
		break;
	case MSG_S2W_UNION_Request_Quit:
		{
			CGUID FactionGuid;
			pMsg->GetGUID(FactionGuid);
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);

			GetOrganiCtrl()->OnGsQuitUnion(FactionGuid, UnionGuid);
		}
		break;
	case MSG_S2W_UNION_Request_Kick:
		{
			CGUID OptGuid;
			pMsg->GetGUID(OptGuid);
			CGUID AimGuid;
			pMsg->GetGUID(AimGuid);
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);

			GetOrganiCtrl()->OnGsKickUnion(OptGuid, AimGuid, UnionGuid);
		}
		break;
	case MSG_S2W_UNION_Request_SetChairman:
		{
			CGUID MasterGuid;
			pMsg->GetGUID(MasterGuid);
			CGUID AimFacGuid;
			pMsg->GetGUID(AimFacGuid);

			GetOrganiCtrl()->OnGsSetChairmanUnion(MasterGuid, AimFacGuid);
		}
		break;
	case MSG_S2W_UNION_Request_SetPurview:
		{
			CGUID MasterGuid;
			pMsg->GetGUID(MasterGuid);
			CGUID AimFacGuid;
			pMsg->GetGUID(AimFacGuid);
			DWORD dwPurview = pMsg->GetDWord();

			GetOrganiCtrl()->OnGsSetPurviewUnion(MasterGuid, AimFacGuid, dwPurview);
		}
		break;
	case MSG_S2W_UNION_Request_Invit:
		{
			CGUID MasterGuid;
			pMsg->GetGUID(MasterGuid);
			char szAimPlayerName[MAX_MEMBER_NAME_SIZE];
			pMsg->GetStr(szAimPlayerName, MAX_MEMBER_NAME_SIZE);

			GetOrganiCtrl()->OnGsUnionInvit(MasterGuid, szAimPlayerName);
		}
		break;
	case MSG_S2W_UNION_Request_AnswerInvit:
		{
			CGUID InviterGuid;
			pMsg->GetGUID(InviterGuid);
			CGUID RespondentGuid;
			pMsg->GetGUID(RespondentGuid);
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);
			LONG lResult = pMsg->GetLong();

			GetOrganiCtrl()->OnGsUnionAnswerInvit(InviterGuid, RespondentGuid, UnionGuid, lResult);
		}
		break;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_S2W_QUEST_PlayerAddQuest:
		{
			CGUID PlayerGuid;pMsg->GetGUID(PlayerGuid);
			DWORD dwQuestID  = pMsg->GetDWord();
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if(lGameServerID == 0)	break;

			CMessage msg(MSG_W2S_QUEST_PlayerAddQuest);
			msg.Add(PlayerGuid);
			msg.Add(dwQuestID);
			msg.SendGSID(lGameServerID);
		}
		break;
	case MSG_S2W_QUEST_PlayerRemoveQuest:
		{
			CGUID PlayerGuid;pMsg->GetGUID(PlayerGuid);
			DWORD dwQuestID  = pMsg->GetDWord();
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if(lGameServerID == 0)	break;

			CMessage msg(MSG_W2S_QUEST_PlayerRemoveQuest);
			msg.Add(PlayerGuid);
			msg.Add(dwQuestID);
			msg.SendGSID(lGameServerID);
		}
		break;
	case MSG_S2W_QUEST_PlayerRunScript:
		{
			CGUID PlayerGuid;pMsg->GetGUID(PlayerGuid);
			char str[256]={0};
			pMsg->GetStr(str,256);
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if(lGameServerID == 0)	break;

			CMessage msg(MSG_W2S_QUEST_PlayerRunScript);
			msg.Add(PlayerGuid);
			msg.Add(str);
			msg.SendGSID(lGameServerID);
		}
		break;
		//赏金任务消息
	case MSG_S2W_SHARE_QUESTLIST_UPDATE:
		{
			//玩家GUID
			CGUID guid=NULL_GUID;
			WSTeam::tagShareQuestList quest;
			pMsg->GetGUID(guid);
			BYTE btype=pMsg->GetByte();
			quest.lQuestID=pMsg->GetLong();
			pMsg->GetStr(quest.szName,20);
			//得到队伍GUID
			CGUID teamguid=GetOrganiCtrl()->FindPlayerTeamID(guid);
			WSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamguid);
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(guid);
			if(lGameServerID == 0)	break;
			if (pTeam)
			{
				if (0 == btype)
				{
					pTeam->UpdateShareQuestList(lGameServerID,&quest);
				}
				else
					pTeam->UpdateShareQuestList(lGameServerID,NULL);
			}
		}
		break;
	case MSG_S2W_SHARE_CURRQUEST:
		{
 			CGUID PlayerGuid=NULL_GUID;
			CGUID teamguid=NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(teamguid);
			long lQuestID=pMsg->GetLong();
			WSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamguid);
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if (pTeam)
			{
				if ( NULL_GUID!=pTeam->IsMaster(PlayerGuid) && pTeam->FindShareQuest(lQuestID) )
				{
					pTeam->SetCurrShareQuest(lGameServerID,lQuestID);
					//清空共享列表
					if(0 == lQuestID)
						pTeam->UpdateShareQuestList(lGameServerID,NULL);
					else
						pTeam->UpdateShareQuestList(lGameServerID,NULL,true);
				}	
			}
		}
		break;
	case MSG_S2W_SHARE_DELQUEST:
		{
			CGUID teamGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			long len = pMsg->GetLong();
			char *szName = (char*)M_ALLOC(len+1);
			pMsg->GetEx(szName,len);
			szName[len]='\0';
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(teamGuid);
			WSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamGuid);
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if (pTeam)
			{
				pTeam->DelCurrShareQuest(lGameServerID,szName);
			}
			M_FREE(szName,len+1);
		}
		break;
	case MSG_S2W_SHARE_SETMEMBERQUEST:
		{
			CGUID teamGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(teamGuid);
			long lQuestID=pMsg->GetLong();
			WSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamGuid);
			long lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
			if (pTeam)
			{
				pTeam->SetMemberQuest(lGameServerID,PlayerGuid,lQuestID);
			}
		}
		break;
	case MSG_S2W_Faction_ChangeTickets:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);
			LONG lTickets=pMsg->GetLong();
			
			CMessage msg(MSG_W2S_Faction_ChangeTickets_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(cRet);
			msg.Add(cBaseName);
			long leftTickets=0;
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			BYTE bySucc=0;
			if (pFaction)
			{
				leftTickets=pFaction->AddTickets(cBaseName,lTickets);
				//设置成功
				if(leftTickets>=0)
					bySucc=1;
			}
			msg.Add(bySucc);
			msg.Add(leftTickets);
			msg.SendAll();
		}
		break;
	case MSG_S2W_Faction_ChangeBase:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);

			CMessage msg(MSG_W2S_Faction_ChangeBase_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(cRet);
			msg.Add(cBaseName);
			BYTE bySucc=0;
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction)
			{
				char succ=(BYTE)pFaction->ChangeBaseInfo(cBaseName);
				if (succ>=0)
					bySucc=1;
			}
			msg.Add(bySucc);
			msg.SendAll();
		}
		break;
	case MSG_S2W_Faction_ChangeBattle_Reques:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			LONG lValue=pMsg->GetLong();
			pMsg->GetGUID(PlayerGuid);
			CMessage msg(MSG_W2S_Faction_ChangeBattle_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(PlayerGuid);
			msg.Add(cRet);
			
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			BYTE bySucc=0;
			DWORD leftBattle=0;
			if (pFaction)
			{
				leftBattle=pFaction->AddBattle(lValue,PlayerGuid);
				//操作成功
				if (leftBattle>=0)
					bySucc=1;
			}
			msg.Add(bySucc);
			msg.Add(leftBattle);
			msg.SendAll();
		}
		break;
	case MSG_S2W_Faction_ChangeArmyInvest_Reques:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			LONG lValue=pMsg->GetLong();
			//修改成员值,为空则修改公会值
			pMsg->GetGUID(PlayerGuid);
			CMessage msg(MSG_W2S_Faction_ChangeArmyInvest_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(PlayerGuid);
			msg.Add(cRet);
			
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			BYTE bySucc=0;
			DWORD leftArmyInvest=0;
			if (pFaction)
			{
				leftArmyInvest=pFaction->AddArmyInvest(lValue,PlayerGuid);
				//操作成功
				if (leftArmyInvest>=0)
					bySucc=1;
			}
			msg.Add(bySucc);
			msg.Add(leftArmyInvest);
			msg.SendAll();
		}
		break;
	case MSG_S2W_Faction_ChangeJoinBattle:
		{
			//被修改参与权的成员GUID
			CGUID PlayerGuid=NULL_GUID;
			CGUID FacGuid=NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(FacGuid);
			BYTE bJoin=pMsg->GetByte();
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction==NULL)
				break;
			pFaction->SetJoin(PlayerGuid,(LONG)bJoin);
			CMessage msg(MSG_W2S_Faction_ChangeJoinBattle_Re);
			msg.Add(FacGuid);
			msg.Add(PlayerGuid);
			msg.Add(bJoin);
			msg.SendAll();
		}
		break;
		//据点的战功值修改
	case MSG_S2W_Faction_ChangeBaseBattle:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);
			LONG lBaseBattle=pMsg->GetLong();
			
			CMessage msg(MSG_W2S_Faction_ChangeBaseBattle_re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(cRet);
			msg.Add(cBaseName);
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			BYTE bSucc=0;
			long leftBaseBattle=0;
			if (pFaction)
			{
				leftBaseBattle=pFaction->AddBaseBattle(cBaseName,lBaseBattle);
				if(leftBaseBattle>=0)
					bSucc=1;
			}
			msg.Add(bSucc);
			msg.Add(leftBaseBattle);
			msg.SendAll();
		}
		break;
	case MSG_S2W_Faction_ChangeTechLevel:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			LONG lTechNo=pMsg->GetLong();
			LONG lLv=pMsg->GetLong();
			BYTE bSucc=0;

			CMessage msg(MSG_W2S_Faction_ChangeTechLevel_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(cRet);
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction)
			{
				if(pFaction->AddTechLv(lTechNo,lLv))
					bSucc=1;
			}
			msg.Add(lTechNo);
			msg.Add(lLv);
			msg.Add(bSucc);
			msg.SendAll();
			
		}
		break;
	case MSG_S2W_Faction_ChangeEnemy:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char szFacName[32]={0};
			pMsg->GetStr(szFacName,32);
			LONG lTime=pMsg->GetLong();
			LONG lBattleType=pMsg->GetLong();
			LONG lFlag=pMsg->GetLong();
			BYTE bSucc=0;

			CMessage msg(MSG_W2S_Faction_ChangeEnemy_Re);
			msg.Add(FacGuid);
			msg.Add(ScriptGuid);
			msg.Add(cRet);
			CFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			DWORD dwStartTime=0;
			if (pFaction)
			{
				
				SYSTEMTIME st;
				GetLocalTime(&st);
				//当天剩余秒数
				DWORD dwleftTime=86400-(st.wHour*60*60+st.wMinute*60+st.wSecond);
				//战争开始时间=报名前UTC时间+报名当天剩余秒数+间隔天数（秒）+战争开始当天的时间（秒）
				dwStartTime=time(NULL)+dwleftTime+(lTime-1)*24*60*60+
					WSOrganizingParam::getInstance().GetGlobe().lBaseBattleStart*60*60;
				if(pFaction->ChangeEnemy((eBattleType)lBattleType,szFacName,dwStartTime,lFlag))
					bSucc=1;

			}
			msg.Add(szFacName);
			msg.Add(dwStartTime);
			msg.Add(dwStartTime+ 60*60*WSOrganizingParam::getInstance().GetGlobe().lBaseBattleLast);
			msg.Add(lBattleType);
			msg.Add(lFlag);
			msg.Add(bSucc);
			msg.SendAll();
		}
		break;
	}
}