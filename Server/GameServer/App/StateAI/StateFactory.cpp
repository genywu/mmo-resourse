///
/// @file StateFactory.cpp
/// @author Kevin Lynx
/// @brief StateFactory is used to create AI states.
///
#include "stdafx.h"
#include "StateFactory.h"

#include "StateImpl/StateImplDef.h"
#include "StateImpl/PeaceStateImpl.h"
#include "StateImpl/SearchStateImpl.h"
#include "StateImpl/FightStateImpl.h"
#include "StateImpl/HangupStateImpl.h"
#include "StateImpl/CycleState/CycleStateImpl.h"
#include "StateImpl/DeadStateImpl.h"
#include "StateImpl/EscapeStateImpl.h"
#include "StateImpl/PlayerStateImpl.h"
#include "StateImpl/ReturnStateImpl.h"
#include "StateImpl/GuardSearch.h"
#include "StateImpl/ScriptStateImpl.h"

namespace StateAI
{
#define COMBINE_ID( a, b ) ( ( a << 16 ) | b )
#define BIND( className, state, impl ) \
	Add( state, impl, className::Create, className::Destroy )

	StateFactory::StateFactory()
	{
		// cycle implementation
		BIND( MoveToCellCycle, ST_CYCLE, CSI_MOVETO_CELL );
		BIND( MoveByStepCycle, ST_CYCLE, CSI_MOVEBY_STEP );
		BIND( FollowTargetCycle, ST_CYCLE, CSI_FOLLOW_TARGET );
		BIND( CycleState, ST_CYCLE, CSI_WRAPPER ); 

		// search implementation
		BIND( DHFPassiveGladiatorState, ST_SEARCH, SSI_DHF_PASSIVE_GLADIATOR );
		BIND( DHFGladiatorState, ST_SEARCH, SSI_DHF_GLADIATOR );
		BIND( GuardSearch, ST_SEARCH, SSI_GUARD );
		BIND( TauntPassiveGladiatorState, ST_SEARCH, SSI_TAUNT_PASSIVE_GLADIATOR );
		BIND( TauntGladiatorState, ST_SEARCH, SSI_TAUNT_GLADIATOR );

		BIND( FightStateNormal, ST_FIGHT, FSI_NORMAL );
		BIND( PeaceStateNormal, ST_PEACE, PSI_NORMAL );
		BIND( HangupStateNormal, ST_HANGUP, HSI_NORMAL );
		BIND( DeadStateNormal, ST_DEAD, DSI_NORMAL );
		BIND( ReturnStateNormal, ST_RETURN, RSI_NORMAL );
		BIND( EscapeStateNormal, ST_ESCAPE, ESI_NORMAL );
        BIND( PeaceStateNPC, ST_PEACE, PSI_NPC );
        
        // script ai state implementation
        BIND( PeaceStateScript, ST_PEACE, PSI_SCRIPT );
   		BIND( FightStateScript, ST_FIGHT, FSI_SCRIPT );
   		BIND( SearchStateScript, ST_SEARCH, SSI_SCRIPT );
   		BIND( EscapeStateScript, ST_ESCAPE, ESI_SCRIPT );

		// player state implementation
		BIND( PlayerPeaceState, ST_PEACE, PSI_PLAYER );
		BIND( PlayerFightState, ST_FIGHT, FSI_PLAYER );
		BIND( PlayerDeadState, ST_DEAD, DSI_PLAYER );
        BIND( PlayerAgonalState,ST_AGONAL,ASI_PLAYER);
	}

	bool StateFactory::Add( long state_type, long state_impl, CreateAIStateT create, DestroyAIStateT destroy )
	{
		return m_Factory.Add( COMBINE_ID( state_type, state_impl ), create, destroy );
	}

	AIState *StateFactory::Create( long state_type, long state_impl, MachineType *machine )
	{
		long id = COMBINE_ID( state_type, state_impl );
		return m_Factory.Create( id, machine );
	}

	void StateFactory::Destroy( long state_type, long state_impl, AIState *state )
	{
		long id = COMBINE_ID( state_type, state_impl );
		m_Factory.Destroy( id, state );
	}
}
