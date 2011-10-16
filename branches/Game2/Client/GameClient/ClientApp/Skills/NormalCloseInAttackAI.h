#pragma once
#include "SkillAI.h"

class NormalCloseInAttackAI : public CSkillAI
{
public:
	NormalCloseInAttackAI();
	NormalCloseInAttackAI(CSkillXml *pSkill);
	virtual ~NormalCloseInAttackAI();
	
	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepEndAI();

protected:
	CSkillXml *m_pSkill;
};