#include "StdAfx.h"
#include "Faction.h"
#include "OrganizingCtrl.h"
#include "OrganizingParam.h"
#include "../GlobalRgnManager.h"
#include "../Player.h"
#include "../WorldRegion.h"
#include "../Country/Country.h"
#include "../Country/CountryHandler.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"
#include "../Goods/Goods.h"
#include "../Goods/GoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! 用于从DB数据构造以存在的对象
CFaction::CFaction(void)
:m_dwDataChanged(eFDCF_Not)
,m_dwDisbandTime(0)
,m_dwNegativeTime(0)
,m_dwSubResTime(0)
{
	SetName("");
}

//! 用于游戏中玩家创建新的对象
CFaction::CFaction(const CGUID &FacGuid, const char *pName, CPlayer *pMaster,const char *pPro)
:m_dwDataChanged(eFDCF_BaseInfo | eFDCF_Member | eFDCF_Apply)
,m_dwNegativeTime(0)
,m_dwSubResTime(0)
{
	assert(NULL != pMaster);

	SetExID(FacGuid);
	SetName(pName);
	m_FacPublicData.lLevel = 1;
    long    lTime = (long)time(NULL);
	//! 基本信息数据
	LONG lNameSize = (long)strlen(pMaster->GetName());
	lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
	memmove(m_FacBaseData.szMasterName, pMaster->GetName(), lNameSize);

	m_FacBaseData.lCountryID	= pMaster->GetCountry();
	m_FacBaseData.MasterGuid	= pMaster->GetExID();
	m_FacBaseData.lCurrRes		= 0;
	m_FacBaseData.bOpenResWar	= FALSE;
	m_FacBaseData.CreateTime	= lTime;

    strcpy(m_Pronounce.szBody, pPro);
    strcpy(m_Pronounce.szName, pMaster->GetName());
    m_Pronounce.Time = lTime;
    m_Pronounce.MemberGuid = pMaster->GetExID();

	//! 成员信息列表
	AddNewMember(pMaster, TRUE);

	m_dwDisbandTime = lTime;
}

CFaction::~CFaction(void)
{
}


void CFaction::Insert(void)
{
	CEntityGroup* pTmpFaction = NULL;//GetGame().GetEntityManager()->NewBaseEntity(COM_FACTION, NULL_GUID,COM_FACTION, NULL_GUID);
	CEntityManager::DBEntityComponentMapItr facMapItr = GetGame().GetEntityManager()->GetBaseEntityMap().find(string("[factiongroup]"));
	if(facMapItr != GetGame().GetEntityManager()->GetBaseEntityMap().end())
	{
		if(facMapItr->second.size() == 0)
		{
			Log4c::Warn(ROOT_MODULE, GetText("WS_FACTION_10"));
			return;
		}
		CEntityGroup* pFacGroup = (CEntityGroup*)(facMapItr->second.begin()->second);

		pTmpFaction = (CEntityGroup*)pFacGroup->FindEntityBytravelTree(GetExID());
		if(pTmpFaction == NULL)
		{
			pTmpFaction = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[faction]"), GetExID());
			pFacGroup->AddChild(pTmpFaction);
		}
	}
	if(pTmpFaction)
	{
		GetGame().GetEntityManager()->CFactionToCDBFaction(this, pTmpFaction);
		GetGame().GetEntityManager()->CreateInsertFactionSession(pTmpFaction);
	}
}
void CFaction::Save(CEntityGroup** ppTmpFaction)
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame().GetStatckFileName(), "Save start");
#endif

	if(eFDCF_Not == m_dwDataChanged) 
		return;
	//GetGame().GetEntityManager()->NewBaseEntity(COM_FACTION, NULL_GUID,COM_FACTION, NULL_GUID);
	CEntityManager::DBEntityComponentMapItr facMapItr = GetGame().GetEntityManager()->GetBaseEntityMap().find(string("[factiongroup]"));
	if(facMapItr != GetGame().GetEntityManager()->GetBaseEntityMap().end())
	{
		if(facMapItr->second.size() == 0)
		{
			Log4c::Warn(ROOT_MODULE, GetText("WS_FACTION_11"));
			return;
		}
		CEntityGroup* pFacGroup = (CEntityGroup*)(facMapItr->second.begin()->second);

		(*ppTmpFaction) = (CEntityGroup*)pFacGroup->GetChild(GetExID());
		if((*ppTmpFaction) == NULL)
		{
			(*ppTmpFaction) = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[faction]"), GetExID());
			pFacGroup->AddChild((*ppTmpFaction));
		}
	}
	if((*ppTmpFaction))
	{
#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame().GetStatckFileName(), "CFactionToCDBFaction start");
#endif
		GetGame().GetEntityManager()->CFactionToCDBFaction(this, (*ppTmpFaction));
#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame().GetStatckFileName(), "CFactionToCDBFaction end");
#endif
		m_dwDataChanged = eFDCF_Not;
	}

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame().GetStatckFileName(), "Save end");
#endif

}

// 添加到CByteArray
BOOL CFaction::CodeToMsgSet(DBWriteSet &DbWriteSet)
{
	//! 公共信息
	DbWriteSet.AddToByteArray(m_FacPublicData.guid);
	DbWriteSet.AddToByteArray(m_FacPublicData.szName, MAXSIZE_FactionName);
	DbWriteSet.AddToByteArray(m_FacPublicData.lCurrSkillID);
	DbWriteSet.AddToByteArray(m_FacPublicData.lLevel);

	//! 基本信息数据
	DbWriteSet.AddToByteArray(&m_FacBaseData, sizeof(tagFacBaseData));
	//! 宣告宣言
	DbWriteSet.AddToByteArray(&m_Pronounce, sizeof(tagOrgPronounceWord));
	//! 会徽
	DbWriteSet.AddToByteArray((LONG)m_vIconData.size());
	if(0 != m_vIconData.size())
	{
		DbWriteSet.AddToByteArray(&m_vIconData[0],(long)m_vIconData.size());
	}

	//! 成员信息列表
	DbWriteSet.AddToByteArray((LONG)m_mapMemInfo.size());
	for (map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.begin(); ite != m_mapMemInfo.end(); ++ite)
	{
		DbWriteSet.AddToByteArray(&(ite->second), sizeof(tagFacMemInfo));
	}
	//! 申请列表
	DbWriteSet.AddToByteArray((LONG)m_mapFacApplyPerson.size());
	for (map<CGUID, tagFacApplyPerson>::iterator ite = m_mapFacApplyPerson.begin(); ite != m_mapFacApplyPerson.end(); ++ite)
	{
		DbWriteSet.AddToByteArray(&(ite->second), sizeof(tagFacApplyPerson));
	}
	////! 留言列表
	//DbWriteSet.AddToByteArray((LONG)m_vFacLeaveWord.size());
	//for (LONG i = 0; i < m_vFacLeaveWord.size(); ++i)
	//{
	//	DbWriteSet.AddToByteArray(&(m_vFacLeaveWord[i]), sizeof(tagOrgLeaveWord));
	//}
	//! 宣告宣言
	DbWriteSet.AddToByteArray(&m_Pronounce, sizeof(tagOrgPronounceWord));
	DbWriteSet.AddToByteArray(&m_LeaveWord, sizeof(tagOrgPronounceWord));
	//! 数据变更标记
	DbWriteSet.AddToByteArray(m_dwDataChanged);
	
	DbWriteSet.AddToByteArray(m_FacBaseData.lFacBattle);
	DbWriteSet.AddToByteArray(m_FacBaseData.lFacArmyInvest);
	//据点信息
	DbWriteSet.AddToByteArray((long)m_mapBaseInfo.size());
	for (map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.begin();
		it!=m_mapBaseInfo.end();it++)
	{
		DbWriteSet.AddToByteArray(&(it->second),sizeof(tagBaseInfo));
	}
	
	//家族科技
	DbWriteSet.AddToByteArray((long)m_TechLv.size());
	for (map<LONG,LONG>::iterator it=m_TechLv.begin();it!=m_TechLv.end();it++)
	{
		DbWriteSet.AddToByteArray(it->first);
		DbWriteSet.AddToByteArray(it->second);
	}
	//战争信息
	DbWriteSet.AddToByteArray((long)m_Enemy.size());
	map<eBattleType,list<tagEnemy>>::iterator it=m_Enemy.begin();
	for (;it!=m_Enemy.end();it++)
	{
		DbWriteSet.AddToByteArray((BYTE)it->first);
		list<tagEnemy>& Enemy=it->second;
		DbWriteSet.AddToByteArray((long)Enemy.size());
		list<tagEnemy>::iterator itItr=Enemy.begin();
		for (;itItr!=Enemy.end();itItr++)
		{
			DbWriteSet.AddToByteArray(&(*itItr),sizeof(tagEnemy));
		}
	}
	return TRUE;
}

BOOL CFaction::DecodeFromMsgSet(DBReadSet &DbReadSet)
{
	//! 公共信息
	
	DbReadSet.GetBufferFromByteArray(m_FacPublicData.guid);
	DbReadSet.GetBufferFromByteArray(m_FacPublicData.szName, MAXSIZE_FactionName);
	m_FacPublicData.lCurrSkillID	= DbReadSet.GetLongFromByteArray();
	m_FacPublicData.lLevel			= DbReadSet.GetLongFromByteArray();

	//! 基本信息数据
	DbReadSet.GetBufferFromByteArray(&m_FacBaseData, sizeof(tagFacBaseData));
	//! 宣言
	DbReadSet.GetBufferFromByteArray(&m_Pronounce, sizeof(tagOrgPronounceWord));
	//! 会徽
	LONG lIocnSize = DbReadSet.GetLongFromByteArray();
	if(0 != lIocnSize)
	{
		m_vIconData.clear();
		m_vIconData.resize(lIocnSize);
		DbReadSet.GetBufferFromByteArray(&m_vIconData[0], lIocnSize);
	}

	//! 成员信息列表
	m_mapMemInfo.clear();
	LONG lMemInfoSize = DbReadSet.GetLongFromByteArray();
	for (LONG i = 0; i < lMemInfoSize; ++i)
	{
		tagFacMemInfo FacMemInfo;
		DbReadSet.GetBufferFromByteArray(&FacMemInfo, sizeof(tagFacMemInfo));
		FacMemInfo.bIsOnline = FALSE;
		CPlayer *pPlayer = GetGame().GetMapPlayer(FacMemInfo.MemberGuid);
		if (NULL != pPlayer)
		{
			const char* szCdkey = pPlayer->GetAccount();
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				Log4c::Warn(ROOT_MODULE,FormatText("WS_FACTION_12", szCdkey));
				return FALSE;
			}
			if(tLoginInfo && PLAYER_STATE_BACKTOLOGIN != tLoginInfo->GetPlayerState())
			{
				FacMemInfo.bIsOnline = TRUE;
			}
		}
		m_mapMemInfo[FacMemInfo.MemberGuid] = FacMemInfo;
	}
	//! 申请列表
	m_mapFacApplyPerson.clear();
	LONG lApplyPersonSize = DbReadSet.GetLongFromByteArray();
	for (LONG i = 0; i < lApplyPersonSize; ++i)
	{
		tagFacApplyPerson FacApplyPerson;
		DbReadSet.GetBufferFromByteArray(&FacApplyPerson, sizeof(tagFacApplyPerson));
		m_mapFacApplyPerson[FacApplyPerson.PlayerGuid] = FacApplyPerson;
	}
	////! 留言列表
	//m_vFacLeaveWord.clear();
	//LONG lLeaveWordSize = DbReadSet.GetLongFromByteArray();
	//for (LONG i = 0; i < lLeaveWordSize; ++i)
	//{
	//	tagOrgLeaveWord OrgLeaveWord;
	//	DbReadSet.GetBufferFromByteArray(&OrgLeaveWord, sizeof(tagOrgLeaveWord));
	//	m_vFacLeaveWord.push_back(OrgLeaveWord);
	//}
	//! 宣告宣言
	DbReadSet.GetBufferFromByteArray(&m_Pronounce, sizeof(tagOrgPronounceWord));
	DbReadSet.GetBufferFromByteArray(&m_LeaveWord, sizeof(tagOrgPronounceWord));
	//! 数据变更标记
	m_dwDataChanged = DbReadSet.GetLongFromByteArray();

	m_FacBaseData.lFacBattle=DbReadSet.GetLongFromByteArray();
	m_FacBaseData.lFacArmyInvest=DbReadSet.GetLongFromByteArray();
	//据点信息
	m_mapBaseInfo.clear();
	LONG lSize=DbReadSet.GetLongFromByteArray();
	tagBaseInfo tgInfo;
	for (LONG i=0;i<lSize;i++)
	{
		char cBaseName[32]={0};
		DbReadSet.GetBufferFromByteArray(cBaseName,32);
		tgInfo._lTickets=DbReadSet.GetLongFromByteArray();
		tgInfo._lBattleVal=DbReadSet.GetLongFromByteArray();
		m_mapBaseInfo[cBaseName]=tgInfo;
	}
	//家族科技
	m_TechLv.clear();

	//战争信息
	m_Enemy.clear();

	return TRUE;
}

//!		添加一个新会员
void CFaction::AddNewMember(CPlayer *pPlayer, BOOL bIsMaster/* = FALSE*/)
{
	assert(NULL != pPlayer);

	tagFacMemInfo FacMemInfo;
    long    lTime = (long)time(NULL);
	long lNameSize = (long)strlen(pPlayer->GetName());
	lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
	memmove(FacMemInfo.szName, pPlayer->GetName(), lNameSize);

	FacMemInfo.MemberGuid		= pPlayer->GetExID();
	FacMemInfo.lLvl				= pPlayer->GetLevel();
	FacMemInfo.lOccu			= pPlayer->GetOccupation();
	FacMemInfo.lJobLvl			= (bIsMaster) ? Exceptive_JobLvl : 0;
	FacMemInfo.LastOnlineTime	= lTime;
	FacMemInfo.lContribute		= 0;
	FacMemInfo.bIsOnline		= TRUE;
	FacMemInfo.cHide			= pPlayer->GetHideFlag();

	CWorldRegion *pWorldRegion = GetGame().GetGlobalRgnManager()->FindRgnByGUID(pPlayer->GetRegionExID());
	if (pWorldRegion)
	{
		const char *pRegionName = pWorldRegion->GetName();
		long lNameSize = (long)strlen(pRegionName);
		lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
		memmove(FacMemInfo.szRegion, pRegionName, lNameSize);		//! 所在地图
	}
	else
	{
		CGlobalRgnManager::tagRegion* pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalRegion(pPlayer->GetRegionID());
		if(!pTagRgn)
			pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(pPlayer->GetRegionID());
		if(!pTagRgn)
			pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplateTeamRegion(pPlayer->GetRegionID());
		if(!pTagRgn)
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_FACTION_40", pPlayer->GetRegionID()));
	}

	assert(m_mapMemInfo.find(FacMemInfo.MemberGuid) == m_mapMemInfo.end());
	m_mapMemInfo[FacMemInfo.MemberGuid] = FacMemInfo;
	pPlayer->SetFactionInfo(this);
	pPlayer->SetUnionID(GetSuperior());

	//! 人数大于25%，取消解散计时
	const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
	if(NULL != pFacLevelSetup)
	{
		if(GetMemberNum() > pFacLevelSetup->lDisbandMemNum)
			m_dwDisbandTime = 0;
	}
	else
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_FACTION_42"));
}

//! 添加成员
BOOL CFaction::AddMeber(const tagFacApplyPerson& FacApplyPerson, const CGUID &InviterGuid)
{
	CPlayer *pPlayer = GetGame().GetMapPlayer(FacApplyPerson.PlayerGuid);
	if(NULL != pPlayer)
	{
		AddNewMember(pPlayer, FALSE);
	}
	else
	{
		tagFacMemInfo NewFacMemInfo;

		strcpy(NewFacMemInfo.szName, FacApplyPerson.szName);

		NewFacMemInfo.MemberGuid		= FacApplyPerson.PlayerGuid;
		NewFacMemInfo.lLvl				= FacApplyPerson.lLvl;
		NewFacMemInfo.lOccu				= FacApplyPerson.lOccu;
		NewFacMemInfo.lJobLvl			= 0;
		NewFacMemInfo.LastOnlineTime	= (LONG)time(NULL);
		NewFacMemInfo.lContribute		= 0;
		NewFacMemInfo.bIsOnline			= FALSE;

		assert(m_mapMemInfo.find(NewFacMemInfo.MemberGuid) == m_mapMemInfo.end());
		m_mapMemInfo[NewFacMemInfo.MemberGuid] = NewFacMemInfo;
	}

	tagFacMemInfo &FacMemInfo = m_mapMemInfo[FacApplyPerson.PlayerGuid];
	CMessage msg(MSG_W2S_Faction_AddMember_Notify);
	msg.AddEx(&FacMemInfo, sizeof(tagFacMemInfo));
	msg.Add(GetExID());
	msg.Add(InviterGuid);
	msg.SendToAllGS();
	m_dwDataChanged |= eFDCF_Member;

	CPlayer *pInviter = GetGame().GetMapPlayer(InviterGuid);
	const char *pInviterName = "";
	if(NULL != pInviter)
		pInviterName = pInviter->GetName();

	GetLogicLogInterface()->LogT170_faction_agree_kick_log(pInviterName, &InviterGuid, FacMemInfo.szName, &(FacMemInfo.MemberGuid), this, 0);

	//! 人数大于25%，取消解散计时
	const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
	if(NULL != pFacLevelSetup)
	{
		if(GetMemberNum() > pFacLevelSetup->lDisbandMemNum)
			m_dwDisbandTime = 0;
	}
	else
    {
		//PutStringToFile("Organize", "工作定时操作：没有找到家族对应等级的配置");
        Log4c::Warn(ROOT_MODULE,"工作定时操作：没有找到家族对应等级的配置");
    }
	return TRUE;
}

//! 得到当前资源数
LONG CFaction::GetRes(VOID)
{
	return m_FacBaseData.lCurrRes;
}
//! 得到当前经验数
LONG CFaction::GetExp(VOID)
{
	return m_FacBaseData.lCurrExp;
}
//! 得到当前成员贡献数
LONG CFaction::GetMemberContribute(const CGUID &MemberGuid)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
		return ite->second.lContribute;
	return -1;
}

//! 增加资源（降低传负数）
BOOL CFaction::AddRes(LONG lDeltaRes)
{
	/*if(0 > m_FacBaseData.lCurrRes + lDeltaRes)
		return FALSE;*/
	m_FacBaseData.lCurrRes += lDeltaRes;
	//如果资源变为正数,清空家族降级计时
	if (m_FacBaseData.lCurrRes>0)
	{
		m_dwNegativeTime=0;
	}
	CMessage msg(MSG_W2S_Faction_ChangeRes_Notify);
	msg.Add(GetExID());
	msg.Add(lDeltaRes);
	msg.Add(m_FacBaseData.lCurrRes);
	msg.SendToAllGS();

	m_dwDataChanged |= eFDCF_BaseInfo;
	return TRUE;
}
//! 增加经验（降低传负数）
BOOL CFaction::AddExp(LONG lDeltaExp)
{
	if(0 == lDeltaExp)
		return TRUE;
	if(0 > m_FacBaseData.lCurrExp + lDeltaExp)
		return FALSE;

	LONG lBeforeExp = m_FacBaseData.lCurrExp;
	m_FacBaseData.lCurrExp += lDeltaExp;
	GetLogicLogInterface()->LogT260_faction_experience_log(this, lBeforeExp);
	
	CMessage msg(MSG_W2S_Faction_ChangeExp_Notify);
	msg.Add(GetExID());
	msg.Add(lDeltaExp);
	msg.Add(m_FacBaseData.lCurrExp);
	msg.SendToAllGS();

	m_dwDataChanged |= eFDCF_BaseInfo;
	return TRUE;
}
//! 增加成员贡献（降低传负数）
BOOL CFaction::AddMemberContribute(const CGUID &MemberGuid, LONG lDeltaContribute)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
	{
		if(0 <= ite->second.lContribute + lDeltaContribute)
		{
			ite->second.lContribute += lDeltaContribute;

			CMessage msg(MSG_W2S_Faction_ChangeMemberContribute_Notify);
			msg.Add(GetExID());
			msg.Add(MemberGuid);
			msg.Add(lDeltaContribute);
			msg.Add(ite->second.lContribute);
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_Member;
			return TRUE;
		}
	}
	return FALSE;
}

//! 设置一个职级的名称
BOOL CFaction::SetJobName(LONG lJobLvl, const char *pNewName)
{
	if (0 < lJobLvl && MAXNUM_JobLvl >= lJobLvl && NULL != pNewName)
	{
		if(0 != strcmp(m_FacBaseData.arr_szJobName[lJobLvl - 1], pNewName))
		{
			assert(strlen(pNewName) < MAX_MEMBER_NAME_SIZE);
			strcpy(m_FacBaseData.arr_szJobName[lJobLvl - 1], pNewName);
			CMessage msg(MSG_W2S_ORGSYS_SetJobName_Notify);
			msg.Add(lJobLvl);
			msg.Add(m_FacBaseData.arr_szJobName[lJobLvl - 1]);
			msg.Add(GetExID());
			msg.SendToAllGS();
			m_dwDataChanged |= eFDCF_BaseInfo;
			return TRUE;
		}
	}
	return FALSE;
}


//! 设置一个职级的权限
BOOL CFaction::SetJobPurview(LONG lJobLvl, LONG lPurrview)
{
	if (0 < lJobLvl && MAXNUM_JobLvl >= lJobLvl)
	{
		LONG lAllPurview = WSOrganizingParam::getInstance().GetCurrLvlAllPurview(GetLvl(eUT_FactionLevel));
		m_FacBaseData.arrJobPurview[lJobLvl - 1] = lAllPurview & lPurrview;
		CMessage msg(MSG_W2S_ORGSYS_SetJobPurview_Notify);
		msg.Add(lJobLvl);
		msg.Add(m_FacBaseData.arrJobPurview[lJobLvl - 1]);
		msg.Add(GetExID());
		msg.SendToAllGS();
		m_dwDataChanged |= eFDCF_BaseInfo;
		return TRUE;
	}
	return FALSE;
}

//!		设置成员的职级
BOOL	CFaction::SetJobLvl(const CGUID& MemberGuid, LONG lJobLvl)
{
	if (0 <= lJobLvl && MAXNUM_JobLvl >= lJobLvl)
	{
		map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
		if(m_mapMemInfo.end() != ite && MemberGuid != m_FacBaseData.MasterGuid)
		{
			ite->second.lJobLvl = lJobLvl;

			CMessage msg(MSG_W2S_ORGSYS_SetMemJobLvl_Notify);
			msg.Add(GetExID());
			msg.Add(MemberGuid);
			msg.Add(lJobLvl);
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_BaseInfo;
			return TRUE;
		}
	}

	return FALSE;
}

//! 设置成员称号
BOOL CFaction::SetMemberTitle(const CGUID& OptGuid, const CGUID& MemberGuid, const char *pNewTitle)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
	{
		if (MemberGuid == m_FacBaseData.MasterGuid && OptGuid != m_FacBaseData.MasterGuid)
			return FALSE;

		if (0 != strcmp(ite->second.szTitle, pNewTitle))
		{
			strcpy(ite->second.szTitle, pNewTitle);

			CMessage msg(MSG_W2S_Faction_SetMemberTitle_Notify);
			msg.Add(GetExID());
			msg.Add(OptGuid);
			msg.Add(MemberGuid);
			msg.Add(pNewTitle);
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_Member;
			return TRUE;
		}
	}
	return FALSE;
}

//!		设置成员地图
BOOL CFaction::MemberMapChange(const CGUID& MemberGuid, const char *pMapName)
{
	if(NULL != pMapName)
	{
		map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
		if(m_mapMemInfo.end() != ite)
		{
			long lNameSize = (long)strlen(pMapName);
			lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
			memmove(ite->second.szRegion, pMapName, lNameSize);		//! 所在地图

			CMessage msg(MSG_W2S_Faction_MemberMap_Notify);
			msg.Add(GetExID());
			msg.Add(MemberGuid);
			msg.Add(pMapName);
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_Member;
			return TRUE;
		}
	}
	return FALSE;
}

//! 设置成员等级
BOOL CFaction::MemberLevelChange(const CGUID& MemberGuid, LONG lLevel)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
	{
		ite->second.lLvl = lLevel;

		CMessage msg(MSG_W2S_Faction_MemberLevel_Notify);
		msg.Add(GetExID());
		msg.Add(MemberGuid);
		msg.Add(lLevel);
		msg.SendToAllGS();

		m_dwDataChanged |= eFDCF_Member;
		return TRUE;
	}
	return FALSE;
}

//! 设置成员隐匿状态
BOOL CFaction::MemberHideChange(const CGUID& MemberGuid, BYTE cHide)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
	{
		ite->second.cHide = cHide;

		CMessage msg(MSG_W2S_Faction_MemberHide_Notify);
		msg.Add(GetExID());
		msg.Add(MemberGuid);
		msg.Add(cHide);
		msg.SendToAllGS();

		m_dwDataChanged |= eFDCF_Member;
		return TRUE;
	}
	return FALSE;
}

//! 设置成员职业
BOOL CFaction::MemberOccuChange(const CGUID& MemberGuid, LONG lOccu)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemberGuid);
	if(m_mapMemInfo.end() != ite)
	{
		ite->second.lOccu = lOccu;

		CMessage msg(MSG_W2S_Faction_MemberOccu_Notify);
		msg.Add(GetExID());
		msg.Add(MemberGuid);
		msg.Add(lOccu);
		msg.SendToAllGS();

		m_dwDataChanged |= eFDCF_Member;
		return TRUE;
	}
	return FALSE;
}

//! 添加一个申请者
BOOL CFaction::AddApplicant(const CGUID& ApplicantGuid)
{
	map<CGUID, tagFacApplyPerson>::iterator ite = m_mapFacApplyPerson.find(ApplicantGuid);
	if (m_mapFacApplyPerson.end() == ite)
	{
		CPlayer *pPlayer = GetGame().GetMapPlayer(ApplicantGuid);
		if (NULL != pPlayer)
		{
			tagFacApplyPerson FacApplyPerson;
			FacApplyPerson.lLvl			= pPlayer->GetLevel();
			FacApplyPerson.lOccu		= pPlayer->GetOccupation();
			FacApplyPerson.PlayerGuid	= ApplicantGuid;
			FacApplyPerson.lTime		= (long)time(NULL);
			strcpy_s(FacApplyPerson.szName, MAX_MEMBER_NAME_SIZE, pPlayer->GetName());

			m_mapFacApplyPerson[ApplicantGuid] = FacApplyPerson;

			CMessage msg(MSG_W2S_Faction_Applicant_Notify);
			msg.Add(GetExID());
			msg.Add((LONG)TRUE);
			msg.AddEx(&FacApplyPerson, sizeof(tagFacApplyPerson));
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_Apply;
			return TRUE;
		}	
	}
	return FALSE;
}


//! 找到一个申请者
const	tagFacApplyPerson*	CFaction::FindApplicant(const CGUID& ApplicantGuid)
{
	map<CGUID, tagFacApplyPerson>::iterator ite = m_mapFacApplyPerson.find(ApplicantGuid);
	if (m_mapFacApplyPerson.end() != ite)
		return &(ite->second);
	return NULL;
}

//! 删除一个申请者
BOOL CFaction::DelApplicant(const CGUID& ApplicantGuid)
{
	map<CGUID, tagFacApplyPerson>::iterator ite = m_mapFacApplyPerson.find(ApplicantGuid);
	if (m_mapFacApplyPerson.end() != ite)
	{
		m_mapFacApplyPerson.erase(ite);

		CMessage msg(MSG_W2S_Faction_Applicant_Notify);
		msg.Add(GetExID());
		msg.Add((LONG)FALSE);
		msg.Add(ApplicantGuid);	
		msg.SendToAllGS();

		m_dwDataChanged |= eFDCF_Apply;
		return TRUE;
	}
	return FALSE;
}

//! 将申请者加入到一个外部传入的列表
void CFaction::AddApplicantToMap(map<CGUID, CGUID> &mapaLLApplicant)
{
	map<CGUID, tagFacApplyPerson>::iterator ite = m_mapFacApplyPerson.begin();
	for (; ite != m_mapFacApplyPerson.end(); ++ite)
	{
		mapaLLApplicant[ite->first] = GetExID();
	}
}

//初始化
BOOL CFaction::Initial()
{
	return FALSE;
}

//组织内部聊天
void CFaction::Talk(const CGUID& PlayerGuid, const char *pSourceName, const char *pTxt)
{	
	if (NULL == pSourceName || NULL == pTxt)
	{
		return;
	}
	CMessage msg(MSG_W2S_OTHER_FACTIONTALK);
	msg.Add(GetExID());
	msg.Add((long)TYPE_PLAYER);
	msg.Add(PlayerGuid);
	msg.Add((long)strlen(pSourceName) + 1);
	msg.Add(pSourceName);
	msg.Add((long)strlen(pTxt) + 1);
	msg.Add(pTxt);
	msg.SendToAllGS();
}

const CGUID&	CFaction::GetExID()
{ 
	return m_FacPublicData.guid; 
}
const CGUID&	CFaction::SetExID(const CGUID& guid) 
{ 
	return m_FacPublicData.guid = guid; 
}

const char* CFaction::GetName()const
{
	return m_FacPublicData.szName;
}
void CFaction::SetName(const char* pName)
{
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (MAXSIZE_FactionName - 1 >= lSize ) ? lSize : (MAXSIZE_FactionName - 1);
		memmove(m_FacPublicData.szName, pName, lSize);
	}
}

//! 申请加入
BOOL CFaction::ApplyForJoin(const CGUID& PlayerGuid)
{
	return FALSE;
}
//! 清除申请列表
BOOL CFaction::ClearApplyList(void)
{
	return FALSE;
}
//! 添加成员
BOOL CFaction::AddMeber(CPlayer *pPlayer, const CGUID &InviterGuid)
{
	if(NULL == pPlayer) return FALSE;
	AddNewMember(pPlayer);
	DelApplicant(pPlayer->GetExID());

	tagFacMemInfo &FacMemInfo = m_mapMemInfo[pPlayer->GetExID()];
	CMessage msg(MSG_W2S_Faction_AddMember_Notify);
	msg.AddEx(&FacMemInfo, sizeof(tagFacMemInfo));
	msg.Add(GetExID());
	msg.Add(InviterGuid);
	msg.SendToAllGS();
	m_dwDataChanged |= eFDCF_Member;

	CPlayer *pInviter = GetGame().GetMapPlayer(InviterGuid);
	const char *pInviterName = "";
	if(NULL != pInviter)
		pInviterName = pInviter->GetName();

	GetLogicLogInterface()->LogT170_faction_agree_kick_log(pInviterName, &InviterGuid, FacMemInfo.szName, &(FacMemInfo.MemberGuid), this, 0);

	return TRUE;
}
//! 退出
BOOL CFaction::Exit(const CGUID& PlayerGuid)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(PlayerGuid);
	if(m_mapMemInfo.end() != ite && PlayerGuid != m_FacBaseData.MasterGuid)
	{
		GetLogicLogInterface()->LogT170_faction_agree_kick_log("", &NULL_GUID, ite->second.szName, &(ite->second.MemberGuid), this, 1);

		m_mapMemInfo.erase(ite);
		CMessage msg(MSG_W2S_ORGSYS_Quit_Notify);
		msg.Add(GetExID());
		msg.Add(PlayerGuid);
		msg.SendToAllGS();
		m_dwDataChanged |= eFDCF_Member;

		CPlayer *pPlayer = GetGame().GetMapPlayer(PlayerGuid);
		if (NULL != pPlayer)
		{
			pPlayer->SetFactionID(NULL_GUID);
			pPlayer->SetUnionID(NULL_GUID);
		}

		//! 人数小于25%,开始解散计时
		const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
		if(NULL != pFacLevelSetup)
		{
			if(GetMemberNum() <= pFacLevelSetup->lDisbandMemNum && 0 != m_dwDisbandTime)
				m_dwDisbandTime = (long)time(NULL);
		}
		else
        {
			//PutStringToFile("Organize", "工作定时操作：没有找到家族对应等级的配置");
            Log4c::Warn(ROOT_MODULE,"工作定时操作：没有找到家族对应等级的配置");
        }

		return TRUE;
	}
	return FALSE;
}
//! 开除
BOOL CFaction::FireOut(const CGUID& FireID,const CGUID& BeFiredID)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(BeFiredID);
	if(m_mapMemInfo.end() != ite)
	{
		CPlayer *pPlayer = GetGame().GetMapPlayer(BeFiredID);
		if (NULL != pPlayer)
		{
			pPlayer->SetFactionID(NULL_GUID);
			pPlayer->SetUnionID(NULL_GUID);
		}

		CPlayer *pInviter = GetGame().GetMapPlayer(FireID);
		const char *pInviterName = "";
		if(NULL != pInviter)
			pInviterName = pInviter->GetName();

		GetLogicLogInterface()->LogT170_faction_agree_kick_log(pInviterName, &FireID, ite->second.szName, &(ite->second.MemberGuid), this, 1);

		m_mapMemInfo.erase(ite);

		CMessage msg(MSG_W2S_ORGSYS_Fire_Notify);
		msg.Add(GetExID());
		msg.Add(FireID);
		msg.Add(BeFiredID);
		msg.SendToAllGS();

		m_dwDataChanged |= eFDCF_Member;

		//! 人数小于25%,开始解散计时
		const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
		if(NULL != pFacLevelSetup)
		{
			if(GetMemberNum() <= pFacLevelSetup->lDisbandMemNum && 0 != m_dwDisbandTime)
				m_dwDisbandTime = (long)time(NULL);
		}
		else
        {
			//PutStringToFile("Organize", "工作定时操作：没有找到家族对应等级的配置");
            Log4c::Trace(ROOT_MODULE, "工作定时操作：没有找到家族对应等级的配置");
        }

		return TRUE;
	}

	return FALSE;
}
//! 传位
BOOL CFaction::Demise(const CGUID& MasterID,const CGUID& NewMasterID)
{
	if (MasterID != NewMasterID)
	{
		map<CGUID, tagFacMemInfo>::iterator iteOld = m_mapMemInfo.find(MasterID);
		map<CGUID, tagFacMemInfo>::iterator iteNew = m_mapMemInfo.find(NewMasterID);
		if(m_mapMemInfo.end() != iteOld && m_mapMemInfo.end() != iteNew)
		{
			iteNew->second.lJobLvl = Exceptive_JobLvl;
			iteOld->second.lJobLvl = 0;

			m_FacBaseData.MasterGuid = NewMasterID;
			strcpy(m_FacBaseData.szMasterName, iteNew->second.szName);

			CMessage msg(MSG_W2S_Faction_SetMaster_Notify);
			msg.Add(GetExID());
			msg.AddEx(&iteNew->second, sizeof(tagFacMemInfo));
			msg.AddEx(&iteOld->second, sizeof(tagFacMemInfo));
			msg.SendToAllGS();

			m_dwDataChanged |= (eFDCF_Member | eFDCF_BaseInfo);

			//GetLogicLogInterface()->LogT250_faction_master_log(this, &(iteOld->second), &(iteNew->second));

			return TRUE;

		}
	}
	return FALSE;
}	
//! 授予称号
BOOL CFaction::DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle)
{
	return FALSE;
}
//! 得到领导者ID
const CGUID& CFaction::GetMasterID(void)
{
	return m_FacBaseData.MasterGuid;
}
//! 得到领导者ID
const char* CFaction::GetMasterName(void)
{
	return m_FacBaseData.szMasterName;
}
//! 成员判定
const CGUID& CFaction::IsMember(const CGUID& guid)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(guid);
	if(m_mapMemInfo.end() != ite)
		return ite->first;
	return NULL_GUID;
}
//! 得到成员数
LONG CFaction::GetMemberNum(void)
{
	return (long)m_mapMemInfo.size();
}		

//! 解散
BOOL CFaction::Disband(const CGUID& guid)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.begin();
	for ( ; ite != m_mapMemInfo.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame().GetMapPlayer(ite->first);
		if (NULL != pPlayer)
		{
			pPlayer->SetFactionID(NULL_GUID);
			pPlayer->SetUnionID(NULL_GUID);
		}
	}

	CMessage msg(MSG_W2S_ORGSYS_DelFaction_Notify);
	msg.Add(GetExID());
	msg.SendToAllGS();

	return TRUE;
}

//! 得到等级
LONG CFaction::GetLvl(DWORD dwUpgradeType)
{
	switch(dwUpgradeType)
	{
	case eUT_FactionLevel:		//! 工会等级
		return m_FacPublicData.lLevel;
	case eUT_ExpLevel:			//! 经验福利等级
		return m_FacBaseData.lSubExpLevel;
	case eUT_SpecialityLevel:		//! 专业等级
		return m_FacBaseData.lSpecialityLevel;
	case eUT_BattleLevel:		//! 战争等级
		return m_FacBaseData.lBattleLevel;
	case eUT_BussinessLevel:	//! 商业等级
		return m_FacBaseData.lBussinessLevel;
	case eUT_NobleLevel:		//! 贵族等级
		return m_FacBaseData.lNoblelevel;
	}
	return 0;
}				
//! 设置等级
BOOL CFaction::SetLvl(LONG lLvl, DWORD dwUpgradeType)
{
	BOOL bRe = TRUE;
	switch(dwUpgradeType)
	{
	case eUT_FactionLevel:		//! 工会等级
		{
			if (m_FacPublicData.lLevel != lLvl)
			{			
				m_FacPublicData.lLevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	case eUT_ExpLevel:			//! 经验福利等级
		{
			if (m_FacBaseData.lSubExpLevel != lLvl)
			{	
				m_FacBaseData.lSubExpLevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	case eUT_SpecialityLevel:	//! 专业等级
		{
			if (m_FacBaseData.lSpecialityLevel != lLvl)
			{
				m_FacBaseData.lSpecialityLevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	case eUT_BattleLevel:		//! 战争等级
		{
			if (m_FacBaseData.lBattleLevel != lLvl)
			{
				m_FacBaseData.lBattleLevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	case eUT_BussinessLevel:	//! 商业等级
		{
			if (m_FacBaseData.lBussinessLevel != lLvl)
			{
				m_FacBaseData.lBussinessLevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	case eUT_NobleLevel:		//! 贵族等级
		{
			if (m_FacBaseData.lNoblelevel != lLvl)
			{
				m_FacBaseData.lNoblelevel = lLvl;
			}
			else
				return FALSE;
		}
		break;
	default:
		bRe = FALSE;
	}

	if(!bRe)
    {
		return bRe;
    }
    else
    {
        // 记录日志
        GetLogicLogInterface()->LogT270_faction_level_log(this, lLvl, dwUpgradeType);
    }

	CMessage msg(MSG_W2S_Faction_LvlChange_Notify);
	msg.Add(dwUpgradeType);
	msg.Add(PLAYER_OPT_SUCCEED);
	msg.Add(GetExID());
	msg.Add(lLvl);
	msg.SendToAllGS();

	m_dwDataChanged |= eFDCF_BaseInfo;

	return bRe;
}			
//! 得到国家ID
BYTE CFaction::GetCountry()
{
	return (BYTE)m_FacBaseData.lCountryID;
}			
//! 设置国家ID
void CFaction::SetCountry(BYTE c)
{

}
//! 得到上级组织ID
const CGUID& CFaction::GetSuperior(void)
{
	return m_FacBaseData.SuperiorGuid;
}

//! 设置上级组织ID
void  CFaction::SetSuperior(CUnion *pUnion)
{
	if(NULL != pUnion)
		m_FacBaseData.SuperiorGuid = pUnion->GetExID();
	else
		m_FacBaseData.SuperiorGuid = NULL_GUID;

	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.begin();
	for ( ; ite != m_mapMemInfo.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame().GetMapPlayer(ite->first);
		if (NULL != pPlayer)
		{
			pPlayer->SetUnionInfo(pUnion);
		}
	}

	m_dwDataChanged |= eFDCF_BaseInfo;
}	

//! 得到该建立时间
LONG CFaction::GetEstablishedTime(void) const
{
	return m_FacBaseData.CreateTime;
}	
//! 宣告
BOOL CFaction::Pronounce(CPlayer *pPlayer, const char *pPro)
{
	if (NULL != pPlayer)
	{
		if(0 != stricmp(m_Pronounce.szBody, pPro))
		{
			strcpy(m_Pronounce.szBody, pPro);
			strcpy(m_Pronounce.szName, pPlayer->GetName());
			m_Pronounce.Time = (long)time(NULL);
			m_Pronounce.MemberGuid = pPlayer->GetExID();

			CMessage msg(MSG_W2S_Faction_EditPronounce_Notify);
			msg.Add(GetExID()); //! 为了防止在成员被删除时正好编辑了公告，GS不应通过编辑者的ID来进行工会查找
			msg.AddEx(&m_Pronounce, sizeof(tagOrgPronounceWord));
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_BaseInfo;
			return TRUE;
		}
	}
	return FALSE;
}
//! 备忘
BOOL CFaction::Leaveword(CPlayer *pPlayer, const char *pPro)
{
	if (NULL != pPlayer)
	{
		if(0 != stricmp(m_LeaveWord.szBody, pPro))
		{
			strcpy(m_LeaveWord.szBody, pPro);
			strcpy(m_LeaveWord.szName, pPlayer->GetName());
			m_LeaveWord.Time = (long)time(NULL);
			m_LeaveWord.MemberGuid = pPlayer->GetExID();

			CMessage msg(MSG_W2S_Faction_EditLeaveword_Notify);
			msg.Add(GetExID()); //! 为了防止在成员被删除时正好编辑了公告，GS不应通过编辑者的ID来进行工会查找
			msg.AddEx(&m_LeaveWord, sizeof(tagOrgPronounceWord));
			msg.SendToAllGS();

			m_dwDataChanged |= eFDCF_BaseInfo;
			return TRUE;
		}
	}
	return FALSE;
}

//! 设置会徽
BOOL CFaction::SetIcon(BYTE* IconData, long size)
{
	if(MAX_ICON_SIZE > size && NULL != IconData)
	{
		m_vIconData.clear();
		m_vIconData.resize(size);
		if(0 != m_vIconData.size())
			memmove(&m_vIconData[0], IconData, size);
		m_dwDataChanged |= eFDCF_BaseInfo;
		return TRUE;
	}
	return FALSE;
}

//! 得到会徽
void CFaction::GetIcon(vector<BYTE> &vIconData)
{
	vIconData.clear();
	vIconData.resize(m_vIconData.size());
	if(0 != m_vIconData.size())
		memmove(&vIconData[0], &m_vIconData[0], m_vIconData.size());
}

//!	发送对象到GS
void CFaction::SendToGameServer(LONG lSocketID)
{

}

//出发一个成员进入游戏
void CFaction::OnMemberEnterGame(CPlayer *pPlayer)
{
	const CGUID& MemGuid = pPlayer->GetExID();
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemGuid);
	if(m_mapMemInfo.end() != ite)
	{
		ite->second.bIsOnline = TRUE;
		CPlayer *pPlayer = GetGame().GetMapPlayer(ite->second.MemberGuid);
		assert(NULL != pPlayer);
		if(NULL == pPlayer)return;

		//! 等级
		ite->second.lLvl = pPlayer->GetLevel();
		ite->second.lOccu = pPlayer->GetOccupation();
		strcpy(ite->second.szName, pPlayer->GetName()); 
		ite->second.cHide = pPlayer->GetHideFlag();

		//! 所在地图
		map<LONG, CGlobalRgnManager::tagRegion> &mapRegionList = *(GetGame().GetGlobalRgnManager()->GetTemplateRegionListMap());
		map<LONG, CGlobalRgnManager::tagRegion>::iterator iteRegion = mapRegionList.find(pPlayer->GetRegionID());
		if (iteRegion != mapRegionList.end())
		{
			CWorldRegion *pWorldRegion = iteRegion->second.pRegion;
			if (NULL != pWorldRegion)
			{
				const char *pRegionName = pWorldRegion->GetName();
				LONG lNameSize = (long)strlen(pRegionName);
				lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
				memmove(ite->second.szRegion, pRegionName, lNameSize);		//! 所在地图
			}
			else
			{
				Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_FACTION_40", pPlayer->GetRegionID()));
			}
		}
		else
		{
			CGlobalRgnManager::tagRegion* pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalRegion(pPlayer->GetRegionID());
			if(!pTagRgn)
				pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(pPlayer->GetRegionID());
			if(!pTagRgn)
				pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplateTeamRegion(pPlayer->GetRegionID());
			if(!pTagRgn)
				Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_FACTION_41", pPlayer->GetRegionID()));
		}

		DWORD dwCurrTime = (long)time(NULL);
		ite->second.LastOnlineTime = dwCurrTime;
		m_dwDataChanged |= eFDCF_Member;

		//! 3天不上线的族长予以提示
		BOOL bDemiseWarning = FALSE;
		map<CGUID, tagFacMemInfo>::iterator iteMaster = m_mapMemInfo.find(GetMasterID());
		if(m_mapMemInfo.end() != iteMaster)
		{
			pPlayer->SetFacDemiseWarning(iteMaster->second.LastOnlineTime);
		}
		else
			assert(false);

		CMessage msg(MSG_W2S_Faction_OnlineChange_Notify);
		msg.Add(GetExID());
		msg.Add(MemGuid);
		msg.Add(pPlayer->GetLevel());
		msg.Add((WORD)pPlayer->GetOccupation());
		msg.Add(pPlayer->GetName());
		msg.Add((LONG)TRUE);
		msg.Add(ite->second.szRegion);
		msg.SendToAllGS();

		pPlayer->SetFacDisbandWarning(m_dwDisbandTime);
	}
}

//触发一个成员离开游戏
void CFaction::OnMemberExitGame(CPlayer *pPlayer)
{
	if(NULL == pPlayer) 
		return;
	const CGUID& MemGuid = pPlayer->GetExID();
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(MemGuid); 
	if(m_mapMemInfo.end() != ite)
	{
		ite->second.bIsOnline = FALSE;
		ite->second.LastOnlineTime = (long)time(NULL);
		ite->second.lLvl = pPlayer->GetLevel();
		m_dwDataChanged |= eFDCF_Member;

		CMessage msg(MSG_W2S_Faction_OnlineChange_Notify);
		msg.Add(GetExID());
		msg.Add(MemGuid);
		msg.Add(pPlayer->GetLevel());
		msg.Add((WORD)pPlayer->GetOccupation());
		msg.Add(pPlayer->GetName());
		msg.Add((LONG)FALSE);
		msg.Add(ite->second.LastOnlineTime);
		msg.SendToAllGS();
	}
}

//! 执行工会验证操作
DWORD CFaction::OnValidate(DWORD dwTime)
{
	//! 7天不上线则转让会长
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.find(GetMasterID());
	if(m_mapMemInfo.end() != ite)
	{
		long dwCurrTime = (long)time(NULL);
		if(0 == ite->second.LastOnlineTime)
			ite->second.LastOnlineTime = dwCurrTime;
		LONG lDemiseTime=WSOrganizingParam::getInstance().GetGlobe().lDemiseTime * 24 * 60 * 60;
		if(dwCurrTime - ite->second.LastOnlineTime > lDemiseTime)
		{
			if(m_mapMemInfo.size() > 1)
			{
				LONG lMaxContribute = 0, lMaxLevel = 0, lLatestOnlineTime = ite->second.LastOnlineTime;
				map<CGUID, tagFacMemInfo>::iterator iteFind = m_mapMemInfo.end();
				for (map<CGUID, tagFacMemInfo>::iterator iteMem = m_mapMemInfo.begin(); iteMem != m_mapMemInfo.end(); ++iteMem)
				{
					if(iteMem->first != GetMasterID() && (lDemiseTime > 
						dwCurrTime - iteMem->second.LastOnlineTime) )
					{
						BOOL bSwap = FALSE;

						if(iteMem->second.lContribute > lMaxContribute)
							bSwap = TRUE;
						else if(iteMem->second.lContribute == lMaxContribute && (iteMem->second.lLvl > lMaxLevel || iteMem->second.LastOnlineTime < lLatestOnlineTime))
							bSwap = TRUE;

						if(bSwap)
						{
							iteFind			= iteMem;
							lMaxContribute	= iteMem->second.lContribute;
							lMaxLevel		= iteMem->second.lLvl;
						}
					}
				}
				if(m_mapMemInfo.end() != iteFind)
				{
					Demise(GetMasterID(), iteFind->first);
// 					PutStringToFile("Organize", "工作定时操作：对7一个天不上线的族长进行了传位。");
// 					PutStringToFile("Organize", GetName());
                    Log4c::Trace(ROOT_MODULE,"工作定时操作：对7一个天不上线的族长进行了传位。");
                    Log4c::Trace(ROOT_MODULE,GetName());
				}
				else
				{
// 					PutStringToFile("Organize", "工作定时操作：对7一个天不上线的族长进行传位时，已经找不到更合适的传位者了。");
// 					PutStringToFile("Organize", GetName());
                    Log4c::Trace(ROOT_MODULE, "工作定时操作：对7一个天不上线的族长进行传位时，已经找不到更合适的传位者了。");
                    Log4c::Trace(ROOT_MODULE,GetName());
				}
			}
		}
	}
	else
	{
		assert(false);
		//没有成员的公会,直接解散
		return ImmediatelyDisband;
	}

	//检查成员最后上线时间
	CheckLastTime();
	
	//! 连续3天成员人数小于最大人数的25%，予以解散
	//! 人数大于25%，取消解散计时
	{
		long dwCurrTime = (long)time(NULL);
		//! 每天更新解散时间，防止出错
		const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
		float fMemberPercent = 0.0f;
		if(NULL == pFacLevelSetup)
		{
			Log4c::Trace(ROOT_MODULE,"工作定时操作：没有找到家族对应等级的配置");
			return Nothing;
		}
		else if(pFacLevelSetup->lDisbandMemNum >= GetMemberNum())
		{
			if(0 == m_dwDisbandTime)
				m_dwDisbandTime = dwCurrTime;
			else if(dwCurrTime - (long)m_dwDisbandTime  >=	WSOrganizingParam::getInstance().GetGlobe().lDisbandFactionTime * 24 * 60 * 60)
			{
				Log4c::Trace(ROOT_MODULE, "工作定时操作：对一个连续3天成员人数小于最大人数的百分之二十五的家族予以解散。");
				Log4c::Trace(ROOT_MODULE, GetName());
				return ImmediatelyDisband;
			}
		}
		else
			m_dwDisbandTime = 0;
	}
	//清空家族占领的据点
	tm *pCurrData = localtime((time_t*)&dwTime);
	if (pCurrData->tm_wday==WSOrganizingParam::getInstance().GetGlobe().byClearBaseTime-1)
	{
		ClearBase();
	}
	//! 家族日常消耗
	return DailyConsume();
}

BOOL CFaction::CheckLastTime()
{
	//30天
	long dwTime= 30 * 24 * 60 * 60;
	long dwCurrTime=(long)time(NULL);
	map<CGUID, tagFacMemInfo>::iterator it=m_mapMemInfo.begin();
	for (;it!=m_mapMemInfo.end();)
	{
		tagFacMemInfo& tgInfo=it->second;
		if (dwCurrTime-tgInfo.LastOnlineTime>dwTime)
		{
			GetLogicLogInterface()->LogT170_faction_agree_kick_log("", &NULL_GUID, tgInfo.szName, &(tgInfo.MemberGuid), this, 1);
			
			CMessage msg(MSG_W2S_ORGSYS_Quit_Notify);
			msg.Add(GetExID());
			msg.Add(tgInfo.MemberGuid);
			msg.SendToAllGS();
			m_dwDataChanged |= eFDCF_Member;
			CPlayer *pPlayer = GetGame().GetMapPlayer(tgInfo.MemberGuid);
			if (NULL != pPlayer)
			{
				pPlayer->SetFactionID(NULL_GUID);
				pPlayer->SetUnionID(NULL_GUID);
			}
			m_mapMemInfo.erase(it++);
			//! 人数小于25%,开始解散计时
			const tagFacLevelSetup *pFacLevelSetup = WSOrganizingParam::getInstance().GetLevelSetup(GetLvl(eUT_FactionLevel));
			if(NULL != pFacLevelSetup)
			{
				if(GetMemberNum() <= pFacLevelSetup->lDisbandMemNum && 0 != m_dwDisbandTime)
					m_dwDisbandTime = (ulong)time(NULL);
			}
			else
            {
				Log4c::Warn(ROOT_MODULE, "工作定时操作：没有找到家族对应等级的配置");
            }
		}
		else
			++it;
	}
	return TRUE;
}

void CFaction::ClearBase() 
{
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.begin();
	for (;it!=m_mapBaseInfo.begin();it++)
	{
		it->second._FacGuid=NULL_GUID;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//检查某ID执行某个操作的合法性
BOOL CFaction::CheckOperValidate(const CGUID& ExecuteGuid,eMemberPurview eMP)
{
	return FALSE;
}

//! 重设成员数据
VOID CFaction::ResetMember(VOID)
{
	map<CGUID, tagFacMemInfo>::iterator ite = m_mapMemInfo.begin();
	for ( ; ite != m_mapMemInfo.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame().GetMapPlayer(ite->first);
		if (NULL != pPlayer)
		{
			pPlayer->SetUnionID(m_FacBaseData.SuperiorGuid);
			pPlayer->SetFactionID(GetExID());
			pPlayer->SetFactionInfo(this);

			ite->second.bIsOnline	= TRUE;
			ite->second.lLvl		= pPlayer->GetLevel();
			ite->second.lOccu		= pPlayer->GetOccupation();
			ite->second.cHide		= pPlayer->GetHideFlag();
			strcpy(ite->second.szName, pPlayer->GetName());

			//! 所在地图
			map<LONG, CGlobalRgnManager::tagRegion> &mapRegionList = *(GetGame().GetGlobalRgnManager()->GetTemplateRegionListMap());
			map<LONG, CGlobalRgnManager::tagRegion>::iterator iteRegion = mapRegionList.find(pPlayer->GetRegionID());
			if (iteRegion != mapRegionList.end())
			{
				CWorldRegion *pWorldRegion = iteRegion->second.pRegion;
				if (NULL != pWorldRegion)
				{
					const char *pRegionName = pWorldRegion->GetName();
					long lNameSize = (long)strlen(pRegionName);
					lNameSize = (lNameSize < MAX_MEMBER_NAME_SIZE) ? lNameSize : MAX_MEMBER_NAME_SIZE - 1;
					memmove(ite->second.szRegion, pRegionName, lNameSize);		//! 所在地图
				}
				else
				{
					Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_FACTION_40", pPlayer->GetRegionID()));
				}
			}
			else
			{
				CGlobalRgnManager::tagRegion* pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalRegion(pPlayer->GetRegionID());
				if(!pTagRgn)
					pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(pPlayer->GetRegionID());
				if(!pTagRgn)
					pTagRgn = GetGame().GetGlobalRgnManager()->GetTemplateTeamRegion(pPlayer->GetRegionID());
				if(!pTagRgn)
					Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_FACTION_41", pPlayer->GetRegionID()));
			}
		}
	}
}

DWORD CFaction::DailyConsume()
{
	LONG dwCurrRes=m_FacBaseData.lCurrRes;
	//需要扣除的资源
	const tagBaseConsume* pC=WSOrganizingParam::getInstance().GetConsume(m_FacPublicData.lLevel);
	if(pC==NULL)
		return Nothing;
	float fBussBase=WSOrganizingParam::getInstance().GetBussBase(m_FacBaseData.lBussinessLevel);
	long lConsume= (long) ( pC->_BaseVal* ( 1.0 + fBussBase/100 ) );
	long dwCurrTime = (long)time(NULL);
	//WS重启后,判断在24小时之内不能减少了第二次
	if(dwCurrTime-m_dwSubResTime > 1*24*60*60)
	{
		if (m_FacBaseData.lCurrRes-lConsume < pC->_Critical)
		{
			m_FacBaseData.lCurrRes=pC->_Critical;
		}
		else
		{
			m_FacBaseData.lCurrRes-=lConsume;
		}
		m_dwSubResTime=dwCurrTime;
	}
	CMessage msg(MSG_W2S_Faction_ChangeRes_Notify);
	msg.Add(GetExID());
	msg.Add((long)0);
	msg.Add(m_FacBaseData.lCurrRes);
	msg.SendToAllGS();
	//记录资源为负的时间
	if (m_FacBaseData.lCurrRes<0)
	{
		if(m_dwNegativeTime==0)
			m_dwNegativeTime= (ulong)time(NULL);
		else 
		{
			tagGlobalSetup tgsetup=WSOrganizingParam::getInstance().GetGlobe();
			if (dwCurrTime-(long)m_dwNegativeTime> tgsetup.lNegativeTime * 24 * 60 * 60)
			{
				m_FacPublicData.lLevel-=1;
				if (m_FacPublicData.lLevel==0)
				{
					return ImmediatelyDisband;
				}
				ChangeTechLv();
			}
		}
	}
	return Nothing;
}

LONG CFaction::AddTickets(char* cBaseName,LONG lNum)
{
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.find(cBaseName);
	if (it!=m_mapBaseInfo.end())
	{
		if (it->second._lTickets+lNum>=0)
		{
			it->second._lTickets+=lNum;
			return it->second._lTickets;
		}
		else
			return -1;
	}
	//第一次添加信息
	else
	{
		//只有lNum>0才处理
		if (lNum>0)
		{
			tagBaseInfo tgInfo;
			tgInfo._lTickets=lNum;
			tgInfo._FacGuid=NULL_GUID;
			strcpy(tgInfo._cBaseName,cBaseName);
			m_mapBaseInfo[cBaseName]=tgInfo;
			return lNum;
		}
		else
			return -1;	
	}
	return -1;
}

BOOL CFaction::ChangeBaseInfo(char* cName)
{
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.find(cName);
	if (it!=m_mapBaseInfo.end())
	{
		it->second._FacGuid=m_FacPublicData.guid;
		return TRUE;
	}
	else
		return FALSE;
}

LONG CFaction::GetBaseNum()
{
	LONG lBaseNum=0;
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.begin();
	for(;it!=m_mapBaseInfo.end();it++)
	{
		if(it->second._FacGuid==m_FacPublicData.guid)
			lBaseNum+=1;
	}
	return lBaseNum;
}

LONG CFaction::AddBaseBattle(char* cBaseName,LONG lNum)
{
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.find(cBaseName);
	if (it!=m_mapBaseInfo.end())
	{
		if (it->second._lBattleVal+lNum>=0)
		{
			it->second._lBattleVal+=lNum;
			return it->second._lBattleVal;
		}
		else
			return -1;
	}
	//第一次添加信息
	else
	{
		//只有lNum>0才处理
		if (lNum>0)
		{
			tagBaseInfo tgInfo;
			tgInfo._lBattleVal=lNum;
			tgInfo._FacGuid=NULL_GUID;
			strcpy(tgInfo._cBaseName,cBaseName);
			m_mapBaseInfo[cBaseName]=tgInfo;
			return lNum;
		}
		else
			return -1;	
	}
	return -1;
}

LONG CFaction::AddBattle(long lValue,CGUID& PlayerGuid)
{
	if (PlayerGuid==NULL_GUID)
	{
		if (m_FacBaseData.lFacBattle+lValue<0)
		{
			return -1;
		}
		m_FacBaseData.lFacBattle+=lValue;
		return m_FacBaseData.lFacBattle;
	}
	else
	{
		map<CGUID, tagFacMemInfo>::iterator it=m_mapMemInfo.find(PlayerGuid);
		if (it!=m_mapMemInfo.end())
		{
			tagFacMemInfo& tgInfo=it->second;
			if(tgInfo.lBattle+lValue<0)
				return -1;
			tgInfo.lBattle+=lValue;
			return tgInfo.lBattle;
		}
	}
	return -1;
}

LONG CFaction::AddArmyInvest(long lValue,CGUID& PlayerGuid)
{
	if (PlayerGuid==NULL_GUID)
	{
		if (m_FacBaseData.lFacArmyInvest+lValue<0)
		{
			return -1;
		}
		m_FacBaseData.lFacArmyInvest+=lValue;
		return m_FacBaseData.lFacArmyInvest;
	}
	else
	{
		map<CGUID, tagFacMemInfo>::iterator it=m_mapMemInfo.find(PlayerGuid);
		if (it!=m_mapMemInfo.end())
		{
			tagFacMemInfo& tgInfo=it->second;
			if(tgInfo.lArmyInvest+lValue<0)
				return -1;
			tgInfo.lArmyInvest+=lValue;
			return tgInfo.lArmyInvest;
		}
	}
	return -1;
}

BYTE CFaction::SetJoin(CGUID& PlayerGuid,LONG lFlags)
{
	map<CGUID, tagFacMemInfo>::iterator it=m_mapMemInfo.find(PlayerGuid);
	if (it!=m_mapMemInfo.end())
	{
        it->second.bJoinBattle= lFlags > 0 ? true : false ;
		return 1;
	}
	return 0;
}

BOOL CFaction::ChangeTechLv()
{
	const tagTechInfo* pInfo=WSOrganizingParam::getInstance().GetTechInfo(m_FacPublicData.lLevel);
	if (pInfo)
		return FALSE;
	//调整到家族等级段的最高等级
	m_FacBaseData.lSubExpLevel=min(pInfo->_lSubExpLv,m_FacBaseData.lSubExpLevel);
	m_FacBaseData.lSpecialityLevel=min(pInfo->_lSpecialityLv,m_FacBaseData.lSpecialityLevel);
	m_FacBaseData.lBattleLevel=min(pInfo->_lBattleLv,m_FacBaseData.lBattleLevel);
	m_FacBaseData.lBussinessLevel=min(pInfo->_lBussinessLv,m_FacBaseData.lBussinessLevel);
	m_FacBaseData.lNoblelevel=min(pInfo->_lNoblelv,m_FacBaseData.lNoblelevel);
	return TRUE;
}

BOOL CFaction::AddTechLv(LONG lTechNo,LONG Lv)
{	
	m_TechLv[lTechNo]=Lv;	
	return TRUE;
}

void CFaction::AddTechLvToByteArray(vector<BYTE>* pByte)
{
	_AddToByteArray(pByte,(long)m_TechLv.size());
	map<LONG,LONG>::iterator it=m_TechLv.begin();
	for (;it!=m_TechLv.end();it++)
	{
		_AddToByteArray(pByte,it->first);
		_AddToByteArray(pByte,it->second);
	}
}

void CFaction::DecodeTechLvFromByteArray(BYTE* pBYTE,long& pos)
{
	m_TechLv.clear();
	LONG lSize=_GetLongFromByteArray(pBYTE,pos);
	LONG lTechNo,Lv;
	for (LONG i=0;i<lSize;i++)
	{
		lTechNo=_GetLongFromByteArray(pBYTE,pos);
		Lv=_GetLongFromByteArray(pBYTE,pos);
		m_TechLv[lTechNo]=Lv;
	}
}

void CFaction::AddBaseInfoToByteArray(vector<BYTE>*pByte)
{
	_AddToByteArray(pByte,(long)m_mapBaseInfo.size());
	map<string,tagBaseInfo>::iterator it=m_mapBaseInfo.begin();
	for (;it!=m_mapBaseInfo.end();it++)
	{
		_AddToByteArray(pByte,&(it->second),sizeof(tagBaseInfo));
	}
}

void CFaction::DecodeBaseInfoFromByteArray(BYTE* pByte,long& pos)
{
	m_mapBaseInfo.clear();
	LONG lSize=_GetLongFromByteArray(pByte,pos);
	tagBaseInfo tgInfo;
	for (LONG i=0;i<lSize;i++)
	{
		_GetBufferFromByteArray(pByte,pos,&tgInfo,sizeof(tgInfo));
		m_mapBaseInfo[tgInfo._cBaseName]=tgInfo;
	}
}

BOOL CFaction::ChangeEnemy(eBattleType eType,char* cName,DWORD dwTime,LONG lFlags/* =0 */)
{
	map<eBattleType,list<tagEnemy>>::iterator it=m_Enemy.find(eType);
	//删除指定家族名的信息
	if (lFlags==0)
	{
		if (it==m_Enemy.end())
			return FALSE;
		list<tagEnemy>& Enemy=it->second;
		list<tagEnemy>::iterator itItr=Enemy.begin();
		for (;itItr!=Enemy.end();itItr++)
		{
			if (stricmp(cName,(*itItr)._cName)==0)
			{
				Enemy.erase(itItr);
				return TRUE;
			}
		}
		return FALSE;
	}
	//添加信息
	tagEnemy tgEnemy;
	strcpy(tgEnemy._cName,cName);
	tgEnemy._dwStartTime=dwTime;
	tgEnemy._dwEndTime=tgEnemy._dwStartTime +
		60 * 60 * WSOrganizingParam::getInstance().GetGlobe().lBaseBattleLast;	
	if (it!=m_Enemy.end())
	{
		list<tagEnemy>& Enemy=it->second;
		Enemy.push_back(tgEnemy);
	}
	else
	{
		list<tagEnemy>Enemy;
		Enemy.push_back(tgEnemy);
		m_Enemy[eType]=Enemy;
	}
	return TRUE;
}
