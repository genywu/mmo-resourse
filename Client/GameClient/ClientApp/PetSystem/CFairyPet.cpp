// 精灵 模块  nikai 2009/1/10/ 
#include "stdafx.h"
#include "CPet.h"
#include "CFairyPet.h"
#include "../GameClient/Game.h"
#include "Player.h"
CFairy::CFairy()
{
	m_emFoodsSatiefy = PFS_COMMON;
	m_PetType = PET_TYPE_ELF;
}

CFairy::~CFairy()
{

}
PET_FOOD_SATIETY CFairy::GetSatietyState(void)
{
	long lGorgeLv = m_tagPetDetail->lGorgeLevel;
	if (lGorgeLv > PFS_PLEASED)
	{
		lGorgeLv = PFS_PLEASED;
	}
	return (PET_FOOD_SATIETY)lGorgeLv;
}
void CFairy::AI()
{
	CMoveShape::AI();
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID())
		return;
	CPlayer	*pMaster = GetGame()->GetMainPlayer();
	CMoveShape *pTarget = pMaster;

	int iRet = -1;
	m_lFightAImode = PetFightAI();

	long lX = GetTileX();
	long lY = GetTileY();
	long lDestX = pTarget->GetTileX();
	long lDestY = pTarget->GetTileY();
	switch (m_lFightAImode)
	{
	case FMR_Trace_Far:		//追踪目标
		if((IsArrive() == true || GetAction() == ACT_STAND || GetAction() == ACT_IDLE))
		{	
			//iRet = FollowObjectAI(m_pCurAttackShape);
			long x = m_pCurAttackShape->GetPosX() ;
			long y = m_pCurAttackShape->GetPosY() ;
			iRet = AutoMoveAI_A(x,y);
		}
		break;
	case FMR_Trace_Near:;

	case FMR_Attack:
		{
			if ( !(GetAction() == ACT_RUN || GetAction() == ACT_WALK) )
			{
				PetStopMove();
				SetAction(ACT_STAND);
				SetState(STATE_FIGHT);
			}


			//释放技能更新方向
			lDestX = m_pCurAttackShape->GetTileX();
			lDestY = m_pCurAttackShape->GetTileY();
			float fRadianDir =  GetRadianDir(m_fPosX,m_fPosY,lDestX,lDestY);
			SetDirEx(fRadianDir);

			UsePetSkill(m_lCurUseSkillPos);
		}
		break;
	case FMR_No_TARGET:
		{
			switch (m_tagPetDetail->lMoveType)
			{	
			case PET_AS_FOLLOW:
				{
					pMaster->GetPosFromRelativeDir(CShape::DIR_RIGHTDOWN,lDestX,lDestY);
				}
				break;
			case PET_AS_STOP:
				{
					lDestX = m_lStayModePosX;
					lDestY = m_lStayModePosY;
				}
				break;
			default:
				break;
			}

			if( (abs(lX - lDestX) > FAR_FROM_TARGET_DISTANCE || abs(lY - lDestY) > FAR_FROM_TARGET_DISTANCE))  	//判断是否已经到达离主人最大允许范围内
			{	
				if (IsArrive() == true || !(GetAction() == ACT_WALK || GetAction() == ACT_RUN) ) //已经到达 或 动作不是移动。
				{
					//iRet = FollowObjectAI(lDestX,lDestY);
					iRet = AutoMoveAI_A(lDestX,lDestY);
				}
			}
			else
			{	
				if (GetAction() != ACT_STAND)
				{	
					PetStopMove();
					SetAction(ACT_STAND);
					SetDestXY(GetTileX(),GetTileY());
				}
			}
		}
		break;
	case	FMR_NO_SKILL:
		break;
	case	FMR_SKILL_CD_NOT_ARRIVAL:
		break;
	case	FMR_SKILL_NOT_ENOUGH_MP:
		break;
	case	FMR_SKILL_ACTION_NOT_FINISH:
		break;
	case	FMR_SKILL_IS_NOT_AUTO:
		break;
	case	FMR_SKILL_LEVEL_FAIL:
		break;
	default:
		break;
	}
}

bool CFairy::Display()
{
	if( !CPet::Display() )
		return false;
	return true;
}

bool CFairy::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodeFromDataBlock(setReadDB,bExData);
	if(!ret)
		MessageBox(g_hWnd,"CFairy Decoded failed!!","error",MB_OK);
	
	m_bIsActived = true;
	return true;
}
