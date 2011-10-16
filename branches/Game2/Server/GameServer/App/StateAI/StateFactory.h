///
/// @file StateFactory.h
/// @author Kevin Lynx
/// @brief StateFactory is used to create AI states.
///
#ifndef ___STATE_FACTORY_H_
#define ___STATE_FACTORY_H_

#include "AIStateDefs.h"

namespace StateAI
{
	class AIState;

	/// Functions to create/destroy an AI state. Your own create/destroy functions
	/// must fit these prototypes below.
	typedef AIState *(*CreateAIStateT)( MachineType * );
	typedef void (*DestroyAIStateT)( AIState *state );

	/// A singleton class to create AI states.
	class StateFactory
	{
	public:
		/// This constructor will bind all AI states' create/destroy functions.
		StateFactory();

		/// Add an AI state create/destroy policy.
		bool Add( long state_type, long state_impl, CreateAIStateT create, DestroyAIStateT destroy );

		/// Use the policy added previos to create an AI state object.
		AIState *Create( long state_type, long state_impl,  MachineType *machine );

		/// Destroy an AI state object.
		void Destroy( long state_type, long state_impl, AIState *state );

	private:
		kl_common::Factory<long, AIState, CreateAIStateT, DestroyAIStateT> m_Factory;
	};
}

#endif
