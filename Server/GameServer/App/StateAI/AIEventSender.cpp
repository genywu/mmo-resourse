///
/// @file AIEventSender.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "AIEventSender.h"
#include "BaseAI.h"
#include "EventImpl/CycleEvent.h"
#include "EventImpl/Event.h"

namespace StateAI
{
    static AIEventSender s_eventSender;

    AIEventSender &GetEventSender( BaseAI *ai )
    {
        s_eventSender.SetAI( ai );
        return s_eventSender;
    }

    void AIEventSender::SetAI( BaseAI *ai )
    {
        m_AI = ai;
    }

	void AIEventSender::Wakeup()
	{
		m_AI->SendEvent( WakeupEvent() );
	}

	void AIEventSender::WhenBeenHurted( long attackerType, const CGUID &attackerId, long hurt )
	{
		m_AI->SendEvent( HurtEvent( hurt, attackerId, attackerType ) );
	}

	void AIEventSender::WhenBeenKilled( long attackerType, const CGUID &attackerId )
	{
		m_AI->SendEvent( KillEvent( attackerId, attackerType ) );
	}

    void AIEventSender::WhenPlayerAgonal()
    {
        m_AI->SendEvent( AgonalEvent() );
    }

    void AIEventSender::WhenPlayerLeavePhase()
    {
        m_AI->SendEvent( LeaveAgonalEvent() );
    }

    void AIEventSender::Relive()
    {
        m_AI->SendEvent( ReLiveEvent() );
    }

	void AIEventSender::MoveToCell( long x, long y )
	{
		CycleState()->ReceiveEvent( m_AI, CEMoveToCell( x, y ) );
	}

	void AIEventSender::MoveByStep( long direct, long step )
	{
		CycleState()->ReceiveEvent( m_AI, CEMoveByStep( direct, step ) );
	}

	void AIEventSender::FollowTarget( const CGUID &targetID, long targetType, long time )
	{
		CycleState()->ReceiveEvent( m_AI, CEFollowTarget( targetID, targetType, 
			timeGetTime() + time ) );
	}

    void AIEventSender::ReturnPeace()
    {
        m_AI->SendEvent( ReturnEvent() );
    }

	void AIEventSender::OnSkillEnd( long sub )
	{
		m_AI->SendEvent( UseSkillEndEvent( sub ) );
	}

    void AIEventSender::OnBorn()
    {
        m_AI->SendEvent( BornEvent() );
    }

    void AIEventSender::OnReborn()
    {
        m_AI->SendEvent( RebornEvent() );
    }

	AIState *AIEventSender::CycleState()
	{
		return m_AI->GetState( ST_CYCLE );
	}
}

