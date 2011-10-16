#pragma once
#include "MonsterAI.h"
#include "..\Monster.h"

class CLord :
	public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
*/
public:
	CLord											();
	~CLord											();
/*
	virtual AI_EXEC_STATE		Run					();
	virtual VOID				SelectAttackSkill	();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};

