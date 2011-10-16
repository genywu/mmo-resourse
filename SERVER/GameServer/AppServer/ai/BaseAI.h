#include <queue>
#include "..\..\..\..\Public\SkillRelated.h"
//#include "..\MoveShape.h"
#pragma once

class CRegion;
class CMoveShape;
class CPlayer;
class CGibe;
class CPet;
//class CSkill;
struct stModuParam;


const LONG  EXCEPTION_STATE_SPACE=500;
const LONG  POST_AREA=8;
const LONG  SKILL_FAIL_SPACE_TIME=1000;
const LONG  MOVE_FAIL_APACE_TIME=1000;
const LONG  STAND_ONCE_TIME=1000;
const LONG  SKILL_SPACE_TIME=500;

class CBaseModule;
class CBaseAI : public BaseMemObj
{
public:

	//AI执行状态
	enum AI_EXEC_STATE
	{
		AES_INVALID = -1, // 无效AI状态，用于一些临界状态，可避免很多临界BUG
		AES_FIGHT_NORMAL=0,							//战斗行走
		//AES_FIGHT_ATTACK,							//战斗攻击（对自己技能）
		AES_FIGHT_ESCAPE,							//战斗逃跑
		AES_FIGHT_TRANSITION,						//战斗向和平过渡状态
		AES_FIGHT_NOHP,								//战斗状态中无hp的时候
	};

	//超时事件类型
	enum AI_EVENT_TYPE
	{
		AET_NULL=0,                         //无动作
		AET_CYCLE,							//周期
		AET_ACT,							//动作（攻击，对自己技能等）
		AET_MOVE,							//行走,（站立）
		//AET_RELIVE,							//复活
		AET_RETURNPEACE,					//恢复和平
		AET_DIED,							//死亡事件
		AET_CHANGEAI,						//改变ai
		AET_SKILL_CONTINUE,		// 脚本AI，技能动作完毕，继续执行脚本
	};

	struct AI_EVENT							//超时记录结构
	{
		AI_EVENT();
		VOID Clean();
		VOID operator=(AI_EVENT& ae);

		AI_EVENT_TYPE			aetEventType;		//超时事件类型
		LONG					lTimerID;			//时间id
		DWORD					dwSpringTime;		//触发时间
	};



	//周期中注册的对主动行为有影响的事件，主要在周期中运用。也有可能被脚本调用
	enum CYCLE_SPRINGACT
	{
		CS_NULL=0,
		CS_MOVETO_CELL,
		CS_MOVEBY_STEP,
		CS_MOVEBY_TIME,
		CS_RANDOMMOVE,
		CS_STAND,
		CS_SKILL,
		CS_ESCAPE,
		CS_SENDEXPACT,
		CS_SENDEXPACTNEXT,
		CS_LEVELFIGHT,
		CS_SKILLEX, // 扩展使用技能，可以对指定GUID的玩家或怪物使用技能，CS_SKILL只可以对当前目标使用技能
		CS_SKILLCELL, // 对某个格子使用技能
	};

	struct SCycleSpringEvent			//周期触发结构。分别包含枚举CYCLE_SPRINGACT 中的不同类型
	{
		SCycleSpringEvent();
		VOID Clean();

		CYCLE_SPRINGACT csSpringType;	//周期触发的类型（上面5类）
		union 
		{
			struct USESKILL
			{
				LONG lSkillID;			//技能id
				LONG lSkillLevel;		//技能等级
				LONG lPos;		//施放目标格子(x|y)
			}tgUseSkill;

			struct  MOVEBYSTEP    //行走by方向
			{
				LONG lDirect;
				LONG lStepNum;	
			} tgMoveByStep;

			struct MOVEBYTIME		//行走by时间
			{
				LONG lDirect;
				LONG lStopTime;
			}tgMoveByTime;

			struct MOVETOCELL		//行走到某格
			{
				LONG lX;
				LONG lY; 
			} tgMoveToCell;

			LONG lRandomMoveTime;	//乱走的时间

			struct STAND //站立
			{
				LONG lStandTimes;
				LONG lStandTime;
			}tgStand;
					
			struct SENDEXPACTION  //播放动作
			{
				LONG lActionID;
				LONG lActTime;
				LONG lAimType;
				
			}tgSendExpAct;

			struct LEVELFIGHT
			{
				LONG lTime;
				LONG AimX;
				LONG AimY;
			}tgLevelFight;

		} unActType;				//触发行为
		CGUID gAimGUID;

	};

public:
	CBaseAI					();
	virtual	~CBaseAI		();
	//CBaseAI					(CBaseAI& AI);
	//VOID operator=			(CBaseAI& AI);
	
//定时器封装基本操作
protected:
	//添加一个ai事件
	BOOL						AddAIEvent				(AI_EVENT_TYPE iOutTimeType ,DWORD dwOutTime=0);
	//删除一个AI事件
	BOOL						DeleteAIEvent			(AI_EVENT_TYPE iOutTimerType);
	//在定时器上注销一个事件
	BOOL						LogoutEvent				(AI_EVENT& aeAiEvent);


//行为框架
public:
	//事件驱动的调用接口
	virtual BOOL				Run						(DWORD dwTimerID=0,int iOutTimeType=0);
protected:
	////周期调用函数
	virtual BOOL				OnCycle					();
	////主动行为响应
	virtual BOOL				OnActive				();

	////主动行为处理的不同状态。分为  和平，战斗，逃跑，死亡，挂起
	virtual BOOL				OnPeaceState			();
	virtual BOOL				OnFightState			();
	virtual BOOL				OnEscapeState			();
	virtual BOOL				OnNoHpState				();
	virtual BOOL				OnDeadState				();
	virtual BOOL				OnTransitionState		();

//其他定时器消息就及外部调用函数
public:
	// 重置状态，用于死亡等清除处理
	virtual void Reset();

	//启动主动行为
	VOID						StartActiveAct			();
	//注销当前的主动行为
	BOOL						LogoutCurrentEvent		(){return LogoutEvent(m_aeActive);}
	//添加改变ai事件
	VOID						StartDiedEvent			(DWORD dwTime){AddAIEvent(AET_DIED,dwTime);}
	//进入战斗
	virtual void				EnterFight				(){};
public:
	//改变ai事件
	virtual BOOL				OnChangeAI				();
	//处理出生
	VOID						OnBoring				();
	//处理死亡
	//BOOL						OnCheckDeadState		(){return OnDeadState();}		

	//转换为和平状态
	virtual BOOL				OnReturnPeace			();
	//当技能结束的时候，（技能结束的时候调用）
	virtual BOOL				OnSkillEnd				(long lAddSub);
	//
	//virtual BOOL				CheckReturnPeace		(){return FALSE;}

//具体行为
public:
	//向一个指定格子移动
	virtual BOOL				MoveTo					( float fDestX, float fDestY);
	//取得坐标
	BOOL						GetNearPosition				(LONG &x, LONG &y, LONG dir=-1,LONG searchDir=-1);
protected:
	//在一个方向上移动
	virtual BOOL				MoveTo					( long lDir) {return TRUE;}

	//寻找技能
	virtual BOOL				SearchSkill				(CMoveShape* pTarget);
	//使用技能
	virtual BOOL				BeginSkill				(stModuParam *pModuParam, LONG lx=0, LONG ly=0,CMoveShape* pTarget=NULL);

	//沿路点行走
	virtual BOOL                RunByRoad				();

	//随意走动
	virtual BOOL				RandomRun				();
	//和平周期的无任务处理
	virtual BOOL				OnPeaceIdle				();
	//处理周期触发
	BOOL						CheckCycleSpring		(SCycleSpringEvent& cycleevent);


//执行的检测和执行移动
protected:
	//判断该格子是否能移动
	virtual BOOL				IsCanMoveTo			(CRegion* pRegion,LONG lX, LONG lY);
	//就检测能否移动到指定的格子
	virtual BOOL				MoveToCell			(LONG lX, LONG lY);
	virtual BOOL				MoveToCell			(LONG lDir,LONG lCurrentX,LONG lCurrentY);
	//得到下一个移动的目标点
	BOOL GetNextMoveDestCell(long lDir,long lMaxDistance,long &lX,long &lY);
	BOOL GetNextMoveDestCell(long lDestX,long lDestY,long lMaxDistance,long &lX,long &lY);
	//执行移动
	virtual BOOL				OnMove					(LONG lX, LONG lY,LONG actTime);
	//删除周期触发
	bool						CleanCycleSpring		();


//改变状态
protected:
	virtual	VOID				ChangeStateTo			(INT state);
	//回到和平状态
	virtual BOOL                ReturnPeace				();


//脚本触发：战斗周期触发，战斗触发，重伤触发，死亡触发
public:
	virtual DWORD				CycleSpring				();
	virtual DWORD				BornSpring				();
	virtual DWORD				FightSpring				();
	virtual DWORD				GBHSpring				(int hurt);
	virtual DWORD				DeadSpring				();
	virtual DWORD				FindTargetSpring		();
	virtual DWORD				EscapeSpring			();

//自身属性的设置
public:
	//设置所有者
	CMoveShape*					GetOwner				();
	VOID						SetOwner				(CMoveShape* pOwner);
	//AI类型
	LONG						GetAIType				();
	VOID						SetAIType				(LONG lAIType);
	//取得当前战斗状态中的具体状态
	AI_EXEC_STATE				GetCurrentActiveAction	(){return m_aesAIState;}
	//是否正在使用技能
	BOOL						IsInAct					();
//目标的设置
public:
	//目标： 设置，询问，获取类型，获取id，获取指针
	virtual VOID				SetForceTarget			( LONG lType, const CGUID& TargetGuid );
	virtual VOID				CleanForceTarget		();
	virtual VOID				SetTarget				( LONG lType, const CGUID& TargetGuid );
	BOOL				        HasTarget				();
	LONG						GetTargetType			();
	const CGUID&				GetTargetID				();
	CMoveShape*					GetTarget				();
	//丢失目标
	virtual	BOOL				WhenLoseTarget			();
protected:
	BOOL						LoseTarget              ();
	//virtual BOOL				ForceTarget				(CMoveShape * pTarget , DWORD dwForceTime);
//冬眠：冬眠，苏醒，获取冬眠时间，查询是否冬眠
public:
	virtual VOID				WakeUp					();
	BOOL						IsHibernated			();
protected:	
	virtual VOID				Hibernate				();
	DWORD						GetDormancyInterval		();


//外部接口
public:
	//当被攻击的时候
	virtual BOOL				WhenBeenHurted			(LONG lType,const CGUID& guid,DWORD a);
	//被杀害的时候
	virtual BOOL				WhenBeenKilled			(LONG lType,const CGUID& guid);
	//攻击
	virtual void				Attack					( long tID, LONG lX=0, LONG lY=0, CMoveShape* pTarget=NULL );

//其他函数
public:
	//加入周期触发
	BOOL						AddCycleSpring			(CYCLE_SPRINGACT type, LONG m_1, LONG m_2=0, LONG m_3=0,CGUID guid= NULL_GUID);
	enum{ RIGHT=-1, ERROR_MOVING, ERROR_SKILLUSING, ERROR_FIGHTSTATE};
	//能否使用技能
	virtual	INT					CanUseSkill				(BOOL bRunUse){return 0;}
	//##清除AI管线中所有事件的接口
	virtual VOID				Clear					();


	//添加空闲时候的脚本名
	virtual VOID                AddIdleActScript		(string & strScriptName);


//脚本接口
public:
	//使用一个技能
	BOOL				UseSkillByID			(LONG skillid,LONG skilllevel, const CGUID &targetID = NULL_GUID);
	//对指定格子释放技能，缺省对当前目标所在坐标释放
	BOOL				UseSkillByIDOnCell		(LONG skillID, LONG skillLevel, LONG posX=0, LONG posY=0);
	//在和平状态中移动到某点
	BOOL				MoveToCellOnPeace		(LONG x,LONG y);
	//在战斗状态中移动到某点
	BOOL				MoveToCellOnFight		(LONG x,LONG y);
	//移动都某点的附近，如果已经在这范围内，则不一定。如果没在，就移动到这点上。
	BOOL				MoveToCellAround		(LONG x,LONG y);
	//在和平状态下往某一方向移动几步
	BOOL				MoveByStepOnPeace		(LONG dir, LONG step);
	//在战斗状态下往某一方向移动几步
	BOOL				MoveByStepOnFight		(LONG dir, LONG step);

	//在和平状态下播放一个动作
	BOOL				SendExpActOnPeace		(LONG lActID,LONG lAimTime,LONG lAimType=0,CGUID guid=NULL_GUID );
	//在战斗状态下播放一个动作
	BOOL				SendExpActOnFight		(LONG lActID,LONG lAimTime,LONG lAimType=0,CGUID guid=NULL_GUID );

	//在和平状态下往某方向移动多少时间
	BOOL				MoveByTimeOnPeace		(LONG dir, LONG time);
	//在战斗状态下往某方向移动多少时间
	BOOL				MovebyTimeOnFight		(LONG dir, LONG time);
	//在战斗状态的使之随意走动
	BOOL				RandomMoveByTime		(LONG time);
	//站立一段时间	（次数，每次停留的时间）
	BOOL				Stand					(LONG times,LONG time);
	//就站立一段时间 
	VOID				Stand					(LONG time);
	//逃跑一段时间(和平状态下，  只需要模拟逃跑，即为反方向行走)
	BOOL				EscapeByTimeOnPeace		(LONG dir,LONG time);
	//逃跑一段时间（战斗状态下）
	BOOL				EscapeByTimeOnFight		(LONG time);
	//脱离战斗状态处理
	BOOL				LevelFight				(LONG time,LONG aimX,LONG aimY);
	//取消注册的行为
	BOOL				DeleteSpring			();
	//关闭逃跑状态
	BOOL				EndEscape				();
	//开始周期行为
	VOID				StartCycle				(DWORD dwStatrTime, DWORD dwCycleTime);
	//结束周期行为
	VOID				EndCycle				();

protected:
	LONG						m_lAIType;					//##AI类型编号
	DWORD						m_dwLastExecTimeStamp;		//##AI最近一次执行的时间

	CGUID						m_TargetGuid;				//##攻击对象ID
	LONG						m_lTargetType;				//##攻击对象类型

	LONG						m_lTargetX;					//##目标X
	LONG						m_lTargetY;					//##目标Y

	CMoveShape*					m_pOwner;					//##AI主人 怪物 OR 玩家
	//
	SCycleSpringEvent			m_csCycleSpring;			//周期触发事件 
	string						m_strIdleActScript;			//空闲行为脚本
	AI_EXEC_STATE               m_aesAIState;               //AI状态
	AI_EVENT                    m_aeActive;					//主动行为记录
	AI_EVENT                    m_aeCycle;					//周期行为记录
	AI_EVENT					m_aeScriptAISkill;			//脚本AI技能
protected:
	DWORD						m_dwDormancyTime;			// 休眠开始时间
	DWORD						m_dwDormancyInterval;		// 休眠时间间隔
//

//帮助测试的变量
	
	int checknum;

public:
	friend class CMoveShape;
	friend class CPlayer;
	friend class CGibe;
	friend class CPet;
};