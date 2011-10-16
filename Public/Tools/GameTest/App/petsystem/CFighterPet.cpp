#include "stdafx.h"
#include "CFighterPet.h"
#include "..\..\App\MoveShape.h"
#include "..\..\App\Player.h"
#include "..\..\Utility\FindPath.h"
#include "..\..\Utility\BasePetDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CFighterPet::CFighterPet()
{
	m_lPetType = PET_TYPE_HUNTER;
}

CFighterPet::~CFighterPet()
{

}

bool CFighterPet::DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodePetFromDataBlock(setReadDB,bExData);
	return true;
}

void CFighterPet::AI()
{
	if( m_pMaster && m_pRegion )
	{
		CMoveShape* pAimShape = m_pMaster;
		long lX = GetTileX();
		long lY = GetTileY();
		long lAimX = pAimShape->GetTileX();
		long lAimY = pAimShape->GetTileY();

		//	¸úËæÖ÷ÈËÒÆ¶¯
		if( abs(lX-lAimX) > 3 || abs(lY-lAimY) > 3 )
		{
			if( IsActive())
				AutoMoveAI(lAimX,lAimY);
		}
	}
}