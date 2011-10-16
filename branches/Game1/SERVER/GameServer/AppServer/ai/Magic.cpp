#include "StdAfx.h"
#include "Magic.h"
#include "..\Player.h"
#include "..\Monster.h"
#include <mmsystem.h>



CMagic::CMagic()
{
}

CMagic::~CMagic()
{
}
BOOL CMagic::Tracing(CMoveShape* pTarget)
{
	if ( timeGetTime()<m_dwSkillSpaceTimeStamp  && m_dwSkillSpaceTimeStamp!=0)//这块具体来做。还需要改动
	{
		if (!0)//EscapeNearEnemy(3)
		{
			AddAIEvent(AET_MOVE,SKILL_SPACE_TIME);
		}
		return TRACE_RESULT_TRACE;
	}
	else
	{
		//m_dwSkillSpaceTimeStamp=0;
		return CMonsterAI::Tracing(pTarget);
	}
}

BOOL CMagic::SearchSkill(CMoveShape* pTarget)
{
	CMonsterAI::SearchSkill(pTarget);
	//if (!CBaseAI::SearchSkill(pTarget))
	//{
	//	return FALSE;
	//}

	//CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	//vector<CMonster::tagSkill> SkillList= pOwner->GetSkillList();
	//int SkillNum=SkillList.size();
	//if (SkillNum==0)
	//{
	//	return FALSE;
	//}
	//else
	//{
	//	int SkillIndex=random(SkillNum);
	//	pOwner->AddActiveSkill(SkillList[SkillIndex].wID, SkillList[SkillIndex].wLevel);
	//}

	return TRUE;
}

//BOOL CMagic::BeginSkill(CSkill *pSkill,CMoveShape* pTarget)
//{
//	//Stand(500);
//	AddAIEvent(AET_MOVE,700);
//	return FALSE;
//}
//BOOL CMagic::BeginSkill(CSkill* pSkill, LONG lx, LONG ly)
//{
//	AddAIEvent(AET_MOVE,700);
//	return FALSE;
//}