///
/// @file ScriptMonsterFunc.cpp
/// @brief ²Î¼û ScriptMonsterFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Monster.h"
#include "../../StateAI/BaseAI.h"
#include "../../StateAI/MonsterAI.h"
#include "../../StateAI/AIEventSender.h"

#define NULL_RET ;
#define CHECK_MONSTER( ret ) \
    CMonster *monster = dynamic_cast<CMonster*>( m_RunEnv.srcShape ); \
    if( monster == NULL ) return ret

int Script::ContinueAI( ulong millSecs )
{
    CHECK_MONSTER( 0 );
    StateAI::BaseAI *ai = monster->GetAI();
    ai->Resume( millSecs );
    return 1;
}

void Script::MoveToCell( long x, long y )
{
    CHECK_MONSTER( NULL_RET );
    AI_EVENT_SENDER( monster->GetAI() ).MoveToCell( x, y );
}

void Script::RandomMove()
{
    CHECK_MONSTER( NULL_RET );
    StateAI::MonsterAI *ai = dynamic_cast<StateAI::MonsterAI*>( monster->GetAI() );
    ai->RandomRun();
}

void Script::MoveByStep( long direct, long step )
{
    CHECK_MONSTER( NULL_RET );
    AI_EVENT_SENDER( monster->GetAI() ).MoveByStep( direct, step );
}

void Script::FollowTarget( const CGUID &targetID, long targetType, long time )
{
    CHECK_MONSTER( NULL_RET );
    AI_EVENT_SENDER( monster->GetAI() ).FollowTarget( targetID, targetType, time );
}

CGUID Script::GetTarget( long *targetType )
{
    *targetType = 0;
    CHECK_MONSTER( NULL_GUID );
    CMoveShape *target = monster->GetAI()->GetTarget();
    if( target == NULL )
    {
        *targetType = 0;
        return NULL_GUID;
    }
    *targetType = target->GetType();
    return target->GetExID();
}
