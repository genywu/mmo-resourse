#pragma once
#include "BaseAI.h"
#include "..\Player.h"
//

const INT	FIGHT_STATE_TIME = 7000;
class CPlayerAI :public CBaseAI
{
private:
	//后备消息。只保留一个。
	struct tagMessage					
	{
		//				无类型		攻击某点		攻击某目标			使用副职业技能
		enum MessageType{MT_NULL=0,MT_USESKILL};

		tagMessage(void);
		VOID Clean(void);

		MessageType	mtType;		//消息类型

		LONG lSkillID;
		LONG lAttackX;
		LONG lAttackY;
		LONG lAttackType;
		CGUID AttackPlayerID;
		BOOL bIsRun;
	};

	tagMessage m_tNextAct;
	long	m_lCount;

protected:
	//主动行为
	virtual BOOL				OnActive			();
	//周期行为
	virtual BOOL				OnCycle				();
	//丢失目标
	//virtual BOOL				OnLoseTarget		();			
	//追击
	virtual INT					Tracing				();
	//随机走动
	BOOL						RandomRun			();
	//移动到某点
	BOOL						MoveTo				(float fDestX, float fDestY);
	//强制攻击
	BOOL						ForceMove			(long lDir);
	//攻击
	virtual void				Attack				( long tID, LONG lX=0, LONG lY=0, CMoveShape* pTarget=NULL );
	//技能结束时候调用
	virtual BOOL				OnSkillEnd			(long lAddSub);
	//当被攻击的时候
	virtual BOOL				WhenBeenHurted			(LONG lType,const CGUID& guid,DWORD a);

	//被杀害的时候
	virtual BOOL				WhenBeenKilled			(LONG lType,const CGUID& guid);

//////////////////////////////////////////////////////////////////////////
	//加入一个消息到后备消息中
	BOOL						CheckMessage		(tagMessage message);
	//处理后备消息
	BOOL						CheckNextAct		();

	//判断该格子是否能移动
	BOOL						IsCanMoveTo			(CRegion* pRegion,LONG lX, LONG lY);

	//是否可以使用技能
	BOOL						CanUseSkill			();
	//是否可以使用技能
	INT							CanUseSkill			(BOOL bRunUse);
public:
	CPlayerAI										();
	~CPlayerAI										();
//进入战斗状态，恢复到和平状态
public:
	//注册返回和平事件
	void						AddRetPeaceAct		();
	//注销返回和平事件
	void						LogoutRetPeaceAct	();
	//恢复到和平状态
	virtual BOOL				OnReturnPeace		();
	//进入战斗
	VOID						ChangeStateToFight	(BOOL bUseSkill);

//////////////////////////////////////////////////////////////////////////

	//开始技能动作
	//VOID						StartSkillAct		(LONG lTime);
	VOID						SetForceTarget		( LONG lType, const CGUID& TargetGuid );
	VOID						CleanForceTarget	();
	VOID						SetTarget			( LONG lType, const CGUID& TargetGuid );


	//
protected:
	LONG	m_iReturnPeaceTime;
	AI_EVENT m_aeRetPeace;
	BOOL	m_bFightUseSkill;

};
