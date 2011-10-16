///
/// @file FightObj.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "FightObj.h"
#include "../EventImpl/Event.h"
#include "../MonsterAI.h"
#include "../../PhaseSystem/PhaseManager.h"
#include "../../Monster.h"
#include "../../MonsterTaunt/TauntEvents.h"
#include "../../../../../Public/Setup/GlobalSetup.h"

namespace StateAI
{
	FightObj::FightObj( MonsterAI *ai ) : m_AI( ai )
	{
		m_Owner = static_cast<CMonster*>( ai->GetOwner() );
        Reset();
	}

	void FightObj::Reset()
	{
		memset( &m_EnterFightPos, 0, sizeof( m_EnterFightPos ) );
		m_HurtTime = 0;
		m_GuardTime = 0;
	}

	void FightObj::OnHurted()
	{
		m_HurtTime = timeGetTime() + m_Owner->GetChaseTime();
	}

	void FightObj::LoseTarget()
	{
		m_GuardTime = timeGetTime() + m_Owner->GetGuardTime();
		m_AI->SetTarget( NULL_GUID, 0 );
	}

	bool FightObj::CheckReturnPeace()
	{
		long dis = Distance( m_EnterFightPos.x, m_EnterFightPos.y, m_Owner->GetTileX(), 
			m_Owner->GetTileY() );

		unsigned long thisTime = timeGetTime();
		if( m_AI->HasTarget() )
		{
            CMoveShape *target = m_AI->GetTarget();

            if ( target == NULL || PHASEMAN.PhasePlayerFilter(target,m_Owner) )
            {
                ReturnPeace();
                return true;
            }

            if( target->IsDied() ||
                !target->IsAttackAble( m_Owner ) ||
                !target->IsVisible( m_Owner )  )
            {
                ReturnPeace();
                return true;
            }

            if( dis > m_Owner->GetBaseChaseRange() )
            {
                ReturnPeace();
                return true;
            }

			if( dis > m_Owner->GetReturnRange() && thisTime > m_HurtTime )
			{
                ReturnPeace();
				return true;
			}
		}
		else if( thisTime > m_GuardTime )
		{
            ReturnPeace();
			return true;
		}

		return false;
	}

	void FightObj::EnterFight()
	{
		m_EnterFightPos.x = m_Owner->GetTileX();
		m_EnterFightPos.y = m_Owner->GetTileY();
	}

    void FightObj::ReturnPeace()
    {
        CMoveShape *target = m_AI->GetTarget();
        if( target )
        {
            TauntEvents events( m_Owner->GetTauntHandler() );
            events.TargetLost( target->GetExID(), target->GetType() );
        }
        m_AI->ChangeState( ST_RETURN, ReturnEvent( m_EnterFightPos.x, m_EnterFightPos.y ) );
    }
}
