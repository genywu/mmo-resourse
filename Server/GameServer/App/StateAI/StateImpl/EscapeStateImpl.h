///
/// @file EscapeStateImpl.h
///
#ifndef ___ESCAPE_STATE_IMPL_H_
#define ___ESCAPE_STATE_IMPL_H_

#include "../AIStateDefs.h"

namespace StateAI
{
	class EscapeStateNormal : public AIState
	{
	public:
		EscapeStateNormal( MachineType *machine ) : AIState( machine, ST_ESCAPE )
		{
		}

		void Enter( BaseType::EntityType *entity, const BaseType::TransitionType &tran );

		void Leave( BaseType::EntityType *entity, const BaseType::TransitionType &tran );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( EscapeStateNormal, AIState );
	};
}

#endif
