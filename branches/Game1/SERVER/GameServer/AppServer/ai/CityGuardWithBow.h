#pragma once
#include "MonsterAI.h"

class CCityGuardWithBow : public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule				();
	virtual VOID				OnIdle					();
	virtual BOOL				OnSearchEnemy			();
	virtual BOOL				OnMoving				();
	virtual BOOL				OnFighting				();
	virtual CMoveShape*			SearchEnemyGuildMember	();
	virtual CMoveShape*			SearchEnemyGuildPet		();
	virtual CMoveShape*			SearchEnemyGuildCarriage();
	*/
public:
	CCityGuardWithBow									();
	~CCityGuardWithBow									();
/*
	virtual AI_EXEC_STATE		Run						();
//	virtual VOID				WhenBeenHurted			( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};
