///
/// @file GuardSearch.cpp
///
#include "stdafx.h"
#include "GuardSearch.h"
#include "../../Monster.h"
#include "../../Player.h"
#include "../../ServerRegion.h"
#include "../../country/CountryParam.h"
#include "../helper/AIUtils.h"
#include "../MonsterAI.h"
#include "../AIEventSender.h"

namespace StateAI
{
    IMPL_LIFE_POLICY_FUNC( GuardSearch, AIState );

#define PLAYER_CASTC( shape_p ) const_cast<CPlayer*>( static_cast<const CPlayer*>( shape_p ) )
#define MONSTER_CASTC( shape_p ) const_cast<CMonster*>( static_cast<const CMonster*>( shape_p ) )

    template <typename _Fn>
    void filter_shape( GuardSearch::ShapeListType &shape_list, _Fn fn )
    {
        for( GuardSearch::ShapeListType::iterator it = shape_list.begin(); it != shape_list.end(); )
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

            if( AIUtils::IsInGodState( pShape ) )
            {
                return true;
            }

            return false;
        }
        long lRange;
        CMonster *pOwner;
    };

    void GuardSearch::Execute( BaseType::EntityType *entity )
    {
        CMonster *pSelf = dynamic_cast<CMonster*>( entity->GetOwner() );
        ShapeListType player_list;
        ShapeListType monster_list;
        CServerRegion *pRegion = static_cast<CServerRegion*>( pSelf->GetFather() );
        CMoveShape *pTarget = NULL;

        if( CanAttackCurrent( pSelf, entity ) )
        {
            return;
        }

        // find the around players
        pRegion->FindAroundObject( pSelf, TYPE_PLAYER, player_list );
        // find the around monsters including the guards.
        pRegion->FindAroundObject( pSelf, TYPE_MONSTER, monster_list );

        // filter the result
        Filter filter( static_cast<CMonster*>( pSelf ) );
        filter_shape( player_list, filter );
        filter_shape( monster_list, filter );


        // retrieve the ai reference
        DWORD ai_ref = pSelf->GetAIReferrence( 0 );
        if( ai_ref >= 0 )
        {
            pTarget = SearchByAIRef1( pSelf, player_list, ai_ref );
        }

        if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 1 ) ) > 0 )
        {
            pTarget = SearchByAIRef2( pSelf, player_list, ai_ref );
        }	

        if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 2 ) ) > 0 )
        {
            pTarget = SearchByAIRef3( pSelf, monster_list, ai_ref );
        }

        if( pTarget == NULL && ( ai_ref = pSelf->GetAIReferrence( 3 ) ) > 0 )
        {
            pTarget = SearchByAIRef4( pSelf, monster_list, ai_ref );
        }

        if( pTarget != NULL )
        {
            // search successfully
            entity->SetTarget( pTarget->GetExID(), pTarget->GetType() );
        }
        else if( entity->HasTarget() )
        {
            AI_EVENT_SENDER( entity ).ReturnPeace();
        }
    }

    BOOL GuardSearch::CanAttackCurrent( CMonster *owner, BaseType::EntityType *entity )
    {
    	if( !entity->HasTarget() )
    	{
    		return FALSE;
    	}
    
    	CMoveShape *pTarget = entity->GetTarget();
    	long dis = owner->Distance( pTarget );
    	long attack_range = owner->GetAttribute( string( "C_GuardRange" ) );
    
    	if( dis > attack_range )
    	{
    		return FALSE;
    	}
    	
    	DWORD ai_ref = 0;
    	
    	if( pTarget->GetType() == TYPE_PLAYER )
    	{
    		CPlayer *pPlayer = static_cast<CPlayer*>( pTarget );
    		// ai reference 1
    		ai_ref = owner->GetAIReferrence( 0 );
    		if( ai_ref >= 0 )
    		{
    			if( pPlayer->GetPVPValue() >= ai_ref && !IsSameCamp( pPlayer, owner ) )
    			{
    				return TRUE;
    			}
    		}
    		// ai reference 2
    		ai_ref = owner->GetAIReferrence( 1 );
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
    
    CMoveShape *GuardSearch::SearchByAIRef1( CMonster *owner, const ShapeListType &player_list, DWORD ai_ref )
    {
    	LONG min_distance = 0x7ffffff;	
    	CMoveShape *pTarget = NULL;
    	for( ShapeListType::const_iterator it = player_list.begin(); it != player_list.end(); ++ it )
    	{
    		CPlayer *pPlayer = PLAYER_CASTC( *it );
      		LONG distance = owner->Distance( pPlayer );
    		if( pPlayer->GetPVPValue() >= ai_ref && !IsSameCamp( pPlayer, owner ) &&
    			distance < min_distance )
    		{
    			min_distance = distance;
    			pTarget = pPlayer;
    		}		
    	}
    
    	return pTarget;
    }
    
    CMoveShape *GuardSearch::SearchByAIRef2( CMonster *owner, const ShapeListType &player_list, DWORD ai_ref )
    {
    	LONG min_distance = 0x7ffffff;	
    	CMoveShape *pTarget = NULL;
    	for( ShapeListType::const_iterator it = player_list.begin(); it != player_list.end(); ++ it )
    	{
    		CPlayer *pPlayer = PLAYER_CASTC( *it );
    		LONG distance = owner->Distance( pPlayer );
    		if( pPlayer->GetPkValue() >= ai_ref &&
    			distance < min_distance )
    		{
    			min_distance = distance;
    			pTarget = pPlayer;
    		}		
    	}
    
    	return pTarget;	
    }
    
    bool GuardSearch::_ai_ref3_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref )
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
    		
    CMoveShape *GuardSearch::SearchByAIRef3( CMonster *owner, const ShapeListType &monster_list, DWORD ai_ref )
    {
    	LONG min_distance = 0x7ffffff;	
    	CMoveShape *pTarget = NULL;
    	for( ShapeListType::const_iterator it = monster_list.begin(); it != monster_list.end(); ++ it )
    	{
    		CMonster *pMonster = MONSTER_CASTC( *it );
    		LONG distance = owner->Distance( pMonster );
    		if( pMonster->GetNpcType() == NT_Monster && 
    			_ai_ref3_helper( static_cast<CMonster*>( owner ), pMonster, ai_ref ) &&
    			distance < min_distance )
    		{
    			min_distance = distance;
    			pTarget = pMonster;
    		}		
    	}
    
    	return pTarget;	
    }
    
    bool GuardSearch::_ai_ref4_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref )
    {
    	// currently its the same as rule 3
    	return _ai_ref3_helper( pSelf, pOther, ai_ref );
    }
    
    CMoveShape *GuardSearch::SearchByAIRef4( CMonster *owner, const ShapeListType &monster_list, DWORD ai_ref )
    {
    	LONG min_distance = 0x7ffffff;	
    	CMoveShape *pTarget = NULL;
    	for( ShapeListType::const_iterator it = monster_list.begin(); it != monster_list.end(); ++ it )
    	{
    		CMonster *pMonster = MONSTER_CASTC( *it );
    		LONG distance = owner->Distance( pMonster );
    		if( pMonster->GetNpcType() == NT_Guard && 
    			_ai_ref4_helper( static_cast<CMonster*>( owner ), pMonster, ai_ref ) &&
    			distance < min_distance )
    		{
    			min_distance = distance;
    			pTarget = pMonster;
    		}		
    	}
    
    	return pTarget;
    }
    
    BOOL GuardSearch::IsSameCamp( BYTE country_id1, BYTE country_id2 )
    {
    	CCountryParam *cp = GetCountryParam();
    	return cp->GetCountryStartCamp( country_id1 ) == cp->GetCountryStartCamp( country_id2 );
    }
    

}
