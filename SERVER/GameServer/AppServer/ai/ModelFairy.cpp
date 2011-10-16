#include "StdAfx.h"
#include "ModelFairy.h"
#include "..\Player.h"
#include "..\Monster.h"



CModelFairy::CModelFairy()
{

}

CModelFairy::~CModelFairy()
{

}

BOOL CModelFairy::SearchSkill(CMoveShape* pTarget)
{
	CMonsterAI::SearchSkill(pTarget);
	//CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	//vector<CMonster::tagSkill> & SkillList=pOwner->GetSkillList();
	//int SkillNum=SkillList.size();
	//if (SkillNum==0)
	//{
	//	return FALSE;
	//}
	//else if (SkillNum<=2 )
	//{
	//	int SkillIndex=random(SkillNum);
	//	pOwner->AddActiveSkill(SkillList[SkillIndex].wID, SkillList[SkillIndex].wLevel);
	//}
	//else if (random(100) < pOwner->GetSkillProportion())
	//{
	//	int SkillIndex=random(2);
	//	pOwner->AddActiveSkill(SkillList[SkillIndex].wID, SkillList[SkillIndex].wLevel);
	//}
	//else
	//{
	//	int SkillIndex=random(SkillNum-2)+2;
	//	pOwner->AddActiveSkill(SkillList[SkillIndex].wID, SkillList[SkillIndex].wLevel);
	//}
////////////
	//list<CNewSkill*> & SkillList= m_pOwner->GetNewSkillList();
	//CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	//int SkillNum=SkillList.size();
	//list<CNewSkill*>::iterator it=SkillList.begin();

	//if (SkillNum==0)
	//{
	//	m_pCurrentSkill=NULL;
	//	return FALSE;
	//}

	//if(CBaseAI::SearchSkill())
	//{
	//	if( SkillNum>2 && random(100) < pOwner->GetSkillProportion() )	//使用魔法的几率
	//	{

	//		it++;
	//		it++;//两次移动就指针，到非基本攻击技能。
	//		int SkillChangedNum= random(SkillNum-2);
	//		for (int a=0; a< SkillNum && it!=SkillList.end() ; a++)
	//		{
	//			it++;
	//		}
	//		m_pCurrentSkill=*it;
	//		return TRUE;
	//	}
	//	else
	//	{
	//		//list<CNewSkill*>::iterator it=SkillList.begin();
	//		CMoveShape* pTarget=GetTarget();
	//		if (m_pOwner->Distance(pTarget)> 1)
	//		{
	//			it++;
	//		}
	//		m_pCurrentSkill=*it;
	//		return TRUE;

	//	}

	//}
	//else												//使用基本攻击
	//{
	//	//list<CNewSkill*>::iterator it=SkillList.begin();
	//	CMoveShape* pTarget=GetTarget();
	//	if (m_pOwner->Distance(pTarget)> 1)
	//	{
	//		it++;
	//	}
	//	m_pCurrentSkill=*it;
	//	return TRUE;
	//}
	return TRUE;
}