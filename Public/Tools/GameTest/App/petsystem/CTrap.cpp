#include "stdafx.h"
#include "CTrap.h"

#include "CFairyPet.h"
CTrap::CTrap() 
{
	m_lPetType = PET_TYPE_TRAP;
}

CTrap::~CTrap()
{

}

bool CTrap::DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodePetFromDataBlock(setReadDB,bExData);
	return true;
}

void CTrap::AI()
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