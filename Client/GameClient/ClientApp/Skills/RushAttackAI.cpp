#include "StdAfx.h"
#include "RushAttackAI.h"
#include "Monster.h"
#include "Player.h"
#include "SkillListXml.h"
#include "Effect.h"
#include "PetSystem/Pet.h"
#include "SkillXml.h"
#include "ClientRegion.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRushAttackAI::CRushAttackAI()
{
	m_fStartX = 0.0f;
	m_fStartY = 0.0f;
	m_fEndX = 0.0f;
	m_fEndY = 0.0f;
	m_pTrail = NULL;
	m_pSkill = NULL;
}
CRushAttackAI::CRushAttackAI(CSkillXml* pSkill)
{
	m_fStartX = 0.0f;
	m_fStartY = 0.0f;
	m_fEndX = 0.0f;
	m_fEndY = 0.0f;
	m_pTrail = NULL;
	m_pSkill = pSkill;
}
CRushAttackAI::~CRushAttackAI()
{
	tagTrail *pTrail, *p;
	pTrail = m_pTrail;
	while (pTrail)
	{
		p = pTrail;
		pTrail = pTrail->pNext;
		SAFE_DELETE(p);
	}
}
void CRushAttackAI::AI()
{
	DWORD dwTime = GetCurTickCount();
	if(GetGame()->GetMainPlayer()->GetType() == m_pSkill->GetSourType() &&
		GetGame()->GetMainPlayer()->GetExID() == m_pSkill->GetSourID())
	{
		if ((long)(dwTime - m_dwStartTime) < (long)m_dwFlyingTime)
		{
			// 先停留一段时间
			if (dwTime < m_dwStartTime)
			{
				GetGame()->SetScrollCenterX(m_fStartX);
				GetGame()->SetScrollCenterY(m_fStartY);
				return;
			}

			float x = (dwTime-m_dwStartTime) * (m_fEndX - m_fStartX) / m_dwFlyingTime + m_fStartX;
			float y = (dwTime-m_dwStartTime) * (m_fEndY - m_fStartY) / m_dwFlyingTime + m_fStartY;

			GetGame()->SetScrollCenterX(x);
			GetGame()->SetScrollCenterY(y);
		}
	}
}
bool CRushAttackAI::Display()
{
	CShape *pShape = GetGame()->GetMainPlayer();
	CClientRegion *pRegion = GetGame()->GetRegion();
	render::Camera *pCamera = pRegion->GetCamera();

	// 获得释放者
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if(pMoveshape)
	{
		// 释放阶段
		if(m_pSkill->GetCurStep() == CSkillXml::STEP_RUN && m_pSkill->GetRunStepActExQueue())
		{
			// 释放阶段续动作列表不为空，且当前动作是站立，将当前动作设置为后续动作列表的第一个动作
			if(!m_pSkill->GetRunStepActExQueue()->empty() && pMoveshape->GetAction() == CShape::ACT_STAND)
			{
				pMoveshape->SetAction(m_pSkill->GetRunStepActExQueue()->front());
				m_pSkill->GetRunStepActExQueue()->pop();
			}
		}
	}
	return true;
}

bool CRushAttackAI::StepBeginAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
// 执行具体功能(返回false表示删除)
bool CRushAttackAI::StepRunAI(vector<long> vecType, vector<CGUID> vecID)
{
	CMoveShape *pSour = (CMoveShape*)GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID());
	if (pSour == NULL)
		return false;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pSour);
	if(pPlayer == NULL)
		return false;

	m_fStartX = pPlayer->GetPosX();
	m_fStartY = pPlayer->GetPosY();

	CMoveShape* pDest = (CMoveShape*)GetGame()->GetRegion()->FindChildObject(m_pSkill->GetDestType(), m_pSkill->GetDestID());
	if (pDest && m_pSkill->GetDestX() == pPlayer->GetTileX() && m_pSkill->GetDestY() == pPlayer->GetTileY())
	{
		m_fEndX = pDest->GetTileX()+0.5f;
		m_fEndY = pDest->GetTileY()+0.5f;
	}
	else
	{
		m_fEndX = m_pSkill->GetDestX()+0.5f;
		m_fEndY = m_pSkill->GetDestY()+0.5f;
	}

	m_dwStartTime = GetCurTickCount() + 1000;
	m_dwFlyingTime = 2000;
	
	if (m_pSkill->GetDestX() != pPlayer->GetTileX() &&
		m_pSkill->GetDestY() != pPlayer->GetTileY())
	{
		//pPlayer->SetPosXY(m_fEndX, m_fEndY);
	}
	pPlayer->SetDir((CShape::eDIR)(GetLineDir(m_fStartX, m_fStartY, m_fEndX, m_fEndY)));
	pPlayer->SetNowDir((float)pPlayer->GetDir());

	return true;
}
// 执行结束处理(返回false表示删除)
bool CRushAttackAI::StepEndAI()
{
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if (pMoveshape->GetType() == TYPE_PET)
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