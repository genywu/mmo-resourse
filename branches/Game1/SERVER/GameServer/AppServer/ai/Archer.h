#pragma once
#include "MonsterAI.h"


class CArcher :	public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
*/
	virtual INT					Tracing					(CMoveShape* pTarget);

public:
	CArcher											();
	~CArcher										();

//	virtual AI_EXEC_STATE		Run					();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
};

