#include "stdafx.h"
#include "CFairyPet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CFairy::CFairy()
{
	m_lPetType = PET_TYPE_ELF;
}

CFairy::~CFairy()
{

}

bool CFairy::DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodePetFromDataBlock(setReadDB,bExData);
	return true;
}

void CFairy::AI()
{
	if( m_pMaster && m_pRegion )
	{
		CMoveShape* pAimShape = m_pMaster;
		long lFightMode = PetFightAI();
		long lX = GetTileX();
		long lY = GetTileY();
		long lAimX = pAimShape->GetTileX();
		long lAimY = pAimShape->GetTileY();
		int nRet = -1;
		switch(lFightMode)
		{
		case FMR_Trace_Far:
			if( IsActive() )
			{
				long x = m_pCurAtkShape->GetPosX();
				long y = m_pCurAtkShape->GetPosY();
				nRet = AutoMoveAI(x,y);
			}
			break;
		case FMR_Trace_Near:
		case FMR_Attack:
			{
				UsePetSkill(m_lCurUseSkillPos);
			}
			break;
		case FMR_No_TARGET:
			{
				//	¸úËæÖ÷ÈËÒÆ¶¯
				if( abs(lX-lAimX) > 3 || abs(lY-lAimY) > 3 )
				{
					if( IsActive())
						AutoMoveAI(lAimX,lAimY);
				}
			}
			break;
		default: break;
		}
	}
}