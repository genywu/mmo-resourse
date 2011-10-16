///
/// @file Event.h
/// @author Kevin Lynx
/// @brief Declare some AI events.
///
#ifndef ___STATEAI_EVENT_H_
#define ___STATEAI_EVENT_H_

#include "../AIStateDefs.h"
#include "../GlobalDef.h"

namespace StateAI
{
    struct InitEvent : public AIEvent
    {
        InitEvent( void *arg ) : AIEvent( ET_INIT )
        {
            this->arg = arg;
        }
        void *arg;
    };

	struct HurtEvent : public AIEvent
	{
		HurtEvent( long hurt, const CGUID &attackerId, long attackerType ) : 
			AttackerId( attackerId ), AIEvent( ET_HURT )
		{
			Hurt = hurt; 
			AttackerType = attackerType;
		}

		const CGUID &AttackerId;
		long AttackerType;
		long Hurt;
	};

	struct KillEvent : public AIEvent
	{
		KillEvent( const CGUID &attackerId, long attackerType ) :
			AttackerId( attackerId ), AIEvent( ET_KILL )
		{
			AttackerType = attackerType;
		}

		const CGUID &AttackerId;
		long AttackerType;
	};

	struct ReturnEvent : public AIEvent
	{
		ReturnEvent( long destX = 0, long destY = 0 ) : AIEvent( ET_RETURN )
		{
			this->destX = destX;
			this->destY = destY;
		}

		long destX;
		long destY;
	};

    struct ScriptUseSkillEvent : public AIEvent
    {
        enum { SHAPE, CELL };
        ScriptUseSkillEvent( const CGUID &sid, long x, long y, long id, long lvl ) : AIEvent( ET_SCRIPT_USESKILL ),
            scriptId( sid )
        {
            this->x = x; this->y = y;
            this->id = id; this->lvl = lvl;
            type = CELL;
            trace = false;
            traceTime = 0;
        }

        ScriptUseSkillEvent( const CGUID &sid, const CGUID &tid, long id, long lvl, bool trace, ulong traceTime ) :
            AIEvent( ET_SCRIPT_USESKILL ), scriptId( sid ), targetId( tid )
        {
            this->x = this->y = 0;
            this->id = id; this->lvl = lvl;
            this->trace = trace;
            this->traceTime = traceTime;
            type = SHAPE;
        }

        int type;
        long x, y;
        CGUID targetId;
        CGUID scriptId;
        long id, lvl;
        bool trace;
        ulong traceTime;
    };

    struct UseSkillEndEvent : public AIEvent
    {
        UseSkillEndEvent( long sub ) : AIEvent( ET_USESKILLEND ), type( sub )
        {
        }
        long type;
    };

    /// Default event template, only has a type.
    template <int type>
    struct DefaultEvent : public AIEvent
    {
        DefaultEvent() : AIEvent( type )
        {
        }
    };

    /// Some concrete default event type
    typedef DefaultEvent<ET_WAKEUP> WakeupEvent;
    typedef DefaultEvent<ET_USESKILL> UseSkillEvent;
    typedef DefaultEvent<ET_BORN> BornEvent;
    typedef DefaultEvent<ET_TIMERCYCLE>      CycleTimerEvent;
    typedef DefaultEvent<ET_REBORN>          RebornEvent;
    typedef DefaultEvent<ET_SCRIPT_SKILLEND> ScriptSkillEndEvent;
    typedef DefaultEvent<ET_AGONAL>          AgonalEvent;
    typedef DefaultEvent<ET_LEAVE_PHASE>     LeaveAgonalEvent;
    typedef DefaultEvent<ET_RELIVE>          ReLiveEvent;
}

#endif
