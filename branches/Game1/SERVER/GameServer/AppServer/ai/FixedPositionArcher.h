#pragma once
#include "MonsterAI.h"

class CFixedPositionArcher : public CMonsterAI
{
protected:
	virtual INT					Tracing					(CMoveShape* pTarget);
	virtual BOOL				RandomRun				();
	virtual BOOL				SearchEnemy				();
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
	virtual BOOL				OnFighting			();
	virtual BOOL				OnChangeSkill		();
*/
public:
	CFixedPositionArcher								();
	~CFixedPositionArcher							();
/*
	virtual AI_EXEC_STATE		Run					();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};
