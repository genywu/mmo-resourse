#pragma once
#ifndef _GuardCountry_h_
#define _GuardCountry_h_
#include "MonsterAI.h"

class CGuardCountry : public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
	virtual BOOL				OnFighting			();
			CMoveShape*			SearchOtherCountry	();
	virtual CMoveShape*			SearchCriminal		();
	virtual CMoveShape*			SearchMonster		();
*/
public:
	CGuardCountry									();
	~CGuardCountry									();
/*
	virtual AI_EXEC_STATE		Run					();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};
#endif