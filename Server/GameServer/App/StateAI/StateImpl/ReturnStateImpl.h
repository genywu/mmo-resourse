///
/// @file ReturnStateImpl.h
///
#ifndef ___RETURN_STATE_IMPL_H_
#define ___RETURN_STATE_IMPL_H_

#include "../AIStateDefs.h"
#include "../EventImpl/Event.h"

namespace StateAI
{
	class ReturnStateNormal : public AIState
	{
		typedef ReturnEvent ReturnData;
	public:
		ReturnStateNormal( MachineType *machine ) : AIState( machine, ST_RETURN )
		{
		}

		void Enter( BaseType::EntityType *entity, const BaseType::TransitionType &tran );

		void Leave( BaseType::EntityType *entity, const BaseType::TransitionType &tran );

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( ReturnStateNormal, AIState );
	private:
		ReturnData m_Data;
		long m_OutTime;
	};
}

#endif
