#include "stdafx.h"
#include "CPet.h"
#include "CSummonPet.h"
#include "Player.h"
#include "../GameClient/Game.h"
CSummonPet::CSummonPet()
{
	m_PetType = PET_TYPE_CONJURED;
}

CSummonPet::~CSummonPet()
{
	
}

void CSummonPet::AI()
{
	CMoveShape::AI();
	if (!CheckBornIsComplete())
		return;
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID() ||GetState() == STATE_DIED)
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
			long x = m_pCurAttackShape->GetPosX() ;
			long y = m_pCurAttackShape->GetPosY() ;
			iRet = AutoMoveAI_A(x,y);
		}
		break;
	case FMR_Trace_Near:
		//if((IsArrive() == true || GetAction() == ACT_STAND || GetAction() == ACT_IDLE))
		//{	
		//	//iRet = FollowObjectAI(m_pCurAttackShape);
		//	long x = pMaster->GetPosX() ;
		//	long y = pMaster->GetPosY() ;
		//	
		//	long lDir = GetLineDir(lX, lY, x, y);
		//	CClientRegion *pRegion = GetGame()->GetRegion();
		//	if(pRegion == NULL) break;
		//	pRegion->GetLeastShapesCellFromSpace(x, y, 0, 0,
		//		1, lDir,x,y);
		//	iRet = AutoMoveAI_A(x,y);
		//}
		m_lCurUseSkillPos++;
		break;
	case FMR_Attack:
		{
			if (GetAction() == ACT_RUN || GetAction() == ACT_WALK )
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
					CShape::eDIR ePetDir = GetRandomPlaceBesideMaster();
					pMaster->GetPosFromRelativeDir(ePetDir,lDestX,lDestY);
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
					iRet = AutoMoveAI_A(lDestX,lDestY);
				}
			}
			else
			{	
				if ((GetAction() == ACT_RUN || GetAction() == ACT_WALK))
				{	
					PetStopMove();
					SetAction(ACT_STAND);
					SetDestXY(GetTileX(),GetTileY());
				}
			}
		}
		break;
	
	case	FMR_SKILL_NOT_FINISH:
	case	FMR_SKILL_ACTION_NOT_FINISH:
		if ((GetAction() == ACT_RUN || GetAction() == ACT_WALK))
		{	
			PetStopMove();
			SetAction(ACT_STAND);
			SetDestXY(GetTileX(),GetTileY());
		}
		break;
	case	FMR_NO_SKILL:

	case	FMR_SKILL_CD_NOT_ARRIVAL:
		
	case	FMR_SKILL_NOT_ENOUGH_MP:
		
	case	FMR_SKILL_IS_NOT_AUTO:
		
	case	FMR_SKILL_LEVEL_FAIL:
		if ((GetAction() == ACT_RUN || GetAction() == ACT_WALK))
		{	
			PetStopMove();
			SetAction(ACT_STAND);
			SetDestXY(GetTileX(),GetTileY());
		}
		m_lCurUseSkillPos++;
	default:
		break;
	}
	//GetGame()->GetCGuiEx()->GetPetHotKeyBarPage()->UpdateSkillCoolDown(m_PetType);
}

bool CSummonPet::Display()
{
	if( !CPet::Display() )
		return false;
	return true;
}

bool CSummonPet::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodeFromDataBlock(setReadDB,bExData);
	if(!ret)
		MessageBox(g_hWnd,"CSummonPet Decoded failed!!","error",MB_OK);
	//设置等级

	m_bIsActived = true;
	return true;
}