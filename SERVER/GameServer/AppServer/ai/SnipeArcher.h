#pragma once
#include "MonsterAI.h"

class CSnipeArcher: public CMonsterAI
{
protected:
	virtual BOOL				SearchEnemy				();
public:
	CSnipeArcher();
	~CSnipeArcher();

};