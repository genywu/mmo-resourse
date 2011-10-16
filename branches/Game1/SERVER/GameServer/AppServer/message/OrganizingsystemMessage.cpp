//OrganizingsystemMessage.cpp
#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "..\serverregion.h"
#include "..\..\..\Setup\QuestSystem.h"
#include "..\script\script.h"
#include "..\script\VariableList.h"


// [2007-7-6-add]
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GameFaction.h"
#include "../OrganizingSystem/GameUnion.h"
// [2007-7-6-add end]
#include "../RgnManager.h"
#include "../../GameServer/GameManager.h"
#include "../Organizingsystem/GSTeam.h"


void OnOrgasysMessage(CMessage* pMsg)
{
	if(0 == CMessage::GetWSSocket())
	{
		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		msg.Add( (DWORD)0xffff0000 );
		msg.Add( (DWORD)0 );
		msg.Add((BYTE)eNOTIFYPOS_CENTER);
		msg.Add( CStringReading::LoadString(209, 70) );
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	switch(pMsg->GetType())
	{
		//! 2008 04 16 添加
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_W2S_ORGSYS_InitGsFaction: //! 初始化GS工会系统
		{
			GetOrganiCtrl()->OnInitGsFaction(pMsg);
		}
		break;

	case MSG_W2S_ORGSYS_CreateFaction_Re: //! 回复工会创建
		{
			GetOrganiCtrl()->OnCreateFactionRe(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_InitPlayerFacSys:
		{
		}
		break;
	case MSG_W2S_ORGSYS_DelFaction_Notify:
		{
			GetOrganiCtrl()->OnWsDelFaction(pMsg);
		}
		break;
	case MSG_W2S_Faction_InvitePlayer_Re:
		{
			GetOrganiCtrl()->OnWsInvitePlayerRe(pMsg);
		}
		break;
	case MSG_W2S_Faction_AddMember_Notify:
		{
			GetOrganiCtrl()->OnWsAddMember(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_SetJobName_Notify:
		{
			GetOrganiCtrl()->OnWsSetJobName(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_SetJobPurview_Notify:
		{
			GetOrganiCtrl()->OnWsSetJobPurview(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_Quit_Notify:
		{
			GetOrganiCtrl()->OnWsMemberQuit(pMsg);
		}
		break;
	case MSG_W2S_Faction_EditPronounce_Notify:
		{
			GetOrganiCtrl()->OnWsEditPronounce(pMsg);
		}
		break;
	case MSG_W2S_Faction_EditLeaveword_Notify:
		{
			GetOrganiCtrl()->OnWsEditLeaveword(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_Fire_Notify:
		{
			GetOrganiCtrl()->OnWsFire(pMsg);
		}
		break;
	case MSG_W2S_ORGSYS_SetMemJobLvl_Notify:
		{
			GetOrganiCtrl()->OnWsSetMemJobLvl(pMsg);
		}
		break;
	case MSG_W2S_Faction_SetMemberTitle_Notify:
		{
			GetOrganiCtrl()->OnWsSetMemberTitle(pMsg);
		}
		break;
	case MSG_W2S_Faction_MemberMap_Notify:
		{
			GetOrganiCtrl()->OnWsMemberMap(pMsg);
		}
		break;
	case MSG_W2S_Faction_MemberLevel_Notify:
		{
			GetOrganiCtrl()->OnWsMemberLevel(pMsg);
		}
		break;
	case MSG_W2S_Faction_MemberHide_Notify:
		{
			GetOrganiCtrl()->OnWsMemberHide(pMsg);
		}
		break;
	case MSG_W2S_Faction_MemberOccu_Notify:
		{
			GetOrganiCtrl()->OnWsMemberOccu(pMsg);
		}
		break;
	case MSG_W2S_Faction_SetMaster_Notify:
		{
			GetOrganiCtrl()->OnWsSetMaster(pMsg);
		}
		break;
	case MSG_W2S_Faction_OnlineChange_Notify:					//! 成员在线状态改变
		{
			GetOrganiCtrl()->OnWsMemOnlineChange(pMsg);
		}
		break;
	case MSG_W2S_Faction_Applicant_Notify:
		{
			GetOrganiCtrl()->OnWsApplicantChange(pMsg);
		}
		break;
	case MSG_W2S_Faction_Join_Re:
		{
			pMsg->GetLong();
			pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pMsg->SetType(MSG_S2C_ORGSYS_JoinFac_Re);
			pMsg->SendToPlayer(PlayerGuid);
		}
		break;
	case MSG_W2S_Faction_LvlChange_Notify: //! 工会等级改变通知
		{
			GetOrganiCtrl()->OnWsFacLvlChange(pMsg);
		}
		break;
	case MSG_W2S_Faction_IconChange_Notify:
		{
			GetOrganiCtrl()->OnWsIconChange(pMsg);
		}
		break;
	case MSG_S2W_Faction_ChangeRes_Reques:					//! 修改资源请求
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				LONG lDeltaRes = pMsg->GetLong();//! 这个值GS不使用
				LONG lCurrRes = pMsg->GetLong();
				pFaction->SetRes(lCurrRes);
			}	
		}
		break;
	case MSG_S2W_Faction_ChangeExp_Reques:					//! 修改经验请求
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				LONG lDeltaExp = pMsg->GetLong();//! 这个值GS不使用
				LONG lCurrExp = pMsg->GetLong();
				pFaction->SetExp(lCurrExp);
			}
		}
		break;
	case MSG_S2W_Faction_ChangeMemberContribute_Reques:		//! 修改成员贡献请求
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if(NULL != pFaction)
			{
				CGUID MemberGuid;
				pMsg->GetGUID(MemberGuid);
				LONG lDeltaContribute = pMsg->GetLong();//! 这个值GS不使用
				LONG lCurrContribute = pMsg->GetLong();
				pFaction->SetMemberContribute(MemberGuid, lCurrContribute);
			}
		}
		break;
	case MSG_W2S_Faction_ChangeRes_Re:						//! 修改资源回复
	case MSG_W2S_Faction_ChangeExp_Re:						//! 修改经验回复
	case MSG_W2S_Faction_ChangeMemberContribute_Re:			//! 修改成员贡献回复
		{
			CGUID ScriptGuid;
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);
			LONG lRet = pMsg->GetLong();

			//取消挂起的脚本
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddVar(szRet, lRet);
			}
			ContinueRunScript(ScriptGuid, stackVarList);
		}
		break;

	case MSG_W2S_Faction_ChangeRes_Notify:					//! 修改资源通知
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (NULL == pFaction) return;

			LONG lDeltaValue = pMsg->GetLong();
			LONG lCurrValue = pMsg->GetLong();
			pFaction->SetRes(lCurrValue);

			CMessage msg(MSG_S2C_Faction_ChangeRes_Notify);
			msg.Add(lCurrValue);
			pFaction->SendToAllConcern(msg);

		}
		break;
	case MSG_W2S_Faction_ChangeExp_Notify:					//! 修改经验通知
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (NULL == pFaction) return;

			LONG lDeltaValue = pMsg->GetLong();
			LONG lCurrValue = pMsg->GetLong();
			pFaction->SetExp(lCurrValue);

			CMessage msg(MSG_S2C_Faction_ChangeExp_Notify);
			msg.Add(lCurrValue);
			pFaction->SendToAllConcern(msg);
		}
		break;
	case MSG_W2S_Faction_ChangeMemberContribute_Notify:		//! 修改成员贡献通知
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (NULL == pFaction) return;

			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			LONG lDeltaValue = pMsg->GetLong();
			LONG lCurrValue = pMsg->GetLong();
			pFaction->SetMemberContribute(PlayerGuid, lCurrValue);

			CMessage msg(MSG_S2C_Faction_ChangeMemberContribute_Notify);
			msg.Add(PlayerGuid);
			msg.Add(lCurrValue);
			pFaction->SendToAllConcern(msg);
		}
		break;
	case MSG_W2S_Faction_BoonRun_Notify:
		{
			GetOrganiCtrl()->OnBoonRun();
		}
		break;

	//! ------------------------------------------------------------------------------------------------------
	case MSG_C2S_ORGSYS_CreateFaction_Reques:
		{
			GetOrganiCtrl()->OnPlayerCreateFactionReques(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_DisbandFaction_Request:
		{
			GetOrganiCtrl()->OnPlayerDisbandFactionReques(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_Quit_Request: //! 退出行会请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
				{
                    //检查场景允许否
                    if (GetOrganiCtrl()->TestRegionForbidMember(pPlayer))
                    {
                        //不允许
                     /*   CMessage msg(MSG_S2C_ORGSYS_Quit_Re);
                        msg.Add(PLAYER_OPT_FAILED);
                        msg.Add((LONG)eFOEI_Err_RegionForbid);
                        msg.SendToPlayer(pPlayer->GetExID(), false);*/
                        return;
                    }

					if (pFaction->GetMaster() != pPlayer->GetExID())
					{
						CMessage msg(MSG_S2W_ORGSYS_Quit_Request);
						msg.Add(pFaction->GetExID());
						msg.Add(pPlayer->GetExID());
						msg.Send();
					}
				}
			}
		}
		break;
	case MSG_C2S_ORGSYS_Kick_Request: //! 踢出行会请求
		{
			GetOrganiCtrl()->OnPlayerKickReques(pMsg);
		}
		break;
	case MSG_C2S_Faction_UpLoadIcon_Request: //! 上传工会图标请求
		{
			GetOrganiCtrl()->OnPlayerUpLoadIconReques(pMsg);
		}
		break;
	case MSG_C2S_FacData_Request: //! 行会基本资料请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
				{
					pFaction->AddAttention(pPlayer->GetExID());

					//! public资料
					CMessage msg(MSG_S2C_Faction_PublicData_Notify);
					pFaction->AddPublicData_ToMsgBack(&msg);
					msg.SendToPlayer(pPlayer->GetExID());
					//! base资料
					CMessage msgB(MSG_S2C_Faction_BaseData_Notify);
					pFaction->AddBaseData_ToMsgBack(&msgB);
					msgB.SendToPlayer(pPlayer->GetExID());	
					//! 会员资料
					CMessage msgM(MSG_S2C_Faction_Member_Notify);
					pFaction->AddMember_ToMsgBack(&msgM, pPlayer->GetExID());
					msgM.SendToPlayer(pPlayer->GetExID());
					//! 申请者资料
					CMessage msgA(MSG_S2C_Faction_Apply_Notify);
					pFaction->AddApply_ToMsgBack(&msgA);
					msgA.SendToPlayer(pPlayer->GetExID());
					//! 公告
					CMessage msgP(MSG_S2C_Faction_Placard_Notify);
					pFaction->AddPronounce_ToMsgBack(&msgP);
					msgP.SendToPlayer(pPlayer->GetExID());
					//! 留言
					CMessage msgL(MSG_S2C_Faction_Leaveword_Notify);
					pFaction->AddLeaveWord_ToMsgBack(&msgL);
					msgL.SendToPlayer(pPlayer->GetExID());
					//! 据点战门票记录
					CMessage msgTickets(MSG_S2C_Faction_Beachheads_Notify);
					pFaction->AddBaseInfoToMsg(&msgTickets);
					msgTickets.SendToPlayer(pPlayer->GetExID());
					//! 敌对家族信息
					CMessage msgEnemy(MSG_S2C_Faction_ChangeEnemy_Notify);
					pFaction->AddEnemyInfoToMsg(&msgEnemy);
					msgEnemy.SendToPlayer(pPlayer->GetExID());
				}
			}
		}
		break;
	case MSG_C2S_Faction_AllIconData_Request:
		{
		}
		break;
	case MSG_C2S_Faction_IconData_Request:
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (NULL != pFaction)
			{
				if(0 != pFaction->GetIconCRC32())
				{
					vector<BYTE> vIconData;
					pFaction->GetIcon(vIconData);
					CMessage msgRadiate(MSG_S2C_Faction_IconData_Notify);
					msgRadiate.Add(FacGuid);
					msgRadiate.Add((LONG)vIconData.size());
					msgRadiate.AddEx(&vIconData[0], vIconData.size());
					msgRadiate.SendToSocket(pMsg->GetSocketID());
				}	
			}
		}
		break;
	case MSG_C2S_ORGSYS_SetPlacard_Request: //! 修改公告请求
		{
			GetOrganiCtrl()->OnPlayerFacSetPlacardRequest(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_SetLeaveword_Request: //! 修改备忘请求
		{
			GetOrganiCtrl()->OnPlayerFacSetLeavewordRequest(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_CloseFacBoard_Notifye: //! 玩家关闭工会界面通知
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
					pFaction->DelAttention(pPlayer->GetExID());
			}
		}
		break;
	case MSG_C2S_ORGSYS_SetJobName_Request: //! 修改官阶名字请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
				{
					if (pPlayer->GetExID() == pFaction->GetMaster())
					{
						LONG lJobLvl = pMsg->GetLong();
						if (0 < lJobLvl && MAXNUM_JobLvl >= lJobLvl)
						{
							char szNewName[MAX_MEMBER_NAME_SIZE] = {0};
							pMsg->GetStr(szNewName, MAX_MEMBER_NAME_SIZE);
							CMessage msg(MSG_S2W_ORGSYS_SetJobName_Request);
							msg.Add(pFaction->GetExID());
							msg.Add(lJobLvl);
							msg.Add(szNewName);
							msg.Send();
						}
					}
				}
			}
		}
		break;
	case MSG_C2S_ORGSYS_SetJobPurview_Request: //! 修改官阶权限请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
				{
					if (pPlayer->GetExID() == pFaction->GetMaster())
					{
						LONG lJobLvl = pMsg->GetLong();
						if (0 < lJobLvl && MAXNUM_JobLvl >= lJobLvl)
						{
							LONG lPurview = pMsg->GetLong();
							if(0 != ((~eMP_MemberTiptop) & lPurview))
								return;
							CMessage msg(MSG_S2W_ORGSYS_SetJobPurview_Request);
							msg.Add(pFaction->GetExID());
							msg.Add(lJobLvl);
							msg.Add(lPurview);
							msg.Send();
						}
					}
				}
			}
		}
		break;
	case MSG_C2S_ORGSYS_SetMemberJob_Request: //! 设置成员官阶请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (NULL != pPlayer)
			{
				GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
				if (NULL != pFaction)
				{
					CGUID MemberGuid;
					pMsg->GetGUID(MemberGuid);

					if (pPlayer->GetExID() == pFaction->GetMaster() && pPlayer->GetExID() != MemberGuid)
					{
						if (NULL_GUID != pFaction->IsMember(MemberGuid))
						{
							LONG lJobLvl = pMsg->GetLong();
							if (0 <= lJobLvl && MAXNUM_JobLvl >= lJobLvl)
							{
								CMessage msg(MSG_S2W_ORGSYS_SetMemberJob_Request);
								msg.Add(pFaction->GetExID());
								msg.Add(MemberGuid);
								msg.Add(lJobLvl);
								msg.Send();
							}
						}
					}
				}
			}
		}
		break;
	case MSG_C2S_Faction_InvitePlayer_Request: //! 邀请玩家加入请求
		{
			GetOrganiCtrl()->OnPlayerFacInviteReques(pMsg);
		}
		break;
	case MSG_C2S_Faction_InvitePlayer_Answer: //! 回答邀请
		{
			GetOrganiCtrl()->OnPlayerFacInviteAnswer(pMsg);
		}
		break;
	case MSG_C2S_Faction_SetMemberTitle_Request: //! 设置成员称号请求
		{
			GetOrganiCtrl()->OnPlayerFacSetMemberTitleReques(pMsg);
		}
		break;
	case MSG_C2S_Faction_SetChairman_Request: //! 传位请求
		{
			GetOrganiCtrl()->OnPlayerFacSetChairmanReques(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_CheckRequisition_Request:
		{
			GetOrganiCtrl()->OnPlayerFacCheckRequisition(pMsg);
		}
		break;
	case MSG_C2S_ORGSYS_JoinFac_Request:
		{
			GetOrganiCtrl()->OnPlayerJoinFac(pMsg);
		}
		break;
		//! 联盟
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_W2S_Union_InitGs:
		{
			LONG lDataSize = pMsg->GetLong();
			vector<BYTE> vData(lDataSize + 1);
			pMsg->GetEx(&vData[0], lDataSize);

			GetOrganiCtrl()->OnInitGsUnion(vData);
		}
		break;
	case MSG_C2S_UNION_Request_Creation:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL == pPlayer) return;

			char szNewUnionName[MAXSIZE_FactionName] = {0};
			pMsg->GetStr(szNewUnionName, MAXSIZE_FactionName);

			GetOrganiCtrl()->OnPlayerCreateUnion(pPlayer, szNewUnionName);
		}
		break;
	case MSG_W2S_Union_Create_Notify:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			LONG lResult = pMsg->GetLong();
			if(PLAYER_OPT_FAILED == lResult)
			{
				CMessage msgRe(MSG_S2C_UNION_Re_Creation);
				msgRe.Add(PLAYER_OPT_FAILED);
				msgRe.Add(pMsg->GetLong());
				msgRe.SendToPlayer(PlayerGuid, false);
				return;
			}

			LONG lDataSize = pMsg->GetLong();
			vector<BYTE> vData(lDataSize + 1);
			pMsg->GetEx(&vData[0], lDataSize);

			GetOrganiCtrl()->OnCreateNewUnion(vData);
		}
		break;
	case MSG_C2S_UNION_Request_Disband:					//! 解散联盟请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerDisbandUnion(pPlayer);
		}
		break;
	case MSG_W2S_Union_Disband_Notify:
		{
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);
			
			GetOrganiCtrl()->OnWsDisbandUnion(UnionGuid);
		}
		break;
	case MSG_C2S_UNION_Request_Quit:					//! 行会退出联盟请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerQuitUnion(pPlayer);
		}
		break;
	case MSG_W2S_Union_Quit_Notify:
		{
			CGUID FactionGuid;
			pMsg->GetGUID(FactionGuid);
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);

			GetOrganiCtrl()->OnWsQuitUnion(FactionGuid, UnionGuid);
		}
		break;
	case MSG_C2S_UNION_Request_Kick:					//! 将行会踢出联盟请求
		{
			CGUID AimFactionGuid;
			pMsg->GetGUID(AimFactionGuid);
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerKick(pPlayer, AimFactionGuid);
		}
		break;
	case MSG_W2S_Union_Kick_Notify:
		{
			CGUID OptGuid;
			pMsg->GetGUID(OptGuid);
			CGUID AimGuid;
			pMsg->GetGUID(AimGuid);
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);

			GetOrganiCtrl()->OnWsKickUnion(OptGuid, AimGuid, UnionGuid);
		}
		break;
	case MSG_C2S_UNION_Request_SetChairman:				//! 盟主转让请求
		{
			CGUID AimFactionGuid;
			pMsg->GetGUID(AimFactionGuid);
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerSetChairman(pPlayer, AimFactionGuid);
		}
		break;
	case MSG_S2W_Union_SetChairman_Notify:
		{
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);
			CGUID AimFacGuid;
			pMsg->GetGUID(AimFacGuid);
			char szNewMasterName[MAX_MEMBER_NAME_SIZE] = {0};
			pMsg->GetStr(szNewMasterName, MAX_MEMBER_NAME_SIZE);

			GetOrganiCtrl()->OnWsSetChairman(UnionGuid, AimFacGuid, szNewMasterName);
		}
		break;
	case MSG_C2S_UNION_Request_SetRights:				//! 盟主赋予成员权限请求
		{
			CGUID AimFactionGuid;
			pMsg->GetGUID(AimFactionGuid);
			DWORD dwPurview = pMsg->GetDWord();
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerSetPurview(pPlayer, AimFactionGuid, dwPurview);
		}
		break;
	case MSG_S2W_Union_SetPurview_Notify:
		{
			CGUID UnionGuid;
			pMsg->GetGUID(UnionGuid);
			CGUID AimFacGuid;
			pMsg->GetGUID(AimFacGuid);
			DWORD dwPurview = pMsg->GetDWord();

			GetOrganiCtrl()->OnWsSetPurviewUnion(UnionGuid, AimFacGuid, dwPurview);
		}
		break;
	case MSG_C2S_UNION_Request_Invite:					//! 邀请行会会长 加入联盟的发送请求
		{
			char szAimPlayerName[MAX_MEMBER_NAME_SIZE];
			pMsg->GetStr(szAimPlayerName, MAX_MEMBER_NAME_SIZE);
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
				GetOrganiCtrl()->OnPlayerUnionInvite(pPlayer, szAimPlayerName);
		}
		break;
	case MSG_S2W_Union_Invit_Re:
		{
			//! 条过前面的消息部分...
			if(PLAYER_OPT_SUCCEED == pMsg->GetLong())
			{
				char szTmp[MAX_MEMBER_NAME_SIZE] = {0};
				pMsg->GetStr(szTmp, MAX_MEMBER_NAME_SIZE);
			}
			else
				pMsg->GetLong();
			//! ...找出消息接收者
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			//! 转发消息
			pMsg->SetType(MSG_S2C_UNION_Response_Invite);
			pMsg->SendToPlayer(PlayerGuid, false);
		}
		break;
	case MSG_S2W_Union_TransferInvit:
		{
			CGUID PlayerGuid;
			char szTemp[MAX_MEMBER_NAME_SIZE];
			//! 条过前面的消息部分...
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetStr(szTemp, MAX_MEMBER_NAME_SIZE);
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetStr(szTemp, MAX_MEMBER_NAME_SIZE);
			//! ...找出消息接收者
			pMsg->GetGUID(PlayerGuid);

			//! 转发消息
			pMsg->SetType(MSG_S2C_UNION_Notify_Anwser_Invite);
			pMsg->SendToPlayer(PlayerGuid, false);
		}
		break;
	case MSG_C2S_UNION_Request_Anwser_Invite:			//! 行会会长 发送回复盟主的邀请
		{
			pMsg->SetType(MSG_S2W_UNION_Request_AnswerInvit);
			pMsg->Send(false);
		}
		break;

	case MSG_C2S_FACTION_Upgrade_Request:				//! 工会升级请求
		{
			DWORD dwUpgradeType = pMsg->GetLong();
			GetOrganiCtrl()->OnPlayerUpgradeFac(pMsg->GetPlayer(), dwUpgradeType);
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_W2S_OTHER_FACTIONTALK:
		{

		}
	//! [add end]
	case MSG_W2S_QUEST_PlayerAddQuest:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			DWORD dwQuestID  = pMsg->GetDWord();
			CPlayer* pPlayer = GetGame()->FindPlayer(PlayerID);
			if(pPlayer)
			{
				pPlayer->AddQuest(dwQuestID);
			}
		}
		break;
	case MSG_W2S_QUEST_PlayerRemoveQuest:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			DWORD dwQuestID  = pMsg->GetDWord();
			CPlayer* pPlayer = GetGame()->FindPlayer(PlayerID);
			if(pPlayer)
			{
				pPlayer->RemoveQuest(dwQuestID);
			}
		}
		break;
	case MSG_W2S_QUEST_PlayerRunScript:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			char str[256]={0};
			pMsg->GetStr(str,256);
			CPlayer* pPlayer = GetGame()->FindPlayer(PlayerID);
			if(pPlayer && pPlayer->IsDied() == false)
			{
				pPlayer->PlayerRunScript(str);
			}
		}
		break;
		//完成任务
	case MSG_C2S_QUEST_Complete:
		{
			if(pMsg->GetPlayer() == NULL)	break;

			DWORD wQuestID   = pMsg->GetDWord();//>GetWord();
			CPlayer* pPlayer = pMsg->GetPlayer();
			if(pPlayer /*&& pPlayer->GetQuestState(wQuestID) == eQS_NotComplete*/)
			{
				const CQuest* pQuest = CQuestSystem::GetQuest((LONG)wQuestID);
				if(NULL == pQuest)	break;
				const char* strScript =CQuestSystem::GetCompleteQuestScript();
				if(strScript == NULL)	break;

				stRunScript sScript;
				sScript.pszFileName = (char*)strScript;
				sScript.desShape = NULL;
				sScript.pRegion =  pMsg->GetRegion();
				sScript.srcShape	=pPlayer ;

				RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ), wQuestID ,0);
			}
		}
		break;
		//接受任务
	case MSG_C2S_QUEST_Accept:
		{
			if (pMsg->GetPlayer()==NULL)
				break;
			CPlayer* pPlayer = pMsg->GetPlayer();
			DWORD wQuestID   = pMsg->GetDWord();
			BYTE bMercQuest = pMsg->GetByte();
			BYTE bFlag = pMsg->GetByte();
			BYTE bReAccept = pMsg->GetByte();
			const CQuest* pQuest = CQuestSystem::GetQuest((LONG)wQuestID);
			if(NULL == pQuest)	break;
			
			stRunScript sScript;
			sScript.desShape = NULL;
			sScript.pRegion =  pMsg->GetRegion();
			sScript.srcShape	=pPlayer;
			//队长点击接受为全队任务
			if (bFlag!=0)
			{
				CGUID teamid=pPlayer->GetTeamID();
				if (teamid == NULL_GUID)
					break;
				GSTeam* pTeam=GetOrganiCtrl()->getInstance()->GetGameTeamByID(teamid);
				if (pTeam == NULL)
					break;
				//查找共享列表中是否有此任务
				if ( pTeam->FindShareQuest(wQuestID)!= 0 )
				{
					const char* strScript =CQuestSystem::GetAddQuestScript();
					if (strScript == NULL)
						break;
					sScript.pszFileName = (char*)strScript;
					//执行队长接全队任务的脚本
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ), wQuestID ,1);
				}
				
			}
			//接受共享任务或在队伍中接受个人列表中的任务
			else
			{
				//接受共享任务
				if (bReAccept)
				{
					const char* strReAccept = CQuestSystem::GetQuestAcceptScript();
					if (strReAccept == NULL)
						break;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strReAccept) ), wQuestID);
					break;
				}
				//接受个人列表中的任务
				if( pPlayer->IsMercListQuest(wQuestID) && !pPlayer->IsDisbandQuest(wQuestID))
				{
					const char* strScript =CQuestSystem::GetAddQuestScript();
					if (strScript == NULL)
						break;
					sScript.pszFileName = (char*)strScript;
					RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ), wQuestID ,0);
				}
				
			}
			
			//}
		}
		break;
		//放弃任务
	case MSG_C2S_QUEST_Disband:
		{
			//*
			if(pMsg->GetPlayer() == NULL)	break;

			DWORD wQuestID   = pMsg->GetDWord();//>GetWord();
			CPlayer* pPlayer = pMsg->GetPlayer();
			if(pPlayer)
			{
				const CQuest* pQuest = CQuestSystem::GetQuest((LONG)wQuestID);
				if(NULL == pQuest)	break;
				const char* strScript =pQuest->GetAttribute().GetDeleteScript();
				if(strScript == NULL)	break;

				stRunScript sScript;
				sScript.pszFileName = (char*)strScript;
				sScript.desShape = NULL;
				sScript.pRegion =  pMsg->GetRegion();
				sScript.srcShape	=pPlayer ;

				RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ), wQuestID ,0);
			}
			//*/
		}
		break;
		//刷性俑兵任务
	case MSG_C2S_MERCQUEST_Update:
		{
			if(pMsg->GetPlayer())
			{
				int ListID= pMsg->GetLong();
				BYTE byUseGoods= pMsg->GetByte();
				CPlayer* pPlayer= pMsg->GetPlayer();
				if(pPlayer)
				pPlayer->FreshMercQuest(ListID,byUseGoods!=0);
			}
		}
		break;
	case MSG_W2S_SHARE_QUESTLIST_UPDATE:
		{
			CGUID teamGuid=NULL_GUID;
			pMsg->GetGUID(teamGuid);
			DWORD dwTime=pMsg->GetDWord();
			GSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamGuid);
			if (pTeam)
			{
				long lsize=pMsg->GetLong();
				if ( 0 == lsize)
				{
					pTeam->ClearShareQuestList();
				}
				else
				{
					pTeam->SetAcceptTime(dwTime);
					pTeam->ClearShareQuestList();
					for (int i=0;i<lsize;i++)
					{
						GSTeam::tagShareQuestList quest;
						quest.lQuestID=pMsg->GetLong();
						pMsg->GetStr(quest.szName,20);
						pTeam->UpdateShareQuestList(&quest);
					}
				}
				pTeam->UpdateShareQuestList(NULL,true);
			}
		}
		break;
	case MSG_C2S_SHARE_QUESTLIST_UPDATE:
		{
			//玩家GUID
			CGUID guid=NULL_GUID;
			GSTeam::tagShareQuestList quest;
			pMsg->GetGUID(guid);
			BYTE btype=pMsg->GetByte();
			quest.lQuestID=pMsg->GetLong();
			CPlayer* pPlayer=pMsg->GetPlayer();
			if (pPlayer)
			{
				//0表示推荐任务
				if (0 == btype)
				{
					//判断赏金任务列表中是否有此任务，有则删除
					if ( !pPlayer->UpdateTeamQuestList(quest.lQuestID) )
					{
						break;
					}
				}
			}
			pMsg->SetType(MSG_S2W_SHARE_QUESTLIST_UPDATE);
			pMsg->Send(false);
		}
		break;
	case MSG_W2S_SHARE_CURRQUEST:
		{
			CGUID teamGuid=NULL_GUID;
			pMsg->GetGUID(teamGuid);
			DWORD dwTime=pMsg->GetDWord();
			long lQuestID=pMsg->GetLong();
			GSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamGuid);
			if (pTeam)
			{	
				pTeam->SetAcceptTime(dwTime);
				pTeam->SetCurrShareQuest(lQuestID);
			}
		}
		break;
	case MSG_W2S_SHARE_SETMEMBERQUEST:
		{
			CGUID teamGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(teamGuid);
			long lQuestID=pMsg->GetLong();
			GSTeam* pTeam=GetOrganiCtrl()->GetGameTeamByID(teamGuid);
			if (pTeam)
			{
				pTeam->SetMemberQuest(PlayerGuid,lQuestID);
			}
		}
		break;
	case MSG_W2S_Faction_ChangeTickets_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);
			BYTE bySucc=pMsg->GetByte();
			LONG leftTickets=pMsg->GetLong();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->SetTickets(cBaseName,leftTickets);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}	
				ContinueRunScript(ScriptGuid, stackVarList);
			}				
		}
		break;
	case MSG_W2S_Faction_ChangeBase_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);
			BYTE bySucc=pMsg->GetByte();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->ChangeBaseInfo(cBaseName);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}	
				ContinueRunScript(ScriptGuid, stackVarList);
			}
		}
		break;
	case MSG_W2S_Faction_ChangeBattle_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			pMsg->GetGUID(PlayerGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			BYTE bySucc=pMsg->GetByte();
			LONG leftBattle=pMsg->GetLong();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->AddBattle(leftBattle,PlayerGuid);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}
				ContinueRunScript(ScriptGuid, stackVarList);
			}
		}
		break;
	case MSG_W2S_Faction_ChangeArmyInvest_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			pMsg->GetGUID(PlayerGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			BYTE bySucc=pMsg->GetByte();
			LONG leftArmyInvest=pMsg->GetLong();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->AddArmyInvest(leftArmyInvest,PlayerGuid);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}
				ContinueRunScript(ScriptGuid, stackVarList);	
			}
		}
		break;
	case MSG_W2S_Faction_ChangeJoinBattle_Re:
		{	
			CGUID FacGuid=NULL_GUID;
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(PlayerGuid);
			LONG lFlags=pMsg->GetLong();
			
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction==NULL)
				break;
			pFaction->SetJoin(PlayerGuid,lFlags);
		}
		break;
	case MSG_W2S_Faction_ChangeBaseBattle_re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char cBaseName[32]={0};
			pMsg->GetStr(cBaseName,32);
			BYTE bySucc=pMsg->GetByte();
			LONG leftTickets=pMsg->GetLong();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->SetBaseBattle(cBaseName,leftTickets);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}	
				ContinueRunScript(ScriptGuid, stackVarList);
			}
		}
		break;
	case MSG_W2S_Faction_ChangeTechLevel_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			LONG lTechNo=pMsg->GetLong();
			LONG lLv=pMsg->GetLong();
			BYTE bySucc=pMsg->GetByte();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->AddTechLv(lTechNo,lLv);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}	
				ContinueRunScript(ScriptGuid, stackVarList);	
			}
		}
		break;
	case MSG_W2S_Faction_ChangeEnemy_Re:
		{
			CGUID FacGuid=NULL_GUID;
			CGUID ScriptGuid=NULL_GUID;
			pMsg->GetGUID(FacGuid);
			pMsg->GetGUID(ScriptGuid);
			char cRet[32]={0};
			pMsg->GetStr(cRet,32);
			char szFacName[32]={0};
			pMsg->GetStr(szFacName,32);
			DWORD dwStartTime=pMsg->GetDWord();
			DWORD dwEndTime=pMsg->GetDWord();
			LONG lBattleType=pMsg->GetLong();
			LONG lFlag=pMsg->GetLong();
			BYTE bySucc=pMsg->GetByte();
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction!=NULL && bySucc==1)
			{
				pFaction->ChangeEnemy((eBattleType)lBattleType,szFacName,dwStartTime,dwEndTime,lFlag);
			}
			//继续执行脚本
			if (g_Scripts.find(ScriptGuid)!=g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar(cRet, (double)bySucc);
				}	
				ContinueRunScript(ScriptGuid, stackVarList);
			}
		}
		break;
	case MSG_C2S_Faction_ChangeJoinBattle:
		{
			CPlayer* pPlayer=pMsg->GetPlayer();
			if (pPlayer==NULL)
				break;
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
			if (pFaction==NULL)
				break;
			//不是会长
			if(pFaction->GetMaster()!=pPlayer->GetExID())
				break;
			
			//被修改成员的GUID
			CGUID PlayerGuid=NULL_GUID;
			pMsg->GetGUID(PlayerGuid);
			CMessage msg(MSG_S2W_Faction_ChangeJoinBattle);
			msg.Add(PlayerGuid);
			msg.Add(pPlayer->GetFactionID());
		
			msg.Add(pMsg->GetByte());
			msg.Send();
		}
		break;
	case MSG_W2S_Faction_ClearBase_Notify:
		{
			CGUID FacGuid;
			pMsg->GetGUID(FacGuid);
			GameFaction* pFaction=GetOrganiCtrl()->GetFactionOrganizing(FacGuid);
			if (pFaction==NULL)
				break;
			pFaction->ClearBase(); 

		}
		break;
	}
}