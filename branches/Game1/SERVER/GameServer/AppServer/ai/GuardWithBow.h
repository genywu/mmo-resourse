#pragma once
#include "MonsterAI.h"

class CGuardWithBow : public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
	virtual BOOL				OnFighting			();
	virtual CMoveShape*			SearchCriminal		();
	virtual CMoveShape*			SearchMonster		();
*/
public:
	CGuardWithBow									();
	~CGuardWithBow									();
/*
	virtual AI_EXEC_STATE		Run					();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};
