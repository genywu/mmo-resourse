#include "StdAfx.h"
#include "Archer.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CArcher::CArcher()
{
}

CArcher::~CArcher()
{

}

INT CArcher::Tracing(CMoveShape* pTarget)
{
	if (0)//EscapeNearEnemy(1)
	{
		SearchEnemy();
		return TRACE_RESULT_TRACE;
	}
	else
	{
		return CMonsterAI::Tracing(pTarget);
	}
}
/*
CBaseAI::AI_EXEC_STATE CArcher::Run()
{
	return CMonsterAI::Run();
}

VOID CArcher::OnSchedule()
{
	CMonsterAI::OnSchedule();
}

VOID CArcher::WhenBeenHurted( LONG lType, LONG lID, DWORD dwStiffenTime )
{
	CBaseAI::WhenBeenHurted( lType, lID, dwStiffenTime );
	if( HasTarget() == FALSE )
	{
		SetTarget( lType, lID );
	}
}

//##在每次需要移动的时候，搜寻最近的Player
BOOL CArcher::OnMoving()
{
	if( CMonsterAI::OnMoving() )
	{
		if( m_pOwner -> IsDied() == FALSE )
		{
			AddAIEvent( ASA_SEARCH_ENEMY );
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CArcher::OnSearchEnemy()
{
	if( CMonsterAI::OnSearchEnemy() )
	{
		//##如果所有者不存在.直接返回.
		if( m_pOwner == NULL ) return TRUE;

		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
		if( pRegion == NULL ) return TRUE;
		
		vector<CPlayer*> vOut;
		pRegion -> FindAroundPlayer( m_pOwner, vOut );

		vector<CMonster*> vPets;
		pRegion -> FindAroundPets( m_pOwner, vPets );

		//vector<CMonster*> vCars;
		//pRegion->FindAroundCarriages( m_pOwner, vCars );

		CMoveShape* pTarget = NULL;
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			CPlayer* pPlayer = vOut[i];
			if( pPlayer == NULL ) continue;
			if( pPlayer -> IsDied() ) continue;
			if( pTarget )
			{
				if( m_pOwner -> Distance(pPlayer) <= m_pOwner -> GetAlertRange() &&
					pPlayer -> GetHP() < pTarget -> GetHP() )
				{
					pTarget = pPlayer;
				}
			}
			else
			{
				if( m_pOwner -> Distance(pPlayer) <= m_pOwner -> GetAlertRange() )
				{
					pTarget = pPlayer;
				}
			}
		}

		for( size_t i = 0; i < vPets.size(); i ++ )
		{
			CMonster* pMonster = vPets[i];
			if( pMonster == NULL ) continue;
			if( pMonster -> IsDied() ) continue;
			if( pTarget )
			{
				if( m_pOwner -> Distance(pMonster) <= m_pOwner -> GetAlertRange() &&
					pMonster -> GetHP() < pTarget -> GetHP() )
				{
					pTarget = pMonster;
				}
			}
			else
			{
				if( m_pOwner -> Distance(pMonster) <= m_pOwner -> GetAlertRange() )
				{
					pTarget = pMonster;
				}
			}
		}

		//for( size_t i = 0; i < vCars.size(); i ++ )
		//{
		//	CMonster* pMonster = vCars[i];
		//	if( pMonster == NULL ) continue;
		//	if( pMonster -> IsDied() ) continue;
		//	if( pTarget )
		//	{
		//		if( m_pOwner -> Distance(pMonster) <= m_pOwner -> GetAlertRange() &&
		//			pMonster -> GetHP() < pTarget -> GetHP() )
		//		{
		//			pTarget = pMonster;
		//		}
		//	}
		//	else
		//	{
		//		if( m_pOwner -> Distance(pMonster) <= m_pOwner -> GetAlertRange() )
		//		{
		//			pTarget = pMonster;
		//		}
		//	}
		//}
		if( pTarget )
		{
			//##满足条件设置对象			
			SetTarget( pTarget -> GetType(), pTarget -> GetID() );
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
*/