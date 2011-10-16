

#include "StdAfx.h"
#include <math.h>
#include "SentaiSystem.h"
#include "OrganizingCtrl.h"
#include "..\nets\networld\message.h"



CSentaiSystem::CSentaiSystem(void)
:m_FreeNum(0)
,m_TimerQueue(2, 100)
{

}

//! 读取配置、初始化系统，不设置重读功能
bool CSentaiSystem::Init(const char *pSetupName)
{
	//! ----------------------------根节点----------------------------
	TiXmlNode *pNode = NULL;

	CRFile *prfile = NULL;
	if(NULL == pSetupName || NULL == (prfile = rfOpen(pSetupName)))
		return false;

	TiXmlDocument m_Tdoc(pSetupName);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	pNode=m_Tdoc.FirstChild("Sentai");
	if (pNode==NULL) 
		return false;

	TiXmlElement* pSentai = pNode->ToElement();//获取node 的指针
	if (pSentai==NULL) 
		return false;

	//! ----------------------------开始读取信息----------------------------
	m_Setup.FieldTeamNum		= atoi(pSentai->Attribute("FieldTeamNum"));
	m_Setup.TeamMemberNum		= atoi(pSentai->Attribute("TeamMemberNum"));
	m_Setup.TeamMemberMinLvl	= atoi(pSentai->Attribute("TeamMemberMinLvl"));
	m_Setup.IntoWaitSec			= atoi(pSentai->Attribute("IntoWaitSec"));
	m_Setup.PenaltyPoints		= atoi(pSentai->Attribute("PenaltyPoints"));
	m_Setup.ReadyTimeFactor		= atof(pSentai->Attribute("ReadyTimeFactor"));

	m_WarFields.clear();
	TiXmlElement* pWarField = pSentai->FirstChildElement("WarField");
	for (; pWarField != NULL; pWarField = pWarField->NextSiblingElement("WarField"))
	{
		tagField WarField;
		WarField.IsFree = true;
		WarField.RegionID = atoi(pWarField->Attribute("RegionID"));

		TiXmlElement* pBeginPos = pWarField->FirstChildElement("BeginPos");
		DWORD LoadNum = 0;
		for (; pBeginPos != NULL && LoadNum < m_Setup.FieldTeamNum; pBeginPos = pBeginPos->NextSiblingElement("BeginPos"))
		{
			++LoadNum;
			POINT Point;
			Point.x = atoi(pBeginPos->Attribute("X"));
			Point.y = atoi(pBeginPos->Attribute("Y"));
			WarField.BeginPos.push_back(Point);
		}

		if(m_Setup.FieldTeamNum > WarField.BeginPos.size())
		{
			AddErrorLogText("War Field Error!!! ReginID = %u", WarField.RegionID);
			continue;
		}

		m_WarFields[WarField.RegionID] = WarField;
	}

	//! 只要能取得一个正确的配置，就返回OK
	m_FreeNum = m_WarFields.size();
	return (0 == m_WarFields.size());
}


//! 响应GS转发的加入退出请求
void CSentaiSystem::OnPlayerQueue(const CGUID &PlayerGuid, bool IsJoin)
{
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if (NULL == pPlayer)
		return;

	DWORD Succeed = 0;
	DWORD ErrType = 0;
	
	const CGUID &TeamGuid = pPlayer->GetTeamID();
	do 
	{
		WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(TeamGuid);
		if(NULL == pTeam)
		{
			ErrType = eSEI_NoTeam;
			break;
		}
		if(pTeam->GetMasterID() != PlayerGuid)
		{
			ErrType = eSEI_NotLeader;
			break;
		}

		if (IsWaitField(TeamGuid))
		{
			ErrType = eSEI_Waiting;
			break;
		}

		if (IsEntering(TeamGuid))
		{
			ErrType = eSEI_Entering;
			break;
		}

		if (IsBattle(TeamGuid))
		{
			ErrType = eSEI_Battle;
			break;
		}

		ItrTeamQueue itrTeam = FindQueueTeam(TeamGuid);
		ItrTeamArray itrWaitTeam = FindWaitTeam(TeamGuid);
		if(IsJoin)
		{
			if(m_Setup.TeamMemberNum != pTeam->GetMemberNum())
			{
				ErrType = eSEI_MemberNumErr;
				break;
			}

			if(!pTeam->IsInSameRegion())
			{
				ErrType = eSEI_NotInSameRegion;
				break;
			}

			if(pTeam->GetOnlineMemberNum() != m_Setup.TeamMemberNum)
			{
				ErrType = eSEI_AllOnline;
				break;
			}

			if(pTeam->GetMemberMinLvl() < m_Setup.TeamMemberMinLvl)
			{
				ErrType = eSEI_MemberLvlRrr;
				break;
			}

			if(itrTeam != m_TeamQueue.end() || itrWaitTeam != m_WaitTeam.Teams.end())
			{
				ErrType = eSEI_IsIn;
				break;
			}
			
			Succeed = 1;
			AddToQueue(pTeam->GetSentaiPoint(), TeamGuid, true);
			//! 检测是否可以组团进入等待阶段
			TestGotoWait();

		}
		else
		{
			if(itrTeam == m_TeamQueue.end() && itrWaitTeam == m_WaitTeam.Teams.end())
			{
				ErrType = eSEI_NotJoin;
				break;
			}

			Succeed = 1;
			if (itrTeam != m_TeamQueue.end())
				DelFromQueue(itrTeam, false);
			else
				DelFromWait(itrWaitTeam);
			
		}
	} while(false);

	//!
	CMessage ReMsg(MSG_W2S_SENTAI_RE_QUEUE);
	if(0 != Succeed)
	{
		ReMsg.Add((BYTE)1);
		ReMsg.Add(TeamGuid);
		ReMsg.Add(Succeed);
		ReMsg.Add((DWORD)IsJoin);
		ReMsg.Add((DWORD)1);//! 客户端是否弹出对话框
	}
	else
	{
		ReMsg.Add((BYTE)0);
		ReMsg.Add(PlayerGuid);
		ReMsg.Add(Succeed);
		ReMsg.Add(ErrType);
	}


	if(0 == Succeed)
		ReMsg.SendToSocket(pPlayer->GetGsSocket());
	else
		ReMsg.SendAll();

}

#ifdef _DEBUG_SENTAI_
long GetEndID(void)
{
	static long IdSeed = 0;
	return IdSeed++;
}
#endif

//! 响应脚本系统调用场景空闲请求
void CSentaiSystem::OnWarFieldFree(long FieldRegionID)
{
	//! 正在等待的、正在进入的场景，都不允许解锁
	if (m_WaitTeam.PreparedRegionID == FieldRegionID)
		return;

	for (list<tagWarGroup>::iterator itr = m_ReadyTeams.begin(); itr != m_ReadyTeams.end(); ++itr)
	{
		if(itr->PreparedRegionID == FieldRegionID)
			return;
	}

	map<long, tagField>::iterator itr = m_WarFields.find(FieldRegionID);
	if (itr != m_WarFields.end())
	{
		if (!itr->second.IsFree)
		{
			itr->second.IsFree = true;
			++m_FreeNum;

			for (size_t i = 0; i < itr->second.TeamGuids.size(); ++i)
			{
				WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(itr->second.TeamGuids[i]);
				if(NULL != pTeam)
				{
					pTeam->SetOptControl(0);

#ifdef _DEBUG_SENTAI_
					long EndID = GetEndID();
					char DebugInfo[128] = {0};
					_snprintf(DebugInfo, 128, "MSG_S2W_SENTAI_END_MATCH flag:%d", EndID);
					PutStringToFile("SentaiW2S", DebugInfo);
#endif
					CMessage ReMsg(MSG_W2S_SENTAI_END_MACTH);
#ifdef _DEBUG_SENTAI_
					ReMsg.Add(EndID);
#endif
					ReMsg.Add(itr->second.TeamGuids[i]);
					ReMsg.SendAll();

					if (0 == pTeam->GetOnlineMemberNum())
					{
						GetOrganiCtrl()->DisbandTeam(pTeam);
					}
				}
			}
		}
	}

	TestGotoWait();
}

//! 查询这个队伍是否处在入场的三个阶段
bool CSentaiSystem::IsQueuing(const CGUID &TeamGuid)
{
	//! 正在排队阶段
	if (m_TeamQueue.end() != FindQueueTeam(TeamGuid))
		return true;

	if (IsWaitField(TeamGuid))
	{
		return true;
	}

	//! 正在倒计时阶段
	return IsEntering(TeamGuid);
}

//! 检测队伍是否在等待场地阶段
bool CSentaiSystem::IsWaitField(const CGUID &TeamGuid)
{
	//! 正在等待场地阶段
	for (size_t i = 0; i < m_WaitTeam.Teams.size(); ++i)
	{
		if(TeamGuid == m_WaitTeam.Teams[i].TeamGuid)
			return true;
	}
	return false;
}

//! 检测队伍是否正在倒计时阶段
bool CSentaiSystem::IsEntering(const CGUID &TeamGuid)
{
	for (list<tagWarGroup>::iterator itr = m_ReadyTeams.begin(); itr != m_ReadyTeams.end(); ++itr)
	{
		for (size_t i = 0; i < itr->Teams.size(); ++i)
		{
			if(itr->Teams[i].TeamGuid == TeamGuid)
				return true;
		}
	}

	return false;
}

//! 检测队伍是否正在战斗中
bool CSentaiSystem::IsBattle(const CGUID &TeamGuid)
{
	map<long, tagField>::iterator iteField = m_WarFields.begin();
	size_t TestMaxNum = m_WarFields.size() - m_FreeNum;
	size_t TestNum = 0;
	for (; iteField != m_WarFields.end() && TestNum < TestMaxNum; ++iteField)
	{
		if (!iteField->second.IsFree)
		{
			++TestNum;

			for (size_t i = 0; i < iteField->second.TeamGuids.size(); ++i)
			{
				if(TeamGuid == iteField->second.TeamGuids[i])
					return true;
			}
		}
	}

	return false;
}

//! 响应整个队伍下线
void CSentaiSystem::OnTeamLeaveGame(const CGUID &TeamGuid)
{
	map<long, tagField>::iterator iteField = m_WarFields.begin();
	size_t TestMaxNum = m_WarFields.size() - m_FreeNum;
	size_t TestNum = 0;
	for (; iteField != m_WarFields.end() && TestNum < TestMaxNum; ++iteField)
	{
		if (!iteField->second.IsFree)
		{
			++TestNum;

			if(EndTeamWar(TeamGuid, iteField->second, false))
				return;
		}
	}
}

//! 结束一个队伍的战队战（只有在战斗中才有效）
void CSentaiSystem::EndTeamWar(const CGUID &TeamGuid, long FieldRegionID)
{
	map<long, tagField>::iterator itr = m_WarFields.find(FieldRegionID);
	if (itr != m_WarFields.end())
	{
		if (!itr->second.IsFree)
		{
			EndTeamWar(TeamGuid, itr->second, true);
		}
	}
}

//! 结束一个队伍的战场战斗
bool CSentaiSystem::EndTeamWar(const CGUID &TeamGuid, tagField &Field, bool SendMsg)
{
	for (size_t i = 0; i < Field.TeamGuids.size(); ++i)
	{
		if(TeamGuid == Field.TeamGuids[i])
		{
			WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(Field.TeamGuids[i]);
			if(NULL != pTeam)
			{
				if(SendMsg)
				{
#ifdef _DEBUG_SENTAI_
					long EndID = GetEndID();
					char DebugInfo[128] = {0};
					_snprintf(DebugInfo, 128, "MSG_S2W_SENTAI_END_MATCH flag:%d", EndID);
					PutStringToFile("SentaiW2S", DebugInfo);
#endif
					CMessage ReMsg(MSG_W2S_SENTAI_END_MACTH);
#ifdef _DEBUG_SENTAI_
					ReMsg.Add(EndID);
#endif
					ReMsg.Add(TeamGuid);
					ReMsg.SendAll();
				}

				pTeam->SetOptControl(0);
				Field.TeamGuids[i] = NULL_GUID;
			}
			return true;
		}
	}

	return false;
}


//! 定时器超时响应
long CSentaiSystem::OnTimeOut(DWORD timerid, DWORD curtime, const void* var)
{
	if(-1 != timerid)
	{
		if (timerid == m_WaitTeam.TimerID)
		{
			BeginIn();
		}
		else
			InField(timerid);
	}

	return timerid;
}


//! 找到一个正在排队的队伍
CSentaiSystem::ItrTeamQueue CSentaiSystem::FindQueueTeam(const CGUID &TeamGuid)
{
	map<CGUID, long>::iterator itePoint = m_TeamPoint.find(TeamGuid);
	if (itePoint != m_TeamPoint.end())
	{
		return FindQueueTeam(itePoint->second, TeamGuid);
	}
	return m_TeamQueue.end();
}
CSentaiSystem::ItrTeamQueue CSentaiSystem::FindQueueTeam(long TeamPoint, const CGUID &TeamGuid)
{
	pair<ItrTeamQueue, ItrTeamQueue> Rand;
	Rand = m_TeamQueue.equal_range(TeamPoint);
	while (Rand.first != Rand.second)
	{
		if(Rand.first->second.TeamGuid == TeamGuid)
			return Rand.first;
		++Rand.first;
	}
	return m_TeamQueue.end();
}

//! 吧一个队伍加入到排队
void CSentaiSystem::AddToQueue(long TeamPoint, const CGUID &TeamGuid, bool ReplaceWait)
{
	assert(m_TeamQueue.end() == FindQueueTeam(TeamGuid));

	tagEntryTeam EntryTeam;
	EntryTeam.SentaiPoint = TeamPoint;
	EntryTeam.TeamGuid = TeamGuid;

	AddToQueue(EntryTeam, ReplaceWait);
}

void CSentaiSystem::AddToQueue(tagEntryTeam EntryTeam, bool ReplaceWait)
{
	assert(m_TeamQueue.end() == FindQueueTeam(EntryTeam.TeamGuid));

	//! 设置不参加活动的惩罚积分
	OnTeamInSys(EntryTeam.TeamGuid, true, false);

	//! 是否尝试用新的队伍去替换等待中的队伍
	if (ReplaceWait && 0 < m_WaitTeam.Teams.size())
	{
		//! 若替换成功，将被替换下来的队伍插入队列
		TestAndSwapWait(EntryTeam);
	}

	m_TeamQueue.insert(make_pair(EntryTeam.SentaiPoint, EntryTeam));
	m_TeamPoint.insert(make_pair(EntryTeam.TeamGuid, EntryTeam.SentaiPoint));

	
}

//! 删除排队
CSentaiSystem::ItrTeamQueue CSentaiSystem::DelFromQueue(ItrTeamQueue itrTeam, bool IsInNext)
{
	ItrTeamQueue itrRe = m_TeamQueue.end();
	if(m_TeamQueue.end() != itrTeam)
	{
		//! 取消不参加活动的惩罚积分
		if(!IsInNext)
			OnTeamInSys(itrTeam->second.TeamGuid, false, false);

		m_TeamPoint.erase(itrTeam->second.TeamGuid);
		itrRe = m_TeamQueue.erase(itrTeam);
	}

	return itrRe;
}

//! 找到一个已经获得场地的队伍
CSentaiSystem::ItrTeamArray CSentaiSystem::FindWaitTeam(const CGUID &TeamGuid)
{
	ItrTeamArray itrTeam = m_WaitTeam.Teams.begin();
	for (; itrTeam != m_WaitTeam.Teams.end(); ++itrTeam)
	{
		if (TeamGuid == itrTeam->TeamGuid)
			break; 
	}

	return itrTeam;
}
//! 删除一个已经获得场地的队伍
void CSentaiSystem::DelFromWait(ItrTeamArray iteWaitTeam)
{
	if (0 < m_TeamQueue.size())
	{
		//! 取消不参加活动的惩罚积分
		OnTeamInSys(iteWaitTeam->TeamGuid, false, false);

		*iteWaitTeam = m_TeamQueue.begin()->second;
		m_TeamQueue.erase(m_TeamQueue.begin());

		OptimizeWait();
	}
}

//! 尝试用新的队伍去替换等待中的队伍
bool CSentaiSystem::TestAndSwapWait(tagEntryTeam &NewTeam)
{
	if(0 < m_WaitTeam.Teams.size())
	{
		long PointSum = NewTeam.SentaiPoint;
		for (size_t i = 0; i < m_WaitTeam.Teams.size(); ++i)
		{
			PointSum += m_WaitTeam.Teams[i].SentaiPoint;
		}
		long Average = PointSum / (1 + m_WaitTeam.Teams.size());
		
		//! 方差和队伍积分的差
		long NewEccentricity = abs(NewTeam.SentaiPoint - Average);
		long MaxEccentricity = 0;
		long MaxEccentricityIndex = 0;
		for (size_t i = 0; i < m_WaitTeam.Teams.size(); ++i)
		{
			long Eccentricity = abs(m_WaitTeam.Teams[i].SentaiPoint - Average);
			if (MaxEccentricity < Eccentricity)
			{
				MaxEccentricityIndex = i;
				MaxEccentricity = Eccentricity;
			}
		}

		if(MaxEccentricity > NewEccentricity)
		{
			tagEntryTeam TmpTeam = NewTeam;
			NewTeam = m_WaitTeam.Teams[MaxEccentricityIndex];
			m_WaitTeam.Teams[MaxEccentricityIndex] = TmpTeam;
			return true;
		}
	}
	
	return false;
}

//! 检测是否可以组团进入等待阶段，并开始倒计时
void CSentaiSystem::TestGotoWait(void)
{
	if(0 < m_FreeNum && 0 == m_WaitTeam.Teams.size() && m_TeamQueue.size() >= m_Setup.FieldTeamNum)
	{
		for (DWORD i = 0; i < m_Setup.FieldTeamNum; ++i)
		{
			ItrTeamQueue iteTeamQueue;
			if (m_GetInHead)
			{
				iteTeamQueue = m_TeamQueue.begin();
			}
			else
			{
				iteTeamQueue = m_TeamQueue.end();
				--iteTeamQueue;
			}

			tagEntryTeam EntryTeam;
			EntryTeam = iteTeamQueue->second;
			DelFromQueue(iteTeamQueue, true);
			m_WaitTeam.Teams.push_back(EntryTeam);
		}
		
		//! 在剩下的队伍中逐个尝试优化替换
		OptimizeWait();
		

		//! 计算返回值（队伍积分标准差 * 2 / 3）
		float PointSum = 0;
		for (size_t i = 0; i < m_WaitTeam.Teams.size(); ++i)
		{
			PointSum += m_WaitTeam.Teams[i].SentaiPoint;
		}
		float Average = PointSum / (1 + m_WaitTeam.Teams.size()); 

		float Squares = 0;
		for (size_t i = 0; i < m_WaitTeam.Teams.size(); ++i)
		{
			Squares += pow(m_WaitTeam.Teams[i].SentaiPoint - Average, 2);
		}

		long WaitSec = sqrt(Squares) * 2 / 3 * m_Setup.ReadyTimeFactor;

		//! 开始进场倒计时
		m_WaitTeam.PreparedRegionID = GetAndLockFreeField();
		assert(0 != m_WaitTeam.PreparedRegionID);
		
		//! 至少等2秒钟，防止排队和开场消息倒序的问题
		if(WaitSec <= 0)
			WaitSec = 2;
		//! 至多等60分钟，防止出现意外
		WaitSec = min(WaitSec, 60 * 60);

		m_WaitTeam.TimerID = m_TimerQueue.Schedule(this, NULL, timeGetTime() + WaitSec * 1000, 0);
	}
}

//! 优化等待的团队
void CSentaiSystem::OptimizeWait(void)
{
	TeamQueue TmpQueue;
	for (ItrTeamQueue iteTeamQueue = m_TeamQueue.begin(); iteTeamQueue != m_TeamQueue.end(); ++iteTeamQueue)
	{
		tagEntryTeam EntryTeam = iteTeamQueue->second;
		if (TestAndSwapWait(EntryTeam))
		{
			TmpQueue.insert(make_pair(EntryTeam.SentaiPoint, EntryTeam));
			iteTeamQueue = DelFromQueue(iteTeamQueue, true);

			if(iteTeamQueue == m_TeamQueue.end())
				break;
		}
	}

	//! 把替换下来的队伍放入队列
	for (ItrTeamQueue iteTeamQueue = TmpQueue.begin(); iteTeamQueue != TmpQueue.end(); ++iteTeamQueue)
	{
		AddToQueue(iteTeamQueue->second, false);
	}
}

//! 的到一个空的场地，并锁定
long CSentaiSystem::GetAndLockFreeField(void)
{
	assert(0 != m_FreeNum);
	vector<map<long, tagField>::iterator> vFreeFields;
	map<long, tagField>::iterator ite = m_WarFields.begin();
	for (; ite != m_WarFields.end(); ++ite)
	{
		if (ite->second.IsFree)
		{
			vFreeFields.push_back(ite);
			if(m_FreeNum <= vFreeFields.size())
				break;
		}
	}

	assert(0 != vFreeFields.size());
	size_t RandIdx = random(vFreeFields.size());
	ite = vFreeFields[RandIdx];
	ite->second.IsFree = false;
	--m_FreeNum;

	return ite->second.RegionID;
}

//! 开始进入倒计时
void CSentaiSystem::BeginIn(void)
{
	assert(m_WaitTeam.Teams.size() > 0);

	m_ReadyTeams.push_front(m_WaitTeam);
	m_ReadyTeams.begin()->TimerID = m_TimerQueue.Schedule(this, NULL, timeGetTime() + m_Setup.IntoWaitSec * 1000, 0);

	CMessage msg(MSG_W2S_SENTAI_INTO_READY);
	msg.Add(m_Setup.IntoWaitSec);
	msg.Add((long)m_WaitTeam.Teams.size());
	for (long i = 0; i < m_WaitTeam.Teams.size(); ++i)
	{
		msg.Add(m_WaitTeam.Teams[i].TeamGuid);
	}
	msg.SendAll();

	m_WaitTeam.Teams.clear();
	m_WaitTeam.PreparedRegionID = 0;
	m_WaitTeam.TimerID = -1;

	TestGotoWait();
}

//! 进场
void CSentaiSystem::InField(DWORD TimerID)
{
	for (list<tagWarGroup>::iterator itr = m_ReadyTeams.begin(); itr != m_ReadyTeams.end(); ++itr)
	{
		if (itr->TimerID == TimerID)
		{
			//!
			map<long, tagField>::iterator itrField = m_WarFields.find(itr->PreparedRegionID);
			if(itrField != m_WarFields.end())
			{
				if(itr->Teams.size() == itrField->second.BeginPos.size())
				{
					CMessage msg(MSG_W2S_SENTAI_INTO_FIELD);
					msg.Add(itrField->second.RegionID);
					msg.Add((long)itr->Teams.size());

					for (long i = 0; i < itr->Teams.size(); ++i)
					{
						WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(itr->Teams[i].TeamGuid);
						if (NULL != pTeam && 0 == pTeam->GetOnlineMemberNum())
						{
							itr->Teams[i].TeamGuid = NULL_GUID;
							pTeam->SetOptControl(0);
							GetOrganiCtrl()->DisbandTeam(pTeam);
							itrField->second.TeamGuids.push_back(NULL_GUID);
						}
						else
						{
							itrField->second.TeamGuids.push_back(itr->Teams[i].TeamGuid);
						}
						msg.Add(itr->Teams[i].TeamGuid);
						msg.Add(itrField->second.BeginPos[i].x);
						msg.Add(itrField->second.BeginPos[i].y);

						OnTeamInSys(itr->Teams[i].TeamGuid, false, true);
					}

					msg.SendAll();
				}
				else
					assert(false);
			}
			else
				assert(false);

			m_ReadyTeams.erase(itr);
			break;
		}
	}
}

//! 设置队伍进入退出战队系统
void CSentaiSystem::OnTeamInSys(const CGUID &TeamGuid, const  bool IsIn, bool IsChangeRgn)
{
	//! 惩罚积分
	WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(TeamGuid);
	if (NULL != pTeam)
	{
		if(IsIn)
			pTeam->SetPenaltyPoints(m_Setup.PenaltyPoints);
		else
			pTeam->SetPenaltyPoints(0);

		if(!IsChangeRgn)
		{
			if(IsIn)
				pTeam->SetOptControl(eTOCT_CantJoin | eTOCT_CantExit | eTOCT_CantDisband);
			else
				pTeam->SetOptControl(0);
		}
	}
}

