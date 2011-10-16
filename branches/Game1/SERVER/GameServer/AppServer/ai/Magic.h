#pragma once
#include "MonsterAI.h"
//Ä§·¨¹¥»÷ÀàÐÍ

class CMagic: public CMonsterAI
{
public:
	CMagic();
	~CMagic();
protected:
	virtual BOOL Tracing(CMoveShape* pTarget);
	virtual BOOL SearchSkill(CMoveShape* pTarget);
	/*virtual BOOL				BeginSkill				(CSkill *pSkill,CMoveShape* pTarget);
	virtual BOOL				BeginSkill				(CSkill* pSkill, LONG lx, LONG ly);*/
};