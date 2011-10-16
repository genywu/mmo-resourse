///
/// @file PlayerStateImpl.cpp
/// @brief Implement player's AI state.
///
#include "stdafx.h"
#include "PlayerStateImpl.h"
#include "../PlayerAI.h"
#include "../../Player.h"

#include "../../../../../Public/Setup/GlobalSetup.h"

namespace StateAI
{
	IMPL_LIFE_POLICY_FUNC( PlayerPeaceState, AIState );

	void PlayerPeaceState::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_HURT )
		{
			entity->ChangeState( ST_FIGHT, ev );
		}
        else if ( ev.Type() == ET_AGONAL )
        {
            entity->ChangeState( ST_AGONAL,ev );
        }
	}

	IMPL_LIFE_POLICY_FUNC( PlayerFightState, AIState );

	void PlayerFightState::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		PlayerAI *ai = static_cast<PlayerAI*>( entity );
		if( ev.Type() == ET_USESKILLEND )
		{
			ai->SetInSkillActTime( false );
			ai->Resume( GlobalSetup::GetSetup()->dwFightStateTime );
		}
		else if( ev.Type() == ET_HURT )
		{
			ai->SetReturnPeaceTimer( false );
		}
        else if( ev.Type() == ET_AGONAL )
        {
            entity->ChangeState( ST_AGONAL,ev );
        }
	}

	void PlayerFightState::Enter( BaseType::EntityType *entity, const AITransition &tran )
	{
		PlayerAI *ai = static_cast<PlayerAI*>( entity );
		const EventTransition &evTran = static_cast<const EventTransition&>( tran );
		long eventType = evTran.event.Type();

		ai->SetReturnPeaceTimer( eventType == ET_USESKILL );
	}

	void PlayerFightState::Execute( BaseType::EntityType *entity )
	{
        PlayerAI *ai = static_cast<PlayerAI*>( entity );
        if ( !ai->IsInSkillActTime() )
        {
            entity->ChangeState( ST_PEACE );
        }		
	}

    IMPL_LIFE_POLICY_FUNC (PlayerAgonalState,AIState);

    void  PlayerAgonalState::ReceiveEvent(BaseType::EntityType* entity,const BaseType::EventType &ev )
    {
        PlayerAI *ai = static_cast<PlayerAI*>( entity ); 
        if ( ev.Type() == ET_LEAVE_PHASE )
        {
            entity->ChangeState( ST_PEACE , ev );
        }
    }

    void  PlayerAgonalState::Enter(BaseType::EntityType* entity, const AITransition& tran)
    {
        PlayerAI *ai = static_cast<PlayerAI*>( entity );

        entity->Stop();
        ai->SetReturnPeaceTimer( false );
    }

    void  PlayerAgonalState::Execute(BaseType::EntityType *entity )
    {
        PlayerAI *ai = static_cast<PlayerAI*>( entity );
        if( ai->GetOwner()->GetPhaseID() == NULL_GUID)
        {
            entity->ChangeState( ST_PEACE );
        }
        else
        {
            ai->Resume( GlobalSetup::GetSetup()->dwFightStateTime );
        }
    }
    

	IMPL_LIFE_POLICY_FUNC( PlayerDeadState, AIState );

	void PlayerDeadState::Enter( BaseType::EntityType *entity, const AITransition &tran )
	{
		CPlayer *player = static_cast<CPlayer*>( entity->GetOwner() );
		player->OnDied();
	}

    void PlayerDeadState::ReceiveEvent(BaseType::EntityType* entity,const BaseType::EventType &ev)
    {
        PlayerAI *ai = static_cast<PlayerAI*>( entity );

        if ( ev.Type() == ET_RELIVE )
        {
            entity->ChangeState( ST_PEACE , ev );
        }
    }
}
