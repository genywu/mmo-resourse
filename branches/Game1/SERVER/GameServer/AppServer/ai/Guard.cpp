///
/// @file Guard.cpp
///
#include "stdafx.h"
#include "Guard.h"
#include "../Monster.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../country/CountryParam.h"
#include "AIUtilities.h"



#define PLAYER_CASTC( shape_p ) const_cast<CPlayer*>( static_cast<const CPlayer*>( shape_p ) )
#define MONSTER_CASTC( shape_p ) const_cast<CMonster*>( static_cast<const CMonster*>( shape_p ) )

template <typename _Fn>
void filter_shape( Guard::ShapeListType &shape_list, _Fn fn )
{
	for( Guard::ShapeListType::iterator it = shape_list.begin(); it != shape_list.end(); )
	{
		if( fn( *it ) )
		{
			it = shape_list.erase( it );
		}
		else
		{
			++ it;
		}
	}
}

struct Filter
{
	Filter( CMonster *pOwner ) 
	{
		this->pOwner = pOwner;
		lRange = this->pOwner->GetAttribute( std::string( "C_GuardRange" ));
	}

	/// return true to filter shape.
	bool operator() ( CShape *pShape )
	{
		CMoveShape *pDest = static_cast<CMoveShape*>( pShape );
		if( pDest->IsDied() )
		{
			return true;
		}
		if( pOwner->Distance( pShape ) > lRange )
		{
			return true;
		}

		if (!pDest->IsVisible(pOwner) )			// ÒþÐÎ
		{
			return true;
		}

		if(  pDest->GetExID() == pOwner->GetBefoolID() )
		{
			return true;
		}

		if( AIUtilities::IsInGodState( pShape ) )
		{
			return true;
		}

		return false;
	}
	long lRange;
	CMonster *pOwner;
};

BOOL Guard::SearchEnemy()
{
	if( !CanSearchEnemy() )
	{
		return FALSE;
	}

	if( CanAttackCurrent() )
	{
		return FALSE;
	}

	ShapeListType player_list;
	ShapeListType monster_list;
	CServerRegion *pRegion = static_cast<CServerRegion*>( m_pOwner->GetFather() );
	CMonster *pSelf = static_cast<CMonster*>( m_pOwner );
	CMoveShape *pTarget = NULL;

	// find the around players
	pRegion->FindAroundObject( m_pOwner, TYPE_PLAYER, player_list );
	// find the around monsters including the guards.
	pRegion->FindAroundObject( m_pOwner, TYPE_MONSTER, monster_list );

	// filter the result
	Filter filter( static_cast<CMonster*>( m_pOwner ) );
	filter_shape( player_list, filter );
	filter_shape( monster_list, filter );


	// retrieve the ai reference
	DWORD ai_ref = pSelf->GetAIReferrence( 0 );
	if( ai_ref >= 0 )
	{
		pTarget = SearchByAIRef1( player_list, ai_ref );
	}

	if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 1 ) ) > 0 )
	{
		pTarget = SearchByAIRef2( player_list, ai_ref );
	}	

	if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 2 ) ) > 0 )
	{
		pTarget = SearchByAIRef3( monster_list, ai_ref );
	}

	if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 3 ) ) > 0 )
	{
		pTarget = SearchByAIRef4( monster_list, ai_ref );
	}

	if( pTarget != NULL )
	{
		// search successfully
		SetTarget( pTarget->GetType(), pTarget->GetExID() );
		return TRUE; 
	}
	else if( HasTarget() )
	{
		WhenLoseTarget();
	}

	return FALSE;	
}

BOOL Guard::CanAttackCurrent()
{
	if( !HasTarget() )
	{
		return FALSE;
	}

	CMoveShape *pTarget = GetTarget();
	long dis = m_pOwner->Distance( pTarget );
	long attack_range = static_cast<CMonster*>( m_pOwner )->GetAttribute( string( "C_GuardRange" ) );

	if( dis > attack_range )
	{
		return FALSE;
	}

	CMonster *pSelf = static_cast<CMonster*>( m_pOwner );
	DWORD ai_ref = 0;
	
	if( pTarget->GetType() == TYPE_PLAYER )
	{
		CPlayer *pPlayer = static_cast<CPlayer*>( pTarget );
		// ai reference 1
		ai_ref = pSelf->GetAIReferrence( 0 );
		if( ai_ref >= 0 )
		{
			if( pPlayer->GetPVPValue() >= ai_ref && !IsSameCamp( pPlayer, pSelf ) )
			{
				return TRUE;
			}
		}
		// ai reference 2
		ai_ref = pSelf->GetAIReferrence( 1 );
		if( ai_ref > 0 )
		{
			if( pPlayer->GetPkValue() >= ai_ref )
			{
				return TRUE;
			}
		}
	}
	
	// actually, it's not necessary to judge monsters, that because the property for the monsters will
	// not changed .So ignored here.
	// ...

	return FALSE;
}

CMoveShape *Guard::SearchByAIRef1( const ShapeListType &player_list, DWORD ai_ref )
{
	LONG min_distance = 0x7ffffff;	
	CMoveShape *pTarget = NULL;
	for( ShapeListType::const_iterator it = player_list.begin(); it != player_list.end(); ++ it )
	{
		CPlayer *pPlayer = PLAYER_CASTC( *it );
  		LONG distance = m_pOwner->Distance( pPlayer );
		if( pPlayer->GetPVPValue() >= ai_ref && !IsSameCamp( pPlayer, m_pOwner ) &&
			distance < min_distance )
		{
			min_distance = distance;
			pTarget = pPlayer;
		}		
	}

	return pTarget;
}

CMoveShape *Guard::SearchByAIRef2( const ShapeListType &player_list, DWORD ai_ref )
{
	LONG min_distance = 0x7ffffff;	
	CMoveShape *pTarget = NULL;
	for( ShapeListType::const_iterator it = player_list.begin(); it != player_list.end(); ++ it )
	{
		CPlayer *pPlayer = PLAYER_CASTC( *it );
		LONG distance = m_pOwner->Distance( pPlayer );
		if( pPlayer->GetPkValue() >= ai_ref &&
			distance < min_distance )
		{
			min_distance = distance;
			pTarget = pPlayer;
		}		
	}

	return pTarget;	
}

bool Guard::_ai_ref3_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref )
{
	bool ret = false;
	switch( ai_ref )
	{
	case 1:
		ret = !IsSameCamp( pSelf, pOther );
	   	break;
   	case 2:
		ret = !IsSameCountry( pSelf, pOther );
		break;
	case 3:
		ret = ( strcmp( pSelf->GetOriginName(), pOther->GetOriginName() ) != 0 );
		break;
	case 4:
		ret = true;
		break;
	default:
		assert( "Invalid ai reference value" && 0 );
	};

	return ret;
}
		
CMoveShape *Guard::SearchByAIRef3( const ShapeListType &monster_list, DWORD ai_ref )
{
	LONG min_distance = 0x7ffffff;	
	CMoveShape *pTarget = NULL;
	for( ShapeListType::const_iterator it = monster_list.begin(); it != monster_list.end(); ++ it )
	{
		CMonster *pMonster = MONSTER_CASTC( *it );
		LONG distance = m_pOwner->Distance( pMonster );
		if( pMonster->GetNpcType() == NT_Monster && 
			_ai_ref3_helper( static_cast<CMonster*>( m_pOwner ), pMonster, ai_ref ) &&
			distance < min_distance )
		{
			min_distance = distance;
			pTarget = pMonster;
		}		
	}

	return pTarget;	
}

bool Guard::_ai_ref4_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref )
{
	// currently its the same as rule 3
	return _ai_ref3_helper( pSelf, pOther, ai_ref );
}

CMoveShape *Guard::SearchByAIRef4( const ShapeListType &monster_list, DWORD ai_ref )
{
	LONG min_distance = 0x7ffffff;	
	CMoveShape *pTarget = NULL;
	for( ShapeListType::const_iterator it = monster_list.begin(); it != monster_list.end(); ++ it )
	{
		CMonster *pMonster = MONSTER_CASTC( *it );
		LONG distance = m_pOwner->Distance( pMonster );
		if( pMonster->GetNpcType() == NT_Guard && 
			_ai_ref4_helper( static_cast<CMonster*>( m_pOwner ), pMonster, ai_ref ) &&
			distance < min_distance )
		{
			min_distance = distance;
			pTarget = pMonster;
		}		
	}

	return pTarget;
}

BOOL Guard::IsSameCamp( BYTE country_id1, BYTE country_id2 )
{
	CCountryParam *cp = GetCountryParam();
	return cp->GetCountryStartCamp( country_id1 ) == cp->GetCountryStartCamp( country_id2 );
}

