///
/// @file PeaceStateImpl.cpp
///
#include "stdafx.h"
#include "CycleState/CycleStateImpl.h"
#include "PeaceStateImpl.h"
#include "../BaseAI.h"
#include "../Helper/RunByRoad.h"
#include "../MonsterAI.h"
#include "../../Monster.h"

namespace StateAI
{
    bool PeaceStateBase::SearchEnemy( BaseType::EntityType *entity )
    {
        MonsterAI *ai = static_cast<MonsterAI*>( entity );
        if( ai->IsInKindlyTime() )
        {
            return false;
        }
		AIState *searchAction = entity->GetState( ST_SEARCH );
		assert( searchAction );
		searchAction->Execute( entity );
        if( entity->HasTarget() )
        {
            entity->ChangeState( ST_FIGHT );
            entity->Resume( 0 );
            return true;
        }
        return false;
    }

    bool PeaceStateBase::CheckCycleState( BaseType::EntityType *entity )
    {
        AIState *cycle_action = entity->GetState( ST_CYCLE );
		CycleStateExecutor cycleExecutor;
		if( cycleExecutor.Execute( cycle_action, entity ) )
		{
			return true;
		}
        return false;
    }

    bool PeaceStateBase::CheckHangup( BaseType::EntityType *entity )
    {
        MonsterAI *ai = static_cast<MonsterAI*>( entity );
        // should be hangup ?
        CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
        if( monster->GetAIHangupType() == 0 && monster->GetArea()->IsHibernate() )
        {
            // area hangup
            Point bornPos = ai->GetBornPos();
            long x = monster->GetTileX() / AREA_WIDTH;
            long y = monster->GetTileY() / AREA_HEIGHT;
            long bornX = bornPos.x / AREA_WIDTH;
            long bornY = bornPos.y / AREA_HEIGHT;
            if( x == monster->GetArea()->GetX() && y == monster->GetArea()->GetY() &&
                bornX == x && bornY == y )
            {
                // hangup
                entity->ChangeState( ST_HANGUP );
                return true;
            }
        }
        // else if ... // region hangup
        return false;
    }

    bool PeaceStateBase::CheckRunByRoad( BaseType::EntityType *entity )
    {
        MonsterAI *ai = static_cast<MonsterAI*>( entity );
        return ai->GetRunByRoad().CheckRun();
    }

    ////////////////////////////////////////////////////////////////////////////////////////
	IMPL_LIFE_POLICY_FUNC( PeaceStateNormal, AIState );

	void PeaceStateNormal::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_HURT )
		{
			entity->ChangeState( ST_FIGHT );
			// may cache the hurt data like:
			AIState *search_action = entity->GetState( ST_SEARCH );
			assert( search_action );
			search_action->ReceiveEvent( entity, ev );
		}
		else if( ev.Type() == ET_KILL ) 
		{
			entity->ChangeState( ST_DEAD ); 
		}
	} 

	void PeaceStateNormal::Execute( BaseType::EntityType *entity )
	{
        if( SearchEnemy( entity ) ||
            CheckHangup( entity ) ||
            CheckCycleState( entity ) )
        {
            return;
        }

        MonsterAI *ai = static_cast<MonsterAI*>( entity );
        if( CheckRunByRoad( entity ) )
        {
            return;
        }
		ai->RandomRun();
	}

    ////////////////////////////////////////////////////////////////////////////////////////
    IMPL_LIFE_POLICY_FUNC( PeaceStateNPC, AIState );

    void PeaceStateNPC::Execute( BaseType::EntityType *entity )
    {
        if( CheckHangup( entity ) ||
            CheckCycleState( entity ) )
        {
            return;
        }
        if( CheckRunByRoad( entity ) )
        {
            return;
        }
        entity->Stand( 1000 );
    }
}
