///
/// @file PeaceStateImpl.h
///
#ifndef ___PEACE_STATE_IMPL_H_
#define ___PEACE_STATE_IMPL_H_

#include "../AIStateDefs.h"

namespace StateAI
{
    class PeaceStateBase : public AIState
    {
    public:
        PeaceStateBase( MachineType *machine ) : AIState( machine, ST_PEACE )
        {
        }
    protected:
        /// some common operations for monster ai
        bool SearchEnemy( BaseType::EntityType *entity );

        bool CheckCycleState( BaseType::EntityType *entity );

        bool CheckHangup( BaseType::EntityType *entity );

        bool CheckRunByRoad( BaseType::EntityType *entity );
    };

	class PeaceStateNormal : public PeaceStateBase
	{
	public:
		PeaceStateNormal( MachineType *machine ) : PeaceStateBase( machine )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( PeaceStateNormal, AIState );
	};

    class PeaceStateNPC : public PeaceStateBase
    {
    public:
        PeaceStateNPC( MachineType *machine ) : PeaceStateBase( machine )
        {
        }

        void Execute( BaseType::EntityType *entity );

        DECL_LIFE_POLICY_FUNC( PeaceStateNPC, AIState );
    };
}

#endif
