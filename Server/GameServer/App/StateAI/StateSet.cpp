///
/// @file StateSet.cpp
///
#include "stdafx.h"
#include "StateSet.h"
#include "StateFactory.h"
#include "EventImpl/Event.h"

namespace StateAI
{
	size_t StateSet::Create( const ImplConfig &implCfg, MachineType *machine )
	{
        Destroy();
		m_Impl = implCfg.Raw();
        
		for( ImplTableT::const_iterator it = m_Impl.begin();
			it != m_Impl.end(); ++ it )
		{
			long type = it->first;
            const ImplConfig::Impl &impl = it->second;
            AIState *state = GetInst( StateFactory ).Create( type, impl.impl, machine );
			assert( state );
            if( impl.script[0] != '\0' )
            {
                // script implementation
                // ugly codes
                state->ReceiveEvent( NULL, InitEvent( (void*) impl.script ) );
            }
			m_States.insert( StateTableT::value_type( type, state ) );
		}
		return m_States.size();
	}

	void StateSet::Destroy()
	{
		for( StateTableT::iterator it = m_States.begin();
			it != m_States.end(); ++ it )
		{
			long type = it->first;
			GetInst( StateFactory ).Destroy( type, m_Impl[type].impl, it->second );
		}
		m_States.clear();
		m_Impl.clear();
	}

	void StateSet::Reset()
	{
		for( StateTableT::iterator it = m_States.begin();
			it != m_States.end(); ++ it )
		{
			it->second->Reset();
		}
	}

	AIState *StateSet::operator[] ( long type )
	{
		StateTableT::iterator it = m_States.find( type );
		return it == m_States.end() ? NULL : it->second;
	}
}
