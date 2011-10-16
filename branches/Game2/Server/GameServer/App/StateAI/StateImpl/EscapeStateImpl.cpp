///
/// @file EscapeStateImpl.h
///
#include "stdafx.h"
#include "CycleState/CycleStateImpl.h"
#include "EscapeStateImpl.h"
#include "../Helper/FightObj.h"
#include "../MonsterAI.h"
#include "../../Monster.h"

namespace StateAI
{
	IMPL_LIFE_POLICY_FUNC( EscapeStateNormal, AIState );

	void EscapeStateNormal::Enter( BaseType::EntityType *entity, const BaseType::TransitionType &tran )
	{
		CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
		//monster->AddNewState( monster, STATE_BASE_WEAKNESS, 1, 10 );
	}

	void EscapeStateNormal::Leave( BaseType::EntityType *entity, const BaseType::TransitionType &tran )
	{
		CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
		//monster->DelNewState( STATE_BASE_WEAKNESS, 1 );
	}

	void EscapeStateNormal::Execute( BaseType::EntityType *entity )
	{
		MonsterAI *monsterAI = static_cast<MonsterAI*>( entity );

		// check cycle action
		AIState *cycle_action = entity->GetState( ST_CYCLE );
		CycleStateExecutor cycleExecutor;
		if( cycleExecutor.Execute( cycle_action, entity ) )
		{
			return ;
		}

		// check return peace
		if( monsterAI->GetFightObj().CheckReturnPeace() )
		{
			return ;
		}

		CMoveShape *target = entity->GetTarget();
		if( target == NULL )
		{
			monsterAI->GetFightObj().LoseTarget();
			entity->Resume( OPER_FAILED_TIME );
		}
		else
		{
			CMoveShape *owner = entity->GetOwner();
			long dir = GetLineDir( target->GetTileX(), target->GetTileY(), 
				owner->GetTileX(), owner->GetTileY() );
			entity->Move( dir );
		}
	}
}
