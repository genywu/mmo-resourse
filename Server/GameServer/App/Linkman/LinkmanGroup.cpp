

#include "StdAfx.h"
#include "LinkmanGroup.h"
#include "../Player.h"


LinkmanGroup::LinkmanGroup(const LinkmanGroupSetup* pSetup, const CGUID &OwnerGuid)
:
m_pSetup(pSetup),
m_OwnerGuid(OwnerGuid)
{
	assert(NULL != pSetup);
};

LinkmanGroup::~LinkmanGroup(void)
{
};

//! 添加一个联系人
void LinkmanGroup::AddMember(tagLinkman &Linkman, BOOL bSend2Client)
{
	assert(!FindMember(Linkman.guid));
	assert(NULL_GUID != Linkman.guid);

	if (m_mapAddMe.find(Linkman.guid) != m_mapAddMe.end())
	{
		Linkman.bMutual = TRUE;
	}

	m_mapMeAdd[Linkman.guid] = Linkman;

	//! 通知玩家
	if(bSend2Client)
	{
		CMessage msgClient(MSG_S2C_LINKMAN_ADD);
		msgClient.Add((LONG)TRUE);
		msgClient.Add(m_pSetup->m_uID);
		msgClient.AddEx(&Linkman, sizeof(tagLinkman));
		msgClient.SendToPlayer(m_OwnerGuid);
	}

	
}

//! 删除一个联系人
void LinkmanGroup::DelMember(const CGUID &PlayerGuid, BOOL bSend2Client, const char *pAimName)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if(m_mapMeAdd.end() == ite) return;
	m_mapMeAdd.erase(PlayerGuid);

	//! 通知玩家
	if(bSend2Client)
	{
		CMessage msgClient(MSG_S2C_LINKMAN_DEL);
		msgClient.Add((LONG)TRUE);
		msgClient.Add(m_pSetup->m_uID);
		msgClient.Add(pAimName);
		msgClient.SendToPlayer(m_OwnerGuid);
	}
}

//! 找到一个联系人
BOOL LinkmanGroup::FindMember(const CGUID &PlayerGuid)
{
	return (m_mapMeAdd.find(PlayerGuid) != m_mapMeAdd.end());
}

//! 找到一个联系人的GUID
CGUID LinkmanGroup::FindMemberID(const char *pPlayerName)
{
	for(map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.begin(); ite != m_mapMeAdd.end(); ++ite)
	{
		if (0 == stricmp(ite->second.szName, pPlayerName))
		{
			return ite->second.guid;
		}
	}
	return NULL_GUID;
}

//! 得到联系人数量
LONG LinkmanGroup::GetMemberNum(void)
{
	return (long)m_mapMeAdd.size();
}

//! 得到第一个联系人的GUID
const tagLinkman* LinkmanGroup::GetFirstMember(void)
{
	if(m_mapMeAdd.begin() != m_mapMeAdd.end())
		return &(m_mapMeAdd.begin()->second);
	return NULL;
}

//! 添加一个添加我为联系人的人
void LinkmanGroup::AddCompriseMePlayer(tagLinkman_DB &Linkman, const char *pOptName)
{
	m_mapAddMe[Linkman.OwnerGUID] = Linkman;
	
	//! 通知被添加玩家
	if (m_pSetup->m_NotifySetup.bMessage_AddAim)
	{
		CMessage msgNotify(MSG_S2C_LINKMAN_OTHER_HANDLE);
		msgNotify.Add(m_pSetup->m_uID);
		msgNotify.Add(pOptName);
		msgNotify.Add((LONG)eLHT_Add);
		msgNotify.SendToPlayer(m_OwnerGuid);
	}
}

//! 删除一个添加我为联系人的人
void LinkmanGroup::DelCompriseMePlayer(const CGUID &PlayerGuid, const char *pOptName)
{
	map<CGUID, tagLinkman_DB>::iterator ite = m_mapAddMe.find(PlayerGuid);
	if(m_mapAddMe.end() == ite) return;
	m_mapAddMe.erase(ite);

	//! 通知被删除玩家
	if (m_pSetup->m_NotifySetup.bMessage_AddAim)
	{
		CMessage msgNotify(MSG_S2C_LINKMAN_OTHER_HANDLE);
		msgNotify.Add(m_pSetup->m_uID);
		msgNotify.Add(pOptName);
		msgNotify.Add((LONG)eLHT_Del);
		msgNotify.SendToPlayer(m_OwnerGuid);
	}
}



//! 一个联系人上线
void LinkmanGroup::LinkmanLogin(tagLinkman_data &Linkman_data)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(Linkman_data.guid);
	if (ite != m_mapMeAdd.end())
	{
		memmove(&(ite->second), &Linkman_data, sizeof(tagLinkman_data));
		CMessage msg(MSG_S2C_LINKMAN_LOGIN);
		msg.Add(m_pSetup->m_uID);
		msg.AddEx(&(ite->second), sizeof(Linkman_data));
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 一个联系人下线
void LinkmanGroup::LinkmanLogout(const CGUID &PlayerGuid)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if (ite != m_mapMeAdd.end())
	{
		ite->second.uState = eLST_Offline;

		CMessage msg(MSG_S2C_LINKMAN_LOGOUT);
		msg.Add(m_pSetup->m_uID);
		msg.Add(ite->second.szName);
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 一个联系人删除状态改变
void LinkmanGroup::LinkmanDelStateChange(const CGUID &PlayerGuid, BOOL bDeleted)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if (ite != m_mapMeAdd.end())
	{
		ite->second.uState = (bDeleted) ? eLST_Deleted : eLST_Offline;

		CMessage msg(MSG_S2C_LINKMAN_DELSTATE_CHANGE);
		msg.Add(m_pSetup->m_uID);
		msg.Add(ite->second.szName);
		msg.Add((LONG)bDeleted);
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 一个联系人地图改变
void LinkmanGroup::LinkmanMapChange(const CGUID &PlayerGuid, const char *pMapName)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if (ite != m_mapMeAdd.end())
	{
		memmove(ite->second.szMapName, pMapName, MAX_PLAYER_TITLE_SIZE - 1);
		
		CMessage msg(MSG_S2C_LINKMAN_INFO_CHANGE);
		msg.Add(m_pSetup->m_uID);
		msg.Add(ite->second.szName);
		msg.Add((LONG)eLIT_Map);
		msg.Add(pMapName);
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 一个联系人等级改变
void LinkmanGroup::LinkmanLevelChange(const CGUID &PlayerGuid, LONG lLevel)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if (ite != m_mapMeAdd.end())
	{
		ite->second.lLevel = lLevel;

		CMessage msg(MSG_S2C_LINKMAN_INFO_CHANGE);
		msg.Add(m_pSetup->m_uID);
		msg.Add(ite->second.szName);
		msg.Add((LONG)eLIT_Level);
		msg.Add(lLevel);
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 一个联系人职业改变
void LinkmanGroup::LinkmanOccuChange(const CGUID &PlayerGuid, LONG lOccu)
{
	map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.find(PlayerGuid);
	if (ite != m_mapMeAdd.end())
	{
		ite->second.lOccupation = lOccu;

		CMessage msg(MSG_S2C_LINKMAN_INFO_CHANGE);
		msg.Add(m_pSetup->m_uID);
		msg.Add(ite->second.szName);
		msg.Add((LONG)eLIT_Occupation);
		msg.Add(lOccu);
		msg.SendToPlayer(m_OwnerGuid);
	}
}


//! 解码初始化数据
void LinkmanGroup::Decode_FromWs(CMessage *pMsg)
{
	m_mapMeAdd.clear();
	LONG lLinkmanNum = pMsg->GetLong();
	for (LONG i = 0; i < lLinkmanNum; ++i)
	{
		tagLinkman Linkman;
		pMsg->GetEx(&Linkman, sizeof(tagLinkman));
		m_mapMeAdd[Linkman.guid] = Linkman;
	}

	m_mapAddMe.clear();
	LONG lGuidNum = pMsg->GetLong();
	for (LONG i = 0; i < lGuidNum; ++i)
	{
		tagLinkman_DB Linkman_DB;
		pMsg->GetEx(&Linkman_DB, sizeof(tagLinkman_DB));
		m_mapAddMe[Linkman_DB.OwnerGUID] = Linkman_DB;
	}
}

//! 编码到数组
void LinkmanGroup::Serialize_ForClient(CMessage *pMsg)
{
	pMsg->Add((LONG)m_pSetup->m_uGroupType);
	pMsg->Add((LONG)m_mapMeAdd.size());
	for (map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.begin(); ite != m_mapMeAdd.end(); ++ite)
	{
		pMsg->AddEx(&(ite->second), sizeof(tagLinkman));
	}
}

//! 编码到WS
void LinkmanGroup::Serialize_ForWS(vector<BYTE> *pVecData)
{
	//! 我添加的玩家
	_AddToByteArray(pVecData, (LONG)m_mapMeAdd.size());
	
	for (map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.begin(); ite != m_mapMeAdd.end(); ++ite)
	{
		tagLinkman_data &Linkman_data = ite->second;
		_AddToByteArray(pVecData, &Linkman_data, sizeof(tagLinkman_data));
	}
	
	//! 添加我的玩家
	_AddToByteArray(pVecData, (LONG)m_mapAddMe.size());
	for (map<CGUID, tagLinkman_DB>::iterator ite = m_mapAddMe.begin(); ite != m_mapAddMe.end(); ++ite)
	{
		_AddToByteArray(pVecData, &(ite->second), sizeof(tagLinkman_DB));
	}
}

void LinkmanGroup::AddMemberToVec(vector<tagLinkman> &vLinkman)
{
	for(map<CGUID, tagLinkman>::iterator ite = m_mapMeAdd.begin(); ite != m_mapMeAdd.end(); ++ite)
	{
		vLinkman.push_back(ite->second);
	}
}

void LinkmanGroup::AddCompriseMePlayerToVec(vector<tagLinkman_DB> &vCompriseMePlayer)
{
	for(map<CGUID, tagLinkman_DB>::iterator ite = m_mapAddMe.begin(); ite != m_mapAddMe.end(); ++ite)
	{
		vCompriseMePlayer.push_back(ite->second);
	}
}