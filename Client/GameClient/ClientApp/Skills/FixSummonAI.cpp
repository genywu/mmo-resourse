#include "stdafx.h"
#include "PetSystem/Pet.h"
#include "FixSummonAI.h"
#include "SkillXml.h"
#include "Other/AudioList.h"
#include "../GameClient/Game.h"
#include "ClientRegion.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFixSummonAI::CFixSummonAI()
{
	m_pSkill = NULL;
	m_dwLifeTime = 0;
	m_dwTimeStamp = 0;
}
CFixSummonAI::CFixSummonAI(CSkillXml *pSkill)
{
	m_pSkill = pSkill;
	m_vecSummonEffect = m_pSkill->GetSummonEffect();
	m_vecSummonSound = m_pSkill->GetSummonSound();
	m_dwLifeTime = 0;
	m_dwTimeStamp = 0;
}
CFixSummonAI::~CFixSummonAI()
{
	m_vecSummonEffect.clear();
	m_vecSummonSound.clear();
}
void CFixSummonAI::AI()
{
	if(GetRemainedTime() == 0)
	{
		StepEndAI();
		return;
	}
	else
	{
		if(FindEffect(m_vecSummonEffect[0]) == NULL)
		{
			CClientRegion *pRegion = GetGame()->GetRegion();
			float x = GetPosX();
			float y = GetPosY();
			float h = pRegion->GetCellHeight(x, y);
			AddEffect(m_vecSummonEffect[0], x, y, h, true);

			char strFile[MAX_PATH];
			sprintf(strFile, "sounds/skills/%d.wav", m_vecSummonSound[0]);
			GetGame()->GetAudioList()->Play(strFile, x, y, h);
		}
	}
}
bool CFixSummonAI::Display(void)
{
	return CShape::Display();
}
bool CFixSummonAI::StepBeginAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool CFixSummonAI::StepRunAI(vector<long> vecType, vector<CGUID> vecID)
{
	m_vecSummonEffect = m_pSkill->GetSummonEffect();
	m_vecSummonSound = m_pSkill->GetSummonSound();
	DWORD m_dwTotalTime = m_pSkill->GetSummonLifeTime();
	m_dwTimeStamp = GetCurTickCount();
	if(GetCurTickCount() >= m_dwTimeStamp + m_dwTotalTime)
		m_dwLifeTime = 0;
	else
		m_dwLifeTime = m_dwTimeStamp + m_dwTotalTime - GetCurTickCount();
	SetPosXY((float)m_pSkill->GetDestX(), (float)m_pSkill->GetDestY());
	return true;
}
bool CFixSummonAI::StepEndAI()
{
	DelAllEffect();
	m_pSkill->SetIsDeleted(true);
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if(pMoveshape && pMoveshape->GetType() == TYPE_PET)
	{
		CPet *pPet = (CPet*)pMoveshape;
		if (pPet->IsMainPlayerPet())
		{
			tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(m_pSkill->GetSkillID());
			if(pPetSkill)
				pPetSkill->bIsSkillUsingEnd = true;
		}
	}
	return true;
}
DWORD CFixSummonAI::GetRemainedTime()
{
	if(GetCurTickCount() >= m_dwTimeStamp + m_dwLifeTime)
		return 0;
	else
		return m_dwTimeStamp + m_dwLifeTime - GetCurTickCount();
}