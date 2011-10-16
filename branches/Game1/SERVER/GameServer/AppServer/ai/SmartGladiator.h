#pragma once
#include "monsterai.h"


/*
*AI基本类型:肉搏骚扰
*/
class CSmartGladiator : public CMonsterAI
{
private:
	struct tagCell
	{
		LONG lX;
		LONG lY;
	};

	queue<tagCell>									m_qTarget;
	
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();
*/
public:
	CSmartGladiator									();
	~CSmartGladiator								();
/*
	virtual AI_EXEC_STATE		Run					();
	//virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	*/
};
