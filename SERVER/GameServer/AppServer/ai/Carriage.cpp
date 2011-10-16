#include "StdAfx.h"
#include <MMSystem.h>
#include "Carriage.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"
#include "..\..\..\setup\globesetup.h"

//#include "../Skills/SkillBaseProperties.h"
//#include "../skills/SkillFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCarriage::CCarriage()
{
	m_dwInvalidMasterTimeStamp=0;
	m_dwSeekMasterTimeStamp=0;
	m_bMasterLogout=0;

	m_dwPetLifeCycleTimeStamp=0;
	m_dwPetLifeCycleCounter=0;
	m_caFollowState=STAYING;
}

CCarriage::~CCarriage()
{}
BOOL CCarriage::WhenBeenHurted()
{
	return TRUE;
}
CMoveShape*	CCarriage::GetMaster()
{
	CMoveShape* pResult = NULL;
	CMonster* pCarriage = dynamic_cast<CMonster*>( GetOwner() );
	if( pCarriage )
	{
		LONG lMasterType	= pCarriage -> GetMasterInfo().lMasterType;
		const CGUID& MasterID		= pCarriage -> GetMasterInfo().MasterID;

		if( lMasterType == TYPE_PLAYER )
		{
			pResult = GetGame() -> FindPlayer( MasterID );
		}
		else
		{
			CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pCarriage -> GetFather() );
			if( pRegion )
			{
				pResult = dynamic_cast<CMoveShape*>( pRegion -> FindChildObject(lMasterType, MasterID) );
			}
		}
	}
		return pResult;
}

VOID CCarriage::ChangeStateTo(AI_EXEC_STATE aes)
{
	CBaseAI::ChangeStateTo(aes);
	if (m_pOwner->GetState() != CShape::STATE_PEACE)// m_aesAIState!= AES_PEACE)
	{
		m_caFollowState=NOMONSTER;
	}
}

BOOL CCarriage::SearchEnemy()
{
	if (m_pOwner->GetState()== CShape::STATE_PEACE && m_caFollowState== NOMONSTER)
	{
		CMoveShape* pTarget=GetMaster();


		if (pTarget && this->m_pOwner->Distance(pTarget->GetTileX(),pTarget->GetTileY())< (dynamic_cast<CMonster*>(m_pOwner))->GetAttribute(string("C_GuardRange")))
		{
			StartActiveAct();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCarriage::RandomRun()
{
	CMoveShape* pTarget=GetMaster();

	if (pTarget &&m_pOwner->Distance(pTarget->GetTileX(),pTarget->GetTileY())<=CARRIAGE_STATE_DISTANCE)
	{
		AddAIEvent(AET_MOVE, CARRIAGE_STAY_TIME);
		m_caFollowState=FALLOWING;
		return TRUE;
	}
	else if (pTarget && 
		m_pOwner->Distance(pTarget->GetTileX(),pTarget->GetTileY())< (dynamic_cast<CMonster*>(m_pOwner))->GetAttribute(string("C_GuardRange"))
		&& m_pOwner->Distance(pTarget->GetTileX(),pTarget->GetTileY())>CARRIAGE_STATE_DISTANCE)
	{
		m_caFollowState=STAYING;
		return TRUE;
	}
	else
	{
		m_caFollowState=NOMONSTER;
		return FALSE;
	}
}