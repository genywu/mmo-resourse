#pragma once
#ifndef _GuardCountry2_h_
#define _GuardCountry2_h_
#include "MonsterAI.h"

class CGuardCountry2 : public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
	virtual BOOL				OnFighting			();
			CMoveShape*			SearchOtherCountry	();
*/
public:
	CGuardCountry2									();
	~CGuardCountry2									();
/*
	virtual AI_EXEC_STATE		Run					();
	//virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	*/
};
#endif