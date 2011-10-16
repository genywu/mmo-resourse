#include "StdAfx.h"
#include ".\organizingctrl.h"

#include "WSTeam.h"
#include "..\Player.h"

#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"

#include "../../../setup/LogSystem.h"
#include "../GlobalRgnManager.h"

//#include "..\dbaccess\worlddb\rsfaction.h"
//#include "..\dbaccess\worlddb\Rsunion.h"
#include ".\faction.h"
#include "OrganizingParam.h"
#include "../dbentity/entityManager.h"
#include "SentaiSystem.h"




//比较仿函数
struct KeyCompareByEarly
{
	bool operator()(const tagKey& KeyX, const tagKey& KeyY)
	{
		if(KeyX.l > KeyY.l)
		{
			return true;
		}
		else if(KeyX.l == KeyY.l)
		{
			if(KeyY.Time >= KeyX.Time)
				return true;
		}
		return false;
	}
};

struct KeyCompareByLate
{
	bool operator()(const tagKey& KeyX, const tagKey& KeyY)
	{
		if(KeyX.l > KeyY.l)
		{
			return true;
		}
		else if(KeyX.l == KeyY.l)
		{
			if(KeyX.Time >= KeyY.Time)
				return true;
		}
		return false;
	}
};

COrganizingCtrl* COrganizingCtrl::instance = NULL;



COrganizingCtrl::COrganizingCtrl(VOID)
:m_bFacDataFull(FALSE)
,m_bUnionDataFull(FALSE)
,m_InviteSession(60, FALSE)
,m_UnionSession(60, FALSE)
,m_TimerQueue(4, 5000)
,m_lExpTimerID(0)
{
}

COrganizingCtrl::~COrganizingCtrl(VOID)
{
	m_TimerQueue.Cancel(this);
}

//得到唯一实例句柄
//该函数不支持多线程操作
inline COrganizingCtrl*COrganizingCtrl:: getInstance()
{
	if(instance == NULL)
	{
		instance = new COrganizingCtrl();
	}
	return instance;
}

//初始化
bool   COrganizingCtrl::Initialize()
{
	if (m_bFacDataFull) return true;

	//装载所有同盟
	//GetGame()->GetEntityManager()->CreateLoadUnionGroupSession();
	
	//AddLogText("发送读取同盟消息...");


	//装载所有帮派
	//GetGame()->GetEntityManager()->CreateLoadFactionGroupSession();
	//AddLogText("发送读取帮会消息...");

	//装载所有帮派
	AddLogText("装载组队配置...");


	WSTeam::s_tagDefaultSetup.lCorrelativeSkillID	= CGlobeSetup::GetSetup()->lCorrelativeSkillID;		//与组队关联的技能ID
	WSTeam::s_tagDefaultSetup.lMinSkillLevel		= CGlobeSetup::GetSetup()->lMinSkillLevel;			//可以组队的最小技能等级
	WSTeam::s_tagDefaultSetup.lGoodsAllot			= CGlobeSetup::GetSetup()->new_lGoodsAllot;			//默认分配方式
	WSTeam::s_tagDefaultSetup.lMaxMemberNum			= CGlobeSetup::GetSetup()->new_lMaxMemberNum;		//默认最大人数
	WSTeam::s_tagDefaultSetup.lSessionTimeoutS		= CGlobeSetup::GetSetup()->new_lSessionTimeoutS;	//会话超时秒数
	WSTeam::s_tagDefaultSetup.fIncreasedExp_2		= CGlobeSetup::GetSetup()->new_fIncreasedExp_2;		//2人经验加成比例
	WSTeam::s_tagDefaultSetup.fIncreasedExp_3		= CGlobeSetup::GetSetup()->new_fIncreasedExp_3;		//3人经验加成比例
	WSTeam::s_tagDefaultSetup.fIncreasedExp_4		= CGlobeSetup::GetSetup()->new_fIncreasedExp_4;		//4人经验加成比例
	WSTeam::s_tagDefaultSetup.fIncreasedExp_5		= CGlobeSetup::GetSetup()->new_fIncreasedExp_5;		//5人经验加成比例
	WSTeam::s_tagDefaultSetup.fIncreasedExp_6		= CGlobeSetup::GetSetup()->new_fIncreasedExp_6;		//6人经验加成比例
	WSTeam::s_tagDefaultSetup.lExperienceRadii		= CGlobeSetup::GetSetup()->new_lExperienceRadii;	//经验分配半径（单位：格）
	WSTeam::s_tagDefaultSetup.lExpDiff				= CGlobeSetup::GetSetup()->new_lExpDiff;			//经验差
	WSTeam::s_tagDefaultSetup.lExpLimit				= CGlobeSetup::GetSetup()->new_lExpLimit;			//玩家经验值份额基本保障
	WSTeam::s_tagDefaultSetup.fExpAmerce			= CGlobeSetup::GetSetup()->new_fExpAmerce;			//经验惩罚系数

	//sprintf(str,"成功装载组队配置");
	AddLogText("成功装载组队配置");

	SYSTEMTIME	stTime;
	GetLocalTime(&stTime);
	//! 当前离凌晨12点的时间（毫秒）
	DWORD dwToZeroTime = (((24 - stTime.wHour) * 60 - stTime.wMinute) * 60 - stTime.wSecond) * 1000;

	return true;
}

//释放资源
VOID COrganizingCtrl::Release()
{
	GetTimerQueue().Cancel(this);

	for(map<CGUID, CFaction*>::iterator it = m_FactionOrganizings.begin(); it != m_FactionOrganizings.end(); it++)
	{
		MP_DELETE((*it).second);
	}

	for(map<CGUID, CUnion*>::iterator it = m_mapUnion.begin(); it != m_mapUnion.end(); it++)
	{
		MP_DELETE((*it).second);
	}

	//释放队伍
	for(map<CGUID,WSTeam*>::iterator ite = m_TeamOrganizings.begin(); ite != m_TeamOrganizings.end(); ++ite)
	{
		MP_DELETE((*ite).second);
	}
	m_TeamOrganizings.clear();

	m_bFacDataFull = FALSE;
}

VOID COrganizingCtrl::ClearFaction(VOID)
{
	for(map<CGUID, CFaction*>::iterator it = m_FactionOrganizings.begin(); it != m_FactionOrganizings.end(); it++)
	{
		MP_DELETE((*it).second);
	}
	m_FactionOrganizings.clear();
}

VOID COrganizingCtrl::ClearUnion(VOID)
{
	for(map<CGUID, CUnion*>::iterator it = m_mapUnion.begin(); it != m_mapUnion.end(); it++)
	{
		MP_DELETE((*it).second);
	}
	m_mapUnion.clear();
}


VOID COrganizingCtrl::OutOrganiInfo(VOID)
{
	char szTmp[1204] = {0};

	LONG lFacNum		= (LONG)m_FactionOrganizings.size();
	LONG lApplicantNum	= (LONG)m_mapAllApplicant.size();
	LONG lTeamNum		= (LONG)m_TeamOrganizings.size();
	LONG lTeamMemberNum = (LONG)m_MemberIDvsTeamID.size();
	LONG lTeamSession	= m_InviteSession.Size();
	sprintf(szTmp, "Faction Num = %d\r\n Applicant Player Num = %d\r\n Team Num = %d\r\n Team's Member = %d\r\n Team Session = %d\r\n\r\n",
				   lFacNum, lApplicantNum, lTeamNum, lTeamMemberNum, lTeamSession
				   );
	PutStringToFile("MemAllocInfo", szTmp);
}

//组织系统的Run
VOID	COrganizingCtrl::Run(long lInterTime)
{

}

//! 响应GS的连接
VOID COrganizingCtrl::OnGsConnect(LONG lGsSocket)
{
	//! 发送工会信息到GS
	if (m_bFacDataFull)
	{
		SendFactionDataToGS(lGsSocket);
	}
	else//! 如果数据初始化还未完成，先将客户端保存至发送列表
	{
		m_setWaitFacSocket.insert(lGsSocket);
	}

	//! 发送联盟信息到GS
	if (m_bFacDataFull)
	{
		SendUnionDataToGS(lGsSocket);
	}
	else//! 如果数据初始化还未完成，先将客户端保存至发送列表
	{
		m_setWaitUnionSocket.insert(lGsSocket);
	}

	//! 发送组队信息到GS
	SendTeamDataToGS(lGsSocket);
}


//玩家死亡时触发
VOID COrganizingCtrl::OnPlayerDied(const CGUID& PlayerGuid, const CGUID& lKiller)
{
	
}

//触发一个玩家进入游戏
VOID COrganizingCtrl::OnPlayerEnterGame(const CGUID& PlayerGuid)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;

	const CGUID& FactionGuid = FindPlayerFacionID(PlayerGuid);
	CFaction* pFaction = GetFactionOrganizing(FactionGuid);
	if(pFaction != NULL)	
	{
		pPlayer->SetFactionID(FactionGuid);
		CUnion *pUnion = GetUnionOrganizing(pFaction->GetSuperior());
		pFaction->OnMemberEnterGame(pPlayer);
		if(NULL != pUnion)
		{
			pPlayer->SetUnionID(pUnion->GetExID());
		}
	}

	const CGUID& TeamID = FindPlayerTeamID(PlayerGuid);
	pPlayer->SetTeamID(TeamID);

	//! 判断是否有积分惩罚
	long PenaltyPoints = pPlayer->GetPenaltyPoints();
	if(0 != PenaltyPoints)
	{
		if(!GetInst(CSentaiSystem).IsQueuing(TeamID))
		{
			pPlayer->SetSentaiPoints(pPlayer->GetSentaiPoints() - PenaltyPoints, true);
			pPlayer->SetPenaltyPoints(0);
		}
	}
}

VOID COrganizingCtrl::OnPlayerEnterGameLaterDataSend(const CGUID& PlayerGuid)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;

	WSTeam* pTeam = GetGameTeamByID(pPlayer->GetTeamID());
	if(pTeam != NULL)	
	{
		pTeam->OnMemberEnterGame(pPlayer);
	}
}

//触发一个玩家离开游戏
VOID COrganizingCtrl::OnPlayerExitGame(CPlayer *pPlayer, bool IsChangeGS)
{
	if(NULL == pPlayer) return;

	const CGUID& FactionGuid = pPlayer->GetFactionID();
	CFaction* pFaction = GetFactionOrganizing(FactionGuid);
	if(pFaction != NULL && (!IsChangeGS))	
	{
		pFaction->OnMemberExitGame(pPlayer);
	}

	const CGUID& TeamID = pPlayer->GetTeamID();
	WSTeam* pTeam = (WSTeam*)GetGameTeamByID(TeamID);
	if(NULL != pTeam && (!IsChangeGS))	
	{
		//! 因为正在下线这个玩家还没被清除在线状态
 		if(1 == pTeam->GetOnlineMemberNum())
		{
			GetInst(CSentaiSystem).OnTeamLeaveGame(TeamID);
			DisbandTeam(pTeam);
			return;
		}

		pTeam->OnMemberExitGame(pPlayer->GetExID());
	}

	
	
}
//玩家删除角色时触发
int COrganizingCtrl::OnDeleteRole(const CGUID& PlayerGuid)
{
	///////////////////////////////////////////////////
	const CGUID& FactionGuid = FindPlayerFacionID(PlayerGuid);
	if( FactionGuid == NULL_GUID ) return 0;
	CFaction* pFaction = GetFactionOrganizing(FactionGuid);
	if( pFaction == NULL ) return 0;

	if(pFaction->GetMasterID() == PlayerGuid)
	{
		if(pFaction->GetSuperior() != NULL_GUID)
		{
			CUnion* pUnion = GetUnionOrganizing(pFaction->GetSuperior());
			//不是盟主退出
			if(pUnion )
			{
				if(pUnion->GetMasterID() == FactionGuid)
				{
					//解散同盟
					return 3;
				}
				else
				{
					//退出同盟
					return 2;
				}
			}
		}
		else
		{
			//拥有主城则不能删除帮会角色信息


			//解散帮会
			GetOrganiCtrl()->OnDelFaction(pFaction->GetExID(), FALSE);
		}
	}
	else
	{	//退出帮会
		pFaction->Exit(PlayerGuid);
	}
	return 0;
}

//响应玩家地图改变
VOID COrganizingCtrl::OnPlayerMapChange(const CGUID &PlayerGuid, const char* pMapName)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;
	if (NULL_GUID == pPlayer->GetFactionID()) return;

	CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
	if (NULL == pFaction) return;

	pFaction->MemberMapChange(PlayerGuid, pMapName);
}

//响应玩家职业改变
VOID COrganizingCtrl::OnPlayerOccuChange(CPlayer *pPlayer, LONG lOccu)
{
	if (NULL == pPlayer) return;
	if (NULL_GUID == pPlayer->GetFactionID()) return;

	CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
	if (NULL == pFaction) return;

	pFaction->MemberOccuChange(pPlayer->GetExID(), lOccu);
	
	const CGUID& TeamID = pPlayer->GetTeamID();
	if (NULL_GUID != TeamID)
	{
		WSTeam* pTeam = GetGameTeamByID(TeamID);
		if(pTeam != NULL)	
		{
			CMessage msg(MSG_W2S_TEAM_OCCUCHANGE);
			msg.Add(TeamID);
			msg.Add(pPlayer->GetExID());
			msg.Add(lOccu);
			msg.SendAll();
		}
	}
}

//响应玩家等级改变
VOID COrganizingCtrl::OnPlayerLevelChange(CPlayer *pPlayer, LONG lLevel)
{
	if (NULL == pPlayer) return;
	if (NULL_GUID != pPlayer->GetFactionID())
	{
		CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
		if (NULL == pFaction) return;

		pFaction->MemberLevelChange(pPlayer->GetExID(), lLevel);
	}

	const CGUID& TeamID = pPlayer->GetTeamID();
	if (NULL_GUID != TeamID)
	{
		WSTeam* pTeam = GetGameTeamByID(TeamID);
		if(pTeam != NULL)	
		{
			CMessage msg(MSG_W2S_TEAM_LEVELCHANGE);
			msg.Add(TeamID);
			msg.Add(pPlayer->GetExID());
			msg.Add(lLevel);
			msg.SendAll();
		}
	}
}

//响应玩家隐匿状态改变
VOID COrganizingCtrl::OnPlayerHideChange(CPlayer *pPlayer, BYTE cHide)
{
	if (NULL == pPlayer) return;
	if (NULL_GUID != pPlayer->GetFactionID())
	{
		CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(pPlayer->GetFactionID());
		if (NULL == pFaction) return;

		pFaction->MemberHideChange(pPlayer->GetExID(), cHide);
	}
}

VOID COrganizingCtrl::SendOrgaInfoToClient(const CGUID& PlayerGuid,string& strSendCon,string& strSender,long lGameServerID,DWORD dwFontColor,DWORD dwBkColor)
{
	if(lGameServerID==-1)
	{
		lGameServerID = GetGame()->GetGameServerNumber_ByPlayerID(PlayerGuid);
	}
	if(lGameServerID==-1) return;
	CMessage msg( MSG_W2S_SERVER_ADDINFO );
	msg.Add( PlayerGuid );
	msg.Add(strSendCon.c_str());
	msg.Add( dwFontColor );
	msg.Add( dwBkColor );
	msg.Add((LONG)eNOTIFYPOS_LEFT_BOTTOM);
	msg.Add(strSender.c_str());
	msg.SendGSID(lGameServerID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 工会
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID COrganizingCtrl::SaveAllFaction(VOID)
{
	vector<CEntityGroup*> vCEntityGroup;
	
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SaveAllFaction start");
#endif

	// 先清除旧数据
	CEntityManager::DBEntityComponentMapItr facMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[factiongroup]"));
	if(facMapItr != GetGame()->GetEntityManager()->GetBaseEntityMap().end())
	{
		if(facMapItr->second.size() == 0)
		{
			AddLogText("Err SaveAllFaction()");
			return;
		}
		CEntityGroup* pFacGroup = (CEntityGroup*)(facMapItr->second.begin()->second);
		if(pFacGroup)
			pFacGroup->ReleaseChilds();
	}

	for (map<CGUID, CFaction*>::const_iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
	{
		CEntityGroup* pTmpFaction = NULL;
		ite->second->Save(&pTmpFaction);
		if(NULL != pTmpFaction)
			vCEntityGroup.push_back(pTmpFaction);
	}

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SaveAllFaction create session start");
#endif
	GetGame()->GetEntityManager()->CreateSaveFactionSession(vCEntityGroup);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SaveAllFaction create session end");
#endif

	AddLogText("定时保存[%d]个工会数据!", m_FactionOrganizings.size());
}
VOID COrganizingCtrl::SaveAllUnion(VOID)
{
	for (map<CGUID, CUnion*>::const_iterator ite = m_mapUnion.begin(); ite != m_mapUnion.end(); ++ite)
	{
		ite->second->Save();
	}

	AddLogText("定时保存[%d]个联盟数据!", m_mapUnion.size());
}



//! 响应初始化工会结尾消息(FORM DBS)
VOID COrganizingCtrl::OnInitFactionEndMsg(VOID)
{
	//! DB数据转换加载
	//CEntityManager::DBEntityComponentMapItr facMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(COM_FACTIONGROUP);
	//if(facMapItr == GetGame()->GetEntityManager()->GetBaseEntityMap().end()) return;

	//for (map<CGUID, CEntityGroup*>::iterator ite = facMapItr->second.begin(); ite != facMapItr->second.end(); ++ite)
	//{
	//	CFaction *pFaction = new CFaction();
	//	GetGame()->GetEntityManager()->CDBFactionToCFaction(ite->second, pFaction);
	//	BOOL bRe = AddFactionOrganizing(pFaction);
	//	assert(bRe);
	//}

//	GetGame()->GetEntityManager()->ClearFactionMap();

	//! 设置完成标记
	m_bFacDataFull = TRUE;
	GetGame()->SetInitFactionLoadFlag(true);
	AddLogText("工会[%d]个数据读取完成!", m_FactionOrganizings.size());

	if(m_FactionOrganizings.size() > 0)
		BeginTimer();

	//! 想还在等待的GS发送数据
	SendFactionDataToGS(m_setWaitFacSocket);
	m_setWaitFacSocket.clear();
}

//! 相应工会删除的回复
VOID COrganizingCtrl::OnDelFactionFinishMsg(const CGUID &AimGuid, BOOL bIsSysOpt)
{
	CFaction* pFaction = GetFactionOrganizing(AimGuid);
	if (NULL != pFaction)
	{
		if(bIsSysOpt)
			GetGameLogInterface()->LogT150_faction_log("系统", NULL_GUID, pFaction, 1);
		else
		{
			CPlayer *pPlayer = GetGame()->GetMapPlayer(pFaction->GetMasterID());
			if(NULL != pPlayer)
				GetGameLogInterface()->LogT150_faction_log(pPlayer->GetName(), pPlayer->GetExID(), pFaction, 1);
			else
				GetGameLogInterface()->LogT150_faction_log("未找到操作者！", NULL_GUID, pFaction, 1);
		}

		pFaction->Disband();
		m_FactionOrganizings.erase(AimGuid);

		for (map<CGUID, CGUID>::iterator ite = m_mapAllApplicant.begin(); ite != m_mapAllApplicant.end(); )
		{
			if(ite->second == AimGuid)
				ite = m_mapAllApplicant.erase(ite);
			else
				++ite;
		}

		MP_DELETE(pFaction);
	}
		
	if(m_FactionOrganizings.size() == 0)
		EndTimer();
}

//! 响应GS的工会创建请求
VOID COrganizingCtrl::OnGsCreateFaction (CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;

	CMessage msg(MSG_W2S_ORGSYS_CreateFaction_Re);
	DBWriteSet DbWriteSet;
	msg.GetDBWriteSet(DbWriteSet);

	DbWriteSet.AddToByteArray(PlayerGuid);

	if(NULL_GUID != FindPlayerFacionID(PlayerGuid))
	{
		DbWriteSet.AddToByteArray(PLAYER_OPT_FAILED);
		DbWriteSet.AddToByteArray((LONG)eFOEI_Err_HadFaction);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	//! 验证新会名称（防止同名的两个新会同时请求创建）
	char szNewFactionName[MAXSIZE_FactionName] = {0};
	pMsg->GetStr(szNewFactionName, MAXSIZE_FactionName);
	for (map<CGUID, CFaction*>::iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
	{
		if (0 == stricmp(ite->second->GetName(), szNewFactionName))
		{
			DbWriteSet.AddToByteArray(PLAYER_OPT_FAILED);
			DbWriteSet.AddToByteArray((LONG)eFOEI_Err_ReName);
			msg.SendToSocket(pMsg->GetSocketID());
			return;
		}
	}

	CGUID NewFactionGuid;
	CGUID::CreateGUID(NewFactionGuid);

	CFaction *pFaction = MP_NEW CFaction(NewFactionGuid, szNewFactionName, pPlayer);
	if (NULL != pFaction)
	{
		if(AddFactionOrganizing(pFaction))
		{
			DbWriteSet.AddToByteArray(PLAYER_OPT_SUCCEED);
			pFaction->CodeToMsgSet(DbWriteSet);

			msg.SendAll();

			pFaction->Insert();

			map<CGUID, CGUID>::iterator ite = m_mapAllApplicant.find(PlayerGuid);
			if(m_mapAllApplicant.end() != ite)
			{
				CFaction *pApplyFaction = GetFactionOrganizing(ite->second);
				assert(pApplyFaction);
				if(NULL != pApplyFaction)
				{
					pApplyFaction->DelApplicant(PlayerGuid);
				}
				m_mapAllApplicant.erase(ite);
			}

			BeginTimer();
			GetGameLogInterface()->LogT150_faction_log(pPlayer->GetName(), pPlayer->GetExID(), pFaction, 0);
			return;
		}
		else
		{
			MP_DELETE(pFaction);
		}
	}

	DbWriteSet.AddToByteArray(PLAYER_OPT_FAILED);
	DbWriteSet.AddToByteArray((LONG)eFOEI_CoreServerError);
	msg.SendToSocket(pMsg->GetSocketID());
}

//! 响应GS的工会升级请求
VOID COrganizingCtrl::OnGsUpgradeFaction(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	DWORD dwUpgradeType = pMsg->GetDWord();
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	LONG lRe = UpgradeFac_ByCondition(pFaction, dwUpgradeType);
	if(eFOEI_NotErr != lRe)
	{
		CMessage msg(MSG_W2S_Faction_LvlChange_Notify);
		msg.Add(dwUpgradeType);
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add(lRe);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	pFaction->SetLvl(pFaction->GetLvl(dwUpgradeType) + 1, dwUpgradeType);
}

//! 响应GS的删除工会请求
VOID COrganizingCtrl::OnDelFaction(const CGUID &DelFacGuid, BOOL bIsSysOpt)
{
	if(GetGame()->GetEntityManager()->CreateDelFactionSession(DelFacGuid))
		OnDelFactionFinishMsg(DelFacGuid, bIsSysOpt);
	else
	{
		char szGUID[128] = {0};
		DelFacGuid.tostring(szGUID);
		char szTMP[256] = {0};
		sprintf(szTMP, "解散帮会[%s]失败！", szGUID);
		PutStringToFile("Organize", szTMP);
	}
}

//! 响应邀请玩家入会
VOID COrganizingCtrl::OnGsFacInvitePlayer(CMessage *pMsg)
{
	CGUID InviterGuid;
	char szAimName[MAX_MEMBER_NAME_SIZE] = {0};

	pMsg->GetGUID(InviterGuid);
	pMsg->GetStr(szAimName, MAX_MEMBER_NAME_SIZE);

	CPlayer *pInviter = GetGame()->GetMapPlayer(InviterGuid);
	if (NULL == pInviter)
		return;

	CFaction *pFaction = GetFactionOrganizing(pInviter->GetFactionID());
	if (NULL == pFaction)
		return;

	CMessage msg(MSG_W2S_Faction_InvitePlayer_Re);

	//! 判断是否发送名字为“隐匿者”
	if(0 == strcmp(szAimName, CGlobeSetup::GetSetup()->szHideName))
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOET_Err_AimHideName);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	CPlayer *pAim = GetGame()->GetMapPlayer(szAimName);
	if (NULL == pAim)
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_AimAbsent);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	if( pAim->GetHideFlag() == 1 && !GetGame()->PlayerIsGM(pInviter)
		&& (pAim->GetTeamID() != pInviter->GetTeamID() || pAim->GetTeamID()==NULL_GUID || pInviter->GetTeamID()==NULL_GUID) ) // 隐匿者
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOET_Err_AimHideName);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	//! 国家
	if(pFaction->GetCountry()%2!=pAim->GetCountry()%2)
	//if ((LONG)(pAim->GetCountry()) != pFaction->GetCountry())
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_Country);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	if (NULL_GUID != pAim->GetFactionID())
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_AimHasFac);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	if(pFaction->GetMemberNum() >= WSOrganizingParam::getInstance().GetLevelSetup(pFaction->GetLvl(eUT_FactionLevel))->lMaxMemNum)
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_MemberFull);
		msg.Add(InviterGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	msg.Add(PLAYER_OPT_SUCCEED);
	msg.Add(InviterGuid);
	msg.Add(pInviter->GetName());
	msg.Add(pAim->GetExID());
	msg.Add(pFaction->GetExID());
	msg.SendToSocket(pAim->GetGsSocket());
}

//! 响应添加玩家动作
VOID COrganizingCtrl::OnGsFacAddMember(CMessage *pMsg)
{
	CGUID InviterGuid;
	pMsg->GetGUID(InviterGuid);
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if(NULL == pPlayer) return;
	if(NULL_GUID != FindPlayerFacionID(PlayerGuid))
	{
		CMessage msg(MSG_W2S_Faction_InvitePlayer_Re);
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_HadFaction);
		msg.Add(PlayerGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	if(pFaction->GetMemberNum() >= WSOrganizingParam::getInstance().GetLevelSetup(pFaction->GetLvl(eUT_FactionLevel))->lMaxMemNum)
	{
		CMessage msg(MSG_W2S_Faction_InvitePlayer_Re);
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_MemberFull);
		msg.Add(PlayerGuid);
		msg.SendToSocket(pMsg->GetSocketID());
		return;
	}

	if(pFaction->AddMeber(pPlayer, InviterGuid))
	{
		map<CGUID, CGUID>::iterator ite = m_mapAllApplicant.find(pPlayer->GetExID());
		if(m_mapAllApplicant.end() != ite)
		{
			CFaction *pTmpFaction = GetFactionOrganizing(ite->second);
			if(NULL != pTmpFaction)
			{
				pTmpFaction->DelApplicant(pPlayer->GetExID());
			}
			m_mapAllApplicant.erase(ite);
		}
	}
}

//! 响应设置官阶名称
VOID COrganizingCtrl::OnGsFacSetJobName(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	LONG lJobLvl = pMsg->GetLong();
	char szNewName[MAX_MEMBER_NAME_SIZE] = {0};
	pMsg->GetStr(szNewName, MAX_MEMBER_NAME_SIZE);

	pFaction->SetJobName(lJobLvl, szNewName);
}

//! 响应设置官阶权限
VOID COrganizingCtrl::OnGsFacSetJobPurview(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	LONG lJobLvl = pMsg->GetLong();
	LONG lPurview = pMsg->GetLong();

	pFaction->SetJobPurview(lJobLvl, lPurview);
}

//! 响应成员退出
VOID COrganizingCtrl::OnGsFacQuit(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	pFaction->Exit(PlayerGuid);
}

//! 响应设置公告
VOID COrganizingCtrl::OnGsSetPronounce(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;

	CFaction *pFaction = GetFactionOrganizing(pPlayer->GetFactionID());
	if(NULL == pFaction) return;

	char szPronounce[MAX_PronounceCharNum] = {0};
	pMsg->GetStr(szPronounce, MAX_PronounceCharNum);

	pFaction->Pronounce(pPlayer, szPronounce);
}

//! 响应设置备忘
VOID COrganizingCtrl::OnGsSetLeaveword(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer) return;

	CFaction *pFaction = GetFactionOrganizing(pPlayer->GetFactionID());
	if(NULL == pFaction) return;

	char szPronounce[MAX_PronounceCharNum] = {0};
	pMsg->GetStr(szPronounce, MAX_PronounceCharNum);

	pFaction->Leaveword(pPlayer, szPronounce);
}

//! 响应开除成员
VOID COrganizingCtrl::OnGsFire(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID FireGuid;
	pMsg->GetGUID(FireGuid);
	CGUID BeFireGuid;
	pMsg->GetGUID(BeFireGuid);

	pFaction->FireOut(FireGuid, BeFireGuid);
}


//! 设置成员职级
VOID COrganizingCtrl::OnGsSetMemberJob(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID MemberGuid;
	pMsg->GetGUID(MemberGuid);
	LONG lJobLvl = pMsg->GetLong();

	pFaction->SetJobLvl(MemberGuid, lJobLvl);
}

//! 设置成员称号
VOID COrganizingCtrl::OnGsSetMemberTitle(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID OptGuid;
	pMsg->GetGUID(OptGuid);
	CGUID AimGuid;
	pMsg->GetGUID(AimGuid);
	char szTitle[MAX_MEMBER_NAME_SIZE] = {0};
	pMsg->GetStr(szTitle, MAX_MEMBER_NAME_SIZE);

	pFaction->SetMemberTitle(OptGuid, AimGuid, szTitle);
}

//! 传位
VOID COrganizingCtrl::OnGsSetNewMaster(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID NewMasterGuid;
	pMsg->GetGUID(NewMasterGuid);

	pFaction->Demise(pFaction->GetMasterID(), NewMasterGuid);
}

//! 操作应征者
VOID COrganizingCtrl::OnGsCheckRequisition(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	BOOL bAgree = pMsg->GetLong();
	CGUID AimGuid;
	pMsg->GetGUID(AimGuid);
	CGUID OpterGuid;
	pMsg->GetGUID(OpterGuid);

	//CPlayer *pPlayer = GetGame()->GetMapPlayer(AimGuid);
	CPlayer *pOpter = GetGame()->GetMapPlayer(OpterGuid);
	const tagFacApplyPerson *pFacApplyPerson = pFaction->FindApplicant(AimGuid);
	
	if(NULL != pFacApplyPerson)
	{
		if(bAgree)
		{	
			if(pFaction->GetMemberNum() >= WSOrganizingParam::getInstance().GetLevelSetup(pFaction->GetLvl(eUT_FactionLevel))->lMaxMemNum)
			{
				CMessage msg(MSG_W2S_Faction_InvitePlayer_Re);
				msg.Add(PLAYER_OPT_FAILED);
				msg.Add((LONG)eFOEI_Err_MemberFull);
				msg.Add(OpterGuid);
				msg.SendToSocket(pMsg->GetSocketID());
				return;
			}
			if(NULL_GUID != FindPlayerFacionID(AimGuid))
			{
				CMessage msg(MSG_W2S_Faction_InvitePlayer_Re);
				msg.Add(PLAYER_OPT_FAILED);
				msg.Add((LONG)eFOEI_Err_AimHasFac);
				msg.Add(OpterGuid);
				msg.SendToSocket(pMsg->GetSocketID());
				return;
			}
			pFaction->AddMeber(*pFacApplyPerson, OpterGuid);
			GetGameLogInterface()->LogT160_faction_join_leave_log(pFacApplyPerson->szName, &(pFacApplyPerson->PlayerGuid), pFaction, 0);
		}
		else
		{
			pFaction->DelApplicant(AimGuid);
			GetGameLogInterface()->LogT160_faction_join_leave_log(pFacApplyPerson->szName, &(pFacApplyPerson->PlayerGuid), pFaction, 1);
		}
	}
	
	pFaction->DelApplicant(AimGuid);
	m_mapAllApplicant.erase(AimGuid);
}

//! 申请加入
VOID COrganizingCtrl::OnGsJoinFac(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID ApplicantGuid;
	pMsg->GetGUID(ApplicantGuid);

	CPlayer *pPlayer = GetGame()->GetMapPlayer(ApplicantGuid);
	if (NULL == pPlayer) return;
	if (NULL_GUID != pPlayer->GetFactionID()) return;

	BOOL bIsJoin = pMsg->GetLong();

	if (bIsJoin)
	{
		if(m_mapAllApplicant.end() != m_mapAllApplicant.find(ApplicantGuid))
		{
			CMessage msg(MSG_W2S_Faction_Join_Re);
			msg.Add(PLAYER_OPT_FAILED);
			msg.Add((LONG)eFOEI_Err_ReJoin);
			msg.Add(ApplicantGuid);
			msg.SendToSocket(pMsg->GetSocketID());
			return;
		}

		if(pFaction->AddApplicant(ApplicantGuid))
		{
			m_mapAllApplicant[ApplicantGuid] = pFaction->GetExID();
		}
	}
	else
	{
		map<CGUID, CGUID>::iterator ite = m_mapAllApplicant.find(ApplicantGuid);
		if(m_mapAllApplicant.end() != ite)
		{
			pFaction = GetFactionOrganizing(ite->second);
			if (NULL != pFaction)
			{
				if(NULL != pFaction->DelApplicant(ApplicantGuid))
				{
					m_mapAllApplicant.erase(ite);
				}
			}
		}
	}
}

//! 上传图标
VOID COrganizingCtrl::OnGsUpLoadIcon(CMessage *pMsg)
{
	CGUID FactionGuid;
	pMsg->GetGUID(FactionGuid);
	CFaction *pFaction = GetFactionOrganizing(FactionGuid);
	if(NULL == pFaction) return;

	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	LONG lDataSize = pMsg->GetLong();
	assert(MAX_ICON_SIZE >= lDataSize);
	if(MAX_ICON_SIZE < lDataSize) return;
	vector<BYTE> vIconData(lDataSize);
	pMsg->GetEx(&vIconData[0], lDataSize);

	if(pFaction->SetIcon(&vIconData[0], vIconData.size()))
	{
		CMessage msg(MSG_W2S_Faction_IconChange_Notify);
		msg.Add(FactionGuid);
		msg.Add(PlayerGuid);
		msg.Add((LONG)vIconData.size());
		msg.AddEx(&vIconData[0], vIconData.size());
		msg.SendAll();
	}
}

//! 将一个工会对象加入管理
BOOL COrganizingCtrl::AddFactionOrganizing(CFaction *pFaction)
{
	if (NULL != pFaction)
	{
		assert(pFaction->GetExID() != NULL_GUID);
		if(NULL_GUID == pFaction->GetExID())
			return FALSE;
		assert(m_FactionOrganizings.end() == m_FactionOrganizings.find(pFaction->GetExID()));
		if(m_FactionOrganizings.end() != m_FactionOrganizings.find(pFaction->GetExID()))
			return FALSE;

		m_FactionOrganizings[pFaction->GetExID()] = pFaction;
		pFaction->AddApplicantToMap(m_mapAllApplicant);
		pFaction->ResetMember();
		return TRUE;

	}
	return FALSE;
}

CFaction* COrganizingCtrl::GetFactionOrganizing(const CGUID &FacGuid)
{
	map<CGUID,CFaction*>::iterator ite = m_FactionOrganizings.find(FacGuid);
	if (m_FactionOrganizings.end() != ite)
	{
		return ite->second;
	}
	return NULL;
}

//! 找出一个玩家的工会ID
const CGUID& COrganizingCtrl::FindPlayerFacionID(const CGUID &PlayerGuid)
{
	for (map<CGUID,CFaction*>::iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
	{
		if (NULL_GUID != ite->second->IsMember(PlayerGuid))
		{
			return ite->first;
		}
	}
	return NULL_GUID;
}

//! 发送初始化GS工会消息
VOID COrganizingCtrl::SendFactionDataToGS(LONG lGsSocket)
{
	assert(m_bFacDataFull);

	CMessage msg(MSG_W2S_ORGSYS_InitGsFaction);
	DBWriteSet DbWriteSet;
	msg.GetDBWriteSet(DbWriteSet);

	//! 配置信息
	if(!WSOrganizingParam::getInstance().CodeToMsgSet(DbWriteSet))
		return;

	//! 工会数据
	DbWriteSet.AddToByteArray((LONG)m_FactionOrganizings.size());
	for (map<CGUID, CFaction*>::iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
	{
		ite->second->CodeToMsgSet(DbWriteSet);
	}
	
	msg.SendToSocket(lGsSocket, false);
}

VOID COrganizingCtrl::SendFactionDataToGS(set<LONG> &setSocket)
{
	assert(m_bFacDataFull);

	CMessage msg(MSG_W2S_ORGSYS_InitGsFaction);
	DBWriteSet DbWriteSet;
	msg.GetDBWriteSet(DbWriteSet);

	//! 配置信息
	if(!WSOrganizingParam::getInstance().CodeToMsgSet(DbWriteSet))
		return;

	//! 工会数据
	DbWriteSet.AddToByteArray((LONG)m_FactionOrganizings.size());
	for (map<CGUID, CFaction*>::iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
	{
		ite->second->CodeToMsgSet(DbWriteSet);
	}

	msg.SendAll();
	/*
	for (set<LONG>::iterator ite = setSocket.begin(); ite != setSocket.end(); ++ite)
	{
		msg.SendToSocket(*ite);
	}
	*/
}

//! 检测升级工会的条件
eFactionOptErrInfo COrganizingCtrl::TestUpgradeFacCondition(CFaction *pFaction, DWORD dwUpgradeType)
{
	assert(NULL != pFaction);
	const VecCondition*	pVecCondition = WSOrganizingParam::getInstance().GetUpgradeVecCondition(dwUpgradeType, pFaction->GetLvl(dwUpgradeType) + 1);
	if (NULL == pVecCondition)
		return eFOEI_Err_SetupLoading;
	const VecCondition &vCondition = *pVecCondition;

	for (LONG i = 0; i < vCondition.size(); ++i)
	{
		switch(vCondition[i].lConditionType)
		{
		case eUCT_FacRes:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(vCondition[i].lValue > pFaction->GetRes())
					return eFOEI_FacRes_NotEnough;
			}
			break;
		case eUCT_FacExp:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(vCondition[i].lValue > pFaction->GetExp())
					return eFOEI_FacExp_NotEnough;
			}
			break;
		case eUCT_Money:
		case eUCT_LeaderLevel:
		case eUCT_FacLevel:
			break;
			//占领据城镇的条件
		case eUCT_BaseNum:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(vCondition[i].lValue > pFaction->GetBaseNum())
					return eFOET_Err_BaseNum;
			}
			break;
		default:
			{
				return eFOEI_Err_Unknown;
			}
		}
	}

	return eFOEI_NotErr;
}
//! 升级工会，按配置的条件
eFactionOptErrInfo COrganizingCtrl::UpgradeFac_ByCondition(CFaction *pFaction, DWORD dwUpgradeType)
{
	assert(NULL != pFaction);
	eFactionOptErrInfo testRe = TestUpgradeFacCondition(pFaction, dwUpgradeType);
	if(eFOEI_NotErr != testRe)
		return testRe;

	const VecCondition*	pVecCondition = WSOrganizingParam::getInstance().GetUpgradeVecCondition(dwUpgradeType, pFaction->GetLvl(dwUpgradeType) + 1);
	if (NULL == pVecCondition)
		return eFOEI_Err_SetupLoading;
	const VecCondition &vCondition = *pVecCondition;

	for (LONG i = 0; i < vCondition.size(); ++i)
	{
		switch(vCondition[i].lConditionType)
		{
		case eUCT_FacRes:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(!pFaction->AddRes(-(vCondition[i].lValue)))
					return eFOEI_FacRes_NotEnough;
			}
			break;
		case eUCT_FacExp:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(pFaction->GetExp() < vCondition[i].lValue)
					return eFOEI_FacExp_NotEnough;
			}
			break;
		case eUCT_Money:
		case eUCT_LeaderLevel:
		case eUCT_FacLevel:
			break;
			//占领城镇的条件
		case eUCT_BaseNum:
			{
				if(NULL == pFaction)
					return eFOEI_Err_NotInFaction;
				if(vCondition[i].lValue > pFaction->GetBaseNum())
					return eFOET_Err_BaseNum;
			}
			break;
		default:
			{
				return eFOEI_Err_Unknown; 
			}
		}
	}
	return eFOEI_NotErr;
}

//! 响应新的一天到了
void COrganizingCtrl::OnNewDay(DWORD dwTime)
{
	if(m_bFacDataFull)
	{
		list<CGUID> listFacGuid;
		for (map<CGUID, CFaction*>::iterator ite = m_FactionOrganizings.begin(); ite != m_FactionOrganizings.end(); ++ite)
		{
			if(CFaction::ImmediatelyDisband == ite->second->OnValidate(dwTime))
			{
				listFacGuid.push_back(ite->first);
			}
		}

		for (list<CGUID>::iterator ite = listFacGuid.begin(); listFacGuid.end() != ite; ++ite)
		{
			OnDelFaction(*ite, TRUE);
		}
	}
}

//! 定时器超时响应
long COrganizingCtrl::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	if(m_lExpTimerID == timerid)
	{
		CMessage msg(MSG_W2S_Faction_BoonRun_Notify);
		msg.SendAll();
	}
	
	return 0;
}
//! 当定时器被取消的调用
void COrganizingCtrl::OnTimerCancel(DWORD timerid,const void* var)
{
}

//!
VOID COrganizingCtrl::BeginTimer(VOID)
{
	if(0 == m_lExpTimerID)
		m_lExpTimerID = GetTimerQueue().Schedule(this, NULL, 5000, ADD_EXP_SPACE_TIME);
}
//!
VOID COrganizingCtrl::EndTimer(VOID)
{
	if(0 != m_lExpTimerID)
	{
		GetTimerQueue().Cancel(m_lExpTimerID);
		m_lExpTimerID = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 联盟
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 发送初始化GS联盟消息
VOID COrganizingCtrl::SendUnionDataToGS(LONG lGsSocket)
{
	assert(m_bUnionDataFull);

	vector<BYTE> vData;

	//! 工会数据
	_AddToByteArray(&vData, (LONG)m_mapUnion.size());
	for (map<CGUID, CUnion*>::iterator ite = m_mapUnion.begin(); ite != m_mapUnion.end(); ++ite)
	{
		ite->second->AddToByteArray(vData);
	}

	CMessage msg(MSG_W2S_Union_InitGs);
	msg.Add((LONG)vData.size());
	msg.AddEx(&vData[0], vData.size());
	msg.SendToSocket(lGsSocket, false);
}

VOID COrganizingCtrl::SendUnionDataToGS(set<LONG> &setSocket)
{
	assert(m_bUnionDataFull);

	vector<BYTE> vData;

	//! 工会数据
	_AddToByteArray(&vData, (LONG)m_mapUnion.size());
	for (map<CGUID, CUnion*>::iterator ite = m_mapUnion.begin(); ite != m_mapUnion.end(); ++ite)
	{
		ite->second->AddToByteArray(vData);
	}

	CMessage msg(MSG_W2S_Union_InitGs);
	msg.Add((LONG)vData.size());
	msg.AddEx(&vData[0], vData.size());

	for (set<LONG>::iterator ite = setSocket.begin(); ite != setSocket.end(); ++ite)
	{
		msg.SendToSocket(*ite);
	}
}

//! 将一个联盟对象加入管理
BOOL COrganizingCtrl::AddUnionOrganizing(CUnion *pUnion)
{
	if (NULL != pUnion)
	{
		assert(pUnion->GetExID() != NULL_GUID);
		if(NULL_GUID == pUnion->GetExID())
			return FALSE;
		assert(m_mapUnion.end() == m_mapUnion.find(pUnion->GetExID()));
		if(m_mapUnion.end() != m_mapUnion.find(pUnion->GetExID()))
			return FALSE;

		m_mapUnion[pUnion->GetExID()] = pUnion;
		return TRUE;
	}
	return FALSE;
}

CUnion* COrganizingCtrl::GetUnionOrganizing(const CGUID &UnionGuid)
{
	map<CGUID,CUnion*>::iterator ite = m_mapUnion.find(UnionGuid);
	if (m_mapUnion.end() != ite)
	{
		return ite->second;
	}
	return NULL;
}

//! 响应初始化联盟完成
VOID COrganizingCtrl::OnInitUnionEndMsg(VOID)
{
	m_bUnionDataFull = TRUE;
	GetGame()->SetInitUnionLoadFlag(true);
	AddLogText("联盟数据读取完成!");

	SendUnionDataToGS(m_setWaitUnionSocket);
	m_setWaitUnionSocket.clear();
}

//! 响应玩家创建联盟
VOID COrganizingCtrl::OnGsCreateUnion(CPlayer *pPlayer, const char *pUnionName)
{
	CMessage msg(MSG_W2S_Union_Create_Notify);
	msg.Add(pPlayer->GetExID());

	//! 验证名称（防止同名的两个新联盟同时请求创建）
	for (map<CGUID, CUnion*>::iterator ite = m_mapUnion.begin(); ite != m_mapUnion.end(); ++ite)
	{
		if (0 == stricmp(ite->second->GetName(), pUnionName))
		{
			msg.Add(PLAYER_OPT_FAILED);
			msg.Add((LONG)eFOEI_Err_ReName);
			msg.SendToSocket(pPlayer->GetGsSocket(), false);
			return;
		}
	}

	CFaction *pFaction = GetFactionOrganizing(pPlayer->GetFactionID());
	if(NULL == pFaction)
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_NotInFaction);
		msg.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}
	if(pFaction->GetMasterID() != pPlayer->GetExID())
	{
		msg.Add(PLAYER_OPT_FAILED);
		msg.Add((LONG)eFOEI_Err_Purview_Noenough);
		msg.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}

	CGUID NewUnionGuid;
	CGUID::CreateGUID(NewUnionGuid);
	CUnion *pUnion = MP_NEW CUnion(NewUnionGuid, pUnionName, pFaction);

	if(NULL != pUnion)
	{
		AddUnionOrganizing(pUnion);

		vector<BYTE> vUnionData;
		pUnion->AddToByteArray(vUnionData);

		msg.Add(PLAYER_OPT_SUCCEED);
		msg.Add((LONG)vUnionData.size());
		msg.AddEx(&vUnionData[0], vUnionData.size());
		msg.SendAll();

		pUnion->Insert();

		return;
	}

	msg.Add(PLAYER_OPT_FAILED);
	msg.Add((LONG)eFOEI_CoreServerError);
	msg.SendToSocket(pPlayer->GetGsSocket(), false);
}

//! 响应删除里面成功
VOID COrganizingCtrl::OnDelUnionFinishMsg(const CGUID &UnionGuid)
{
	CUnion *pUnion = GetUnionOrganizing(UnionGuid);
	if(NULL != pUnion)
	{
		pUnion->Disband();

		CMessage msg(MSG_W2S_Union_Disband_Notify);
		msg.Add(UnionGuid);
		msg.SendAll();

		MP_DELETE(pUnion);
		m_mapUnion.erase(UnionGuid);
	}
}

//! 响应玩家解散联盟
VOID COrganizingCtrl::OnGsDisbandUnion(CPlayer *pPlayer)
{
	assert(NULL != pPlayer);
	CUnion *pUnion = GetUnionOrganizing(pPlayer->GetUnionID());
	if(NULL != pUnion)
	{
		//! 战争状况判断

		//! 盟主判断
		if(pUnion->GetMasterID() == pPlayer->GetExID())
			GetGame()->GetEntityManager()->CreateDelFactionSession(pUnion->GetExID());
	}	
}

//! 响应退出联盟请求
VOID COrganizingCtrl::OnGsQuitUnion(const CGUID &FactionGuid, const CGUID &UnionGuid)
{
	CFaction	*pFaction	= GetFactionOrganizing(FactionGuid);
	CUnion		*pUnion		= GetUnionOrganizing(UnionGuid);
	if(NULL == pFaction || NULL == pUnion)
		return;

	if(pUnion->DelMember(FactionGuid))
	{
		pFaction->SetSuperior(NULL);

		CMessage msg(MSG_W2S_Union_Quit_Notify);
		msg.Add(FactionGuid);
		msg.Add(UnionGuid);
		msg.SendAll();
	}
}

//! 响应开除联盟请求
VOID COrganizingCtrl::OnGsKickUnion(const CGUID &OptGuid, const CGUID &AimGuid, const CGUID &UnionGuid)
{
	CFaction	*pAimFaction	= GetFactionOrganizing(AimGuid);
	CUnion		*pUnion		= GetUnionOrganizing(UnionGuid);
	if(NULL == pAimFaction || NULL == pUnion)
		return;

	if(pUnion->DelMember(AimGuid))
	{
		pAimFaction->SetSuperior(NULL);

		CMessage msg(MSG_W2S_Union_Kick_Notify);
		msg.Add(OptGuid);
		msg.Add(AimGuid);
		msg.Add(UnionGuid);
		msg.SendAll();
	}
}

//! 传位
VOID COrganizingCtrl::OnGsSetChairmanUnion(const CGUID &MasterGuid, const CGUID &AimFacGuid)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(MasterGuid);
	if(NULL == pPlayer)
		return;

	CUnion *pUnion = GetUnionOrganizing(pPlayer->GetUnionID());
	if(NULL == pUnion)
		return;

	if(pUnion->GetMasterID() != MasterGuid)
		return;
	if(pUnion->ChangeMaster(AimFacGuid))
	{
		CMessage msg(MSG_S2W_Union_SetChairman_Notify);
		msg.Add(pUnion->GetExID());
		msg.Add(AimFacGuid);
		msg.Add(pUnion->GetMasterName());
		msg.SendAll();
	}
}

//! 设置权限
VOID COrganizingCtrl::OnGsSetPurviewUnion(const CGUID &MasterGuid, const CGUID &AimFacGuid, DWORD dwPurview)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(MasterGuid);
	if(NULL == pPlayer)
		return;

	CUnion *pUnion = GetUnionOrganizing(pPlayer->GetUnionID());
	if(NULL == pUnion)
		return;

	if(pUnion->GetMasterID() != MasterGuid)
		return;

	if(pUnion->SetPurview(AimFacGuid, dwPurview))
	{
		CMessage msg(MSG_S2W_Union_SetPurview_Notify);
		msg.Add(pUnion->GetExID());
		msg.Add(AimFacGuid);
		msg.Add(dwPurview);
		msg.SendAll();
	}
}

//! 邀请加入联盟
VOID COrganizingCtrl::OnGsUnionInvit(const CGUID &MasterGuid, const char *pAimPlayerName)
{
	assert(NULL != pAimPlayerName);
	
	CPlayer *pPlayer = GetGame()->GetMapPlayer(MasterGuid);
	CPlayer	*pAimPlayer = GetGame()->GetMapPlayer(pAimPlayerName);
	if(NULL == pPlayer || NULL == pAimPlayer)
		return;

	CUnion *pUnion = GetUnionOrganizing(pPlayer->GetUnionID());
	if(NULL == pUnion)
		return;

	if(pUnion->GetMasterID() != MasterGuid)
		return;

	CMessage msgRe(MSG_S2W_Union_Invit_Re);

	if(NULL_GUID != pAimPlayer->GetUnionID())
	{
		msgRe.Add(PLAYER_OPT_FAILED);
		msgRe.Add((LONG)eFOEI_Err_AimHadUnion);
		msgRe.Add(pPlayer->GetExID());
		msgRe.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}

	CFaction *pFaction = GetFactionOrganizing(pAimPlayer->GetFactionID());
	if (NULL == pFaction)
	{
		msgRe.Add(PLAYER_OPT_FAILED);
		msgRe.Add((LONG)eFOEI_Err_NotInFaction);
		msgRe.Add(pPlayer->GetExID());
		msgRe.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}

	if (pFaction->GetMasterID() != pAimPlayer->GetExID())
	{
		msgRe.Add(PLAYER_OPT_FAILED);
		msgRe.Add((LONG)eFOEI_Err_NotInFaction);
		msgRe.Add(pPlayer->GetExID());
		msgRe.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}

	if(!m_UnionSession.CreateSession(tagUnionSession(pPlayer->GetExID(), pAimPlayer->GetExID(), pUnion->GetExID())))
	{
		msgRe.Add(PLAYER_OPT_FAILED);
		msgRe.Add((LONG)eFOEI_Err_ContinualInvite);
		msgRe.Add(pPlayer->GetExID());
		msgRe.SendToSocket(pPlayer->GetGsSocket(), false);
		return;
	}

	msgRe.SetType(MSG_S2W_Union_TransferInvit);
	msgRe.Add(pPlayer->GetExID());
	msgRe.Add(pPlayer->GetName());
	msgRe.Add(pUnion->GetExID());
	msgRe.Add(pUnion->GetName());
	msgRe.Add(pAimPlayer->GetExID());
	msgRe.SendToSocket(pAimPlayer->GetGsSocket(), false);
}

//! 应答邀请
VOID COrganizingCtrl::OnGsUnionAnswerInvit(const CGUID &InviterGuid, const CGUID &RespondentGuid, const CGUID & UnionGuid, LONG lResult)
{
	CPlayer *pRespondent = GetGame()->GetMapPlayer(RespondentGuid);
	if(NULL == pRespondent)
		return;
	CFaction *pFaction = GetFactionOrganizing(pRespondent->GetFactionID());
	if (NULL == pFaction)
		return;

	if(!m_UnionSession.TestSession(tagUnionSession(InviterGuid, RespondentGuid, UnionGuid)))
	{
		/*CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimHadTeam)*/
		SendInfoToOneClient(RespondentGuid, "会话已超时！" );
		return;
	}

	CPlayer *pInviter = GetGame()->GetMapPlayer(InviterGuid);

	if(0 == lResult)
	{
		if(NULL != pInviter)
		{
			CMessage msgRe(MSG_S2W_Union_Invit_Re);
			msgRe.Add(PLAYER_OPT_FAILED);
			msgRe.Add((LONG)eFOEI_Err_AimRefuse);
			msgRe.Add(pInviter->GetExID());
			msgRe.SendToSocket(pInviter->GetGsSocket(), false);
		}
		return;
	}

	CUnion *pUnionGuid = GetUnionOrganizing(UnionGuid);
	if(NULL == pUnionGuid)
	{
		SendInfoToOneClient(RespondentGuid, "目标联盟已解散！" );
		return;
	}

	if(pUnionGuid->AddNewMember(pFaction))
	{
		//! 通知各GS添加新成员
		CMessage msg(MSG_S2W_Union_NewMemberJoin);
		msg.Add(pFaction->GetExID());
		msg.Add(UnionGuid);
		msg.SendAll();

		//! 邀请者还在线的话，发回复消息
		
		if(NULL != pInviter)
		{
			CMessage msgRe(MSG_S2W_Union_Invit_Re);
			msgRe.Add(PLAYER_OPT_SUCCEED);
			msgRe.Add(pFaction->GetName());
			msgRe.Add(InviterGuid);
			msgRe.SendToSocket(pInviter->GetGsSocket(), false);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 队伍
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 向指定玩家发送一个信息
VOID COrganizingCtrl::SendInfoToOneClient(const CGUID& PlayerGuid, char *szInfo)
{
	GetOrganiCtrl()->SendOrgaInfoToClient(PlayerGuid,string(szInfo),string("提示"));

}

//! 得到指定的队伍的指针
WSTeam*	COrganizingCtrl::GetGameTeamByID(const CGUID& TeamID)
{
	map<CGUID,WSTeam*>::iterator ite = m_TeamOrganizings.find(TeamID);
	if (NULL_GUID != TeamID && m_TeamOrganizings.end() != ite)
	{
		return ite->second;
	}
	return NULL;
}

long COrganizingCtrl::GetGameTeamNum(VOID)
{
	return m_TeamOrganizings.size();
}

VOID COrganizingCtrl::SendTeamSetupToGS(LONG lGsSocket)
{
	CMessage msg(MSG_W2S_TEAM_SetupInit);
	msg.AddEx(&(WSTeam::s_tagDefaultSetup), sizeof(WSTeam::tagDefaultSetup));
	msg.SendToSocket(lGsSocket);
	//GetGame()->SendAllGameServer(&msg);
}

//! 发送全局队伍数据到GS
VOID COrganizingCtrl::SendTeamDataToGS(LONG lGsSocket)
{
	CMessage msg(MSG_W2S_TEAM_DataInit);
	vector<BYTE> vData;
	_AddToByteArray(&vData, (LONG)m_TeamOrganizings.size());

	map<CGUID,WSTeam*>::iterator ite = m_TeamOrganizings.begin();
	for (; ite != m_TeamOrganizings.end(); ite++)
	{
		ite->second->AddToByteArray(vData);
	}
	msg.Add((LONG)vData.size());
	msg.AddEx(&vData[0], vData.size());

	msg.SendToSocket(lGsSocket);
}




//! 响应玩家发出组队邀请
VOID COrganizingCtrl::OnQueryJoin(CMessage *pMsg)
{
	CGUID InviteeID;	
	pMsg -> GetGUID(InviteeID);
	CGUID CaptainID;
	pMsg -> GetGUID(CaptainID);

	if(InviteeID == CaptainID)
	{
		return;
	}

	CPlayer* pInvitee = GetGame() -> GetMapPlayer( InviteeID );
	CPlayer* pCaptain = GetGame() -> GetMapPlayer( CaptainID );

	if (NULL == pCaptain || NULL == pInvitee)
	{
		return;
	}

	const CGUID& InviteeTeamID = pInvitee->GetTeamID();
	const CGUID& CaptainTeamID = pCaptain->GetTeamID();

	if(NULL_GUID != InviteeTeamID)//被邀请者已有队伍
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimHadTeam) );
		return;
	}

	if (NULL_GUID != CaptainTeamID)//操作者有队伍
	{
		WSTeam* pTeam = GetGameTeamByID(CaptainTeamID);
		if (NULL == pTeam)
		{
			//! GS消息发送过来说明队伍在GS上存在，但在WS不能找到该队伍，因此强制解散
			CMessage msg(MSG_W2S_TEAM_Disband);
			msg.Add(CaptainTeamID);
			GetGame()->SendAllGameServer(&msg);
			return;
		}

		if (pTeam->IsAbleControl(eTOCT_CantJoin))
			return;

		if( NULL_GUID == pTeam->IsMaster(CaptainID) )//不是队长
		{
			SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_OperatorNotMaster) );
			return;
		}

		if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
		{
			SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_TeamIsFull) );
			return;
		}
	}
	
	tagPlayerTeamSession PlayerTeamSession;
	PlayerTeamSession.SourceID = CaptainID;
	PlayerTeamSession.TargetID = InviteeID;

	if(!m_InviteSession.CreateSession(PlayerTeamSession))
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_PleaseWait) );
		return;
	}

	CMessage msg( MSG_W2S_TEAM_QUERY_JOIN );
	msg.Add( InviteeID );
	msg.Add( CaptainID );
	msg.Add( pCaptain->GetName() );
	msg.SendGSID( GetGame()->GetGameServerNumber_ByPlayerID(InviteeID) );
}

//! 响应玩家发出组队邀请
VOID COrganizingCtrl::OnQueryJoinByName(CPlayer* pInvitee, CPlayer* pCaptain)
{
	if (NULL == pCaptain || NULL == pInvitee)
	{
		return;
	}

	if(pInvitee->GetExID() == pCaptain->GetExID())
	{
		return;
	}

	const CGUID& InviteeTeamID = pInvitee->GetTeamID();
	const CGUID& CaptainTeamID = pCaptain->GetTeamID();

	const CGUID& CaptainID = pCaptain->GetExID();
	const CGUID& InviteeID = pInvitee->GetExID();

	if(NULL_GUID != InviteeTeamID)//被邀请者已有队伍
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimHadTeam));
		return;
	}

	if (NULL_GUID != CaptainTeamID)//操作者有队伍
	{
		WSTeam* pTeam = GetGameTeamByID(CaptainTeamID);
		if (NULL == pTeam)
		{
			//! GS消息发送过来说明队伍在GS上存在，但在WS不能找到该队伍，因此强制解散
			CMessage msg(MSG_W2S_TEAM_Disband);
			msg.Add(CaptainTeamID);
			GetGame()->SendAllGameServer(&msg);
			return;
		}
		if (pTeam->IsAbleControl(eTOCT_CantJoin))
			return;
		if( NULL_GUID == pTeam->IsMaster(CaptainID) )//不是队长
		{
			SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_OperatorNotMaster) );
			return;
		}

		if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
		{
			SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_TeamIsFull));
			return;
		}
	}
	
	tagPlayerTeamSession PlayerTeamSession;
	PlayerTeamSession.SourceID = CaptainID;
	PlayerTeamSession.TargetID = InviteeID;

	if(!m_InviteSession.CreateSession(PlayerTeamSession))
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_PleaseWait) );
		return;
	}	

	CMessage msg( MSG_W2S_TEAM_QUERY_JOIN );
	msg.Add( InviteeID );
	msg.Add( CaptainID );
	msg.Add( pCaptain->GetName() );
	msg.SendGSID( GetGame()->GetGameServerNumber_ByPlayerID(InviteeID) );
}


//long COrganizingCtrl::GetNewTeamID(VOID)
//{
//	do
//	{
//		long lTeamID = timeGetTime();
//		if (m_TeamOrganizings.end() == m_TeamOrganizings.find(lTeamID))
//		{
//			return lTeamID;
//		}
//	}
//	while(true);
//}

//! 响应玩家答复组队邀请
VOID COrganizingCtrl::OnAnswerJoin(CMessage *pMsg)
{
	CGUID InviteeID;
	pMsg -> GetGUID(InviteeID);
	CGUID CaptainID;
	pMsg -> GetGUID(CaptainID);
	LONG lJoin		= pMsg -> GetByte();

	tagPlayerTeamSession PlayerTeamSession;
	PlayerTeamSession.SourceID = CaptainID;
	PlayerTeamSession.TargetID = InviteeID;

	if(!m_InviteSession.TestSession(PlayerTeamSession))
	{
		SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_OutTime));
		return;
	}

	CPlayer *pInvitee = GetGame() -> GetMapPlayer( InviteeID );
	if(NULL == pInvitee) return;

	if(1 == lJoin) //同意
	{
		if (NULL_GUID != pInvitee->GetTeamID())
		{
			SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_YouHadTeam));
			return;
		}

		CPlayer *pPlayer = GetGame() -> GetMapPlayer( CaptainID );
		if(NULL == pPlayer) return;

		WSTeam *pTeam = (WSTeam*)GetGameTeamByID(pPlayer->GetTeamID());;

		if(NULL == pTeam) //没有队伍
		{
			CGUID TeamID;
			CGUID::CreateGUID(TeamID);//GetNewTeamID();
			pTeam = CreateTeam(TeamID, CaptainID, InviteeID);
			if(NULL == pTeam)
			{
				SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_CreateFailedAndTautology));
				SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_JoinFailedAndTautology));
			}
		}
		else //有队伍
		{
			if (pTeam->IsAbleControl(eTOCT_CantJoin))
				return;
			if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
			{
				SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_TeamIsFull));
				return;
			}
			tagTime Time;
			if(!pTeam->DoJoin(CaptainID, InviteeID, lJoin, Time))
			{
				SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_JoinFailedAndTautology));
				SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_JoinFailedAndTautology));
			}
			else
			{
				if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
				{
					CloseRecruit(CaptainID);
				}
			}
		}
	}
	else if(0 == lJoin)//不同意
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimRefuse));
	}
	else if(2 == lJoin)//! 繁忙
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_Busyness));
	}
	else if(3 == lJoin)//! 没有开组
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_NotOpenTeam));
	}
	else if(4 == lJoin)//! 目标死亡
	{
		SendInfoToOneClient(CaptainID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_AimIsDead));
	}
}

//! 响应改变队长
VOID	COrganizingCtrl::OnChangLeader(CMessage *pMsg)
{
	CGUID guid;
	pMsg -> GetGUID(guid);
	CGUID CaptainID;
	pMsg -> GetGUID(CaptainID);

	CPlayer* pInvitee = GetGame() -> GetMapPlayer( guid );
	CPlayer* pCaptain = GetGame() -> GetMapPlayer( CaptainID );

	if (NULL == pCaptain || NULL == pInvitee)
	{
		return;
	}

	const CGUID& TeamID = pInvitee->GetTeamID();
	const CGUID& CaptainTeamID = pCaptain->GetTeamID();

	if(TeamID != CaptainTeamID || guid == CaptainID)//队伍不同或操作者和操作对象是同一个人
	{
		return;
	}

	WSTeam *pTeam = (WSTeam*)GetGameTeamByID(CaptainTeamID);
	if (NULL == pTeam)
	{
		//! GS消息发送过来说明队伍在GS上存在，但在WS不能找到该队伍，因此强制解散
		CMessage msg(MSG_W2S_TEAM_Disband);
		msg.Add(CaptainTeamID);
		GetGame()->SendAllGameServer(&msg);
		return;
	}
	if (NULL_GUID == pTeam->IsMaster(CaptainID))//不是队长
	{
		return;
	}

	pTeam->Demise(CaptainID, guid);
	CloseRecruit(CaptainID);
}

//! 响应踢人
VOID COrganizingCtrl::OnKickPlayer(CMessage *pMsg)
{
	CGUID guid;
	pMsg -> GetGUID(guid);
	CGUID CaptainID;
	pMsg -> GetGUID(CaptainID);

	CPlayer* pCaptain = GetGame() -> GetMapPlayer( CaptainID );

	if (NULL == pCaptain )
	{
		return;
	}

	const CGUID& TeamID = pCaptain->GetTeamID();
	//long lCaptainTeamID = pCaptain->GetTeamID();

	if(TeamID == NULL_GUID)
	{
		return;
	}

	WSTeam *pTeam = (WSTeam*)GetGameTeamByID(TeamID);
	if (NULL == pTeam )
	{
		//! GS消息发送过来说明队伍在GS上存在，但在WS不能找到该队伍，因此强制解散
		CMessage msg(MSG_W2S_TEAM_Disband);
		msg.Add(TeamID);
		GetGame()->SendAllGameServer(&msg);
		return;
	}

	if (pTeam->IsAbleControl(eTOCT_CantExit))
		return;

	if(!pTeam->PlayerInTeam(guid))
	{
		return;
	}

	if(CaptainID == guid)//踢自己=主动退出
	{
		if(pTeam->GetMemberNum() > 2)
		{
			if (NULL_GUID != pTeam->IsMaster(CaptainID))//是队长，则先设置新队长
			{
				const CGUID& MastterID = pTeam->SetNewMastter();
				if (NULL_GUID != MastterID)
				{
					CMessage msgMC(MSG_W2S_TEAM_MastterChange);
					msgMC.Add(pTeam->GetExID());
					msgMC.Add(MastterID);
					GetGame()->SendAllGameServer(&msgMC);
				}
				else//除了队长已无在线玩家
				{
					//队长退出，解散队伍
					DisbandTeam(pTeam);
					return;
				}
			}
		}
	}
	else
	{
		if (NULL_GUID == pTeam->IsMaster(CaptainID))//不是队长，不能踢其他人
		{
			return;
		}
	}

	pTeam->Exit(guid);

	//检测存在条件
	if (pTeam->GetMemberNum() < 2)
	{
		DisbandTeam(pTeam);
	}
}


VOID COrganizingCtrl::OnSetGoods(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	long eSet = pMsg->GetLong();

	CPlayer *pPlayer = GetGame() -> GetMapPlayer( PlayerGuid );

	if(NULL == pPlayer)
	{
		return;
	}

	WSTeam *pTeam = (WSTeam*)GetGameTeamByID(pPlayer->GetTeamID());

	if (NULL == pTeam)
	{
		//! GS消息发送过来说明队伍在GS上存在，但在WS不能找到该队伍，因此强制解散
		CMessage msg(MSG_W2S_TEAM_Disband);
		msg.Add(pPlayer->GetTeamID());
		GetGame()->SendAllGameServer(&msg);
		return;
	}

	if (pTeam->GetMasterID() != PlayerGuid)
	{
		return;
	}

	pTeam->SetGoodsAllot(eSet);
}

VOID COrganizingCtrl::OnRecruit(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	long lFlag = pMsg->GetLong();

	if (0 == lFlag)
	{
		m_RecruitingPlayer.erase(PlayerGuid);
	}
	else
	{
		CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
		if (NULL == pPlayer)
		{
			return;
		}
		WSTeam *pTeam = (WSTeam*)(GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID()));
		if (NULL != pTeam)
		{
			if (pTeam->IsAbleControl(eTOCT_CantJoin))
				return;
			if (pTeam->GetMasterID() != PlayerGuid)
			{
				return;
			}
			if (WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
			{
				return;
			}
		}

		
		m_RecruitingPlayer[PlayerGuid] = pPlayer;
	}

	pMsg->SetType(MSG_W2S_TEAM_RECRUIT);
	pMsg->SendGSID(pMsg->GetGSID());
}

//! 关闭玩家的招募
VOID COrganizingCtrl::CloseRecruit(const CGUID &PlayerGuid)
{
	CMessage msg(MSG_W2S_TEAM_RECRUIT);
	msg.Add(PlayerGuid);
	msg.Add(0L);
	msg.SendAll();
}

VOID COrganizingCtrl::OnBeRecruited(CMessage *pMsg)
{
	CGUID InviteeID;
	pMsg -> GetGUID(InviteeID);
	CGUID CaptainID;
	pMsg -> GetGUID(CaptainID);
	BOOL bJoin		= pMsg -> GetByte();

	CPlayer *pPlayer = GetGame() -> GetMapPlayer( CaptainID );
	if(NULL == pPlayer) return;
	WSTeam *pTeam = (WSTeam*)GetGameTeamByID(pPlayer->GetTeamID());;

	if(NULL == pTeam) //没有队伍
	{
		CGUID TeamID;
		CGUID::CreateGUID(TeamID);//GetNewTeamID();
		pTeam = CreateTeam(TeamID, CaptainID, InviteeID);
		if(NULL == pTeam)
		{
			SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_CreateFailedAndTautology));
		}
	}
	else //有队伍
	{
		if (pTeam->IsAbleControl(eTOCT_CantJoin))
			return;

		if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
		{
			SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_TeamIsFull));
			return;
		}
		tagTime Time;
		if(!pTeam->DoJoin(CaptainID, InviteeID, bJoin, Time))
		{
			SendInfoToOneClient(InviteeID, CStringReading::LoadString(TEAM_STRING_TYPEID, eTSID_JoinFailedAndTautology));
		}
		else
		{
			if(WSTeam::s_tagDefaultSetup.lMaxMemberNum <= pTeam->GetMemberNum())
			{
				CloseRecruit(CaptainID);
			}
		}
	}
}

//! 响应聊天
VOID COrganizingCtrl::OnChat(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL != pPlayer)
	{
		pMsg->SetType(MSG_W2S_TEAM_CHAT);
		pMsg->SendToSocket(pPlayer->GetGsSocket());
	}
}

//! 响应成员详细信息转发
VOID COrganizingCtrl::OnTeamMemberAllIdioinfo(CMessage *pMsg)
{
	tagMemberIdioinfo MemberIdioinfo;
	pMsg->GetEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
	CGUID guid, ReceiverGuid;
	pMsg->GetGUID(guid);
	pMsg->GetGUID(ReceiverGuid);

	CPlayer *pReceiver = GetGame()->GetMapPlayer(ReceiverGuid);
	if(NULL != pReceiver)
	{
		pMsg->SetType(MSG_W2S_TEAM_MemberAllIdioinfo);
		pMsg->SendToSocket(pReceiver->GetGsSocket());
	}
}

//! 响应成员位置信息转发
VOID COrganizingCtrl::OnTeamMemberPos(CMessage *pMsg)
{
	LONG lDataSize = pMsg->GetLong();
	vector<BYTE> vData(lDataSize);
	pMsg->GetEx(&vData[0], lDataSize);

	LONG lReceiverNum = pMsg->GetLong();
	set<LONG> setReceiverGsSocket;
	for (LONG i = 0; i < lReceiverNum; ++i)
	{
		CGUID ReceiverGuid;
		pMsg->GetGUID(ReceiverGuid);
		CPlayer *pReceiver = GetGame()->GetMapPlayer(ReceiverGuid);
		if(NULL != pReceiver)
		{
			setReceiverGsSocket.insert(pReceiver->GetGsSocket());
		}
	}

	pMsg->SetType(MSG_W2S_TEAM_MemberPos);
	for (set<LONG>::iterator ite = setReceiverGsSocket.begin(); ite != setReceiverGsSocket.end(); ++ite)
	{
		pMsg->SendToSocket(*ite);
	}
}

bool COrganizingCtrl::PlayerJoinTeam(const CGUID& PlayerGuid, const CGUID& TeamGuid)
{
	/*if( m_MemberIDvsTeamID.end() == m_MemberIDvsTeamID.find(PlayerGuid) )
	{
		return false;
	}*/
	if (NULL_GUID == TeamGuid)
	{
		return false;
	}
	m_MemberIDvsTeamID[PlayerGuid] = TeamGuid;
	return true;
}

VOID COrganizingCtrl::PlayerLeaveTeam(const CGUID& PlayerGuid)
{
	m_MemberIDvsTeamID.erase(PlayerGuid);
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL != pPlayer)
	{
		pPlayer->SetTeamID(NULL_GUID);
	}
}

const CGUID& COrganizingCtrl::FindPlayerTeamID(const CGUID& PlayerGuid)
{
	if (m_MemberIDvsTeamID.end() == m_MemberIDvsTeamID.find(PlayerGuid))
	{
		return NULL_GUID;
	}
	return m_MemberIDvsTeamID[PlayerGuid];
}

//! 创建队伍，并加入管理列表
WSTeam*	COrganizingCtrl::CreateTeam(const CGUID& TeamGuid, const CGUID& MastterGuid, const CGUID& MemberGuid)
{
	WSTeam* pTeam = MP_NEW WSTeam();
	if(pTeam->Initial(TeamGuid, MastterGuid, MemberGuid))
	{
		map<CGUID, WSTeam*>::iterator ite = m_TeamOrganizings.find(TeamGuid);
		if(m_TeamOrganizings.end() != ite)
			DisbandTeam(ite->second);
		m_TeamOrganizings[TeamGuid] = pTeam;
		return pTeam;
	}

	MP_DELETE(pTeam);
	return NULL;
}

//------------------------

//! 解散队伍
VOID COrganizingCtrl::DisbandTeam(WSTeam *pTeam)
{
	assert(NULL != pTeam);
	if (pTeam->IsAbleControl(eTOCT_CantDisband))
		return;

	const CGUID& TeamID = pTeam->GetExID();

	for (map<CGUID, CGUID>::iterator ite = m_MemberIDvsTeamID.begin(); ite != m_MemberIDvsTeamID.end(); )
	{
		if(ite->second == TeamID)
		{
			ite = m_MemberIDvsTeamID.erase(ite);
			continue;
		}
		++ite;
	}

	pTeam->Disband(TeamID);

	m_TeamOrganizings[TeamID] = NULL;
	m_TeamOrganizings.erase(TeamID);

	MP_DELETE(pTeam);
	
}



//! 日志
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------------------------
<!--帮会创建/解散日志-->
<Table name="t150_faction_log" log_no="150" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="player"   head_name=""></item>
<item type="faction"  head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：创建；1解散）-->
</Table>
*/
bool LogicLogInterface::LogT150_faction_log(LPCSTR pOprName, const CGUID &pOprGUID, CFaction *pFaction, LONG lAct)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;
	if(NULL == m_pLogClient)return false;
	if(NULL == pOprName || NULL == pFaction) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		150, strTime.c_str(),
		&pOprGUID,	pOprName,
		&(pFaction->GetExID()),	pFaction->GetName(),
		lAct
		);
}

/* --------------------------------------------------------------------
<!--成员申请加入/退出日志-->
<Table name="t160_faction_join_leave_log" log_no="160" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="player"   head_name=""></item>
<item type="faction"  head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：申请；1退出）-->
</Table>
*/
bool LogicLogInterface::LogT160_faction_join_leave_log(const char *pOpterName, const CGUID *pOpterGuid, CFaction *pFaction, LONG lAct)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;
	if(NULL == m_pLogClient)return false;
	if(NULL == pOpterGuid || NULL == pOpterName || NULL == pFaction) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		160, strTime.c_str(),
		pOpterGuid,	pOpterName,
		&(pFaction->GetExID()),	pFaction->GetName(),
		lAct
		);
}


/* --------------------------------------------------------------------
<!--同意进入/开除成员日志-->
<Table name="t170_faction_agree_kick_log" log_no="170" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="player"   head_name=""></item>
<item type="player"   head_name="aim"></item>
<item type="faction"  head_name=""></item>
<item type="alone"    Item_name="act_type"        data_type="int32"></item><!--动作类型（0：同意；1开除）-->
</Table>
*/
bool LogicLogInterface::LogT170_faction_agree_kick_log(const char *pOpterName, const CGUID *pOpterGuid, const char *pAimName, const CGUID *pAimGuid, CFaction *pFaction, LONG lAct)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;
	if(NULL == m_pLogClient)return false;
	if(NULL == pOpterName || NULL == pOpterGuid || NULL == pAimName || NULL == pAimGuid || NULL == pFaction) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		170, strTime.c_str(),
		pOpterGuid, pOpterName,
		pAimGuid,	pAimName,
		&(pFaction->GetExID()),	pFaction->GetName(),
		lAct
		);
}