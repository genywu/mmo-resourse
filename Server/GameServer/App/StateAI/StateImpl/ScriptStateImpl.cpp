///
/// @file ScriptStateImpl.cpp
///
#include "stdafx.h"
#include "ScriptStateImpl.h"
#include "../Helper/AIUtils.h"
#include "../MonsterAI.h"
#include "../AIDriver.h"
#include "../../Monster.h"
#include "../../LuaScript/ScriptSys.h"
#include "../../LuaScript/Script.h"

namespace StateAI
{
    IMPL_LIFE_POLICY_FUNC( PeaceStateScript, AIState );

    bool PeaceStateScript::PreExecute( BaseType::EntityType *entity )
    {
        if( SearchEnemy( entity ) ||
            CheckHangup( entity ) ||
            CheckCycleState( entity ) )
        {
            return true;
        }
        if( CheckRunByRoad( entity ) )
        {
            return true;
        }
        return false;
    }

    IMPL_LIFE_POLICY_FUNC( FightStateScript, AIState );

    SkillTracer::SkillTracer()
    {
        Reset();
    }

    void SkillTracer::Reset()
    {
        m_TargetID = NULL_GUID;
        m_SkillID = m_SkillLvl = 0;
        m_SkillMaxDis = m_SkillMinDis = 0;
        m_EndTime = 0;
    }

    void SkillTracer::Init( long id, long lvl, const CGUID &target, ulong traceTime )
    {
        m_SkillID  = id;
        m_SkillLvl = lvl;
        m_TargetID = target;
        m_EndTime = timeGetTime() + traceTime;
        
        m_SkillMaxDis = (float)GetInst(SkillAttribute).GetStrCondif(m_SkillID,lvl,"MaxAtkDistance");
        m_SkillMinDis = (float)GetInst(SkillAttribute).GetStrCondif(m_SkillID,lvl,"MinAtkDistance");

    }

    int SkillTracer::Run( BaseAI *ai )
    {
        if( timeGetTime() >= m_EndTime )
        {
            return TIMEOUT;
        }
        CMoveShape *self = ai->GetOwner();
        CServerRegion *rgn = dynamic_cast<CServerRegion*>( self->GetFather() );
        CMoveShape *target = AIUtils::FindShape( rgn, m_TargetID );
        if( target == NULL )
        {
            return FAILED;
        }
#ifdef MONSTER_DIS_FLOAT
        float dis = self->RealDistanceF( target );
#else
        long dis = self->Distance( target );
#endif
        long dx = target->GetTileX();
        long dy = target->GetTileY();
        if( dis > m_SkillMaxDis )
        {
            ai->Move( dx, dy );
        }
        else if( dis < m_SkillMinDis )
        {
            long dir = GetLineDir( dx, dy, self->GetTileX(), self->GetTileY() );
            ai->Move( dir );
        }
        else
        {
            return Okay;
        }
        return RUNNING;
    }

    void FightStateScript::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
    {
        SelfType::ReceiveEvent( entity, ev );
        if( ev.Type() == ET_SCRIPT_USESKILL )
        {
            const ScriptUseSkillEvent &sev = (const ScriptUseSkillEvent&) ev;
            CMonster *monster = dynamic_cast<CMonster*>( entity->GetOwner() );
            CServerRegion *region = dynamic_cast<CServerRegion*>( monster->GetFather() );
            MonsterAI *ai = dynamic_cast<MonsterAI*>( entity );
            long id = sev.id;
            long lvl = sev.lvl;
            long actTime = monster->GetActModify( sev.id );
            if( sev.type == ScriptUseSkillEvent::SHAPE )
            {
                if( sev.trace )
                {
                    // trace the target
                    m_SkillTracer.Init( id, lvl, sev.targetId, sev.traceTime );
                    m_skillScriptID = sev.scriptId;
                    m_State = TRACING;
                    entity->Resume( 0 );
                    return ;
                }
                else
                {
                    CMoveShape *target = AIUtils::FindShape( region, sev.targetId );
                    if( target == NULL )
                    {
                        OnUseSkillEnd( SkillTracer::FAILED );
                        return ;
                    }
                    ai->BeginSkill( id, lvl, target );
                }
            }
            else if( sev.type == ScriptUseSkillEvent::CELL )
            {
                ai->BeginSkill( id, lvl, sev.x, sev.y );
            }
            m_skillScriptID = sev.scriptId;
            // register act time timer
            ai->SetTimer( AIDriver::SCRIPT_EVENT, actTime );
        }
        else if( ev.Type() == ET_SCRIPT_SKILLEND )
        {
            OnUseSkillEnd( SkillTracer::Okay );
        }
        else if( ev.Type() == ET_KILL )
        {
            entity->ChangeState( ST_DEAD );
        }
    }

    void FightStateScript::Reset()
    {
        m_State = NORMAL;
        m_SkillTracer.Reset();
    }

    bool FightStateScript::PreExecute( BaseType::EntityType *entity )
    {
        // search enemy first
	    SearchEnemy( entity );	

		// check cycle action
        if( CheckCycleState( entity ) )
		{
			return true;
		}
        
        if( CheckTracer( entity ) )
        {
            return true;
        }

        return false;
    }

    bool FightStateScript::CheckTracer( BaseType::EntityType *entity )
    {
        if( m_State == TRACING )
        {
            int ret = m_SkillTracer.Run( entity );
            if( ret == SkillTracer::TIMEOUT ||
                ret == SkillTracer::FAILED )
            {
                OnUseSkillEnd( ret );
            }
            if( ret == SkillTracer::Okay )
            {
                MonsterAI *ai = dynamic_cast<MonsterAI*>( entity );
                CMonster *monster = dynamic_cast<CMonster*>( entity->GetOwner() );
                CServerRegion *region = dynamic_cast<CServerRegion*>( monster->GetFather() );
                CMoveShape *target = AIUtils::FindShape( region, m_SkillTracer.m_TargetID );
                if( target == NULL )
                {
                    OnUseSkillEnd( SkillTracer::FAILED );
                }
                else
                {
                    ai->BeginSkill( m_SkillTracer.m_SkillID, m_SkillTracer.m_SkillLvl, target );
                    long actTime = monster->GetActModify( m_SkillTracer.m_SkillID );
                    ai->SetTimer( AIDriver::SCRIPT_EVENT, actTime );
                }
            }
            return true;
        }

        return false;
    }

    void FightStateScript::OnUseSkillEnd( long ret )
    {
        Script *script = GetInst( ScriptSys ).GetScript( m_skillScriptID );
        if( script != NULL )
        {
            script->PushRetValue( ret );
            GetInst( ScriptSys ).ResumeScript( script, 1 );
        }
        m_State = NORMAL;
    }

    IMPL_LIFE_POLICY_FUNC( EscapeStateScript, AIState );
    IMPL_LIFE_POLICY_FUNC( SearchStateScript, AIState );
}
