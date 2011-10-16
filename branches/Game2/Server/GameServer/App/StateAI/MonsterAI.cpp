///
/// @file MonsterAI.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "AIDriver.h"
#include "AIEventSender.h"
#include "Helper/FightObj.h"
#include "Helper/RunByRoad.h"
#include "EventImpl/Event.h"
#include "MonsterAI.h"
#include "../Monster.h"
#include "../MonsterTaunt/TauntEvents.h"

namespace StateAI
{
	MonsterAI::MonsterAI( CMonster *owner ) : BaseAI( owner )
	{
		m_FightObj = new FightObj( this );
        m_RunByRoad = new RunByRoad( this );
		memset( &m_BornPos, 0, sizeof( m_BornPos ) );
        m_RebornTime = 0;
	}

	MonsterAI::~MonsterAI()
	{
		delete m_FightObj;
        delete m_RunByRoad;
	}

	void MonsterAI::Reset()
	{
		BaseAI::Reset();
		m_FightObj->Reset();
        m_RebornTime = 0;
	}

    void MonsterAI::Run( long timerID, long timerType )
    {
        BaseAI::Run( timerID, timerType );
        if( timerType == AIDriver::CYCLE_EVENT )
        {
            OnEvent( CycleTimerEvent() );
        }
        else if( timerType == AIDriver::SCRIPT_EVENT )
        {
            SendEvent( ScriptSkillEndEvent() );
        }
    }

    bool MonsterAI::StartCycle( ulong trigerTime, ulong interTime )
    {
        return SetTimer( AIDriver::CYCLE_EVENT, trigerTime, interTime );
    }

    void MonsterAI::StopCycle()
    {
        StopTimer( AIDriver::CYCLE_EVENT );
    }

	bool MonsterAI::BeginSkill( long skillId, long skillLvl, long x, long y, CMoveShape* target )
	{
		CMonster *monster = static_cast<CMonster*>( GetOwner() );
		monster->InitSkillInstance( skillId, skillLvl, 0, 0, 0, NULL_GUID, 5 );
		if( !BaseAI::BeginSkill( monster->GetInstanceSkill(), x, y, target ) )
		{
			Resume( OPER_FAILED_TIME );
			return false;
		}
		return true;
	}
    bool MonsterAI::BeginSkill( long skillId, long skillLvl, long x, long y )
    {
        CMoveShape *target = GetTarget();
        if( target == NULL )
        {
            Resume( OPER_FAILED_TIME );
            return false;
        }
        return BeginSkill( skillId, skillLvl, x, y, target );
    }

    bool MonsterAI::BeginSkill( long skillId, long skillLvl, CMoveShape *target )
    {
        return BeginSkill( skillId, skillLvl, target->GetTileX(), target->GetTileY(), target );
    }

	bool MonsterAI::RandomRun()
	{
		CMonster *monster = static_cast<CMonster*>( GetOwner() );
		assert( monster );
		if( random( 10000 ) < monster->GetMoveRandomValue() )
		{
			long dir = 0;
			long curX = monster->GetTileX();
			long curY = monster->GetTileY();
			long dis = monster->Distance( curX, curY, m_BornPos.x, m_BornPos.y );
			if( dis > monster->GetPeaceMoveRange() )
			{
				dir = GetLineDir( curX, curY, m_BornPos.x, m_BornPos.y );
				long gdir = ( 8 - dir ) % 8;
				if( gdir >= 2 || gdir <= 6 )
				{
					dir = ( dir + random( 3 ) - 1 + 8 ) % 8;
				}
			}
			else
			{
				dir = random( 8 );
			}

			long maxRunTimes = monster->GetMaxRunTimes();
			long minRunTimes = monster->GetMinRunTimes();

			AI_EVENT_SENDER( this ).MoveByStep( dir, random( maxRunTimes - minRunTimes ) + minRunTimes );
			// drive the ai
			Resume( 0 );
		}
		else
		{
			Stand( monster->GetStopFrame() );
		}
		return true;
	}

	void MonsterAI::SetBornPos( long x, long y )
	{
		m_BornPos.x = x;
		m_BornPos.y = y;
	}

	void MonsterAI::EscapeSpring()
	{
		CMonster *monster = static_cast<CMonster*>( GetOwner() );
		if( monster->GetHP() * 100.0f / monster->GetMaxHP() <= monster->GetEscapePoint() )
		{
			// change to escape state
			ChangeState( ST_ESCAPE );
		}
	}

    void MonsterAI::GBHSpring( long hurt )
    {
		CMonster *monster = static_cast<CMonster*>( GetOwner() );
        int hurtProportionHp= (int) ( monster->GetMaxHP() * monster->GetHurtProportion() / 100 );

        int hp= monster->GetHP();
        int lastHp= hp + hurt;

        if( hurtProportionHp < lastHp && hurtProportionHp >= hp )
        {
            RunScript( monster->GetGBHScriptName().c_str() );
        }
    }

    bool MonsterAI::IsInKindlyTime()
    {
        if( m_RebornTime == 0 )
        {
            return false;
        }
        return m_RebornTime + GlobalSetup::GetSetup()->dwMonsterKindlyTime >= timeGetTime();	
    }

	void MonsterAI::OnEvent( const AIEvent &event )
	{
        BaseAI::OnEvent( event );
        CMonster *monster = dynamic_cast<CMonster*>( m_Owner );
		switch( event.Type() )
		{
		case ET_HURT:
			{
				m_FightObj->OnHurted();
				EscapeSpring();

                const HurtEvent &hevent = (const HurtEvent&) event;
                GBHSpring( hevent.Hurt );

                // notify taunt event.
                TauntEvents events( monster->GetTauntHandler() );
                events.Attacked( hevent.AttackerId, hevent.AttackerType, hevent.Hurt );
			}
			break;

        case ET_BORN:
            {
                Resume( 0 );
                RunScript( monster->GetBornScriptName().c_str() );
            }
            break;

        case ET_TIMERCYCLE:
            {
                RunScript( monster->GetCycleScript().c_str() );
            }
            break;

        case ET_REBORN:
            {
                m_RebornTime = timeGetTime();
                Resume( 0 );
                RunScript( monster->GetBornScriptName().c_str() );
            }
            break;

        case ET_RETURN:
            {
                m_FightObj->ReturnPeace();
            }
            break;

        case ET_KILL:
            {
                RunScript( monster->GetDeadScriptName().c_str() );
                monster->OnDied();      
            }
            break;
		}
	}
}
