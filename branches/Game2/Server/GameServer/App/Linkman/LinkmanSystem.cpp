
#include "StdAfx.h"
#include "LinkmanSystem.h"
#include "../Player.h"


#ifdef _DEBUG
//!				好友系统实例计数
LONG			g_LinkmanSystemInstanceNum = 0;
#endif
//!				好友系统全局唯一实例
LinkmanSystem	g_LinkmanSystem;


LinkmanSystem::LinkmanSystem(void)
{
#ifdef _DEBUG
	if(0 != g_LinkmanSystemInstanceNum)
		throw;
	++g_LinkmanSystemInstanceNum;
#endif
}
LinkmanSystem::~LinkmanSystem(void){}


//! 得到唯一实例
LinkmanSystem& LinkmanSystem::GetInstance(void)
{
	return g_LinkmanSystem;
}

//! 初始化配置
void LinkmanSystem::InitSetup(BYTE* pByte, LONG& pos)
{
	m_LGSManage.DecordFromByteArray(pByte, pos);
}

//! 响应初始化消息
void LinkmanSystem::OnInitMessage(CMessage *pMsg)
{
	BOOL bSendToC = pMsg->GetByte();
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);;

	if(m_mapPlayerLinkman.end() != m_mapPlayerLinkman.find(PlayerGuid))
	{
		m_mapPlayerLinkman.erase(PlayerGuid);
#ifdef _DEBUG
		char szGuid[64] = {0};
		PlayerGuid.tostring(szGuid);
		char szPlayerName[64] = {0};
		if(NULL != pPlayer)
		{
			memmove(szPlayerName, pPlayer->GetName(), 20);
		}
        //初始化玩家联系人数据时发现联系人已经存在！！[玩家名称：“%s”、玩家ID：“%s”]
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_15", szPlayerName, szGuid));
#endif		
	}

	CMessage msgClient(MSG_S2C_LINKMAN_INIT);

	VecLinkmanGroup vLinkmanGroup;
	LONG lGroupNum = m_LGSManage.GetGroupNum();
	for (LONG i = 0; i < lGroupNum; ++i)
	{
		LinkmanGroup tmpLinkmanGroup(m_LGSManage.GetLinkmanGroupSetup_ByOrder(i), PlayerGuid);
		tmpLinkmanGroup.Decode_FromWs(pMsg);
		if(bSendToC)
			tmpLinkmanGroup.Serialize_ForClient(&msgClient);
		vLinkmanGroup.push_back(tmpLinkmanGroup);
	}

	m_mapPlayerLinkman.insert(pair<CGUID, VecLinkmanGroup>(PlayerGuid, vLinkmanGroup));

	//! 发送到客户端
	if(bSendToC)
		msgClient.SendToPlayer(PlayerGuid);
	
	if(pPlayer)
		pPlayer->UpdateTitle(TRUE);
}

//! 清除玩家数据通知
void LinkmanSystem::OnReleaseMessage(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);
	m_mapPlayerLinkman.erase(PlayerGuid);
}

//! 响应联系人登陆消息
void LinkmanSystem::OnLinkmanLoginMessage(CMessage *pMsg)
{
	tagLinkman_data Linkman_data;
	pMsg->GetEx(&Linkman_data, sizeof(tagLinkman_data));

	LONG lGuidNum = pMsg->GetLong();

	for (LONG i = 0; i < lGuidNum; ++i)
	{
		CGUID ResponserGuid;
		pMsg->GetGUID(ResponserGuid);

		map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(ResponserGuid);
		if (m_mapPlayerLinkman.end() == ite) continue;

		for (LONG j = 0; j < (long)ite->second.size(); ++j)
		{
			if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(j)->m_uKeepWay)
				ite->second[j].LinkmanLogin(Linkman_data);
		}
	}
}

//! 响应联系人离线消息
void LinkmanSystem::OnLinkmanLogoutMessage(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	LONG lGuidNum = pMsg->GetLong();

	for (LONG i = 0; i < lGuidNum; ++i)
	{	
		CGUID ResponserGuid;
		pMsg->GetGUID(ResponserGuid);

		map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(ResponserGuid);
		if (m_mapPlayerLinkman.end() == ite) continue;

		for (LONG j = 0; j < (long)ite->second.size(); ++j)
		{
			if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(j)->m_uKeepWay)
				ite->second[j].LinkmanLogout(PlayerGuid);
		}
	}
}

//! 响应联系人删除状态改变
void LinkmanSystem::OnPlayerDelStateChange(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	BOOL bDeleted = pMsg->GetLong();

	LONG lDataSize = pMsg->GetLong();

	vector<BYTE> vData(lDataSize + 1);
	BYTE *pData = &vData[0];
	pMsg->GetEx(pData, lDataSize);
	LONG lPos = 0;

	for (LONG i = 0; i < lDataSize/(long)sizeof(CGUID); ++i)
	{	
		CGUID ResponserGuid;
		_GetBufferFromByteArray(pData, lPos, ResponserGuid);

		map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(ResponserGuid);
		if (m_mapPlayerLinkman.end() == ite) continue;

		for (LONG j = 0; j < (long)ite->second.size(); ++j)
		{
			if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(j)->m_uKeepWay)
				ite->second[j].LinkmanDelStateChange(PlayerGuid, lDataSize);
		}
	}
}

//! 响应添加联系人操作
void LinkmanSystem::OnAddLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lGroupID, tagLinkman_data &Linkman_data)
{
	assert(NULL != pOptName);
	LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	if (-1 == lIdx) return;

	//! 如果拥有者在本服
	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		assert(NULL != GetGame()->FindPlayer(OwnerGuid));
		//! 找到联系人组
		if((long)ite->second.size() <= lIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_16", szGUID));
			return;
		}
		LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

		//! 创建联系人数据对象
		tagLinkman Linkman;
		memmove(&Linkman, &Linkman_data, sizeof(tagLinkman_data));

		//! 添加联系人
		tmpLinkmanGroup.AddMember(Linkman, TRUE);	
	}

	//! 如果被添加者在本服
	ite = m_mapPlayerLinkman.find(Linkman_data.guid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		assert(NULL != GetGame()->FindPlayer(Linkman_data.guid));
		//! 找到联系人组
		//! 找到联系人组
		if((long)ite->second.size() <= lIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_17", szGUID));
			return;
		}
		LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

		tagLinkman_DB Linkman_DB;
		Linkman_DB.AimGUID = Linkman_data.guid;
		Linkman_DB.OwnerGUID = OwnerGuid;
		memmove(Linkman_DB.szAimName, Linkman_data.szName, MAX_PLAYER_NAME_SIZE);
		Linkman_DB.uGroupID = lGroupID;
		Linkman_DB.uDelState = eLDS_NotDel;

		tmpLinkmanGroup.AddCompriseMePlayer(Linkman_DB, pOptName);
	}

}
//! 响应删除联系人操作
void LinkmanSystem::OnDelLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lGroupID, const CGUID &AimGuid, const char *pAimName)
{
	LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	if (-1 == lIdx) return;

	//! 如果拥有者在本服
	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		assert(NULL != GetGame()->FindPlayer(OwnerGuid));
		if((long)ite->second.size() <= lIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_18", szGUID));
			return;
		}
		//! 找到联系人组
		LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

		//! 删除联系人
		tmpLinkmanGroup.DelMember(AimGuid, TRUE, pAimName);	

		//! 离婚情况，要判断称号
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
		if (NULL != pLinkmanGroupSetup && eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if (eAAT_Spouse == pLinkmanGroupSetup->m_uAutoAddWay)
			{
				CPlayer *pPlayer = GetGame()->FindPlayer(OwnerGuid);
				pPlayer->UpdateTitle(TRUE);
			}
		}
	}

	//! 如果被添加者在本服
	ite = m_mapPlayerLinkman.find(AimGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		assert(NULL != GetGame()->FindPlayer(AimGuid));
		//! 找到联系人组
		if((long)ite->second.size() <= lIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_19", szGUID));
			return;
		}
		LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];
		tmpLinkmanGroup.DelCompriseMePlayer(OwnerGuid, pOptName);

	}
}
//! 响应移动联系人操作
void LinkmanSystem::OnMoveLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lSrcGroupID, LONG lDesGroupID, tagLinkman_data &Linkman_data)
{
	LONG lSrcIdx = m_LGSManage.GetIndexByID(lSrcGroupID);
	LONG lDesIdx = m_LGSManage.GetIndexByID(lDesGroupID);
	if (-1 == lSrcIdx || -1 == lDesIdx)
	{
        //从WS发来的消息中，收到的联系人组是错误的!
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_20"));
		return;
	}
	const LinkmanGroupSetup* pSrcGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx);
	const LinkmanGroupSetup* pDesGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
	if(NULL == pSrcGroupSetup || NULL == pDesGroupSetup) {assert(false); return;}

	//! 如果拥有者在本服
	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		if((long)ite->second.size() <= lSrcIdx || (long)ite->second.size() <= lDesIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_21", szGUID));
			return;
		}
		assert(NULL != GetGame()->FindPlayer(OwnerGuid));

		if (eKW_ServerKeep == pSrcGroupSetup->m_uKeepWay)
		{
			(ite->second)[lSrcIdx].DelMember(Linkman_data.guid, FALSE, Linkman_data.szName);
		}

		//! 创建联系人数据对象
		tagLinkman Linkman;
		memmove(&Linkman, &Linkman_data, sizeof(tagLinkman_data));
		if (eKW_ServerKeep == pDesGroupSetup->m_uKeepWay)
		{	
			(ite->second)[lDesIdx].AddMember(Linkman, FALSE);
		}

		CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
		msgReAdd.Add((LONG)TRUE);
		msgReAdd.Add(lSrcGroupID);
		msgReAdd.Add(lDesGroupID);
		msgReAdd.AddEx(&Linkman, sizeof(tagLinkman));
		msgReAdd.SendToPlayer(OwnerGuid);
	}

	//! 如果被添加者在本服
	ite = m_mapPlayerLinkman.find(Linkman_data.guid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		if((long)ite->second.size() <= lSrcIdx || (long)ite->second.size() <= lDesIdx)
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //玩家[%s]好友的组数小于WS发过来的组数！
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_22", szGUID));
			return;
		}
		assert(NULL != GetGame()->FindPlayer(Linkman_data.guid));

		tagLinkman_DB Linkman_DB;
		Linkman_DB.AimGUID = Linkman_data.guid;
		Linkman_DB.OwnerGUID = OwnerGuid;
		memmove(Linkman_DB.szAimName, Linkman_data.szName, MAX_PLAYER_NAME_SIZE);
		Linkman_DB.uDelState = eLDS_NotDel;

		if (eKW_ServerKeep == pSrcGroupSetup->m_uKeepWay)
		{
			Linkman_DB.uGroupID = lSrcGroupID;
			(ite->second)[lSrcIdx].DelCompriseMePlayer(Linkman_DB.AimGUID, pOptName);
		}

		if (eKW_ServerKeep == pDesGroupSetup->m_uKeepWay)
		{
			Linkman_DB.uGroupID = lDesGroupID;
			(ite->second)[lDesIdx].AddCompriseMePlayer(Linkman_DB, pOptName);
		}
	}
}


//! 响应玩家信息改变消息
void LinkmanSystem::OnPlayerInfoChange(CMessage *pMsg)
{
	CGUID PlayerGuid;
	pMsg->GetGUID(PlayerGuid);

	//! 得到改变的信息类型和值
	LONG lInfoType = pMsg->GetLong();
	LONG lValue	= 0;
	char szValue[MAX_PLAYER_TITLE_SIZE] = {0};

	switch(lInfoType)
	{
	case eLIT_Map://! 地图
		pMsg->GetStr(szValue, MAX_PLAYER_TITLE_SIZE);
		break;
	case eLIT_Level://! 等级
	case eLIT_Occupation://! 职业
		lValue = pMsg->GetLong();
	    break;
	default:
	    return;
	}

	LONG lGuidNum = pMsg->GetLong();

	for (LONG i = 0; i < lGuidNum; ++i)
	{
		CGUID ResponserGuid;
		pMsg->GetGUID(ResponserGuid);

		map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(ResponserGuid);
		if (m_mapPlayerLinkman.end() == ite) continue;

		for (LONG j = 0; j < (long)ite->second.size(); ++j)
		{
			const LinkmanGroupSetup *PLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(j);
			assert(PLinkmanGroupSetup);
			if(NULL == PLinkmanGroupSetup) 
			{
				Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"PLinkman GroupSetup index error!!! index value = %d", j);
				continue;
			}
			if (eKW_ServerKeep == PLinkmanGroupSetup->m_uKeepWay && eTIT_Show_All == PLinkmanGroupSetup->m_uTimelyInfo)
			{
				switch(lInfoType)
				{
				case eLIT_Map://! 地图
					ite->second[j].LinkmanMapChange(PlayerGuid, szValue);
					break;
				case eLIT_Level://! 等级
					ite->second[j].LinkmanLevelChange(PlayerGuid, lValue);
					break;
				case eLIT_Occupation://! 职业
					ite->second[j].LinkmanOccuChange(PlayerGuid, lValue);
					break;
				}
			}
		}

	}
}




//! 响应申请添加联系人
void LinkmanSystem::OnRequestAdd(CMessage *pMsg)
{
	//! new code
	{
		const CGUID &PlayerGuid = pMsg->GetPlayerID();
		ULONG lGroupID = pMsg->GetLong();
		char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
		pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

 
		//! -------------确认操作合法性-------------
		LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
		if (-1 == lIdx) return;
		const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
		if(NULL == pLinkmanGroupSetup) 
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"LinkmanSystem::OnRequestAdd！！！");
			return;
		}
		if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
		if (!pLinkmanGroupSetup->m_bUserAdd) return;
		
		//! -------------向WS发送消息-------------
		CMessage msgToWS(MSG_S2W_LINKMAN_PALYER_REQUEST_ADD);
		msgToWS.Add(PlayerGuid);
		msgToWS.Add(lGroupID);
		msgToWS.Add(szAimName);	
		msgToWS.Send();
	}
	return;

	//const CGUID &PlayerGuid = pMsg->GetPlayerID();
	//map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(PlayerGuid);
	//if (m_mapPlayerLinkman.end() == ite) return;
	//
	//ULONG lGroupID = pMsg->GetLong();
	//LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	//if (-1 == lIdx) return;

	//char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	//pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

	//const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);

	////! 确认合法性
	//if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
	//if (!pLinkmanGroupSetup->m_bUserAdd) return;

	////! 找到联系人组
	//LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

	//CMessage msgReAdd(MSG_S2C_LINKMAN_ADD);

	////! 找到玩家
	//CPlayer *pAimPlayer = GetGame()->FindPlayer(szAimName);

	////! 如果不在同一个GS上，则不添加
	//if (NULL == pAimPlayer)
	//{
	//	msgReAdd.Add((LONG)FALSE);
	//	msgReAdd.Add((LONG)eOET_AimNotBeing);
	//	msgReAdd.Add(szAimName);
	//	msgReAdd.SendToPlayer(PlayerGuid);
	//	return;
	//}

	////! 被添加者不能是自己
	//if (pAimPlayer->GetExID() == PlayerGuid)
	//{
	//	return;
	//}
	//
	//const CGUID &AimGuid = pAimPlayer->GetExID();
	////! 确认玩家没有被这个组和其他组添加
	//for (LONG i = 0; i < ite->second.size(); ++i)
	//{
	//	if(ite->second[i].FindMember(AimGuid)) return;
	//}

	////! 判断人数
	//if (tmpLinkmanGroup.GetMemberNum() >= pLinkmanGroupSetup->m_uMaxMemberNum) return;

	////! 是否需要目标确认
	//if (pLinkmanGroupSetup->m_bAddAimAffirm)
	//{
	//	//! 创建会话数据
	//	tagOptSession OptSession;
	//	OptSession._uEndTime	= time(NULL) + MAX_LINKMAN_OPT_TIME_SEC;
	//	OptSession._uOptType	= eLOT_Add;
	//	OptSession._uSrcGroupID	= INVALID_GROUP_ID;
	//	OptSession._uDesGroupID	= lGroupID;

	//	if(CreateAddSession(AimGuid, PlayerGuid, OptSession))
	//	{
	//		CMessage msgrEquestAdd(MSG_S2C_LINKMAN_REQUEST_ADD);
	//		msgrEquestAdd.Add(lGroupID);
	//		msgrEquestAdd.Add(pMsg->GetPlayer()->GetName());
	//		msgrEquestAdd.Add(PlayerGuid);
	//		msgrEquestAdd.SendToPlayer(AimGuid);
	//	}
	//	else
	//	{
	//		msgReAdd.Add((LONG)FALSE);
	//		msgReAdd.Add((LONG)eOET_OverfullToOne);
	//		msgReAdd.Add(szAimName);
	//		msgReAdd.SendToPlayer(PlayerGuid);
	//	}
	//}
	//else
	//{
	//	//! 创建联系人数据对象
	//	tagLinkman Linkman;
	//	assert(m_mapPlayerLinkman.find(AimGuid) != m_mapPlayerLinkman.end());
	//	Linkman.bMutual = m_mapPlayerLinkman[AimGuid][lIdx].FindMember(PlayerGuid);
	//	Linkman.guid = AimGuid;
	//	Linkman.lLevel = pAimPlayer->GetLevel();
	//	Linkman.lOccupation = pAimPlayer->GetOccupation();
	//	Linkman.uState = eLST_Online;
	//	memmove(Linkman.szName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	//	memmove(Linkman.szMapName, pAimPlayer->GetFather()->GetName(), MAX_PLAYER_TITLE_SIZE - 1);
	//	
	//	//! 添加联系人
	//	tmpLinkmanGroup.AddMember(Linkman, TRUE);
	//	// GetGameLogInterface()->logT450_linkman_opt_log(pMsg->GetPlayer(), AimGuid, Linkman.szName, 1, lGroupID);
	//	//! 添加[添加我的人]
	//	
	//	m_mapPlayerLinkman[AimGuid][lIdx].AddCompriseMePlayer(PlayerGuid);
	//}
}

//! 响应请求删除联系人
void LinkmanSystem::OnRequestDel(CMessage *pMsg)
{
	//! new code
	{
		const CGUID &PlayerGuid = pMsg->GetPlayerID();
		ULONG lGroupID = pMsg->GetLong();
		char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
		pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);


		//! -------------确认操作合法性-------------
		LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
		if (-1 == lIdx) return;
		const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
		if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
		if (!pLinkmanGroupSetup->m_bUserDel) return;

		//! -------------向WS发送消息-------------
		CMessage msgToWS(MSG_S2W_LINKMAN_PALYER_REQUEST_DEL);
		msgToWS.Add(PlayerGuid);
		msgToWS.Add(lGroupID);
		msgToWS.Add(szAimName);	
		msgToWS.Add((BYTE)false);
		msgToWS.Send();
	}
	return;

	//const CGUID &PlayerGuid = pMsg->GetPlayerID();
	//map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(PlayerGuid);
	//if (m_mapPlayerLinkman.end() == ite) return;

	//ULONG lGroupID = pMsg->GetLong();
	//LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	//if (-1 == lIdx) return;

	//char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	//pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

	//const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);

	////! 确认合法性
	//if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
	//if (!pLinkmanGroupSetup->m_bUserDel) return;

	////! 找到联系人组
	//LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

	////! 得到被删除者的ID
	//CGUID AimGuid = tmpLinkmanGroup.FindMemberID(szAimName);
	//if (NULL_GUID == AimGuid) return;

	//tmpLinkmanGroup.DelMember(AimGuid, TRUE, szAimName);
	//// GetGameLogInterface()->logT450_linkman_opt_log(pMsg->GetPlayer(), AimGuid, szAimName, 2, lGroupID);

	////! 更新被删除者的[添加我的联系人]列表
	//map<CGUID, VecLinkmanGroup>::iterator iteLinkmanGroup =	m_mapPlayerLinkman.find(AimGuid); 
	//if(iteLinkmanGroup != m_mapPlayerLinkman.end())
	//{
	//	iteLinkmanGroup->second[lIdx].DelCompriseMePlayer(PlayerGuid);
	//}
}

//! 响应请求移动联系人
void LinkmanSystem::OnRequestMove(CMessage *pMsg)
{
	//! new code
	{
		const CGUID &OwnerGuid = pMsg->GetPlayerID();
		ULONG lSrcGroupID	= pMsg->GetLong();
		ULONG lDesGroupID	= pMsg->GetLong();
		char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
		pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

		//! -------------确认操作合法性-------------
		LONG lSrcIdx = m_LGSManage.GetIndexByID(lSrcGroupID);
		LONG lDesIdx = m_LGSManage.GetIndexByID(lDesGroupID);
		if (-1 == lSrcIdx || -1 == lDesIdx) return;
		const LinkmanGroupSetup* pSrcGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx);
		const LinkmanGroupSetup* pDesGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
		BOOL bNeedServerOpt = FALSE;
		if (eKW_ServerKeep == pSrcGroupSetup->m_uKeepWay) 
		{
			bNeedServerOpt = TRUE;
			if (!pSrcGroupSetup->m_bUserDel) return;
		}
		if (eKW_ServerKeep == pDesGroupSetup->m_uKeepWay) 
		{	
			bNeedServerOpt = TRUE;
			if (!pDesGroupSetup->m_bUserAdd) return;
		}
		if(!bNeedServerOpt) return;

		//! -------------向WS发送消息-------------
		CMessage msgToWS(MSG_S2W_LINKMAN_PALYER_REQUEST_MOVE);
		msgToWS.Add(OwnerGuid);
		msgToWS.Add(lSrcGroupID);
		msgToWS.Add(lDesGroupID);
		msgToWS.Add(szAimName);
		msgToWS.Send();
	}
	return;

	////! 操作的组
	//ULONG lSrcGroupID	= pMsg->GetLong();
	//ULONG lDesGroupID	= pMsg->GetLong();

	//LONG lSrcIdx = m_LGSManage.GetIndexByID(lSrcGroupID);
	//LONG lDesIdx = m_LGSManage.GetIndexByID(lDesGroupID);

	////! 被操作的玩家
	//char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	//pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);
	//CPlayer *pAimPlayer = GetGame()->FindPlayer(szAimName);
	//CGUID AimGuid;

	////! 操作的玩家
	//const CGUID &OwnerGuid = pMsg->GetPlayerID();

	////! 操作对象
	//map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	//if (m_mapPlayerLinkman.end() == ite) return;
	//VecLinkmanGroup &vLinkmanGroup = ite->second;

	////! 先验证删除
	//BOOL bRunDel = TRUE;
	//const LinkmanGroupSetup* pDelSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx);
	//if (eKW_ServerKeep != pDelSetup->m_uKeepWay)
	//{
	//	bRunDel = FALSE;
	//}
	//else
	//{
	//	if(!pDelSetup->m_bUserDel) return;//! 不允许客户端删除操作
	//	AimGuid = vLinkmanGroup[lSrcIdx].FindMemberID(szAimName);
	//	if (NULL_GUID == AimGuid) return;//! 删除对象不在分组中
	//}

	////! 再验证添加
	//BOOL bRunAdd = TRUE;
	//const LinkmanGroupSetup* pAddSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
	//if (eKW_ServerKeep != pAddSetup->m_uKeepWay)
	//{
	//	bRunAdd = FALSE;
	//}
	//else
	//{
	//	if(!pAddSetup->m_bUserAdd) return; //! 不允许客户端添加操作
	//	for (LONG i = 0; i < vLinkmanGroup.size(); ++i)
	//	{
	//		if (i != lSrcIdx)
	//			if (NULL_GUID != vLinkmanGroup[i].FindMemberID(szAimName)) return;
	//	}
	//	if (NULL_GUID != vLinkmanGroup[lDesIdx].FindMemberID(szAimName)) return;//! 添加对象已存在
	//	if ( NULL == pAimPlayer) //! 玩家不在线，找不到玩家的ID，无法进行
	//	{
	//		CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
	//		msgReAdd.Add((LONG)FALSE);
	//		msgReAdd.Add((LONG)eOET_AimNotBeing);
	//		msgReAdd.Add(szAimName);
	//		msgReAdd.SendToPlayer(OwnerGuid);
	//		return;
	//	}
	//	if (pAimPlayer->GetExID() == OwnerGuid)//! 被添加者是操作者自己
	//	{
	//		return;
	//	}
	//	if(vLinkmanGroup[lDesIdx].GetMemberNum() >= m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx)->m_uMaxMemberNum)
	//	{
	//		CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
	//		msgReAdd.Add((LONG)FALSE);
	//		msgReAdd.Add((LONG)eOET_ListFull);
	//		msgReAdd.Add(szAimName);
	//		msgReAdd.SendToPlayer(OwnerGuid);
	//		return;
	//	}
	//	AimGuid = pAimPlayer->GetExID();
	//}

	//if ((!bRunDel) && (!bRunAdd)) return;//! 两个组都管理在客户端，不需要服务器执行


	////! 开始执行
	//if (bRunAdd)
	//{
	//	if (pAddSetup->m_bAddAimAffirm) //! 如果要求目标确认
	//	{
	//		//! 创建会话数据
	//		tagOptSession OptSession;
	//		OptSession._uEndTime	= time(NULL) + MAX_LINKMAN_OPT_TIME_SEC;
	//		OptSession._uOptType	= eLot_Move;
	//		OptSession._uSrcGroupID	= lSrcGroupID;
	//		OptSession._uDesGroupID	= lDesGroupID;

	//		if(CreateAddSession(AimGuid, OwnerGuid, OptSession))
	//		{
	//			CMessage msgrEquestAdd(MSG_S2C_LINKMAN_REQUEST_ADD);
	//			msgrEquestAdd.Add(lDesGroupID);
	//			msgrEquestAdd.Add(pMsg->GetPlayer()->GetName());
	//			msgrEquestAdd.Add(OwnerGuid);
	//			msgrEquestAdd.SendToPlayer(AimGuid);
	//		}
	//		else
	//		{
	//			CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
	//			msgReAdd.Add((LONG)FALSE);
	//			msgReAdd.Add((LONG)eOET_OverfullToOne);
	//			msgReAdd.Add(szAimName);
	//			msgReAdd.SendToPlayer(OwnerGuid);
	//		}

	//		return;
	//	}
	//}

	//map<CGUID, VecLinkmanGroup>::iterator iteAim = m_mapPlayerLinkman.find(AimGuid);
	//if (m_mapPlayerLinkman.end() == iteAim) return;
	//VecLinkmanGroup &vAimLinkmanGroup = iteAim->second;

	//if (bRunDel)
	//{
	//	vLinkmanGroup[lSrcIdx].DelMember(AimGuid, FALSE, szAimName);
	//	// GetGameLogInterface()->logT450_linkman_opt_log(pMsg->GetPlayer(), AimGuid, szAimName, 2, lSrcGroupID);
	//	vAimLinkmanGroup[lSrcIdx].DelCompriseMePlayer(OwnerGuid);
	//}

	////! 创建联系人数据对象
	//tagLinkman Linkman;
	//memmove(Linkman.szName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	//if (bRunAdd)
	//{
	//	Linkman.bMutual = vAimLinkmanGroup[lDesIdx].FindMember(OwnerGuid);
	//	Linkman.guid = AimGuid;
	//	Linkman.lLevel = pAimPlayer->GetLevel();
	//	Linkman.lOccupation = pAimPlayer->GetOccupation();
	//	Linkman.uState = eLST_Online;
	//	memmove(Linkman.szMapName, pAimPlayer->GetFather()->GetName(), MAX_PLAYER_TITLE_SIZE - 1);

	//	vLinkmanGroup[lDesIdx].AddMember(Linkman, FALSE);
	//	// GetGameLogInterface()->logT450_linkman_opt_log(pMsg->GetPlayer(), AimGuid, szAimName, 1, lDesGroupID);
	//	vAimLinkmanGroup[lDesIdx].AddCompriseMePlayer(OwnerGuid);
	//}

	//CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
	//msgReAdd.Add((LONG)TRUE);
	//msgReAdd.Add(lSrcGroupID);
	//msgReAdd.Add(lDesGroupID);
	//msgReAdd.AddEx(&Linkman, sizeof(tagLinkman));
	//msgReAdd.SendToPlayer(OwnerGuid);

}

//! 响应玩家回复被添加
void LinkmanSystem::OnAnswerAdd(CMessage *pMsg)
{
	{
		ULONG lGroupID	= pMsg->GetLong();
		CGUID RequestPlayerGuid;
		pMsg->GetGUID(RequestPlayerGuid);
		BOOL lAgree		= pMsg->GetLong();
		const CGUID &RePlayerGuid = pMsg->GetPlayerID();

		CMessage msgToWS(MSG_S2W_LINKMAN_PALYER_RE_ADD);
		msgToWS.Add(lGroupID);
		msgToWS.Add(RequestPlayerGuid);
		msgToWS.Add((LONG)lAgree);
		msgToWS.Add(RePlayerGuid);
		msgToWS.Send();
	}
	return;

	//ULONG lGroupID	= pMsg->GetLong();
	//CGUID PlayerGuid;
	//pMsg->GetGUID(PlayerGuid);
	//BOOL lAgree		= pMsg->GetLong();
	//const CGUID &AimGuid = pMsg->GetPlayerID();

	////! 创建验证会话的对象
	//tagOptSession OptSession;
	//OptSession._uSrcGroupID = INVALID_GROUP_ID;
	//OptSession._uOptType	= eLOT_Add;
	//OptSession._uDesGroupID = lGroupID;

	//BOOL bLegitimate = FALSE;
	//if (EndAddSession(AimGuid, PlayerGuid, OptSession))
	//{
	//	bLegitimate = TRUE;
	//}
	//else if (eLot_Move == OptSession._uOptType)
	//{
	//	bLegitimate = TRUE;
	//}

	//if (bLegitimate)
	//{
	//	CPlayer *pAimPlayer = pMsg->GetPlayer();
	//	if (lAgree)
	//	{
	//		LONG lDesIdx = m_LGSManage.GetIndexByID(OptSession._uDesGroupID);

	//		map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(PlayerGuid);
	//		if (m_mapPlayerLinkman.end() == ite) return;
	//		LinkmanGroup &DesLinkmanGroup = (ite->second)[lDesIdx];

	//		//! 如果人数满了，则返回错误提示
	//		if(DesLinkmanGroup.GetMemberNum() > m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx)->m_uMaxMemberNum)
	//		{
	//			CMessage msg(MSG_S2C_LINKMAN_ADD);
	//			msg.Add((LONG)FALSE);
	//			msg.Add((LONG)eOET_ListFull);
	//			msg.Add(pAimPlayer->GetName());
	//			msg.SendToPlayer(PlayerGuid);
	//			return;
	//		}

	//		//! 如果不是移动请求，联系人在其他组则返回失败
	//		if (eLot_Move != OptSession._uOptType)
	//		{
	//			for (LONG i = 0; i < ite->second.size(); ++i)
	//			{
	//				if (i != lDesIdx)
	//				{
	//					if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(i)->m_uKeepWay)
	//					{
	//						if (ite->second[i].FindMember(AimGuid))
	//						{
	//							CMessage msg(MSG_S2C_LINKMAN_ADD);
	//							msg.Add((LONG)FALSE);
	//							msg.Add((LONG)eOET_AimInOther);
	//							msg.Add(pAimPlayer->GetName());
	//							msg.SendToPlayer(PlayerGuid);
	//							return;
	//						}
	//					}
	//				}
	//			}
	//		}

	//		//! 创建联系人数据对象
	//		assert(m_mapPlayerLinkman.find(AimGuid) != m_mapPlayerLinkman.end());
	//		tagLinkman Linkman;
	//		Linkman.bMutual = m_mapPlayerLinkman[AimGuid][lDesIdx].FindMember(PlayerGuid);
	//		Linkman.guid = AimGuid;
	//		Linkman.lLevel = pAimPlayer->GetLevel();
	//		Linkman.lOccupation = pAimPlayer->GetOccupation();
	//		Linkman.uState = eLST_Online;
	//		memmove(Linkman.szName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	//		memmove(Linkman.szMapName, pAimPlayer->GetFather()->GetName(), MAX_PLAYER_TITLE_SIZE - 1);

	//		CPlayer *pSrcPlayer = GetGame()->FindPlayer(PlayerGuid);
	//		if (eLot_Move == OptSession._uOptType)
	//		{
	//			LONG lSrcIdx = m_LGSManage.GetIndexByID(OptSession._uSrcGroupID);
	//			if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx)->m_uKeepWay)
	//			{
	//				LinkmanGroup &SrcLinkmanGroup = (ite->second)[lSrcIdx];

	//				SrcLinkmanGroup.DelMember(AimGuid, FALSE, pAimPlayer->GetName());
	//				// GetGameLogInterface()->logT450_linkman_opt_log(pSrcPlayer, AimGuid, pAimPlayer->GetName(), 2, OptSession._uSrcGroupID);

	//				assert(m_mapPlayerLinkman.find(AimGuid) != m_mapPlayerLinkman.end());
	//				m_mapPlayerLinkman[AimGuid][lSrcIdx].DelCompriseMePlayer(PlayerGuid);
	//			}

	//			//! 添加联系人
	//			DesLinkmanGroup.AddMember(Linkman, FALSE);
	//			// GetGameLogInterface()->logT450_linkman_opt_log(pSrcPlayer, AimGuid, pAimPlayer->GetName(), 1, OptSession._uDesGroupID);

	//			CMessage msgReAdd(MSG_S2C_LINKMAN_MOVE);
	//			msgReAdd.Add((LONG)TRUE);
	//			msgReAdd.Add(OptSession._uSrcGroupID);
	//			msgReAdd.Add(OptSession._uDesGroupID);
	//			msgReAdd.AddEx(&Linkman, sizeof(tagLinkman));
	//			msgReAdd.SendToPlayer(PlayerGuid);
	//		}
	//		else
	//		{	
	//			//! 添加联系人
	//			DesLinkmanGroup.AddMember(Linkman, TRUE);
	//			// GetGameLogInterface()->logT450_linkman_opt_log(pSrcPlayer, AimGuid, pAimPlayer->GetName(), 1, lGroupID);
	//		}

	//		//! 添加[添加我的人]
	//		m_mapPlayerLinkman[AimGuid][lDesIdx].AddCompriseMePlayer(PlayerGuid);

	//	}
	//	else
	//	{
	//		CMessage msg(MSG_S2C_LINKMAN_ADD);
	//		msg.Add((LONG)lAgree);
	//		msg.Add((LONG)eOET_AimReject);
	//		msg.Add(pAimPlayer->GetName());
	//		msg.SendToPlayer(PlayerGuid);
	//	}
	//}
}

//! 发送联系人数据到WS
void LinkmanSystem::SendDataToWs(void)
{
	CMessage msgToWs(MSG_S2W_LINKMAN_DATA);
	vector<BYTE> vLinkmanData;

	//! 先给玩家总数占个位置
	_AddToByteArray(&vLinkmanData, (LONG)0L);

	//! 实际发送人数统计
	LONG lNum = 0;
	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.begin();
	for (; ite != m_mapPlayerLinkman.end(); ++ite)
	{
		if(NULL == GetGame()->FindPlayer(ite->first))
		{
			if(NULL == GetGame()->FindSavePlayer(ite->first))
				continue;
		}

		++lNum;
		//! 玩家GUID
		_AddToByteArray(&vLinkmanData, ite->first);
		VecLinkmanGroup &vLinkmanGroup = ite->second;
		//! 联系人组数
		_AddToByteArray(&vLinkmanData, (LONG)vLinkmanGroup.size());
		for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
		{
			//! 联系人数据
			vLinkmanGroup[i].Serialize_ForWS(&vLinkmanData);
		}
	}

	//! 在编码中替换实际人数
	memcpy(&vLinkmanData[0], &lNum, sizeof(LONG));

	msgToWs.Add((LONG)vLinkmanData.size());
	msgToWs.AddEx(&vLinkmanData[0], (long)vLinkmanData.size());
	msgToWs.Send();
}	

//!								响应玩家退出服务器
void LinkmanSystem::OnPlayerExit(const CGUID &PlayerGuid)
{
	if(GetGame()->GetWSSocket() > 0)
		m_mapPlayerLinkman.erase(PlayerGuid);
}

//! 记录PK致死名单
void LinkmanSystem::NotePkPlayer(CPlayer *pDecedent, CPlayer *pMurderer)
{
	assert(NULL != pDecedent && NULL != pMurderer);
	if (NULL == pDecedent || NULL == pMurderer)
	{
        //PK者或者被杀死者的对象指针为空，添加仇人失败！
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_23"));
		return;
	}

	assert(pDecedent->GetExID() != pMurderer->GetExID());
	if(pDecedent->GetExID() == pMurderer->GetExID())
	{
        //PK者与被杀死者为同一个人，添加仇人失败！
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_24"));
		return;
	}

	if(m_mapPlayerLinkman.find(pDecedent->GetExID()) == m_mapPlayerLinkman.end())
	{
        //添加仇人时，找不到受害玩家的好友对象！
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_25"));
		return;
	}
	VecLinkmanGroup &vLinkmanGroup = m_mapPlayerLinkman[pDecedent->GetExID()];

	//! 找出是否存在于某个组中，如果是则不添加
	for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
	{
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if(vLinkmanGroup[i].FindMember(pMurderer->GetExID()))
			{
				return;
			}
		}
	}

	//! 添加
	for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
	{
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if (eAAT_Attacker == (long)pLinkmanGroupSetup->m_uAutoAddWay && vLinkmanGroup[i].GetMemberNum() < (long)pLinkmanGroupSetup->m_uMaxMemberNum)
			{
				CMessage msgToWS(MSG_S2W_LINKMAN_SYSTEM_REQUEST_ADD);
				msgToWS.Add(pDecedent->GetExID());
				msgToWS.Add(pLinkmanGroupSetup->m_uID);
				msgToWS.Add(pMurderer->GetName());	
				msgToWS.Send();

				return;
			}
		}
	}
}

//! 记录结婚对象
void LinkmanSystem::AddSpouse(CPlayer *pSelf, CPlayer *pSide)
{
	assert(NULL != pSelf && NULL != pSide);
	if (NULL == pSelf || NULL == pSide)
	{
        //配偶对象指针为空，添加配偶失败!
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_26"));
		return;
	}

	assert(pSelf->GetExID() != pSide->GetExID());
	if(pSelf->GetExID() == pSide->GetExID())
	{
        //两个结婚者为同一个人，添加仇人失败！
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_27"));
		return;
	}

	if(m_mapPlayerLinkman.find(pSelf->GetExID()) == m_mapPlayerLinkman.end())
	{
        //添加配偶时，找不到结婚玩家的好友对象！
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GS_PLAYER_28"));
		return;
	}

	//! 添加
	VecLinkmanGroup &vLinkmanGroup = m_mapPlayerLinkman[pSelf->GetExID()];
	for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
	{
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if (eAAT_Spouse == (long)pLinkmanGroupSetup->m_uAutoAddWay && vLinkmanGroup[i].GetMemberNum() < (long)pLinkmanGroupSetup->m_uMaxMemberNum)
			{
				CMessage msgToWS(MSG_S2W_LINKMAN_SYSTEM_REQUEST_ADD);
				msgToWS.Add(pSelf->GetExID());
				msgToWS.Add(pLinkmanGroupSetup->m_uID);
				msgToWS.Add(pSide->GetName());	
				msgToWS.Send();

				return;
			}
		}
	}
}

//! 删除结婚对象
void LinkmanSystem::DelSpouse(CPlayer *pSelf)
{
	if (NULL == pSelf)
		return;

	VecLinkmanGroup &vLinkmanGroup = m_mapPlayerLinkman[pSelf->GetExID()];
	for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
	{
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if (eAAT_Spouse == pLinkmanGroupSetup->m_uAutoAddWay && 0 < vLinkmanGroup[i].GetMemberNum())
			{
				const	tagLinkman *pSpouse = vLinkmanGroup[i].GetFirstMember();
				if(NULL != pSpouse)
				{
					CMessage msgToWS(MSG_S2W_LINKMAN_PALYER_REQUEST_DEL);
					msgToWS.Add(pSelf->GetExID());
					msgToWS.Add(pLinkmanGroupSetup->m_uID);
					msgToWS.Add(pSpouse->szName);
					msgToWS.Add((BYTE)true);
					msgToWS.Send();
				}
				return;
			}
		}
	}
}

//!								找到结婚对象信息
const tagLinkman* LinkmanSystem::FindSpouseInfo(CPlayer *pSelf)
{
	const tagLinkman *pRe = NULL; 

	if (NULL == pSelf)
		return pRe;
	
	VecLinkmanGroup &vLinkmanGroup = m_mapPlayerLinkman[pSelf->GetExID()];
	for (LONG i = 0; i < (long)vLinkmanGroup.size(); ++i)
	{
		const	LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			if (eAAT_Spouse == pLinkmanGroupSetup->m_uAutoAddWay && 0 < vLinkmanGroup[i].GetMemberNum())
			{
				pRe = vLinkmanGroup[i].GetFirstMember();
				break;
			}
		}
	}

	return pRe;
}


//! 判断一个玩家【OwnerGuid】是否与另一个玩家【pOtherOneName】存在在指定的关系
BOOL LinkmanSystem::CheckRelation(const CGUID &OwnerGuid, const char *pOtherOneName, eLinkmanGroupType eRelationType)
{
	LONG lIdx = m_LGSManage.GetIndexByType(eRelationType);
	if (-1 == lIdx) return FALSE;

	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		//! 找到联系人组
		if((long)ite->second.size() > lIdx)
		{
			LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

			if(NULL_GUID != tmpLinkmanGroup.FindMemberID(pOtherOneName))
				return TRUE;
		}
		else
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //发生不应该的错误, 玩家的联系人数据中没有第[%d]组,需要检查
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_29", lIdx));
		}	
	}	

	return FALSE;
}

//! 判断一个玩家【OwnerGuid】是否与另一个玩家【OtherOneGuid】存在在指定的关系
BOOL LinkmanSystem::CheckRelation(const CGUID &OwnerGuid, const CGUID &OtherOneGuid, eLinkmanGroupType eRelationType)
{
	LONG lIdx = m_LGSManage.GetIndexByType(eRelationType);
	if (-1 == lIdx) return FALSE;

	map<CGUID, VecLinkmanGroup>::iterator ite = m_mapPlayerLinkman.find(OwnerGuid);
	if (m_mapPlayerLinkman.end() != ite)
	{
		//! 找到联系人组
		if((long)ite->second.size() > lIdx)
		{
			LinkmanGroup &tmpLinkmanGroup = (ite->second)[lIdx];

			return tmpLinkmanGroup.FindMember(OtherOneGuid);
		}
		else
		{
			char szGUID[256] = {0};
			OwnerGuid.tostring(szGUID);
            //发生不应该的错误, 玩家的联系人数据中没有第[%d]组,需要检查
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GS_PLAYER_30", lIdx));
		}	
	}	

	return FALSE;
}