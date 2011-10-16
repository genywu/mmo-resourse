#include "StdAfx.h"
#include "SnipeArcher.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"


CSnipeArcher::CSnipeArcher()
{}

CSnipeArcher::~CSnipeArcher()
{}

BOOL CSnipeArcher::SearchEnemy()
{
	if(!CanSearchEnemy())
		return false;
	if(m_pOwner==NULL)
		return false;
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );//寻找所在场景
	if( pRegion == NULL ) return TRUE;

	vector<CPlayer*> vOut;
	pRegion -> FindAroundPlayer( m_pOwner, vOut );

	//vector<CMonster*> vPets;
	//pRegion -> FindAroundPets( m_pOwner, vPets );

	CMoveShape* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CPlayer* pPlayer = vOut[i];
		if( pPlayer == NULL ) continue;
		if( pPlayer -> IsDied() ) continue;

		if ( !pPlayer->IsVisible(m_pOwner) )			// 隐形
		{
			continue;
		}

		if( pTarget )
		{
			if( m_pOwner -> Distance(pPlayer) >= pOwner -> GetAckRangeMin() &&
				m_pOwner -> Distance(pPlayer) <= pOwner -> GetAckRangeMax() &&
				pPlayer->GetExID()!= m_pOwner->GetBefoolID()&&
				pPlayer -> GetHP() < pTarget -> GetHP() )
			{
				pTarget = pPlayer;
			}
		}
		else
		{
			if( m_pOwner -> Distance(pPlayer) >= pOwner -> GetAckRangeMin() &&
				m_pOwner -> Distance(pPlayer) <= pOwner -> GetAckRangeMax() &&
				pPlayer->GetExID()!= m_pOwner->GetBefoolID())

			{
				pTarget = pPlayer;
			}
		}
	}

	if( pTarget )
	{
		//##满足条件设置对象			
		SetTarget( pTarget -> GetType(), pTarget -> GetExID() );
	}
	return TRUE;
}