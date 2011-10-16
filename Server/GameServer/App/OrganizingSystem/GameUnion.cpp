


#include "StdAfx.h"
#include "GameUnion.h"
#include "GameOrganizingCtrl.h"
#include "../Player.h"


GameUnion::GameUnion(void)
{

}
//! 
GameUnion::~GameUnion(void)
{

}

//! 编码
BOOL GameUnion::AddToByteArray(vector<BYTE>& ByteArray)
{
	//! 基本资料
	_AddToByteArray(&ByteArray, &m_UnionBaseData, sizeof(tagUnionBaseData));

	//! 成员列表
	_AddToByteArray(&ByteArray, (LONG)m_mapUnionMemInfo.size());
	for (map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.begin(); ite != m_mapUnionMemInfo.end(); ++ite)
	{
		_AddToByteArray(&ByteArray, &(ite->second), sizeof(tagUnionMemInfo));
	}
	;
	return TRUE;
}
//! 解码
BOOL GameUnion::Decode(BYTE* pData, LONG &lPos)
{
	_GetBufferFromByteArray(pData, lPos, &m_UnionBaseData, sizeof(tagUnionBaseData));

	LONG lMemberNum = _GetLongFromByteArray(pData, lPos);
	m_mapUnionMemInfo.clear();
	tagUnionMemInfo UnionMemInfo;
	for (LONG i = 0; i < lMemberNum; ++i)
	{
		_GetBufferFromByteArray(pData, lPos, &UnionMemInfo, sizeof(tagUnionMemInfo));
		m_mapUnionMemInfo[UnionMemInfo.MemberGuid] = UnionMemInfo;
	}
	return TRUE;
}

//! 编码基本信息到消息末尾
void GameUnion::AddBaseToMsg(CMessage *pMsg)
{
	if(NULL != pMsg)
		pMsg->AddEx(&m_UnionBaseData, sizeof(tagUnionBaseData));
}

//! 编码成员信息到消息末尾
BOOL GameUnion::AddOneMemberToMsg(const CGUID &FactionGuid, CMessage *pMsg)
{
	if(NULL != pMsg)
	{
		map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.find(FactionGuid);
		if(m_mapUnionMemInfo.end() != iteMember)
		{
			pMsg->Add(GetExID());
			pMsg->AddEx(&(iteMember->second), sizeof(tagUnionMemInfo));
			return TRUE;
		}
	}

	return FALSE;
}
BOOL GameUnion::AddOneMemberToMsg(CPlayer *pPlayer, CMessage *pMsg)
{
	if(NULL != pMsg && NULL != pPlayer)
	{
		map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.find(pPlayer->GetFactionID());
		if(m_mapUnionMemInfo.end() != iteMember)
		{
			pMsg->Add(GetExID());
			pMsg->AddEx(&(iteMember->second), sizeof(tagUnionMemInfo));
			return TRUE;
		}
	}

	return FALSE;
}

//! 编码成员信息到消息末尾
inline void GameUnion::AddAllMemberToMsg(CMessage *pMsg)
{
	if(NULL != pMsg)
	{
		pMsg->Add(GetExID());
		pMsg->Add((LONG)m_mapUnionMemInfo.size());

		map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.begin();
		for (; iteMember != m_mapUnionMemInfo.end(); ++iteMember)
		{
			pMsg->AddEx(&(iteMember->second), sizeof(tagUnionMemInfo));
		}
	}
}

//! 得到组织ID
const CGUID&	GameUnion::GetExID()const
{ 
	return m_UnionBaseData.guid; 
}
//! 设置组织ID
const CGUID&	GameUnion::SetExID(const CGUID& guid) 
{ 
	return m_UnionBaseData.guid = guid; 
}
//! 得到组织的名字
const char* GameUnion::GetName()const
{
	return m_UnionBaseData.szName;
}
//! 设置帮会的名字
void GameUnion::SetName(const char* pName)
{
	string str("");
	str.c_str();
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (MAXSIZE_FactionName - 1 >= lSize ) ? lSize : (MAXSIZE_FactionName - 1);
		memmove(m_UnionBaseData.szName, pName, lSize);
	}
}

//! 得到盟主ID
const CGUID&	GameUnion::GetMasterID()const
{ 
	return m_UnionBaseData.MasterGuid; 
}
//! 设置盟主ID
const CGUID&	GameUnion::SetMasterID(const CGUID& guid) 
{ 
	return m_UnionBaseData.MasterGuid = guid; 
}
//! 得到盟主的名字
const char* GameUnion::GetMasterName()const
{
	return m_UnionBaseData.szMasterName;
}
//! 设置盟主的名字
void GameUnion::SetMasterName(const char* pName)
{
	string str("");
	str.c_str();
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (MAX_MEMBER_NAME_SIZE - 1 >= lSize ) ? lSize : (MAX_MEMBER_NAME_SIZE - 1);
		memmove(m_UnionBaseData.szMasterName, pName, lSize);
	}
}

//! 删除成员
BOOL GameUnion::DelMember(const CGUID& MemberGuid)
{
	map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.find(MemberGuid);
	if(m_mapUnionMemInfo.end() == ite)
		return FALSE;
	m_mapUnionMemInfo.erase(ite);

	CMessage msgNotify(MSG_S2C_UNION_Notify_Quit);
	msgNotify.Add(MemberGuid);
	for (ite = m_mapUnionMemInfo.begin(); ite != m_mapUnionMemInfo.end(); ++ite)
	{
		GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(ite->first);
		if(NULL != pFaction)
			pFaction->SendToAllConcern(msgNotify);
	}
	return TRUE;
}

//! 判断成员
BOOL GameUnion::IsMember(const CGUID& FactionGuid)
{
	return (m_mapUnionMemInfo.end() != m_mapUnionMemInfo.find(FactionGuid));
}

//! 修改盟主
BOOL GameUnion::ChangeMaster(const CGUID& NewMasterFacGuid, const char *pNewMasterName)
{
	assert(NULL != pNewMasterName);
	map<CGUID, tagUnionMemInfo>::iterator iteOldMaster = m_mapUnionMemInfo.find(m_UnionBaseData.MasterFacGuid);
	map<CGUID, tagUnionMemInfo>::iterator iteNewMaster = m_mapUnionMemInfo.find(NewMasterFacGuid);
	assert(m_mapUnionMemInfo.end() != iteOldMaster);
	if(m_mapUnionMemInfo.end() == iteNewMaster || m_mapUnionMemInfo.end() == iteOldMaster)
		return FALSE;
	GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(NewMasterFacGuid);
	if(NULL == pFaction)
		return FALSE;

	iteOldMaster->second.lPurview = eUMP_Null;
	iteNewMaster->second.lPurview = eUMP_All;

	CGUID OldMasterGuid = GetMasterID();
	SetMasterID(pFaction->GetMaster());
	SetMasterName(pNewMasterName);
	m_UnionBaseData.MasterFacGuid = NewMasterFacGuid;

	CMessage msg(MSG_S2C_UNION_Notify_SetChairman);
	msg.Add(iteOldMaster->second.MemberGuid);
	msg.Add(pFaction->GetExID());
	msg.Add(pFaction->GetMaster());
	msg.Add(pNewMasterName);

	msg.SendToPlayer(pFaction->GetMaster());
	msg.SendToPlayer(OldMasterGuid, false);

	return TRUE;
}

//! 设置成员权限
BOOL GameUnion::SetPurview(const CGUID& MemberGuid, DWORD dwNewPurview)
{
	map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.find(MemberGuid);
	if(m_mapUnionMemInfo.end() == ite)
		return FALSE;
	ite->second.lPurview = dwNewPurview;

	CMessage msgRe(MSG_S2C_UNION_Re_SetRights);
	msgRe.Add(PLAYER_OPT_SUCCEED);
	msgRe.Add(MemberGuid);
	msgRe.Add(dwNewPurview);
	msgRe.SendToPlayer(GetMasterID());

	GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(MemberGuid);
	if(NULL != pFaction)
	{
		CMessage msgNotify(MSG_S2C_UNION_Notify_GetRights);
		msgNotify.Add(dwNewPurview);
		msgNotify.SendToPlayer(pFaction->GetMaster());
	}

	return TRUE;
}

//! 得到成员权限
DWORD GameUnion::GetPurview(const CGUID& MemberGuid)
{
	map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.find(MemberGuid);
	if(m_mapUnionMemInfo.end() == ite)
		return eUMP_Null;
	return ite->second.lPurview;
}

//! 解散
BOOL GameUnion::Disband(void)
{
	map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.begin();
	for (; ite != m_mapUnionMemInfo.end(); ++ite)
	{
		GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(ite->second.MemberGuid);
		if(NULL != pFaction)
		{
			pFaction->OnDisbandUnion();
		}
	}
	return TRUE;
}


//! 发送创建通知
void GameUnion::OnCreate(void)
{
	CMessage msgRe(MSG_S2C_UNION_Re_Creation);
	msgRe.Add(PLAYER_OPT_SUCCEED);
	msgRe.SendToPlayer(GetMasterID(), false);

	map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.begin();
	for (; ite != m_mapUnionMemInfo.end(); ++ite)
	{
		GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(ite->second.MemberGuid);
		if(NULL != pFaction)
		{
			pFaction->OnJoinUnion(this);
		}
	}
	
}