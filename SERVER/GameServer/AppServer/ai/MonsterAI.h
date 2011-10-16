#pragma once
#include "baseai.h"

class CMoveShape;
class CPlayer;

class CMonsterAI : public CBaseAI
{
public:
	CMonsterAI();
	~CMonsterAI();

protected:
	//周期调用函数
	virtual BOOL				OnCycle					();
	//主动行为响应
	virtual BOOL				OnActive				();


	//主动行为处理的不同状态。分为  和平，战斗，逃跑，死亡，挂起
	virtual BOOL				OnPeaceState			();
	BOOL						OnPeaceStateOnColony	();
	virtual BOOL				OnFightState			();
	BOOL						OnFightStateOnColony	();
	virtual BOOL				OnEscapeState			();
	virtual BOOL				OnNoHpState				();
	virtual BOOL				OnDeadState				();
	virtual BOOL				OnTransitionState		();


	//和平周期的无任务处理
	virtual BOOL				OnPeaceIdle				();
	//处理返回和平状态
	virtual BOOL				CheckReturnPeace		();
	//就处理改变ai
	virtual BOOL				OnChangeAI				();



//行动
protected:
//寻敌
	//（重新）寻找敌人
	virtual BOOL				SearchEnemy				();
	//随机搜索敌人
	BOOL						SearchEnemyRandom		(int iDisMax,int iDisMin);
	//是否能执行搜索
	BOOL						CanSearchEnemy			();
	//执行搜索敌人
	virtual BOOL				CheckSearchEnemy		();
	//在集群中搜索敌人
	BOOL						SearchEnemyOnColony		();
	//在集群中寻找最近的且有目标的成员
	CMonster*					SearchFriendOnColony	();
	//用脚本寻找敌人
	virtual BOOL				SearchEnemyByScript		();
//跑动
	//追击
	enum{ TRACE_RESULT_SUCCESS=0,TRACE_RESULT_TRACE,TRACE_RESULT_OUT,};
	virtual INT					Tracing					(CMoveShape* pTarget);
	//逃跑
	virtual BOOL				Escape					();
		    VOID				Escape					(CRegion* pRegion, LONG lX, LONG lY, BOOL bRun = FALSE );
			VOID				Escape					(CRegion* pRegion, LONG lDir, BOOL bRun = FALSE);

	//判断该格子是否能移动
	BOOL						IsCanMoveTo			(CRegion* pRegion,LONG lX, LONG lY);
	//向目标方向移动
	virtual BOOL				MoveTo					(long lDir);
	//走路径
	virtual BOOL                RunByRoad				();
	//随机走动
	virtual BOOL				RandomRun				();
	BOOL						RandomRunOnColony		();
	//随机状态处理
	BOOL						CheckRandomRunState		();
//选择技能
	//寻找技能
	virtual BOOL				SearchSkill				(CMoveShape* pTarget);
	BOOL						SearchOneTypeSkill		(DWORD dwSkillID,DWORD dwSkillLevel,DWORD dwType,LONG lDistance);
	//使用技能
	virtual BOOL				BeginSkill				(stModuParam* pSkill, LONG lx=0, LONG ly=0,CMoveShape* pTarget=NULL);

//处理周期触发
	//BOOL						CheckCycleSpring		(SCycleSpringEvent& cycleenent);

//触发：战斗周期触发，战斗触发，重伤触发，死亡触发
protected:
	virtual DWORD				CycleSpring				();
	virtual DWORD				BornSpring				();
	virtual DWORD				FightSpring				();
	virtual DWORD				GBHSpring				(int hurt);
	virtual DWORD				DeadSpring				();
	virtual DWORD				FindTargetSpring		();
	virtual DWORD				EscapeSpring			();
	DWORD						ScriptSpring			(CMonster* pMonster, const string& str);
//状态的改变
protected:
	virtual	VOID				ChangeStateTo			(INT state);
	virtual BOOL                ReturnPeace				();

//接口函数
public:
	//当被攻击的时候
	virtual BOOL				WhenBeenHurted			(LONG lType,const CGUID& guid, DWORD a);
	//被杀害的时候
	virtual BOOL				WhenBeenKilled			(LONG lType,const CGUID& guid);
	//丢失目标
	virtual	BOOL				WhenLoseTarget			();
	//重生
	BOOL						OnReBorn				();

//路径相关
protected:
	//当被攻击的时候(有集群的时候)
	BOOL						WhenBeenHurtedOnColony			(LONG lType,const CGUID& guid, DWORD);
	//VOID						CheckLevelRoad			();
	enum //顺路径寻路的返回类型
	{
		NEXTSTEP_RESULT_NULL=0,	//下一步
		NEXTSTEP_RESULT_NEXT,	//下一个节点，不是首尾节点
		NEXTSTEP_RESULT_RETURN,	//到首尾节点时候转向
		NEXTSTEP_RESULT_NEXTCYCLE,//当路径首尾相连的时候，当走到尾点时候，转到下个周期的头节点
	};
	//计算出下一步要走的位置，并返回是进入下个记录段或则是停留或是。。
	virtual INT					CountNextStepByRoad		(LONG &lX, LONG &lY,INT &CurrentIndex);
	//飞回到和平记录点，或则是路径点
	VOID						FlyToRecordePoint		();	
	//
	BOOL						FlyToFriendOnColony		(CMoveShape* pMoveShape);

public:
	//记录出生点
	VOID						RecordBornPoint			(int x, int y);
	//添加路径
	virtual VOID				AddRoad					(vector<POINT>&);
	//完全恢复，包括ai也调整回以前的状态
	// bForce : 是否强制resume，为真时会忽略‘恢复’标志
	virtual VOID				ResumeAll				( bool bForce = false );
	//死亡处理
	BOOL						OnCheckDeadState		(){return OnDeadState();}
	//进入战斗状态
	void						EnterFight				();//{ChangeStateTo(CShape::STATE_FIGHT);}

	// 判断是否还处于友好时间内，处于友好时间的怪物不索敌。
	BOOL IsInKindlyTime();

//脚本接口
public:
	virtual VOID				SetTarget				( LONG lType, const CGUID& TargetGuid );
	VOID						SetForceTarget		( LONG lType, const CGUID& TargetGuid );
	VOID						CleanForceTarget	();
	//呼唤周围的同类怪物
	BOOL				CallFriendAround		();
	//呼唤周围的所有怪物
	BOOL				CallMonsterAround		();
	//注册改变ai事件
	BOOL				AddChangeAIEvent		(LONG AIType);
	//取得路径
	vector<POINT>&		GetRoad					(){ return m_vectRoad;}	
	//有无路径记录
	bool				HasRoad					(){return m_vectRoad.size()!= 0;}

	// 立即施放技能，失败会添加一个AI定时器以驱动AI继续执行
	bool UseSkillSyncSafe( long skillType, long skillID, long skillLevel, const CGUID &targetID = NULL_GUID );
	// 立即对格子施放技能，失败会添加一个AI定时器以驱动AI继续执行
	bool UseSkillOnCellSyncSafe( long skillType, long skillID, long skillLevel, long targetX = 0, long targetY = 0 );
	// 立即施放技能
	bool UseSkillSync( long skillType, long skillID, long skillLevel, long targetX, long targetY, CMoveShape *pTarget = NULL );

//记录选择技能记录函数
protected:

	//记录选择出的技能： 技能id， 技能等级， 技能记录类型， 技能索引
	VOID				RecordChoiceSkill(DWORD SkillID,DWORD SkillLevel,WORD SkillType,WORD SkillIndex);
	//记录使用当前的技能
	VOID				RecordUsedCurrentSkill();
	//清除选择出的技能
	VOID				CleanChoiceSkillRecord();
	//是否已经选择出了技能
	BOOL				HasChoiceSkill(){return m_dwChoiceSkillID!=0;}
	//
	WORD				GetChoiceSkillType(){return m_wSkillType;}
private:
	int mm;
	//选择技能id
	DWORD m_dwChoiceSkillID;
	//选择技能等级
	DWORD m_dwChoiceSkillLevel;
	//选择技能类型
	WORD  m_wSkillType;
	//选择技能索引
	WORD  m_wSkillIndex;
	//是否必须被使用
	BOOL  m_bMustUsed;
	//是否被使用
	BOOL  m_bBeenUsed;
protected:
	//重伤标识
	//float                       m_fGBHStamp;				
	//警戒时间标识
	DWORD                       m_dwGuardTimeStamp;			
	//最后被攻击时间标识
	DWORD						m_dwHurtedTimeStamp;		
	//技能间隔时间标志
	DWORD                       m_dwSkillSpaceTimeStamp;	
	//过渡阶段的时间标志
	DWORD						m_dwTransitionTimeStamp;
	// 怪物重生时时间
	DWORD m_dwRebornTime;

	//路径相关
protected:
	//出生点
	POINT						m_BornPoint;				
	//路径
	vector<POINT>				m_vectRoad;					
	//行走点记录
	INT                         m_iRoadRecord;				
	//行走方向
	INT                         m_iRoadDirect;             
	//离开路径的记录点
	POINT                       m_LeaveRoadPoint;			
	//进入战斗状态点
	POINT						m_EnterFightPoing;			
	//是否被守卫攻击过
	bool m_bAttackedByGuard;
public:
	bool IsAttackedByGuard() const
	{
		return m_bAttackedByGuard;
	}
public:
	friend class CMoveShape;
};
