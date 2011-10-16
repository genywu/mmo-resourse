///
/// @file AIEventSender.h
/// @author Kevin Lynx
///
#ifndef ___AIEVENT_SENDER_H_
#define ___AIEVENT_SENDER_H_

#include "GlobalDef.h"

namespace StateAI
{
	class BaseAI;
	class AIState;

	/// maybe a friend class to BaseAI so that it can access some 
	/// private properties of BaseAI to accomplish some operations.
	class AIEventSender
	{
	public:
		AIEventSender() : m_AI( NULL )
		{
		}
        
        void SetAI( BaseAI *ai );

		void Wakeup();

		void WhenBeenHurted( long attackerType, const CGUID &attackerId, long hurt );
	
		void WhenBeenKilled( long attackerType, const CGUID &attackerId );

        void WhenPlayerAgonal();

        void WhenPlayerLeavePhase();

        void Relive();

		void MoveToCell( long x, long y );

		void MoveByStep( long direct, long step );

		void FollowTarget( const CGUID &targetID, long targetType, long time );

        void ReturnPeace();

		void OnSkillEnd( long sub = 0 );

        void OnBorn();

        void OnReborn();
	private:
		AIState *CycleState();
	private:
		BaseAI *m_AI;
	};

    AIEventSender &GetEventSender( BaseAI *ai );

#define AI_EVENT_SENDER( ai ) ::StateAI::GetEventSender( ai )
}

#endif
