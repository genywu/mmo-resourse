#pragma once
#include "MonsterAI.h"

//##弱小生物的AI类型

class CPuninessCreature : public CMonsterAI
{
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();

	virtual BOOL				Tracing				();
*/
public:
	CPuninessCreature										();
	~CPuninessCreature									();
/*
	virtual AI_EXEC_STATE		Run					();
	*/
};
