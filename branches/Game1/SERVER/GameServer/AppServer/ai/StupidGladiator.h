#pragma once
#include "MonsterAI.h"
#include "..\Monster.h"

class CStupidGladiator :
	public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
*/
	BOOL				SearchEnemy				();
	BOOL				WhenBeenHurted			(LONG lType,const CGUID& guid, DWORD a);
public:
	CStupidGladiator									();
	~CStupidGladiator								();
/*
	virtual AI_EXEC_STATE		Run					();
	*/
};

