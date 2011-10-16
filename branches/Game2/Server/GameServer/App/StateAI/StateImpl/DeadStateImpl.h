///
/// @file DeadStateImpl.h
///
#ifndef ___DEAD_STATE_IMPL_H_
#define ___DEAD_STATE_IMPL_H_

#include "../AIStateDefs.h"

namespace StateAI
{
	class DeadStateNormal : public AIState
	{
	public:
		DeadStateNormal( MachineType *machine ) : AIState( machine, ST_DEAD )
		{
		}

		void Enter( EntityType *entity, const TransitionType & );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( DeadStateNormal, AIState );
	};
}

#endif
