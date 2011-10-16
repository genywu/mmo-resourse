///
/// @file SearchStateImpl.cpp
///
#include "stdafx.h"
#include "SearchStateImpl.h"
#include "../BaseAI.h"
#include "../EventImpl/Event.h"
#include "../Helper/AIUtils.h"
#include "../../Player.h"
#include "../../MonsterTaunt/TauntHandler.h"
#include "../../MonsterTaunt/TauntEvents.h"


namespace StateAI
{
	void SearchStateNormal::Execute( BaseType::EntityType *entity )
	{
        CMonster *finder = dynamic_cast<CMonster*>( entity->GetOwner() );
        CServerRegion *region = dynamic_cast<CServerRegion*>( finder->GetFather() );
        if( region == NULL )
        {
            return;
        }
        using namespace AIUtils;
        ShapeListType shapeList;
        region->FindAroundObject( finder, TYPE_PLAYER, shapeList );
        // filter some invalid shapes
        FilterShapeList( shapeList, FilterSet::Default( finder ) );
        // get the nearest shape 
#ifdef MONSTER_DIS_FLOAT
        float minDis = 9999999.0f;
#else
	    long minDis = 0x7ffffff ;	
#endif
        CMoveShape *retTarget = NULL;
        for( ShapeListType::iterator it = shapeList.begin(); 
            it != shapeList.end(); ++ it )
        {
            CMoveShape *target = dynamic_cast<CMoveShape*>( *it );
            OnSearched( entity, target->GetExID(), target->GetType() );
#ifdef MONSTER_DIS_FLOAT
            float dis = finder->RealDistanceF( target );
#else
            long dis = finder->Distance( target );
#endif
            if( dis < minDis )
            {
                minDis = dis;
                retTarget = target;
            }
        }
        if( retTarget != NULL )
        {
            entity->SetTarget( retTarget->GetExID(), retTarget->GetType() );
        }
	}

	void DHFSearchState::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_HURT )
		{
			const HurtEvent &hev = (const HurtEvent&) ev;
			m_Attackers.push_back( hev.AttackerId );
		}
	}

    void DHFSearchState::Execute( BaseType::EntityType *entity )
    {
        // search in the attacker list
		if( m_Attackers.size() <= 0 )
		{
			return ;
		}
#ifdef MONSTER_DIS_FLOAT
        float minDis = 9999999.0f;
#else
	    long minDis = 0x7ffffff ;	
#endif
        CMoveShape *retTarget = NULL;
        using namespace AIUtils;
        CMonster *self = dynamic_cast<CMonster*>( entity->GetOwner() );
        if( self == NULL )
        {
            return ;
        }
        CServerRegion *region = dynamic_cast<CServerRegion*>( self->GetFather() );
        FilterSet::Default filter( self );
        for( AttackerListT::iterator it = m_Attackers.begin(); it != m_Attackers.end(); ++ it )
        {
            CMoveShape *target = FindShape( region, *it );
            if( target == NULL )
            {
                continue;
            }
            if( filter( target ) )
            {
                continue;
            }
#ifdef MONSTER_DIS_FLOAT
            float dis = self->RealDistanceF( target );
#else
            long dis = self->Distance( target );
#endif
            if( dis < minDis )
            {
                minDis = dis;
                retTarget = target;
            }
        }
        if( retTarget != NULL )
        {
            entity->SetTarget( retTarget->GetExID(), retTarget->GetType() );
        }
    }

    void DHFSearchState::Reset()
    {
        m_Attackers.clear();
    }

	IMPL_LIFE_POLICY_FUNC( DHFPassiveGladiatorState, AIState );

	void DHFPassiveGladiatorState::Execute( BaseType::EntityType *entity )
	{
        DHFSearchState::Execute( entity );
	}

	IMPL_LIFE_POLICY_FUNC( DHFGladiatorState, AIState );

	void DHFGladiatorState::Execute( BaseType::EntityType *entity )
	{
        if( entity->HasTarget() )
        {
            return;
        }
        DHFSearchState::Execute( entity );
        if( !entity->HasTarget() )
        {
            SearchStateNormal::Execute( entity );
        }
	}

    IMPL_LIFE_POLICY_FUNC( TauntPassiveGladiatorState, AIState );

    void TauntPassiveGladiatorState::Execute( BaseType::EntityType *entity )
    {
        CMonster *finder = dynamic_cast<CMonster*>( entity->GetOwner() );
        TauntHandler &tauntHandler = finder->GetTauntHandler();
        if( tauntHandler.Size() == 0 )
        {
            // calm down now.
            return;
        }
        const TauntHandler::TauntItem *item = tauntHandler.GetBestItem();
        assert( item );
        entity->SetTarget( item->target.id, item->target.type );
    }

    IMPL_LIFE_POLICY_FUNC( TauntGladiatorState, AIState );

    void TauntGladiatorState::Execute( BaseType::EntityType *entity )
    {
        // search by the taunt list first.
        TauntPassiveGladiatorState::Execute( entity );
        if( !entity->HasTarget() )
        {
            // not found, search normally then.
            SearchStateNormal::Execute( entity );
        }
    }

    void TauntGladiatorState::OnSearched( BaseType::EntityType *entity, const CGUID &id, long type )
    {
        CMonster *finder = dynamic_cast<CMonster*>( entity->GetOwner() );
        TauntHandler &tauntHandler = finder->GetTauntHandler();
        TauntEvents events( tauntHandler );
        events.Searched( id, type );
    }
}
