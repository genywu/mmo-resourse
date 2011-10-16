

#include "WSTeam.h"


#pragma once
//! 战队系统，用于战队排队，队伍筛选、战场分配



class CSentaiSystem
{
public:
	CSentaiSystem(void);
	~CSentaiSystem(void){}

	//! 读取配置、初始化系统，不设置重读功能
	bool Init(const char *pSetupName);

public:
	//! 响应GS转发的加入退出请求
	void OnPlayerQueue(const CGUID &PlayerGuid, bool IsJoin);

	//! 响应脚本系统发送的调用场景空闲消息
	void OnWarFieldFree(long FieldRegionID);

	//! 查询这个队伍是否处在入场的三个阶段
	bool IsQueuing(const CGUID &TeamGuid);

	//! 检测队伍是否正在战斗中
	bool IsBattle(const CGUID &TeamGuid);

	//! 响应整个队伍下线
	void OnTeamLeaveGame(const CGUID &TeamGuid);

	//! 结束一个队伍的战队战（只有在战斗中才有效）
	void EndTeamWar(const CGUID &TeamGuid, long FieldRegionID);

public:
	//! 定时器超时响应
	long OnTimeOut(DWORD timerid, DWORD curtime, const void* var);
	//! 当定时器被取消的调用
	void OnTimerCancel(DWORD timerid, const void* var){}
	//! 运行定时器
	void TimerRun(DWORD dwCurrTime){m_TimerQueue.Expire(dwCurrTime);}




	/////////////////////////////////////////////////////////////////////////////
private:

	//! 场地
	struct tagField 
	{
		bool			IsFree;
		long			RegionID;
		vector<POINT>	BeginPos;
		vector<CGUID>	TeamGuids;
	};

	//! 基本配置
	struct tagSSSetup 
	{
		DWORD 			FieldTeamNum;		//! 场地容纳的队伍数
		DWORD			TeamMemberNum;		//! 进场队伍的人员数限制（不能多也不能少）
		DWORD			TeamMemberMinLvl;	//! 参与队员最小等级
		DWORD 			IntoWaitSec;		//! 开始等待的时间
		DWORD 			PenaltyPoints;		//! 不进场惩罚分数
		float 			ReadyTimeFactor;	//! 进场准备的时间系数
	};

	//! 战队
	struct tagEntryTeam 
	{
		long			SentaiPoint;
		CGUID			TeamGuid;	
	};

	typedef multimap<long, tagEntryTeam>	TeamQueue;
	typedef TeamQueue::iterator				ItrTeamQueue;
	typedef vector<tagEntryTeam>			TeamArray;
	typedef TeamArray::iterator				ItrTeamArray;

	typedef CTimerQueue<CSentaiSystem*>			SentaiTimer;

	//! 多只队伍组成的战团
	struct tagWarGroup
	{
		DWORD			TimerID;
		long			PreparedRegionID;
		TeamArray		Teams;				//! 所有参赛队伍的GUID
	};

private:
	

	//! 找到一个正在排队的队伍
	ItrTeamQueue FindQueueTeam(const CGUID &TeamGuid);
	ItrTeamQueue FindQueueTeam(long TeamPoint, const CGUID &TeamGuid);

	//! 吧一个队伍加入到排队
	void AddToQueue(long TeamPoint, const CGUID &TeamGuid, bool ReplaceWait);
	void AddToQueue(tagEntryTeam EntryTeam, bool ReplaceWait);
	//! 删除排队
	ItrTeamQueue DelFromQueue(ItrTeamQueue itrTeam, bool IsInNext);

	//! 找到一个已经获得场地的队伍
	ItrTeamArray FindWaitTeam(const CGUID &TeamGuid);
	//! 删除一个已经获得场地的队伍
	void DelFromWait(ItrTeamArray iteWaitTeam);

	//! 尝试用新的队伍去替换等待中的队伍
	bool TestAndSwapWait(tagEntryTeam &NewTeam);

	//! 检测是否可以组团进入等待阶段，并开始倒计时
	void TestGotoWait(void);

	//! 检测队伍是否在等待场地阶段
	bool IsWaitField(const CGUID &TeamGuid);
	//! 检测队伍是否正在倒计时阶段
	bool IsEntering(const CGUID &TeamGuid);

	

	//! 优化等待的团队
	void OptimizeWait(void);

	//! 的到一个空的场地，并锁定
	long GetAndLockFreeField(void);

	//! 开始进入倒计时
	void BeginIn(void);

	//! 进场
	void InField(DWORD TimerID);

	//! 设置队伍进入退出战队系统
	void OnTeamInSys(const CGUID &TeamGuid, bool IsIn, bool IsChangeRgn);

	//! 结束一个队伍的战场战斗
	bool EndTeamWar(const CGUID &TeamGuid, tagField &Field, bool SendMsg);


private:

	//!					基本配置
	tagSSSetup			m_Setup;
	//!					场地列表
	map<long, tagField>	m_WarFields;
	//!					空闲场地数量
	long				m_FreeNum;

	//!					正在排队的队伍
	TeamQueue			m_TeamQueue;
	//!					排队队伍对应的积分
	map<CGUID, long>	m_TeamPoint;

	//!					从排队的头还是尾取出队伍组团
	bool				m_GetInHead;
	//!					获得场地，等待进入的团组
	tagWarGroup			m_WaitTeam;
	//!					准备进入场地的团组列表
	list<tagWarGroup>	m_ReadyTeams;

	//!
	SentaiTimer			m_TimerQueue;


};