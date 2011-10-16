#pragma once
#include "BaseAI.h"

class CPetAI : public CBaseAI
{
public:
	CPetAI();
	~CPetAI();

	virtual BOOL MoveTo(float fDestX, float fDestY);	// 向一个指定格子移动
	virtual DWORD BornSpring(void);						// 出生后状态等触发
	virtual BOOL OnActive(void);						// 活动状态AI处理
	virtual BOOL OnDeadState(void);
	virtual BOOL OnReturnPeace(void);					// 恢复到和平状态
	virtual BOOL OnSkillEnd(long lAddSub);				// 技能结束触发
	virtual void SetTarget(long lType,
		const CGUID& TargetGuid);						// 设置目标
	virtual void Attack(long lSkillID,
		long lX = 0,
		long lY = 0,
		CMoveShape* pTarget = NULL);
	virtual BOOL WhenBeenHurted(LONG lType,
		const CGUID& guid,
		DWORD a);										// 当被攻击的时候
	virtual BOOL WhenBeenKilled(LONG lType,
		const CGUID& guid);								// 宠物被杀死
	void AddRetPeaceAct(void);							// 注册返回和平事件
	void LogoutRetPeaceAct(void);						// 注销返回和平事件
	void SetHost(CMoveShape* pHost){ m_pHost = pHost; }	// 设置宠物主人

protected:
	bool SearchEnemy(void);								// 寻敌
	bool MissTarget(void);								// 丢失目标
	virtual	void ChangeStateTo(INT lState);

protected:
	BOOL		m_bFightUseSkill;
	long		m_lCount;
	long		m_lReturnPeaceTime;
	AI_EVENT	m_aeReturnPeace;
	AI_EVENT	m_aeRetPeace;
	CMoveShape*	m_pHost;								// 宠物主人
};