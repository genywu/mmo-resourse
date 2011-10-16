///
/// @file FightStateImpl.cpp
///
#include "stdafx.h"
#include "CycleState/CycleStateImpl.h"
#include "FightStateImpl.h"
#include "../AIEventSender.h"
#include "../BaseAI.h"
#include "../Helper/FightObj.h"
#include "../MonsterAI.h"
#include "../../Monster.h"

namespace StateAI
{
    bool FightStateBase::SearchEnemy( BaseType::EntityType *entity )
    {
		AIState *search_action = entity->GetState( ST_SEARCH );
		assert( search_action );
		search_action->Execute( entity );
        return false;
    }

    bool FightStateBase::CheckCycleState( BaseType::EntityType *entity )
    {
		AIState *cycle_action = entity->GetState( ST_CYCLE );
		CycleStateExecutor cycleExecutor;
		if( cycleExecutor.Execute( cycle_action, entity ) )
		{
			return true;
		}
        return false;
    }

	IMPL_LIFE_POLICY_FUNC( FightStateNormal, AIState );

    static std::vector<CMonster::tagSkillRecord> &GetSkillList( CMonster *monster, int type )
    {
        if( type == SKILLTYPE_BASE )
        {
            return monster->GetBaseSkillList();
        }
        else if( type == SKILLTYPE_NORMAL )
        {
            return monster->GetSkillList();
        }
        else
        {
            return monster->GetCycleSkillList();
        }
    }

	FightStateNormal::FightStateNormal( MachineType *machine ) : FightStateBase( machine )
	{
	}

	FightStateNormal::~FightStateNormal()
	{
	}

	void FightStateNormal::Enter( BaseType::EntityType *entity, const AITransition &tran )
	{
		ClearSelSkill();

		// record the enter fight position.
		MonsterAI *monsterAI = static_cast<MonsterAI*>( entity );
		monsterAI->GetFightObj().EnterFight();

        // on fight script
        CMonster *monster = dynamic_cast<CMonster*>( entity->GetOwner() );
        if( monster != NULL )
        {
            entity->RunScript( monster->GetFightScriptName().c_str() );
        }

        m_NormalSkillTimeStamp = 0;
	}

	void FightStateNormal::ClearSelSkill()
	{
		memset( &m_SelSkill, 0, sizeof( m_SelSkill ) );
	}

	void FightStateNormal::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
		if( ev.Type() == ET_HURT )
		{
		}
		else if( ev.Type() == ET_KILL )
		{
			entity->ChangeState( ST_DEAD );
		}
		else if( ev.Type() == ET_USESKILLEND )
		{
			entity->Resume( 0 );
		}
	}

	void FightStateNormal::Execute( BaseType::EntityType *entity )
	{
		// search enemy first
	    SearchEnemy( entity );	

		// check cycle action
        if( CheckCycleState( entity ) )
		{
			return ;
		}

		// can return peace ?
		MonsterAI *monsterAI = static_cast<MonsterAI*>( entity );
		if( monsterAI->GetFightObj().CheckReturnPeace() )
		{
			// return peace
			return ;
		}

		// attack the target
		CMoveShape *target = entity->GetTarget();
		if( target == NULL )
		{
			// return peace ?
			monsterAI->GetFightObj().LoseTarget();
			entity->Resume( OPER_FAILED_TIME );
			return ;
		}
		Attack( entity, target );
	}

	void FightStateNormal::Attack( BaseType::EntityType *entity, CMoveShape *target )
	{
		CMonster *monster= static_cast<CMonster*>( entity->GetOwner() );
        long targetDis = monster->RealDistance( target );
		if( m_SelSkill.id == 0 )
		{
			// search one skill to attack
			if( !SearchSkill( monster, targetDis ) )
			{
				entity->Resume( OPER_FAILED_TIME );
				return ;
			}
		}

		// we assume we have one skill to attack the target
		if( !GetInst(SkillAttribute).IsExist(m_SelSkill.id , m_SelSkill.level) )
		{
			// error
			entity->Resume( OPER_FAILED_TIME );
			ClearSelSkill();
			return ;
		}

		float minAtkDis = (float)monster->GetSkillValue( m_SelSkill.id , m_SelSkill.level , "MinAtkDistance" );
		float maxAtkDis = (float)monster->GetSkillValue( m_SelSkill.id , m_SelSkill.level , "MaxAtkDistance" );
		
        bool used = false;

		// we assume the target is still in the track range.
		if( targetDis > maxAtkDis )
		{
			entity->Move( target->GetTileX(), target->GetTileY() );
		}
		else if( targetDis < minAtkDis )
		{
			long dir = GetLineDir( target->GetTileX(), target->GetTileY(), 
				monster->GetTileX(), monster->GetTileY() );
			entity->Move( dir );
		}
		else
		{
			// in the attack range
			if( static_cast<MonsterAI*>( entity )->BeginSkill( m_SelSkill.id, m_SelSkill.level, 
				target->GetTileX(), target->GetTileY(), target ) )
			{
				// use this skill ok, clear the selection.
				OnSkillUsed( monster );
                used = true;
			}
		}
        // clear the selected skill if it's not a cycle skill
        if( !used && m_SelSkill.type != SKILLTYPE_CYCLE )
        {
            ClearSelSkill();
        }
	}

	void FightStateNormal::OnSkillUsed( CMonster *monster )
	{
        std::vector<CMonster::tagSkillRecord> &skillList = GetSkillList( monster, m_SelSkill.type );

		CMonster::tagSkillRecord &record = skillList[m_SelSkill.index];
		record.dwCoolDownTime = timeGetTime() + record.dwCool;
		ClearSelSkill();
	}

	bool FightStateNormal::SearchSkill( CMonster *monster, long dis )
	{
        // 选择周期技能
        if( SearchSkill( SKILLTYPE_CYCLE, monster, dis ) )
        {
            return true;
        }
        unsigned long thisTime = timeGetTime();
        if( thisTime > m_NormalSkillTimeStamp )
        {
            m_NormalSkillTimeStamp = thisTime + monster->GetSkillSpace();
            int proportion = monster->GetSkillProportion();
            if( random( 100 ) < proportion )
            {
                // 选择普通技能
                if( SearchSkill( SKILLTYPE_NORMAL, monster, dis ) )
                {
                    return true;
                }
            }
        }
        // 选择基本技能
		return SearchSkill( SKILLTYPE_BASE, monster, dis );
	}
    
    bool FightStateNormal::SearchSkill( int type, CMonster *monster, long dis )
    {
        std::vector<CMonster::tagSkillRecord> &skillList = GetSkillList( monster, type );
        if( skillList.size() == 0 )
        {
            return false;
        }
		unsigned long thisTime = timeGetTime();
		std::vector<SkillInfo> selSkillList;
		for( size_t i = 0; i < skillList.size(); ++ i )
		{
			if( skillList[i].dwCoolDownTime > thisTime ) 
			{
				continue;
			}
            if( !CheckSkill( monster, skillList[i].wID, skillList[i].wLevel, type, dis ) )
            {
                continue;
            }
			SkillInfo info;
			info.id = skillList[i].wID;
			info.level = skillList[i].wLevel;
			info.index = (long)i;
			info.type = type;
			selSkillList.push_back( info );
		}

		if( selSkillList.size() == 0 )
		{
			return false;
		}

		size_t i = random( (int)selSkillList.size() );
		m_SelSkill = selSkillList[i];
		return true;
    }

    bool FightStateNormal::CheckSkill( CMonster *monster, int id, int lvl, int type, long dis )
    {
        CNewSkill *skillProperty = CNewSkillFactory::QueryNewSkillProperty( id );
		if( skillProperty == NULL )
		{
            return false;
        }
 
        if( !monster->GetUseRabbSkill() && skillProperty->GetUseType() == SKILL_TYPE_MAGIC )
        {
            return false;
        }
        if( !monster->GetUseFightSkill() && skillProperty->GetUseType() == SKILL_TYPE_PHYSICAL )
        {
            return false;
        }

        /*
        list<long> buffID = skillProperty->GetStaticParam( lvl )->lBuffID;
        for( list<long>::iterator it = buffID.begin(); it != buffID.end(); ++it )
        {
            map<long, stModuParam*>::iterator bt = monster->GetBuff().find( *it );
            if( bt == monster->GetBuff().end() ) // ??????????????????????
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        
        
        list<long> debuffID = skillProperty->GetStaticParam( lvl )->lNBuffID;
        for( list<long>::iterator it = debuffID.begin(); it != debuffID.end(); ++it )
        {
            map<long, stModuParam*>::iterator bt = monster->GetBuff().find( *it );
            if( bt != monster->GetBuff().end() )
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        */
        return true;
    }
}
 
