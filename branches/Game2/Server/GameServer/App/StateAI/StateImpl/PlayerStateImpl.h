///
/// @file PlayerStateImpl.h
/// @brief Implement player's AI state.
///
#ifndef ___PLAYER_STATE_IMPL_H_
#define ___PLAYER_STATE_IMPL_H_

#include "../AIStateDefs.h"

namespace StateAI
{
	class PlayerPeaceState : public AIState
	{
	public:
		PlayerPeaceState( MachineType *machine ) : AIState( machine, ST_PEACE )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		DECL_LIFE_POLICY_FUNC( PlayerPeaceState, AIState );
	};

	class PlayerFightState : public AIState
	{
	public:
		PlayerFightState( MachineType *machine ) : AIState( machine, ST_FIGHT )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Enter( BaseType::EntityType *entity, const AITransition &tran );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( PlayerFightState, AIState );
	};


    class PlayerAgonalState : public AIState
    {
    public:
        PlayerAgonalState( MachineType* machine ) : AIState( machine , ST_AGONAL )
        {   
        }

        void  ReceiveEvent( BaseType::EntityType* entity,const BaseType::EventType &ev);

        void  Enter( BaseType::EntityType* entity, const AITransition& tran);

        void  Execute(BaseType::EntityType *entity );

        DECL_LIFE_POLICY_FUNC( PlayerAgonalState,AIState);
    };

	class PlayerDeadState : public AIState
	{
	public:
		PlayerDeadState( MachineType *machine ) : AIState( machine, ST_DEAD )
		{
		}

		void  Enter( BaseType::EntityType *entity, const AITransition &tran );

        void  ReceiveEvent( BaseType::EntityType* entity,const BaseType::EventType &ev);

		DECL_LIFE_POLICY_FUNC( PlayerDeadState, AIState );
	};
}

#endif
