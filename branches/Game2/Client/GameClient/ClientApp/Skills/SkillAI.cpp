#include "StdAfx.h"

#include "SkillAI.h"

#include "SkillXml.h"
#include "NormalCloseInAttackAI.h"
#include "RushAttackAI.h"
#include "NormalLongDistanceAttackAI.h"
#include "RandomSummonAI.h"
#include "FixSummonAI.h"
#ifdef _DEBUG	
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSkillAI::CSkillAI(void)
{
}
CSkillAI::~CSkillAI()
{
}

void CSkillAI::AI()
{
}
bool CSkillAI::Display()
{
	return true;
}
bool CSkillAI::StepBeginAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool CSkillAI::StepRunAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool CSkillAI::StepEndAI()
{
	return true;
}
CSkillAI* CSkillAI::CreateSkillAI(CSkillXml* pSkill, long lAI)
{
	CSkillAI* pSkillAI = NULL;
	switch(lAI)
	{
	case 0:
		pSkillAI = new NormalCloseInAttackAI(pSkill);
		break;
	case 1:
		pSkillAI = new CRushAttackAI(pSkill);
		break;
	case 2:
		pSkillAI = new CNormalLongDistanceAttackAI(pSkill,CNormalLongDistanceAttackAI::FLY_TRACK);
		break;
	case 3:
		pSkillAI = new CRandomSummonAI(pSkill);
		break;
	case 4:
		pSkillAI = new CFixSummonAI(pSkill);
		break;
	case 5:
		pSkillAI = new CNormalLongDistanceAttackAI(pSkill,CNormalLongDistanceAttackAI::FLY_LOCK_LINE);
		break;
	default: pSkillAI = NULL; break;
	}
	return pSkillAI;
}
