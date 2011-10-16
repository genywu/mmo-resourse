#pragma once
#include "MonsterAI.h"


class CStupidArcher :	public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
	virtual BOOL				OnFighting			();
*/
public:
	CStupidArcher									();
	~CStupidArcher									();
/*
	virtual AI_EXEC_STATE		Run					();
	//virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	*/
};

