#include "stdafx.h"
#include "NormalCloseInAttackAI.h"
#include "../../EngineExtend/DisplayModel.h"
#include "PetSystem/Pet.h"
#include "../GameClient/Game.h"
#include "ClientRegion.h"
#include "SkillXml.h"
#include "SkillListIni.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

NormalCloseInAttackAI::NormalCloseInAttackAI()
{
	m_pSkill = NULL;
}
NormalCloseInAttackAI::NormalCloseInAttackAI(CSkillXml *pSkill)
{
	m_pSkill = pSkill;
}
NormalCloseInAttackAI::~NormalCloseInAttackAI()
{
	
}
void NormalCloseInAttackAI::AI()
{
	//CPlayer *pPlayer = GetGame()->GetMainPlayer();
	if(!m_pSkill)
		return;
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	// 开始阶段可能有吟唱动作
	if(pMoveshape && m_pSkill && m_pSkill->GetCurStep() != CSkillXml::STEP_START && m_pSkill->GetRunStepActExQueue())
	{
		/***********************************************************************/
		/* zhaohang fix */
		/***********************************************************************/
		// 非光环类技能
		CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(m_pSkill->GetSkillID());
		if (pSkill && (pSkill->bLockSkill || pSkill->dwSkillState == 0))
		//tagSkillIni* pSkill = SkillAttrIniSetup::GetSkillIni(m_pSkill->GetSkillID());
		//if( pSkill && pSkill->GetSkillBaseAttr() && 
		//	 pSkill->GetSkillBaseAttr()->IsLockSkill() )
		{
			// 释放阶段无后续动作
			if(m_pSkill->GetRunStepActExQueue()->empty())
			{
				if(pMoveshape->GetAction() == ACT_STAND || pMoveshape->GetAction() == ACT_IDLE)
					StepEndAI();
			}
		}
	}
}



/*
 * 功能: 技能显示
 * 摘要: -
 * 参数: -
 * 返回值: 始终true
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.11.10 - lpf
 *		增加了"仪态动作可切换"
 */
bool NormalCloseInAttackAI::Display()
{
	// 获得释放者
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
	if(pMoveshape)
	{
		if(m_pSkill->GetCurStep() == CSkillXml::STEP_START && m_pSkill->GetIntonateMainAct())
		{
			// 动作信息
			AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();

			// 如果处于吟唱阶段，那么在前期吟唱动作播放完之后，循环播放主要的吟唱动作
			if(pActionInfo &&
			   (pMoveshape->GetAction() == CShape::ACT_STAND ||
				pMoveshape->GetAction() == CShape::ACT_IDLE ||
				(pMoveshape->GetAction() >= CShape::ACT_FACEACTION && 
				pMoveshape->GetAction() <= CShape::ACT_FACEACTIONMAX)))		//仪态动作可切换 - add by lpf(2008.11.10)
			{
				pMoveshape->SetAction(m_pSkill->GetIntonateMainAct());
				pActionInfo->bCurActionLooped = TRUE;
			}
		}
		// 释放阶段
		else if(m_pSkill->GetCurStep() == CSkillXml::STEP_RUN && m_pSkill->GetRunStepActExQueue())
		{
			// 释放阶段续动作列表不为空，且当前动作是站立，将当前动作设置为后续动作列表的第一个动作
			if(!m_pSkill->GetRunStepActExQueue()->empty() && pMoveshape->GetAction() == CShape::ACT_STAND)
			{
				pMoveshape->SetAction(m_pSkill->GetRunStepActExQueue()->front());
				m_pSkill->GetRunStepActExQueue()->pop();
				// 已经是最后一个动作
				if(m_pSkill->GetRunStepActExQueue()->empty())
				{
					AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();
					if(pActionInfo)
						pActionInfo->bCurActionLooped = m_pSkill->IsActExCycle();
				}
			}
		}
	}
	return true;
}
bool NormalCloseInAttackAI::StepBeginAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool NormalCloseInAttackAI::StepRunAI(vector<long> vecType, vector<CGUID> vecID)
{
	return true;
}
bool NormalCloseInAttackAI::StepEndAI()
{
	if(m_pSkill)
	{
		CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_pSkill->GetSourType(), m_pSkill->GetSourID()));
		if(pMoveshape)
		{
			AnimInfo::tagActionInfo *pActionInfo = pMoveshape->GetAnimInfo()->GetActionInfo();
			if(pActionInfo && pMoveshape->GetAction() >= CShape::ACT_PERFORM)
			{
				pActionInfo->bCurActionLooped = FALSE;
			}
		}
		if (pMoveshape->GetType() == TYPE_PET)
		{
			CPet *pPet = (CPet*)pMoveshape;
			if (pPet->IsMainPlayerPet())
			{
				tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(m_pSkill->GetSkillID());
				if(pPetSkill)
					pPetSkill->bIsSkillUsingEnd = true;
				//pPet->SkillSuccReceived();
			}
		}
		// 删除特效
		m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_START);
		m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_RUN);
		m_pSkill->DelAllEffectListByStep(CSkillXml::STEP_END);
		m_pSkill->SetIsDeleted(true);
		return true;
	}
	return false;
}
