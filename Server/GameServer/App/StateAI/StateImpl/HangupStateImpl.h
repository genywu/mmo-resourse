///
/// @file PeaceStateImpl.h
///
#ifndef ___HANGUP_STATE_IMPL_H_
#define ___HANGUP_STATE_IMPL_H_

#include "../AIStateDefs.h"

namespace StateAI
{
	class HangupStateNormal : public AIState
	{
	public:
		HangupStateNormal( MachineType *machine ) : AIState( machine, ST_HANGUP )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Execute( BaseType::EntityType *entity ) {}

		DECL_LIFE_POLICY_FUNC( HangupStateNormal, AIState );
	};
}

#endif
