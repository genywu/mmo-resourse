///
/// @file DHFPassiveGladiator.cpp
/// @author Kevin Lynx
/// @brief 优先记录-被动怪物AI
///

#include "StdAfx.h"
#include "DHFPassiveGladiator.h"
#include "AIUtilities.h"
#include "../../../setup/globesetup.h"
#include "../ServerRegion.h"
#include <algorithm>



BOOL CDHFPassiveGladiator::WhenBeenHurted( LONG lAttackerType, const CGUID &attackerID, DWORD dwHurt )
{
	TargetListType::iterator it = std::find( m_Targets.begin(), m_Targets.end(), attackerID );
	if( it == m_Targets.end() )
	{
		m_Targets.push_back( attackerID );
		if( m_Targets.size() > CGlobeSetup::GetSetup()->dwDHFQueueSize )
		{
			m_Targets.erase( m_Targets.begin() );
		}
	}
	return CMonsterAI::WhenBeenHurted( lAttackerType, attackerID, dwHurt );
}

BOOL CDHFPassiveGladiator::SearchEnemy()
{
	if( !CanSearchEnemy() )
	{
		return FALSE;
	}

	if( m_Targets.size() > 0 )
	{
		CServerRegion *pRegion = static_cast<CServerRegion*>( m_pOwner->GetFather() );
		long lMinDis = 0x7ffffff;
		CMoveShape *pRet = NULL;
		AIUtilities::FilterSet::Default filter( static_cast<CMonster*>( m_pOwner ) );
		for( TargetListType::iterator it = m_Targets.begin(); it != m_Targets.end(); ++ it )
		{
			CMoveShape *pTarget = static_cast<CMoveShape*>( pRegion->FindAroundObject( m_pOwner, TYPE_PLAYER, *it ) );
			if( pTarget == NULL )
			{
				pTarget = static_cast<CMoveShape*>( pRegion->FindAroundObject( m_pOwner, TYPE_MONSTER, *it ) );
			}
			if( pTarget == NULL )
			{
				// error
				continue;
			}
			if( filter( pTarget ) )
			{
				continue;
			}
			long lDis = m_pOwner->Distance( pTarget );
			if( lDis < lMinDis )
			{
				lMinDis = lDis;
				pRet = pTarget;
			}
		}
		if( pRet != NULL )
		{
			SetTarget( pRet->GetType(), pRet->GetExID() );
			return TRUE;
		}
	}

	return FALSE;
}

void CDHFPassiveGladiator::SetTarget( LONG lType, const CGUID &TargetGUID )
{
	CBaseAI::SetTarget( lType, TargetGUID );
}

