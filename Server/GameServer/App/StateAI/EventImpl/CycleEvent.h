///
/// @file CycleEvent.h
/// @author Kevin Lynx
/// @brief Declare Cycle AI state event.
///
#ifndef ___STATEAI_CYCLEEVENT_H_
#define ___STATEAI_CYCLEEVENT_H_

#include "../AIStateDefs.h"
#include "../GlobalDef.h"

namespace StateAI
{
	enum CycleEvenType
	{
		CET_MOVETO_CELL,
		CET_MOVEBY_STEP,
		CET_FOLLOW_TARGET,
	};

	struct CycleEventBase : public AIEvent
	{
		CycleEventBase( long sub_type ) : AIEvent( ET_CYCLE )
		{
			m_SubType = sub_type;
		}

		long SubType() const
		{
			return m_SubType;
		}
		long m_SubType;
	};

	/// Can use kl_common::tuple to support more parameters.
	template <long type, typename P1, typename P2 = long, typename P3 = long>
	struct CycleEventTemplate : public CycleEventBase
	{
		CycleEventTemplate() : p1( P1() ), p2( P2() ), p3( P3() ),  CycleEventBase( type )
		{
		}

		CycleEventTemplate( P1 p1_, P2 p2_ = P2() , P3 p3_ = P3() ) : CycleEventBase( type ),
			p1( p1_ ), p2( p2_ ), p3( p3_ )
		{
		}

		P1 p1;
		P2 p2;
		P3 p3;
	};

	/// <x, y, null>
	typedef CycleEventTemplate<CET_MOVETO_CELL, long, long> CEMoveToCell;

	/// <direction, step, null>
	typedef CycleEventTemplate<CET_MOVEBY_STEP, long, long> CEMoveByStep;

	/// <TargetGUID, TargetType, time>
	typedef CycleEventTemplate<CET_FOLLOW_TARGET, CGUID, long, long> CEFollowTarget;

}


#endif
