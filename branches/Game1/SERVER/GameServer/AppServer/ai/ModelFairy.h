#pragma once
#include "MonsterAI.h"

class  CModelFairy: public CMonsterAI
{
public:
	CModelFairy();
	~CModelFairy();
protected:
	BOOL SearchSkill(CMoveShape* pTarget);
};