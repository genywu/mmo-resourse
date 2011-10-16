#include "StdAfx.h"
#include "GSTeam.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../RgnManager.h"
#include "../../Server/GameManager.h"

#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/Setup/PlayerList.h"
#include "../../../../Public/Common/FollowDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GSTeam::tagDefaultSetup GSTeam::s_tagDefaultSetup(eGA_FREE, 0, 0);

GSTeam::tagDefaultSetup::tagDefaultSetup(long lGA, long lMMN, long lSTS)
:
lGoodsAllot(lGA),
lMaxMemberNum(lMMN),
lSessionTimeoutS(lSTS)
{
}

GSTeam::GSTeam()
:
m_MemberList(0),
m_eGoodsAllot(s_tagDefaultSetup.lGoodsAllot),
m_lOnThisGsMemberNum(0),
m_lCurrShareQuestID(0),
m_dwAcceptTime(0)
{
	//m_iteNextGainer = m_MemberList.end();
    m_TeamPhaseID = NULL_GUID;
	srand(timeGetTime());
	memset(m_szName, 0, 20);
}

GSTeam::~GSTeam(void){}

const	CGUID&	GSTeam::GetExID(void)
{
	return m_guid;
}
const	CGUID&	GSTeam::SetExID(const CGUID& guid)
{
	return m_guid = guid;
}
const	char*	GSTeam::GetName(void)
{
	return m_szName;
}
const	char*	GSTeam::SetName(const char *pName)
{
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (19 >= lSize ) ? lSize : 19;
		memmove(m_szName, pName, lSize);	
	}
	return m_szName;
}

const CGUID& GSTeam::IsMaster(const CGUID& guid)
{
	return (m_MestterGuid == guid)? guid : NULL_GUID;
}

//得到掌门的ID
const CGUID& GSTeam::GetMasterID()
{
	return m_MestterGuid;
}

void GSTeam::GetDataFromArray(BYTE *pData, LONG &lPos)
{
	_GetBufferFromByteArray(pData, lPos, m_guid);
	_GetBufferFromByteArray(pData, lPos, m_MestterGuid);
	m_eGoodsAllot = _GetLongFromByteArray(pData, lPos);

	m_MemberList.clear();
	LONG lMemberNum = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lMemberNum; ++i)
	{
		tagMemberInfo MemberInfo;
		_GetBufferFromByteArray(pData, lPos, &MemberInfo, sizeof(tagWSMemberInfo));
		MemberInfo.bChanged = false;
		m_MemberList.push_back(MemberInfo);
	}
}

//! 发送限制组队的技能ID和限制的级数到客户端
void GSTeam::SendAboutSkillToC(vector<BYTE> *pByteArray)
{
	_AddToByteArray(pByteArray, s_tagDefaultSetup.lMinSkillLevel);
	_AddToByteArray(pByteArray, s_tagDefaultSetup.lCorrelativeSkillID);
}

//! 发送限制组队的技能ID和限制的级数到客户端
void GSTeam::SendAboutSkillToC(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray( s_tagDefaultSetup.lMinSkillLevel);
	setWriteDB.AddToByteArray( s_tagDefaultSetup.lCorrelativeSkillID);
}

//! 判断玩家能否进行组队操作
bool GSTeam::CanTeamOpt(CPlayer *pPlayer)
{
	return true;
	//!if(NULL == pPlayer) return false;
	//!return GSTeam::s_tagDefaultSetup.lMinSkillLevel <= pPlayer->GetSkillLv(GSTeam::s_tagDefaultSetup.lCorrelativeSkillID);
}

//! 初始化对象
bool GSTeam::Initial(CMessage *pMsg)
{
	if(MSG_W2S_TEAM_CREATE != pMsg->GetType())
	{
		return false;
	}

	pMsg->GetGUID(m_guid);
	pMsg->GetGUID(m_MestterGuid);
	m_eGoodsAllot	= pMsg->GetLong();

	long loopNum= pMsg->GetLong();
	tagMemberInfo tmp_tagMemberInfo;

	for (long i = 0; i < loopNum; ++i)
	{
		//! 复制成员
		pMsg->GetEx(&tmp_tagMemberInfo, sizeof(tagWSMemberInfo));
		tmp_tagMemberInfo.bChanged = false;
		m_MemberList.push_back(tmp_tagMemberInfo);

		CPlayer *pPlayer = GetGame()->FindPlayer(tmp_tagMemberInfo.guid);
		
		if (NULL != pPlayer)
		{
			++m_lOnThisGsMemberNum;
			pPlayer->SetTeamID(GetExID());
			pPlayer->SetCaptain(true);

			tagRecruitState &playerRecruitState = pPlayer->GetRecruitState();
			if (playerRecruitState.bIsRecruiting && tmp_tagMemberInfo.guid != m_MestterGuid)
			{
				pPlayer->SetRecruitState(false, NULL, NULL);
			}

			//! 记录日志
			GetLogicLogInterface()->LogT350_team_join(this, pPlayer);
		}

		OnMemberEnter(tmp_tagMemberInfo.guid);
	}

	CMessage msg(MSG_S2C_TEAM_TeamCreate);
	PushBackToMsg(&msg);
	SendToAllMember(&msg);

	CMessage msgGoodsSet(MSG_S2C_TEAM_GoodsSetChange);
	msgGoodsSet.Add(m_eGoodsAllot);
	SendToAllMember(&msgGoodsSet);

	RadiateAllMemberIdioinfo();
	UpdateLeaderRecruitedNum();
	return true;
}


void GSTeam::RadiateMemberData(list<tagMemberInfo>::iterator ite)
{
	tagMemberInfo MemberInfo = (*ite);
	for (list<tagMemberInfo>::iterator iteAim = m_MemberList.begin(); iteAim != m_MemberList.end(); ++iteAim)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(iteAim->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			SendMemberData(MemberInfo, iteAim);
		}
	}
}

void GSTeam::RadiateMemberData(const CGUID& PlayerGuid)
{
	for (list<tagMemberInfo>::iterator iteAim = m_MemberList.begin(); iteAim != m_MemberList.end(); ++iteAim)
	{
		if(PlayerGuid == iteAim->guid)
		{
			RadiateMemberData(iteAim);
		}
	}
}

void GSTeam::SendMemberData(tagMemberInfo &MemberInfo, list<tagMemberInfo>::iterator iteAim)
{
	CMessage msg(MSG_S2C_TEAM_MemberData);
	msg.AddEx(&MemberInfo, sizeof(tagWSMemberInfo));
	msg.SendToPlayer(iteAim->guid, false);
}


//! 将完整的队伍数据添加到消息末尾
void GSTeam::PushBackToMsg(CMessage *pMsg)
{
	pMsg->Add(m_guid);
	pMsg->Add(m_MestterGuid);
	pMsg->Add(m_eGoodsAllot);
	pMsg->Add((long)m_MemberList.size());
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		pMsg->AddEx(&(*ite), sizeof(tagWSMemberInfo));
	}
}

list<GSTeam::tagMemberInfo>::iterator GSTeam::FindMember(const CGUID& PlayerGuid)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		if(ite->guid == PlayerGuid)
		{
			return ite;
		}
	}
	return m_MemberList.end();
}

void GSTeam::SendToAllMember(CMessage *pMsg)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			pMsg->SendToPlayer(ite->guid);
		}
	}
}

void GSTeam::SendToAllRegionMember(CMessage *pMsg, long lRegionID)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer && lRegionID == pPlayer->GetRegionID()) //是本服务器玩家
		{
			pMsg->SendToPlayer(ite->guid);
		}
	}
}

void GSTeam::PlayerLeaveHere(const CGUID& PlayerGuid)
{
	m_lOnThisGsMemberNum = 0;

	CMessage msg(MSG_S2C_TEAM_MemberLeaveHere);
	msg.Add(PlayerGuid);
	SendToAllMember(&msg);

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			++m_lOnThisGsMemberNum;
			/*if(PlayerGuid == ite->guid)
			{
				RadiateMemberIdioinfo(ite);
			}*/
		}
	}
}

void GSTeam::MemberStateChange(const CGUID& PlayerGuid, long lState)
{
	CMessage msg(MSG_S2C_TEAM_MemberStateChange);
	msg.Add(PlayerGuid);
	msg.Add(lState);

	list<tagMemberInfo>::iterator iteSrc = FindMember(PlayerGuid);
	if (iteSrc == m_MemberList.end()) return;

	/*CMessage msg2(MSG_S2C_TEAM_MemberAllIdioinfo);
	if(NULL != pLoginPlayer)
	{
		tagMemberIdioinfo MemberIdioinfo;
		msg2.Add(pLoginPlayer->GetID());
		GetIdioinfo(pLoginPlayer, MemberIdioinfo);
		msg2.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
	}*/

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if (NULL != pPlayer)//是本服务器玩家
		{
			pPlayer->SetTeamID(GetExID());
			if(m_MemberList.end() != iteSrc && iteSrc != ite)
			{
				msg.SendToPlayer(ite->guid);
			}

			if (lState == eMS_ONLINE) //上线
			{
				if(PlayerGuid == ite->guid)
				{
					ite->lState = lState;
					//! 通知新上线者
					CMessage msg3(MSG_S2C_TEAM_TeamData);
					PushBackToMsg(&msg3);
					if (ite->dwJoinTime>m_dwAcceptTime)
					{
						msg.Add( (long)0 );
					}
					else
					{
						msg3.Add(m_lCurrShareQuestID);
					}
					
					msg3.SendToPlayer(PlayerGuid);

					SendAllMemberIdioinfo(ite);
					
					//IdioinfoMemberToMember(, );
					//SendToAllRegionMember(&msg2, pLoginPlayer->GetRegionID());
				}
				else
				{
					IdioinfoMemberToMember(iteSrc, ite);
				}
			}
		}
	}
}
//! 成员地图改变
void GSTeam::MemberMapChange(const CGUID& PlayerGuid)
{
	RadiateMemberIdioinfo(PlayerGuid);
	SendAllMemberIdioinfo(PlayerGuid);
	//list<tagMemberInfo>::iterator iteSrc = FindMember(PlayerGuid);
	//if (iteSrc != m_MemberList.end())
	//{
	//	SendAllMemberIdioinfo(iteSrc);
	//}
}

//! 更新队长的已招募人数
void GSTeam::UpdateLeaderRecruitedNum(void)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(m_MestterGuid);
	if (NULL != pPlayer)
	{
		CMessage msg(MSG_S2C_TEAM_RECRUITED_NUM);
		msg.Add(m_MestterGuid);
		long num = (0 < m_MemberList.size()) ? (long)m_MemberList.size() : 1;
		msg.Add(num);
		msg.SendToRegion(GetGame()->FindRegion(pPlayer->GetRegionGUID()));

#ifdef _RUNSTACKINFO3_
		char szTmp[256] = {0};
		_snprintf(szTmp, 256, "GSTeam::UpdateLeaderRecruitedNum ");
		pPlayer->GetRegionGUID().tostring(&szTmp[strlen(szTmp)]);
		m_MestterGuid.tostring(&szTmp[strlen(szTmp)]);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szTmp);
#endif
	}
	
}

void GSTeam::SendMemberBuffToMember(CPlayer *pPlayer, const CGUID &SendAimGuid)
{
	if (NULL != pPlayer)
	{
		CPlayer *pReceiver = GetGame()->FindPlayer(SendAimGuid);
		if (NULL != pReceiver)
		{
			if (pPlayer->GetRegionID() == pReceiver->GetRegionID())//! 在同一张地图
			{
				list<pair<DWORD, DWORD>>  listBuff;
				pPlayer->GetAllBuffID(listBuff);

				for (list<pair<DWORD, DWORD>>::iterator ite = listBuff.begin(); ite != listBuff.end(); ++ite)
				{
					CMessage teamMsg(MSG_S2C_TEAM_MembereExStateChange);
					teamMsg.Add(pPlayer->GetExID());
					teamMsg.Add((DWORD)0);
					teamMsg.Add((DWORD)ite->first);
					teamMsg.Add((DWORD)ite->second);

					teamMsg.SendToPlayer(SendAimGuid, false);
				}
			}
		}
	}
}

void GSTeam::SendMemberBuffToAll(CPlayer *pPlayer, DWORD dwBuffID, DWORD dwBuffLevel, BOOL bClose)
{
	if(NULL == pPlayer) return;

	CMessage teamMsg(MSG_S2C_TEAM_MembereExStateChange);
	teamMsg.Add(pPlayer->GetExID());
	teamMsg.Add((DWORD)bClose);
	teamMsg.Add(dwBuffID);
	teamMsg.Add(dwBuffLevel);

	for (list<tagMemberInfo>::iterator iteReceiver = m_MemberList.begin(); iteReceiver != m_MemberList.end(); ++iteReceiver)
	{
		if (iteReceiver->guid != pPlayer->GetExID())
		{
			CPlayer *pReceiver = GetGame()->FindPlayer(iteReceiver->guid);
			if (NULL != pReceiver)
			{
				if (pPlayer->GetRegionID() == pReceiver->GetRegionID())//! 在同一张地图
				{
					teamMsg.SendToPlayer(iteReceiver->guid);
				}
			}
		}	
	}
}


void GSTeam::TeamChat(CPlayer *pSender, char *pInfo)
{
	assert(NULL != pSender);

	CMessage msg(MSG_S2C_TEAM_CHAT);
	msg.Add(pSender->GetExID());
	msg.Add(pInfo);

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			msg.SendToPlayer(pPlayer->GetExID());	
		}
		else
		{
			CMessage msgToWS(MSG_S2W_TEAM_CHAT);
			msgToWS.Add(ite->guid);
			msgToWS.Add(pSender->GetExID());
			msgToWS.Add(pInfo);
			msgToWS.Send();
		}
	}
}

const CGUID& GSTeam::GetLeader(void)
{
	return m_MestterGuid;
}

long GSTeam::GetTeamatesAmount(void)
{
	return (long)m_MemberList.size();
}

void GSTeam::RadiateInfo(char* szInfo)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			pPlayer->SendNotifyMessage(szInfo);
		}
	}
}

void GSTeam::SetGoodsAllot(long eSet)
{
	m_eGoodsAllot = eSet;
	CMessage msg(MSG_S2C_TEAM_GoodsSetChange);
	msg.Add(m_eGoodsAllot);
	SendToAllMember(&msg);
}

//const CGUID& GSTeam::GetNextGoodsOwner(void)
//{
//	if(m_MemberList.end() == m_iteNextGainer)
//	{
//		m_iteNextGainer = m_MemberList.begin();
//	}
//
//	return (m_iteNextGainer++)->guid;
//}

//! 根据配置的分配方式，返回一个在拾取者服务器、拾取者地图的玩家指针，
//! 参数为拾取者ID
CPlayer* GSTeam::GetOneOwner(const CGUID& Picker)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(Picker);
	if (NULL == pPlayer)
	{
		return NULL;
	}

	const CGUID &RegionGUID = pPlayer->GetRegionGUID();
	LONG lPosX = (long)pPlayer->GetPosX();
	LONG lPosY = (long)pPlayer->GetPosY();

	switch(m_eGoodsAllot)
	{
	case eGA_FREE://任意拾取
		return pPlayer;
	case eGA_QUEUE://轮流拾取
		{
			LONG lSeed = 100; //! 用于统计可以获得物品者的排序
			map<LONG, CPlayer*> setGeter;

			//! 统计出所有可获得者，按上一次的获得者开始排序
			for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
			{
				if(m_LastAllotAim == ite->guid)//! 从找到上一次获得者开始，获得者后面玩家顺位提前
					lSeed = 0; 

				CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
				if(NULL != pPlayer)
				{
					if (RegionGUID == pPlayer->GetRegionGUID() && (!pPlayer->IsDied()) &&
						(MAX_LUCRATIVE_BOUND >= abs(pPlayer->GetPosX() - lPosX) && MAX_LUCRATIVE_BOUND >= abs(pPlayer->GetPosY() - lPosY))
						)
					{	
						setGeter[lSeed] = pPlayer;
						++lSeed;
					}
				}
			}

			//! 找出获得者
			CPlayer* pAimPlayer = NULL;
			for (map<LONG, CPlayer*>::iterator ite = setGeter.begin(); setGeter.end() != ite; ++ite)
			{
				pAimPlayer = ite->second;	
				if(m_LastAllotAim != pAimPlayer->GetExID())
					break;
			}

			if(NULL != pAimPlayer)
				m_LastAllotAim = pAimPlayer->GetExID();
			return pAimPlayer;
		}
		break;
	case eGA_RANDOM://随机拾取
		list<CGUID> TmpAllotList;
		GetLucrativeArea_Alive(TmpAllotList, RegionGUID, lPosX, lPosY, TRUE);
		long lAim = rand() % ((int)TmpAllotList.size());

		list<CGUID>::iterator ite = TmpAllotList.begin();
		long i = 0;
		for (; ite != TmpAllotList.end(); ++ite, ++i)
		{
			if (i >= lAim)
			{
				CPlayer *pAimPlayer = GetGame()->FindPlayer(*ite);
				if (NULL != pAimPlayer)
				{
					if (RegionGUID == pAimPlayer->GetRegionGUID() && (!pPlayer->IsDied()) &&
						(MAX_LUCRATIVE_BOUND >= abs(pAimPlayer->GetPosX() - lPosX) && MAX_LUCRATIVE_BOUND >= abs(pAimPlayer->GetPosY() - lPosY))
						)
					{
						return pAimPlayer; //! 找到了获得者
					}
				}
			}
		}
	    break;
	    
	}
	
	return NULL;
}

//! 得到可以分配者的数量
long GSTeam::GetOneOwnerNum(void)
{
	list<CGUID> AllotList;
	GetOnServerTeamMember(AllotList);

	return (long)AllotList.size();
}


void	GSTeam::GetOnServerTeamMember(list<CGUID> &relist)
{
	relist.clear();
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			relist.push_back(ite->guid);
		}
	}
}

//void GSTeam::OnTeamChange(eTeamChange eTC, long lValue)
//{
//	switch(eTC)
//	{
//	case eTC_NewMaster:
//		m_lMestterID = lValue;
//		break;
//	case eTC_NewGoodsAllot:
//		m_eGoodsAllot = lValue;
//		break;
//	case eTC_PlayerLogin:
//		{
//			list<tagMemberInfo>::iterator ite = FindMember(lValue);
//			ite->lState = eMS_ONLINE;
//		}
//	    break;
//	case eTC_PlayerLeave:
//		{
//			list<tagMemberInfo>::iterator ite = FindMember(lValue);
//			ite->lState = eMS_LEAVE;
//		}
//	    break;
//	default:
//	    return;
//	}
//
//	CMessage msg(MSG_S2C_TEAM_TeamDataChange);
//	msg.Add((long)eTC);
//	msg.Add(lValue);
//	SendToAllMember(&msg);
//}

void GSTeam::MarkPlayerPosChange(const CGUID& PlayerGuid)
{
	list<tagMemberInfo>::iterator ite = FindMember(PlayerGuid);
	if(m_MemberList.end() != ite)
		ite->bChanged = true;
}

void GSTeam::RadiateAllMemberPos(void)
{
	//! 在地图上维护队员位置
	map<CGUID, set<CPlayer*>> mapInSameMapMember;

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if(mapInSameMapMember.end() == mapInSameMapMember.find(pPlayer->GetRegionGUID()))
				mapInSameMapMember.insert(make_pair(pPlayer->GetRegionGUID(), set<CPlayer*>()));
			mapInSameMapMember[pPlayer->GetRegionGUID()].insert(pPlayer);
		}
	}

	for (map<CGUID, set<CPlayer*>>::iterator ite = mapInSameMapMember.begin(); mapInSameMapMember.end() != ite; ++ite)
	{
		if(1 < ite->second.size())
		{
			vector<BYTE> vData;

			_AddToByteArray(&vData, (LONG)ite->second.size());
			for (set<CPlayer*>::iterator iteMem = ite->second.begin(); ite->second.end() != iteMem; ++iteMem)
			{
				_AddToByteArray(&vData, (*iteMem)->GetExID());
				_AddToByteArray(&vData, (*iteMem)->GetTileX());
				_AddToByteArray(&vData, (*iteMem)->GetTileY());
			}

			CMessage msg(MSG_S2C_TEAM_MemberPos);
			msg.Add((LONG)vData.size());
			msg.AddEx(&vData[0],(long)vData.size());

			for (set<CPlayer*>::iterator iteMem = ite->second.begin(); ite->second.end() != iteMem; ++iteMem)
				msg.SendToPlayer((*iteMem)->GetExID());
		}
	}

	/*
	在全服维护队员的位置
	/////////////////////////////////////////////////
	vector<CPlayer*> vPlayer;

	set<CGUID> setOtherMember;

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
	CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
	if(NULL != pPlayer) //是本服务器玩家
	{
	vPlayer.push_back(pPlayer);
	}
	else
	{
	setOtherMember.insert(ite->guid);
	}
	}

	vector<BYTE> vData;

	_AddToByteArray(&vData, (LONG)vPlayer.size());
	for (LONG i = 0; i < vPlayer.size(); ++i)
	{
	_AddToByteArray(&vData, vPlayer[i]->GetExID());
	_AddToByteArray(&vData, vPlayer[i]->GetTileX());
	_AddToByteArray(&vData, vPlayer[i]->GetTileY());
	}

	CMessage msg(MSG_S2C_TEAM_MemberPos);
	msg.Add((LONG)vData.size());
	if(0 != vData.size())
	msg.AddEx(&vData[0], vData.size());
	SendToAllMember(&msg);

	//! 有成员在其他服务器，则要求WS转发消息
	if (0 != setOtherMember.size())
	{
	CMessage msgToWS(MSG_S2W_TEAM_MemberPos);
	msgToWS.Add((LONG)vData.size());
	if(0 != vData.size())
	msgToWS.AddEx(&vData[0], vData.size());

	msgToWS.Add((LONG)setOtherMember.size());
	for (set<CGUID>::iterator ite = setOtherMember.begin(); ite != setOtherMember.end(); ++ite)
	{
	msgToWS.Add(*ite);
	}
	msgToWS.Send(false);
	}
	*/
}

//void GSTeam::MarkPlayerIdioinfoChange(const CGUID& PlayerGuid)
//{
//	FindMember(PlayerGuid)->bChanged = true;
//}

//! 响应玩家非队伍状态改变
void  GSTeam::OnPlayerIdioinfoChange(const CGUID& PlayerGuid, eUpdateIdioinfo eInfo)
{
	if(NULL_GUID == IsMember(PlayerGuid))
	{
		return;
	}

	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);

	if (NULL == pPlayer)
	{
		return;
	}

	CMessage msg(MSG_S2C_TEAM_UpdateIdioinfo);
	msg.Add(PlayerGuid);
	msg.Add((long)eInfo);

	switch(eInfo)
	{
	case eMIS_Pos:
		msg.Add(pPlayer->GetTileX());
		msg.Add(pPlayer->GetTileY());
		break;
	case eMIS_CurrHp:
		msg.Add((long)pPlayer->GetHP());
		break;
	case eMIS_CurrMp:
		msg.Add((long)pPlayer->GetMP());
		break;
	case eMIS_CurrEnergy:
		msg.Add((long)pPlayer->GetPKCP());
		break;
	case eMIS_MaxHp:
		msg.Add((long)pPlayer->GetMaxHP());
		break;
	case eMIS_MaxMp:
		msg.Add((long)pPlayer->GetMaxMP());
		break;
	case eMIS_MaxEnergy:
		msg.Add((long)pPlayer->GetMaxPKCP());
		break;
	case eMIS_Level:
		msg.Add((long)pPlayer->GetLevel());
		break;
	case eMIS_Occupation:
		msg.Add((long)pPlayer->GetOccupation());
		break;
	case eMIS_Sex:
		msg.Add((long)pPlayer->GetSex());
		break;
	}

	SendToAllRegionMember(&msg, pPlayer->GetRegionID());
}


void GSTeam::OnPlayerIdioinfoChange(const CGUID& PlayerGuid, eUpdateIdioinfo eInfo, long lValue)
{
	if(NULL_GUID == IsMember(PlayerGuid))
	{
		return;
	}

	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);

	if (NULL == pPlayer)
	{
		return;
	}

	CMessage msg(MSG_S2C_TEAM_UpdateIdioinfo);
	msg.Add(PlayerGuid);
	msg.Add((long)eInfo);

	switch(eInfo)
	{
	case eMIS_Pos:
		{
			list<tagMemberInfo>::iterator ite = FindMember(PlayerGuid);
			if(m_MemberList.end() != ite)
			ite->bChanged = true;
		}
		return;
	case eMIS_CurrHp:
	case eMIS_CurrMp:
	case eMIS_CurrEnergy:
	case eMIS_Level:
	case eMIS_Occupation:
	case eMIS_Sex:
		msg.Add(lValue);
		break;
	case eMIS_MaxHp:
		msg.Add(pPlayer->GetAttribute(string("C_MaxHp")));
		break;
	case eMIS_MaxMp:
		msg.Add(pPlayer->GetAttribute(string("C_MaxMp")));
		break;
	case eMIS_MaxEnergy:
		msg.Add(pPlayer->GetAttribute(string("MaxPKCP")));
		break;
	default:
		return;
	}

	SendToAllRegionMember(&msg, pPlayer->GetRegionID());
}

//! 处理要加入组织的人员
bool GSTeam::DoJoin(CMessage *pMsg)
{
	tagMemberInfo MemberInfo;
	pMsg->GetEx(&MemberInfo, sizeof(tagWSMemberInfo));
	MemberInfo.bChanged = false;
	m_MemberList.push_back(MemberInfo);

	list<tagMemberInfo>::iterator ite = --(m_MemberList.end());

	CPlayer *pPlayer = GetGame()->FindPlayer(MemberInfo.guid);
	if (NULL != pPlayer)
	{
		++m_lOnThisGsMemberNum;
		pPlayer->SetTeamID(GetExID());
		pPlayer->SetCaptain(true);
		tagRecruitState &playerRecruitState = pPlayer->GetRecruitState();
		if (playerRecruitState.bIsRecruiting)
		{
			pPlayer->SetRecruitState(false, NULL, NULL);
		}

		//! 记录日志
		GetLogicLogInterface()->LogT350_team_join(this, pPlayer);
	}

	OnMemberEnter(MemberInfo.guid);

	//! 通知新加入者
	CMessage msg(MSG_S2C_TEAM_TeamData);
	PushBackToMsg(&msg);
	msg.SendToPlayer(MemberInfo.guid);

	CMessage msgGoodsSet(MSG_S2C_TEAM_GoodsSetChange);
	msgGoodsSet.Add(m_eGoodsAllot);
	msgGoodsSet.SendToPlayer(MemberInfo.guid);

	RadiateMemberData(ite);

	SendAllMemberIdioinfo(ite);

	//! 通知所有成员
	RadiateMemberIdioinfo(ite);

	UpdateLeaderRecruitedNum();

	
	return true;
}

void GSTeam::RadiateAllMemberIdioinfo(void)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		RadiateMemberIdioinfo(ite);
	}
}

void GSTeam::RadiateMemberIdioinfo(const CGUID& PlayerGuid)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		if(ite->guid == PlayerGuid)
		{
			RadiateMemberIdioinfo(ite);
			return;
		}
	}
}

void GSTeam::RadiateMemberIdioinfo(list<tagMemberInfo>::iterator ite)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);

	CMessage msg(MSG_S2C_TEAM_MemberAllIdioinfo);
	msg.Add(ite->guid);
	msg.Add(1L);
	tagMemberIdioinfo MemberIdioinfo;
	if(!GetIdioinfo(pPlayer, MemberIdioinfo))
	{
		return;
	}
	msg.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));

	for (list<tagMemberInfo>::iterator iteReceiver = m_MemberList.begin(); iteReceiver != m_MemberList.end(); ++iteReceiver)
	{
		if (iteReceiver->guid != ite->guid)
		{
			CPlayer *pReceiver = GetGame()->FindPlayer(iteReceiver->guid);
			if(NULL != pPlayer) //被发送者是本服务器玩家
			{
				if (NULL == pReceiver)//接收者不在本服
				{
					CMessage msgToWS(MSG_S2W_TEAM_MemberAllIdioinfo);
					msgToWS.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
					msgToWS.Add(ite->guid);
					msgToWS.Add(iteReceiver->guid);
					msgToWS.Send();
				}
				else
				{
					SendMemberBuffToMember(pPlayer, iteReceiver->guid);
					msg.SendToPlayer(iteReceiver->guid);
				}	
			}
		}	
	}
}

bool GSTeam::GetIdioinfo(CPlayer *pPlayer, tagMemberIdioinfo &MemberIdioinfo)
{
	if (NULL == pPlayer)
	{
		return false;
	}
	MemberIdioinfo.fPosX		= (float)pPlayer->GetTileX();
	MemberIdioinfo.fPosY		= (float)pPlayer->GetTileY();
	MemberIdioinfo.lRegionID	= pPlayer->GetRegionID();
	MemberIdioinfo.RegionGuid	= pPlayer->GetRegionGUID();

	MemberIdioinfo.lCurrHp		= pPlayer->GetAttribute(string("Hp"));
	MemberIdioinfo.lCurrMp		= pPlayer->GetAttribute(string("Mp"));
	MemberIdioinfo.lCurrEnergy	= pPlayer->GetPKCP();

	MemberIdioinfo.lMaxHp		= pPlayer->GetAttribute(string("C_MaxHp"));
	MemberIdioinfo.lMaxMp		= pPlayer->GetAttribute(string("C_MaxMp"));
	MemberIdioinfo.lMaxEnergy	= pPlayer->GetBaseMaxPKCP();

	MemberIdioinfo.lLevel		= pPlayer->GetAttribute(string("Level"));
	MemberIdioinfo.lOccupation	= pPlayer->GetAttribute(string("Occu"));

	return true;
}

void GSTeam::IdioinfoMemberToMember(list<tagMemberInfo>::iterator iteSrc, list<tagMemberInfo>::iterator iteAim)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(iteSrc->guid);
	CPlayer *pPlayerAim = GetGame()->FindPlayer(iteAim->guid);

	if(NULL == pPlayerAim) return;
	
	tagMemberIdioinfo MemberIdioinfo;
	if(!GetIdioinfo(pPlayer, MemberIdioinfo))
	{
		return;
	}

	if(NULL == pPlayer) //不是本服务器玩家
	{
		CMessage msgToWS(MSG_S2W_TEAM_MemberAllIdioinfo);
		msgToWS.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
		msgToWS.Add(iteSrc->guid);
		msgToWS.Add(iteAim->guid);
		msgToWS.Send(false);
		return;
	}

	CMessage msg(MSG_S2C_TEAM_MemberAllIdioinfo);
	msg.Add(iteSrc->guid);
	msg.Add(1L);
	msg.AddEx(&MemberIdioinfo, sizeof(tagMemberIdioinfo));
	msg.SendToPlayer(iteAim->guid);

	

	SendMemberBuffToMember(pPlayer, iteAim->guid);
}

void GSTeam::SendAllMemberIdioinfo(list<tagMemberInfo>::iterator ite)
{
	for (list<tagMemberInfo>::iterator iteSrc = m_MemberList.begin(); iteSrc != m_MemberList.end(); ++iteSrc)
	{
		if(iteSrc != ite)
			IdioinfoMemberToMember(iteSrc, ite);
	}
}

void GSTeam::SendAllMemberIdioinfo(const CGUID& PlayerGuid)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		if(ite->guid == PlayerGuid)
		{
			SendAllMemberIdioinfo(ite);
			return;
		}
	}
}

void GSTeam::SendAllMemberLeaderChangeRgn(const CGUID& Leader,
                                          long lRegionID,
                                          long  x, long y)
{
    CMessage  Msg(MSG_S2C_FOLLOW_Leader);
    Msg.Add( (long) TF_Team_Member_ChangeRgn );
    Msg.Add( (long) TF_Result_Follow_Success );
    Msg.Add( lRegionID );
    Msg.Add( x );
    Msg.Add( y );
     hash_set<CGUID,hash_guid_compare> PlayerID;PlayerID.clear();
    for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
    {
        if(ite->guid != Leader)
        {
            PlayerID.insert( ite->guid );
        }
    }
    Msg.SendToPlayers(PlayerID);
}

//! 退出
bool GSTeam::Exit(const CGUID& PlayerGuid)
{
#ifdef _RUNSTACKINFO3_
	char szTmp[256] = {0};
	_snprintf(szTmp, 256, "GSTeam::Exit 1");
	PlayerGuid.tostring(&szTmp[strlen(szTmp)]);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szTmp);
#endif
	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
	if(NULL != pPlayer) //是本服务器玩家
	{
		--m_lOnThisGsMemberNum;
		pPlayer->SetTeamID(NULL_GUID);
		pPlayer->SetCaptain(false);

		//! 记录日志
		GetLogicLogInterface()->LogT360_team_leave(this, pPlayer);
#ifdef _RUNSTACKINFO3_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GSTeam::Exit 2");
#endif
	}

#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GSTeam::Exit 3");
#endif

	OnMemberExit(PlayerGuid);


	CMessage msg(MSG_S2C_TEAM_MemberLeave);
	msg.Add(PlayerGuid);
	SendToAllMember(&msg);

	list<tagMemberInfo>::iterator ite = FindMember(PlayerGuid);
	if(m_MemberList.end() != ite)
		m_MemberList.erase(ite);

#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GSTeam::Exit 4");
#endif

	UpdateLeaderRecruitedNum();
	return true;
}


void GSTeam::OnMemberEnter(const CGUID& MemGuid)
{
	CPlayer* pMember = GetGame()->FindPlayer(MemGuid);
	if(pMember)
	{
		if(MemGuid != GetMasterID())
		{
			CServerRegion* pRgn = dynamic_cast<CServerRegion*>( pMember->GetFather() );
			if(pRgn)
			{
				// 判断自己的副本RgnID,是否清人
				if( pRgn->GetRgnType() == RGN_TEAM
					&& (pRgn->GetExID() == GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGUID(GetMasterID(), pRgn->GetID())) )
					pMember->CancelRgnKickPlayerTimer();
			}

			vector<BYTE> vData;
			GameManager::GetInstance()->GetRgnManager()->RgnGuidCode(MemGuid, vData);
			LONG lPos = 0;
			if(vData.size())
				GameManager::GetInstance()->GetRgnManager()->RgnGuidSwapFromArray(GetMasterID(), &vData[0], lPos);
			GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(MemGuid);
		}
	}
}

//! 响应队员退出
void GSTeam::OnMemberExit(const CGUID& MemGuid)
{
	CPlayer* pMember = GetGame()->FindPlayer(MemGuid);
	if(pMember)
	{
		// 判断自己的副本RgnID,是否清人
		CServerRegion* pRgn = dynamic_cast<CServerRegion*>( pMember->GetFather() );
		if(pRgn)
		{
			if( pRgn->GetRgnType() == RGN_TEAM )
			{
				if( MemGuid != GetMasterID() ) 
				{
					pMember->RegisterRgnKickPlayerTimerID(GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
					CMessage msg(MSG_S2C_OTHER_TIME_INFO_BOX);
					msg.Add((BYTE)RGN_CLEARDUPRGNPLAYER);
					msg.Add((long)GlobalSetup::GetSetup()->lRgnKickPlayerDeltaTime);
					msg.SendToPlayer(GetExID());
				}
			}
		}
	}
}


//! 传位
bool GSTeam::Demise(const CGUID& MasterGuid,const CGUID& guid)
{
	//! 交换副本
	vector<BYTE> vData;
	GameManager::GetInstance()->GetRgnManager()->RgnGuidCode(GetMasterID(), vData);
	LONG lPos = 0;
	if(vData.size())
		GameManager::GetInstance()->GetRgnManager()->RgnGuidSwapFromArray(guid, &vData[0], lPos);
	GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(GetMasterID());

	m_MestterGuid = guid;
	CMessage msg(MSG_S2C_TEAM_MastterChange);
	msg.Add(guid);
	SendToAllMember(&msg);
	return true;
}

//! 解散
bool GSTeam::Disband(const CGUID& guid, BOOL bNotifyClient)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			--m_lOnThisGsMemberNum;
			pPlayer->SetTeamID(NULL_GUID);
			pPlayer->SetCaptain(false);
			//! 记录日志
			GetLogicLogInterface()->LogT360_team_leave(this, pPlayer);
		}

		OnMemberExit(ite->guid);

		if(bNotifyClient)
		{
			CMessage msg(MSG_S2C_TEAM_MemberLeave);
			msg.Add(ite->guid);
			SendToAllMember(&msg);
		}
	}

	m_MemberList.clear();

	//UpdateLeaderRecruitedNum();
	return true;
}

////! 得到组织的ID
//long GSTeam::GetID()
//{
//	return m_lID;
//}

//得到成员列表
void GSTeam::GetMemberList(list<CGUID>& TemptList)
{
	TemptList.clear();
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		TemptList.push_back(ite->guid);
	}
}



//该玩家是否是该帮成员否
const CGUID& GSTeam::IsMember(const CGUID& guid)
{
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		if(ite->guid == guid) return guid;
	}
	return NULL_GUID;
}

bool GSTeam::IsMember( const std::string &name ) const
{
	for( list<tagMemberInfo>::const_iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++ it )
	{
		if( strcmp( it->szName, name.c_str() ) == 0 )
		{
			return true;
		}
	}
	return false;
}

//得到成员数
long GSTeam::GetMemberNum()
{
	return (long)m_MemberList.size();
}

long GSTeam::GetMemberNumInRgn(const CGUID& RgnGuid)
{
	long lReNum = 0;
	for( list<tagMemberInfo>::const_iterator it = m_MemberList.begin(); it != m_MemberList.end(); ++ it )
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(it->guid);
		if(NULL != pPlayer && RgnGuid == pPlayer->GetRegionGUID())
			++lReNum;
	}

	return lReNum;
}

//得到头领
long GSTeam::GetPlayerHeader()
{
	assert(false);return 0;
}

//更新玩家的信息到组织的成员列表
bool GSTeam::UpdatePlayerDataToOrgMemberList(const CGUID& PlayerGuid)
{
	assert(false);return false;
}

//出发一个成员进入游戏
void GSTeam::OnMemberEnterGame(const CGUID& MemGuid)
{
	assert(false);
}

//触发一个成员离开游戏
void GSTeam::OnMemberExitGame(const CGUID& MemGuid)
{
	assert(false);
}

//成员等级发生了变化
void GSTeam::OnMemberLvlChange(const CGUID& MemGuid,long lLvl)
{
	assert(false);
}

//成员改变了位置(场景)
void GSTeam::OnMemberPosChange(const CGUID& MemGuid,long lNewPosID)
{
	assert(false);
}


////得到队伍里边在当前场景的活着的成员
//void GSTeam::GetCurrentRegionTeamates_Alive(map<CGUID,CPlayer*>& RegionTeamates,LONG lRegionID)
//{
//	if(lRegionID <= 0)	return;
//	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
//	{
//		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
//		if(NULL != pPlayer) //是本服务器玩家
//		{
//			if((!pPlayer->IsDied()) && lRegionID == pPlayer->GetRegionID())
//			{
//				RegionTeamates[ite->guid] = pPlayer;
//			}
//		}
//	}
//}

//!	 得到一个受益区域内的活着的成员
void GSTeam::GetLucrativeArea_Alive(map<CGUID,CPlayer*>& RegionTeamates, const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods)
{
	if(NULL_GUID == RegionGUID)	return;

	LONG lBound = (bForGoods) ? MAX_LUCRATIVE_BOUND : GlobalSetup::GetSetup()->new_lExperienceRadii;
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if((!pPlayer->IsDied()) && RegionGUID == pPlayer->GetRegionGUID())
			{
				if(lBound >= abs(pPlayer->GetPosX() - lPosX) && lBound >= abs(pPlayer->GetPosY() - lPosY))
					RegionTeamates[ite->guid] = pPlayer;
			}
		}
	}
}
void GSTeam::GetLucrativeArea_Alive(list<CGUID>& listMember, const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods)
{
	if(NULL_GUID == RegionGUID)	return;
	
	LONG lBound = (bForGoods) ? MAX_LUCRATIVE_BOUND : GlobalSetup::GetSetup()->new_lExperienceRadii;

	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if((!pPlayer->IsDied()) && RegionGUID == pPlayer->GetRegionGUID())
			{
				if(lBound >= abs(pPlayer->GetPosX() - lPosX) && lBound >= abs(pPlayer->GetPosY() - lPosY))
					listMember.push_back(pPlayer->GetExID());
			}
		}
	}
}
//long GSTeam::GetCurrentRegionTeamatesAverageLevel_Alive(long lRegionID)
//{
//	if(lRegionID <= 0)	return 0;
//
//	map<CGUID,CPlayer*> RegionTeamates;
//	RegionTeamates.clear();
//
//	GetCurrentRegionTeamates_Alive(RegionTeamates, lRegionID);
//	
//	if (0 == (long)RegionTeamates.size())
//	{
//		return 0;
//	}
//
//	long lLevelSum = 0;
//	for (map<CGUID,CPlayer*>::iterator ite = RegionTeamates.begin(); ite != RegionTeamates.end(); ++ite)
//	{
//		lLevelSum += ite->second->GetLevel();
//	}
//
//	return lLevelSum/(long)RegionTeamates.size();
//}

//! 得到一个受益区域内的活着的成员的平均等级
long GSTeam::GetLucrativeAreaAverageLevel_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods)
{
	if(NULL_GUID == RegionGUID)	return 0;

	map<CGUID,CPlayer*> RegionTeamates;
	RegionTeamates.clear();

	GetLucrativeArea_Alive(RegionTeamates, RegionGUID, lPosX, lPosY, bForGoods);

	if (0 == (long)RegionTeamates.size())
	{
		return 0;
	}

	long lLevelSum = 0;
	for (map<CGUID,CPlayer*>::iterator ite = RegionTeamates.begin(); ite != RegionTeamates.end(); ++ite)
	{
		lLevelSum += ite->second->GetLevel();
	}

	return lLevelSum/(long)RegionTeamates.size();
}


//! 得到一个受益区域内的活着的成员的平均职业等级
long GSTeam::GetLucrativeAreaAverageOccuLevelCoe_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods)
{
	if(NULL_GUID == RegionGUID)	return 0;

	map<CGUID,CPlayer*> RegionTeamates;
	RegionTeamates.clear();

	GetLucrativeArea_Alive(RegionTeamates, RegionGUID, lPosX, lPosY, bForGoods);

	if (0 == (long)RegionTeamates.size())
	{
		return 0;
	}

	long lOccuLevelSum = 0;
	for (map<CGUID,CPlayer*>::iterator ite = RegionTeamates.begin(); ite != RegionTeamates.end(); ++ite)
	{
		CPlayer* pPlayer = ite->second;
		if(pPlayer)
		{
			eOccupation eOccu = (eOccupation)pPlayer->GetOccupation();
			lOccuLevelSum += (long)PlayerList::GetOccuLvlCoe(eOccu,pPlayer->GetOccuLvl(eOccu));
		}
	}

	return lOccuLevelSum/(long)RegionTeamates.size();
}


long GSTeam::GetCurrentRegionTeamatesAmount(const CGUID &RegionGUID)
{
	if(NULL_GUID == RegionGUID)	return 0;
	long lPlayerNum = 0;
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if(RegionGUID == pPlayer->GetRegionGUID())
			{
				++lPlayerNum;
			}
		}
	}
	return lPlayerNum;
}

//! 得到一个受益区域内的活着的成员数量
long GSTeam::GetLucrativeAreaMemberAmount_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods)
{
	if(NULL_GUID == RegionGUID)	return 0;
	long lPlayerNum = 0;
	LONG lBound = (bForGoods) ? MAX_LUCRATIVE_BOUND : GlobalSetup::GetSetup()->new_lExperienceRadii;
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if((!pPlayer->IsDied()) && RegionGUID == pPlayer->GetRegionGUID())
			{
				if(lBound >= abs(pPlayer->GetPosX() - lPosX) && lBound >= abs(pPlayer->GetPosY() - lPosY))
					++lPlayerNum;
			}
		}
	}
	return lPlayerNum;
}

long GSTeam::GetCurrentServerTeamatesAmount()
{
	long lPlayerNum = 0;
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			++lPlayerNum;
		}
	}
	return lPlayerNum;
}
////! 得到这个区域内的一个队伍成员列表
//void GSTeam::FindTeamatesListInCurrentRegion(const CGUID& RegionGUID, list<CPlayer*> &PlayerList)
//{
//	PlayerList.clear();
//	if(NULL_GUID == RegionGUID)	return;
//	
//	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
//	{
//		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
//		if(NULL != pPlayer) //是本服务器玩家
//		{
//			if((!pPlayer->IsDied()) && RegionGUID == pPlayer->GetRegionGUID())
//				PlayerList.push_back(pPlayer);
//		}
//	}
//}
CPlayer* GSTeam::FindTeamatesInCurrentRegion(const CGUID& RegionGUID)
{
	if(NULL_GUID == RegionGUID)	return NULL;
	for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
		if(NULL != pPlayer) //是本服务器玩家
		{
			if((!pPlayer->IsDied()) && RegionGUID == pPlayer->GetRegionGUID())
				return pPlayer;
		}
	}
	return NULL;
}

//! 计算经验
long GSTeam::CalculateExperience(CPlayer *pPlayer, long lMonsterLevel, DWORD dwExp, LONG lPosX, LONG lPosY)
{
	if (NULL == pPlayer) return 0;
	//##计算9个Area范围内队伍成员的平均等级及个数
	FLOAT fAverageLevel = (FLOAT)GetLucrativeAreaAverageLevel_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);
	DWORD dwNumMembers	= GetLucrativeAreaMemberAmount_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);

	float fExp = (float)dwExp;
	//##当前区域内有队伍成员
	if( dwNumMembers > 1 )
	{
		switch(dwNumMembers)
		{
		case 2:
			fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_2;	//2人经验加成
			break;
		case 3:
			fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_3;	//3人经验加成
			break;
		case 4:
			fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_4;	//4人经验加成
			break;
		case 5:
			fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_5;	//5人经验加成
			break;
		case 6:
			fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_6;	//6人经验加成
			break;
		default:
			{
				if(6 < dwNumMembers)
					fExp *= GlobalSetup::GetSetup()->new_fIncreasedExp_6;
			}
			break;
		}

		//基本经验
		float parm1 = (float)(lMonsterLevel - pPlayer->GetLevel());
		if(parm1 >= GlobalSetup::GetSetup()->new_fExpMinLvlDiff) //怪物等级高5级时
			fExp = fExp * (1 + (lMonsterLevel - pPlayer->GetLevel() - GlobalSetup::GetSetup()->new_fExpMinLvlDiff)  * GlobalSetup::GetSetup()->new_fExpHortation / 100) ;
		else if((parm1 < GlobalSetup::GetSetup()->new_fExpMinLvlDiff) && (parm1 >= 0))  //怪物等级高0-4级时
			fExp = fExp;
		else if(parm1 < 0)  //玩家等级高于怪物时
			fExp = fExp * (1 - (pPlayer->GetLevel() - lMonsterLevel)  * GlobalSetup::GetSetup()->new_fExpAmerce / 100) ;

		if(1.0f >= fExp) fExp = 1.0f;

		//A队员经验值获取份额
		float fQuota = pPlayer->GetLevel() / ((float)dwNumMembers * fAverageLevel) + (pPlayer->GetLevel() - fAverageLevel) * GlobalSetup::GetSetup()->new_lExpDiff / 100;

		//份额低保
		fQuota = (fQuota < GlobalSetup::GetSetup() ->new_lExpLimit / 100.0f) ? GlobalSetup::GetSetup() -> new_lExpLimit / 100.0f : fQuota; 
		//份额封顶
		fQuota = (fQuota > 1 - GlobalSetup::GetSetup() -> new_lExpLimit / 100.0f) ? 1 - GlobalSetup::GetSetup() -> new_lExpLimit / 100.0f: fQuota;

		//##返回最终的经验值
		float fReValue = fQuota * fExp + 0.9999999f;//! 只要有小数尾数，则进1
		return (0 >= (LONG)fReValue) ? 1 : (LONG)fReValue; //! 永不返回0
	}
	else if(1 == dwNumMembers)
	{
		float parm1 = (float)(pPlayer->GetLevel() - lMonsterLevel);
		if(0 > parm1)
			parm1 = (GlobalSetup::GetSetup()->new_fExpMinLvlDiff < -parm1) ? (parm1 + GlobalSetup::GetSetup()->new_fExpMinLvlDiff) : 0;
		float fCoefficient = (0 > parm1) ? GlobalSetup::GetSetup()->new_fExpHortation : GlobalSetup::GetSetup()->new_fExpAmerce;
		fExp *= (float)(1.0f - parm1 * fCoefficient / 100.0f);
		fExp += 0.999999f;
		return (0 >= (LONG)fExp) ? 1 : (LONG)fExp;//! 永不返回0
	}
	else
	{
		assert(false);
		return 0;//! 发生了错误
	}
}


//! 计算职业经验
long GSTeam::CalculateOccuExperience(CPlayer *pPlayer, long lMonsterLevel, DWORD lExp, LONG lPosX, LONG lPosY)
{
	if (NULL == pPlayer) return 0;
	//##计算9个Area范围内队伍成员的平均等级及个数
	FLOAT fAverageOccuLevelCoe = (float)GetLucrativeAreaAverageOccuLevelCoe_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);
	DWORD dwNumMembers	= GetLucrativeAreaMemberAmount_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);

	float fExp = (float)lExp;
	//##当前区域内有队伍成员
	if( dwNumMembers > 1 )
	{
		switch(dwNumMembers)
		{
		case 2:
			fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_2;	//2人经验加成
			break;
		case 3:
			fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_3;	//3人经验加成
			break;
		case 4:
			fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_4;	//4人经验加成
			break;
		case 5:
			fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_5;	//5人经验加成
			break;
		case 6:
			fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_6;	//6人经验加成
			break;
		default:
			{
				if(6 < dwNumMembers)
					fExp *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_6;
			}
			break;
		}

		eOccupation byOccu = (eOccupation)pPlayer->GetOccupation();
		//职业等级系数
		long lOccuLvlCoe = (long)PlayerList::GetOccuLvlCoe(byOccu,pPlayer->GetOccuLvl(byOccu));

		float parm1 = (float)(lMonsterLevel - lOccuLvlCoe);
		if(parm1 >= GlobalSetup::GetSetup()->vigour_fExpMinLvlDiff) //怪物等级高5级时
			fExp = fExp * (1 + (lMonsterLevel - lOccuLvlCoe - GlobalSetup::GetSetup()->vigour_fExpMinLvlDiff)  * GlobalSetup::GetSetup()->vigour_fExpHortation / 100) ;
		else if((parm1 < GlobalSetup::GetSetup()->vigour_fExpMinLvlDiff) && (parm1 >= 0))  //怪物等级高0-4级时
			fExp = fExp;
		else if(parm1 < 0)  //玩家等级高于怪物时
			fExp = fExp * (1 - (lOccuLvlCoe - lMonsterLevel)  * GlobalSetup::GetSetup()->vigour_fExpAmerce / 100) ;

		fExp = fExp * GlobalSetup::GetSetup()->vigour_fOccExpSpRatio;

		if(1.0f >= fExp) fExp = 1.0f;

		//A队员经验值获取份额
		float fQuota = lOccuLvlCoe / ((float)dwNumMembers * fAverageOccuLevelCoe) + (lOccuLvlCoe - fAverageOccuLevelCoe) * GlobalSetup::GetSetup()->vigour_lExpDiff / 100;

		//份额低保
		fQuota = (fQuota < GlobalSetup::GetSetup() ->vigour_lExpLimit / 100.0f) ? GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f : fQuota; 
		//份额封顶
		fQuota = (fQuota > 1 - GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f) ? 1 - GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f: fQuota;

		//##返回最终的经验值
		float fReValue = fQuota * fExp + 0.5000f;//! 只要有小数尾数，则进1
		return (0 >= (LONG)fReValue) ? 0 : (LONG)fReValue;
	}
	else if(1 == dwNumMembers)
	{
		eOccupation byOccu = (eOccupation)pPlayer->GetOccupation();
		//职业等级系数
		long lOccuLvlCoe = (long)PlayerList::GetOccuLvlCoe(byOccu,pPlayer->GetOccuLvl(byOccu));
		//基本经验
		float parm1 = (float)(lOccuLvlCoe - lMonsterLevel);
		if(0 > parm1)
			parm1 = (GlobalSetup::GetSetup()->vigour_fExpMinLvlDiff < -parm1) ? (parm1 + GlobalSetup::GetSetup()->vigour_fExpMinLvlDiff) : 0;
		float fCoefficient = (0 > parm1) ? GlobalSetup::GetSetup()->vigour_fExpHortation : GlobalSetup::GetSetup()->vigour_fExpAmerce;
		fExp *= (float)(1.0f - parm1 * fCoefficient / 100.0f);
		fExp = fExp * GlobalSetup::GetSetup()->vigour_fOccExpSpRatio;
		fExp += 0.5000f;
		return (0 >= (LONG)fExp) ? 0 : (LONG)fExp;
	}
	else
	{
		assert(false);
		return 0;//! 发生了错误
	}
}

//! 计算职业点数
long GSTeam::CalculateOccuPoint(CPlayer *pPlayer, long lMonsterLevel, DWORD dwOccuP, LONG lPosX, LONG lPosY)
{
	if(NULL == pPlayer) return 0;
	//##计算9个Area范围内队伍成员的平均等级及个数
	FLOAT fAverageOccuLevelCoe = (float)GetLucrativeAreaAverageOccuLevelCoe_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);
	DWORD dwNumMembers	= GetLucrativeAreaMemberAmount_Alive( pPlayer -> GetRegionGUID(), lPosX, lPosY, FALSE);

	float fOccuP = (float)dwOccuP;
	//##当前区域内有队伍成员
	if( dwNumMembers > 1 )
	{
		switch(dwNumMembers)
		{
		case 2:
			fOccuP *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_2;	//2人经验加成
			break;
		case 3:
			fOccuP *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_3;	//3人经验加成
			break;
		case 4:
			fOccuP *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_4;	//4人经验加成
			break;
		case 5:
			fOccuP *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_5;	//5人经验加成
			break;
		case 6:
			fOccuP *= GlobalSetup::GetSetup()->vigour_fIncreasedExp_6;	//6人经验加成
			break;
		default:
			break;
		}

		//基本经验
		eOccupation byOccu = (eOccupation)pPlayer->GetOccupation();
		//职业等级系数
		long lOccuLvlCoe = (long)PlayerList::GetOccuLvlCoe(byOccu,pPlayer->GetOccuLvl(byOccu));
		//基本经验
		float parm1 = (float)( lOccuLvlCoe- lMonsterLevel );
		parm1 = (parm1 < 0) ? 0 : parm1; 
		fOccuP = fOccuP * (1 - parm1* GlobalSetup::GetSetup()->vigour_fExpAmerce / 100) ;
		if(1.0f >= fOccuP) fOccuP = 1.0f;

		//A队员经验值获取份额
		float fQuota = lOccuLvlCoe / ((float)dwNumMembers * fAverageOccuLevelCoe) + (lOccuLvlCoe - fAverageOccuLevelCoe) * GlobalSetup::GetSetup()->vigour_lExpDiff / 100;

		//份额低保
		fQuota = (fQuota < GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f) ? GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f : fQuota; 
		//份额封顶
		fQuota = (fQuota > 1 - GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f) ? 1 - GlobalSetup::GetSetup() -> vigour_lExpLimit / 100.0f : fQuota;

		//##返回最终的经验值
		float fReValue = fQuota * fOccuP + 0.5000f;//! 只要有小数尾数，则进1
		return (0 >= (LONG)fReValue) ? 0 : (LONG)fReValue;
	}
	//##当前区域内找不到任何队伍成员，包括获取经验的人
	else if(1 == dwNumMembers)
	{
		eOccupation byOccu = (eOccupation)pPlayer->GetOccupation();
		//职业等级系数
		long lOccuLvlCoe = (long)PlayerList::GetOccuLvlCoe(byOccu,pPlayer->GetOccuLvl(byOccu));
		//基本经验
		float parm1 =  (float)(lOccuLvlCoe- lMonsterLevel);
		fOccuP *= (float)(1.0f - parm1 * GlobalSetup::GetSetup()->vigour_fExpAmerce / 100.0f);
		fOccuP += 0.5000f;
		return (0 >= (LONG)fOccuP) ? 0 : (LONG)fOccuP;
	}
	else
	{
		assert(false);
		return 0;//! 发生了错误
	}
}

//给队伍添加一个任务
void GSTeam::AddQuest(DWORD dwQuestID,CPlayer* pPlayer,long lDistance)
{
	if(pPlayer == NULL)	return;

	list<tagMemberInfo>::iterator it = m_MemberList.begin();
	for( ; it != m_MemberList.end(); it ++ )
	{
		CPlayer* pTemptPlayer = GetGame()->FindPlayer( it->guid );
		if( pTemptPlayer )
		{
			if(lDistance == 0)
			{
				pTemptPlayer->AddQuest(dwQuestID);
			}
			else
			{
				if( pPlayer->Distance(pTemptPlayer) <= lDistance)
				{
					pTemptPlayer->AddQuest(dwQuestID);
				}
			}
		}
		else
		{
			if(lDistance == 0)
			{
				CMessage msg(MSG_S2W_QUEST_PlayerAddQuest);
				msg.Add(it->guid);
				msg.Add(dwQuestID);
				msg.Send();
			}
		}

	}
}

//给队伍出发一个脚本
void GSTeam::RunScript(const char* strScirptName,CPlayer* pPlayer,long lDistance, char* strScirptName2)
{
	if(pPlayer == NULL)	return;

	list<tagMemberInfo>::iterator it = m_MemberList.begin();
	for( ; it != m_MemberList.end(); it ++ )
	{

		CPlayer* pTemptPlayer = GetGame()->FindPlayer( it->guid );
		if( pTemptPlayer && pTemptPlayer->IsDied() == false)
		{
			if(lDistance == 0)
			{
				pTemptPlayer->PlayerRunScript(strScirptName);
			}
			else
			{
				if( pPlayer->Distance(pTemptPlayer) <= lDistance)
				{
					pTemptPlayer->PlayerRunScript(strScirptName);
				}
				else if(NULL != strScirptName2)
				{
					pTemptPlayer->PlayerRunScript(strScirptName2);
				}
			}
		}
		else
		{
			if(lDistance == 0)
			{
				CMessage msg(MSG_S2W_QUEST_PlayerRunScript);
				msg.Add( it->guid );
				msg.Add(strScirptName);
				msg.Send();
			}
			else if(NULL != strScirptName2)
			{
				CMessage msg(MSG_S2W_QUEST_PlayerRunScript);
				msg.Add( it->guid );
				msg.Add(strScirptName2);
				msg.Send();
			}
		}
	}
}

void GSTeam::SetCurrShareQuest(long questID)
{
	m_lCurrShareQuestID=questID;
	CMessage msg(MSG_S2C_SHARE_CURRQUEST);
	list<tagMemberInfo>::iterator it = m_MemberList.begin();
	for( ; it != m_MemberList.end(); it ++ )
	{
		//在队长共享任务前加入的成员
		if ( (*it).dwJoinTime<m_dwAcceptTime)
		{	
			//if ( (*it).guid != m_MestterGuid )
			//{
				CPlayer* pPlayer = GetGame()->FindPlayer( (*it).guid );
				if (pPlayer)
				{
					pPlayer->SetMasterQuest(questID);
				}
			//}
			msg.Add(questID);
			msg.SendToPlayer((*it).guid);		
		}
	}	
	ClearMemberQuest();
}

void GSTeam::SetMemberQuest(CGUID &guid,long lQuestID)
{
	list<tagMemberInfo>::iterator it = m_MemberList.begin();
	for( ; it != m_MemberList.end(); it ++ )
	{
		if ( it->guid == guid )
		{
			it->lCompleteQuest=lQuestID;
			break;
		}
	}
}
void GSTeam::ClearMemberQuest()
{
	list<tagMemberInfo>::iterator it=m_MemberList.begin();
	for (;it!=m_MemberList.end();it++)
	{
		it->lCompleteQuest = 0;
	}
}

bool GSTeam::IsCompleteQuest(CGUID &guid,long lQuestID)
{
	list<tagMemberInfo>::iterator it=m_MemberList.begin();
	for (;it!=m_MemberList.end();it++)
	{
		if ( it->guid == guid )
		{
			return it->lCompleteQuest==lQuestID?true:false;
		}
	}
	return false;
}

long GSTeam::FindShareQuest(long lquest)
{
	/*if (lquest == m_lCurrShareQuestID)
	{
		return lquest;
	}*/
	list<tagShareQuestList>::iterator it=m_ShareQuestList.begin();
	for (;it!=m_ShareQuestList.end();it++)
	{
		if ( (*it).lQuestID == lquest )
			return lquest;
	}
	return 0;
}

void GSTeam::ClearShareQuestList()
{
	m_ShareQuestList.clear();
}

//flag为真则发送更新到客户端
void GSTeam::UpdateShareQuestList(tagShareQuestList *quest,bool flag,CGUID pPlayerID)
{
// 	list<tagShareQuestList>::iterator it=m_ShareQuestList.begin();
// 	if (quest)
// 	{
// 		for (;it!=m_ShareQuestList.end();it++)
// 		{
// 			//玩家名字相同,属于重新推荐任务
// 			if ( strcmp((*it).szName,quest->szName)==0 )
// 			{
// 				(*it).lQuestID=quest->lQuestID;
// 				return;
// 			}
// 		}
// 		if (6 == (int)m_ShareQuestList.size())
// 			return;
// 		m_ShareQuestList.push_back(*quest);
// 	}
	if (flag)
	{
        for (list<tagMemberInfo>::iterator ite = m_MemberList.begin(); ite != m_MemberList.end(); ++ite)
        {
            CMessage msg(MSG_S2C_SHARE_QUESTLIST_UPDATE);
            msg.Add( quest->lQuestID );
            msg.Add( quest->szName );
            msg.Add( pPlayerID );
            CPlayer *pPlayer = GetGame()->FindPlayer(ite->guid);
            if ( pPlayerID == pPlayer->GetExID() )
            {
                // 屏蔽接收人
                continue;
            }
            if(NULL != pPlayer) //是本服务器玩家
            {
                /// 当前存在任务，直接返回..
                if ( pPlayer->GetQuestState( quest->lQuestID ) != eQS_No)
                {
                    msg.Add( (BYTE)eQsExist );
                    /// 此处如果推荐人不再一个GS，需要向Ws请求 
                    //msg.SendToPlayer(pPlayerID);
                    continue;
                }
                /// 已经完成任务
                if ( pPlayer->GetRoleFinishedRecord(quest->lQuestID) )
                {
                    msg.Add( (BYTE)eQsExist );
                    //msg.SendToPlayer(ite->guid);
                    continue;
                }
                
                msg.Add( (BYTE) eQsNo ); 
                /// 没有此任务,直接发送
                msg.SendToPlayer(ite->guid);
            }
        }
	}	
}

//! 判断是否为一个结婚队伍，如果是，获得另一方的GUID
const CGUID& GSTeam::GetMarriageOtherOne(CPlayer *pOnePlayer)
{
	//! 判断结婚队伍条件：2个人，同图，异性，非死亡
	if(NULL != pOnePlayer && 2 == m_MemberList.size())
	{
		const tagMemberInfo *pOneMemberInfo = NULL;
		const tagMemberInfo *pOtherOneMemberInfo = NULL;

		list<tagMemberInfo>::iterator iteFirst = m_MemberList.begin();
		list<tagMemberInfo>::iterator iteSecond = ++m_MemberList.begin();
		if ((*iteFirst).guid == pOnePlayer->GetExID())
		{
			pOneMemberInfo		= &(*iteFirst);
			pOtherOneMemberInfo = &(*iteSecond);
		}
		else if((*iteSecond).guid == pOnePlayer->GetExID())
		{
			pOneMemberInfo		= &(*iteSecond);
			pOtherOneMemberInfo = &(*iteFirst);
		}
		else
			return NULL_GUID;

		if(pOneMemberInfo->lSex != pOtherOneMemberInfo->lSex)
		{
			CPlayer *pOtherOne = GetGame()->FindPlayer(pOtherOneMemberInfo->guid);
			if (NULL != pOtherOne)
			{
				if(pOtherOne->GetRegionGUID() == pOnePlayer->GetRegionGUID())
					if(!(pOnePlayer->IsDied()) && !(pOtherOne->IsDied()))
						return pOtherOneMemberInfo->guid;
			}
		}
	}

	return NULL_GUID;
}


