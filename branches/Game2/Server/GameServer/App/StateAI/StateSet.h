///
/// @file StateSet.h
/// @author Kevin Lynx
///
#ifndef ___STATE_SET_H_
#define ___STATE_SET_H_

#include "AIStateDefs.h"
#include "../../../../Public/Setup/MonsterAISetup.h"
#include <map>


namespace StateAI
{
	class BaseAI;
	///
	/// StateSet manages AI state object.
	///
	class StateSet
	{
	public:
		/// [state_type, state_impl]
		typedef ImplConfig::ImplTableT ImplTableT;
		/// [state_type, state_object]
		typedef std::map<long, AIState*> StateTableT;
	public:

		/// Create states by the impl config.
		size_t Create( const ImplConfig &impl, MachineType *machine );

		/// Destroy all states, and clear the impl config.
		void Destroy();

		/// Reset all states.
		void Reset();

		/// Direct access the states.
		AIState *operator[] ( long type );
	private:
		ImplTableT m_Impl;
		StateTableT m_States;
	};
}

#endif
