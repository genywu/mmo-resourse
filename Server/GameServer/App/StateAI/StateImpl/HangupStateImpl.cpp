///
/// @file HangupStateImpl.cpp
///
#include "stdafx.h"
#include "HangupStateImpl.h"
#include "../BaseAI.h"

namespace StateAI
{
	IMPL_LIFE_POLICY_FUNC( HangupStateNormal, AIState );

	void HangupStateNormal::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_WAKEUP )
		{
			entity->ChangeState( ST_PEACE );
			entity->Resume( 0 );
		}
	}
} 
