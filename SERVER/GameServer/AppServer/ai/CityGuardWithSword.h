#pragma once
#include "MonsterAI.h"
#include "..\monster.h"

/*
*AI基本类型:肉搏普通
*/
class CCityGuardWithSword :
	public CMonsterAI
{
private:
	LONG						m_lX;
	LONG						m_lY;

protected:
	/*
	virtual VOID				OnSchedule				();
	virtual BOOL				OnSearchEnemy			();
	virtual VOID				OnIdle					();
	virtual BOOL				OnLoseTarget			();
	virtual BOOL				OnMoving				();

	virtual CMoveShape*			SearchEnemyGuildMember	();
	virtual CMoveShape*			SearchEnemyGuildPet		();
	virtual CMoveShape*			SearchEnemyGuildCarriage();

	//virtual BOOL				Tracing				();
*/
public:
	CCityGuardWithSword									();
	~CCityGuardWithSword								();
/*
	virtual AI_EXEC_STATE		Run						();
//	virtual VOID				WhenBeenHurted			( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};
