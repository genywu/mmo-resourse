///
/// @file CycleStateImpl.cpp
///
#include "stdafx.h"
#include <assert.h>
#include "CycleStateImpl.h"
#include "../StateImplDef.h"
#include "../../BaseAI.h"
#include "../../MonsterAI.h"
#include "../../EventImpl/CycleEvent.h"
#include "../../StateFactory.h"
#include "../../../ServerRegion.h"

namespace StateAI
{
	IMPL_LIFE_POLICY_FUNC( MoveToCellCycle, AIState );

	void MoveToCellCycle::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
        if( ev.Type() == ET_CYCLE )
        {
            const CycleEventBase &ceb = (const CycleEventBase&) ev;
            if( ceb.SubType() == CET_MOVEBY_STEP )
            {
                m_Data = (const CEMoveToCell&) ceb;
            }
        }
	}

	bool MoveToCellCycle::ExecuteWithReturn( BaseType::EntityType *entity )
	{
        if( !entity->Move( m_Data.p1, m_Data.p2 ) )
        {
            return false;
        }
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	IMPL_LIFE_POLICY_FUNC( MoveByStepCycle, AIState );

	void MoveByStepCycle::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_CYCLE )
		{
			const CycleEventBase &ceb = (const CycleEventBase&) ev;
			if( ceb.SubType() == CET_MOVEBY_STEP )
			{
				m_Data = (const CEMoveByStep&) ceb;
			}
		}
	}

	bool MoveByStepCycle::ExecuteWithReturn( BaseType::EntityType *entity )
	{
		if( --m_Data.p2 <= 0 )
		{
			return false;
		}
		entity->Move( m_Data.p1 );
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	IMPL_LIFE_POLICY_FUNC( FollowTargetCycle, AIState );

	void FollowTargetCycle::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_CYCLE )
		{
			const CycleEventBase &ceb = (const CycleEventBase&) ev;
			if( ceb.SubType() == CET_FOLLOW_TARGET )
			{
				m_Data = (const CEFollowTarget&) ceb;
			}
		}
	}

	bool FollowTargetCycle::ExecuteWithReturn( BaseType::EntityType *entity )
	{
		if( timeGetTime() >= (unsigned long)m_Data.p3 )
		{
			return false;
		}

		CMoveShape *owner = entity->GetOwner();
		CServerRegion *region = static_cast<CServerRegion*>( owner->GetFather() );
		CMoveShape *target = static_cast<CMoveShape*>( region->FindAroundObject( owner, 
			m_Data.p2, m_Data.p1 ) );
		if( target == NULL )
		{
			return false;
		}

		entity->Move( target->GetTileX(), target->GetTileY() );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	IMPL_LIFE_POLICY_FUNC( CycleState, AIState );

#define SUB_STATE_INDEX( i ) (CSI_MOVETO_CELL+i)

	CycleState::CycleState( MachineType *machine ) : CycleStateBase( machine )
	{
		SetState( CSI_INVALID );
		// create manually.
		for( int i = 0; i < CSI_SUB_STATE_COUNT; ++ i )
		{
			m_States[i] = (CycleStateBase*)GetInst( StateFactory ).Create( Type(), SUB_STATE_INDEX( i ), machine );
		}
	}

	CycleState::~CycleState()
	{
		for( int i = 0; i < CSI_SUB_STATE_COUNT; ++ i )
		{
			GetInst( StateFactory ).Destroy( Type(), SUB_STATE_INDEX( i ), m_States[i] );
		}
	}

	void CycleState::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_CYCLE )
		{
			const CycleEventBase &ceb = (const CycleEventBase&) ev;
			SetState( ceb.SubType() );
			m_States[m_CurState]->ReceiveEvent( entity, ev );
		}
	}

	bool CycleState::ExecuteWithReturn( BaseType::EntityType *entity )
	{
		if( HasState() )
		{
			return m_States[m_CurState]->ExecuteWithReturn( entity );
		}
		return false;
	}

	void CycleState::SetState( long state )
	{
		assert( state >= 0 && state < CSI_SUB_STATE_COUNT || state == CSI_INVALID );
		m_CurState = state;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CycleStateExecutor::Execute( AIState *state, AIState::EntityType *entity )
	{
		assert( state->Type() == ST_CYCLE );
		CycleStateBase *cycle_state = (CycleStateBase*) state;
		return cycle_state->ExecuteWithReturn( entity );
	}
}
