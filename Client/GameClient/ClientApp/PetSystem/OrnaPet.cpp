//装饰宠模块 nikai	2009.5.24

#include "stdafx.h"
#include "CPet.h"
#include "OrnaPet.h"
#include "Player.h"
#include "../GameClient/Game.h"
COrnaPet::COrnaPet()
{
	m_PetType = PET_TYPE_ORNA;
}

COrnaPet::~COrnaPet()
{

}

void COrnaPet::AI()
{
	CMoveShape::AI();
	if (!CheckBornIsComplete())
		return;
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID() ||GetState() == STATE_DIED)
		return;
	CPlayer	*pMaster = GetGame()->GetMainPlayer();
	long lDestX = pMaster->GetTileX();
	long lDestY = pMaster->GetTileY();
	long lX = GetTileX();
	long lY = GetTileY();
	CShape::eDIR ePetDir = GetRandomPlaceBesideMaster();
	pMaster->GetPosFromRelativeDir(ePetDir,lDestX,lDestY);
	if( (abs(lX - lDestX) > FAR_FROM_TARGET_DISTANCE || abs(lY - lDestY) > FAR_FROM_TARGET_DISTANCE))  	//判断是否已经到达离主人最大允许范围内
	{	
		if (IsArrive() == true || !(GetAction() == ACT_WALK || GetAction() == ACT_RUN) ) //已经到达 或 动作不是移动。
		{
			AutoMoveAI_A(lDestX,lDestY);
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

bool COrnaPet::Display()
{
	if( !CPet::Display() )
		return false;
	return true;
}

bool COrnaPet::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodeFromDataBlock(setReadDB,bExData);
	if(!ret)
    {
        MessageBox(g_hWnd,"Pet Decoded failed!!","error",MB_OK);
    }

	m_bIsActived = true;
	return true;
}