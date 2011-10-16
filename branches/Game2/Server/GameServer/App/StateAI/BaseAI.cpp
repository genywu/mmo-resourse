///
/// @file BaseAI.cpp
/// @author Kevin Lynx
/// 
#include "stdafx.h"
#include "AIDriver.h"
#include "BaseAI.h"
#include "EventImpl/Event.h"
#include "Helper/ScriptCaller.h"
#include "../MoveShape.h"
#include "../LuaScript/ScriptSys.h"
#include "../LuaScript/Script.h"
#include "../PhaseSystem/PhaseManager.h"

namespace StateAI
{
	BaseAI::BaseAI( CMoveShape *owner )
	{
		m_Owner = owner;
		m_Machine = NULL;
		m_AIDriver = new AIDriver( owner );
		m_Machine = new MachineType( this );
	}

	BaseAI::~BaseAI()
	{
		Release();
		delete m_AIDriver;
		delete m_Machine;
		m_Machine = NULL;
	}
		
	bool BaseAI::Init( const ImplConfig &impl, long init_state )
	{
		m_States.Create( impl, m_Machine );
		ChangeState( init_state );
        // script event handler
        m_EventScript = impl.m_EventScript;
		return true;
	}

	void BaseAI::Reset()
	{
		m_States.Reset();
		SetTarget( NULL_GUID, 0 );
        m_AIDriver->StopAll();
        // must place at the end of this function
		ChangeState( ST_PEACE );
	}

	void BaseAI::Release()
	{
		m_States.Destroy();
        m_AIDriver->Release();
	}

	void BaseAI::Run( long timeID, long timerType )
	{
        m_AIDriver->OnTimeOut( timerType );
        if( timerType == AIDriver::AI_EVENT )
        {   
		    m_Machine->Execute();
        }
    }
 
	bool BaseAI::Resume( unsigned long trigerTime )
	{
        return m_AIDriver->Start( trigerTime, AIDriver::AI_EVENT );
	}

	void BaseAI::Stop()
	{
        m_AIDriver->Stop( AIDriver::AI_EVENT );
	}

    bool BaseAI::SetTimer( long type, ulong trigerTime, ulong interTime )
    {
        return m_AIDriver->Start( trigerTime, type, interTime );
    }

    void BaseAI::StopTimer( long type )
    {
        m_AIDriver->Stop( type );
    }

	void BaseAI::ChangeState( long state )
	{
		assert( HasState( state ) );
		ChangeState( state, SimpleTransition( CurState(), m_States[state] ) ); 
	}

	void BaseAI::ChangeState( long state, const AITransition &tran )
	{
		assert( HasState( state ) );
		m_Machine->Change( m_States[state], tran );
		// change the owner's state
		long shapeState = ToShapeState( state );
		if( shapeState != -1 )
		{
			m_Owner->ChangeStateTo( (CShape::eSTATE)shapeState );
		}
	}

	void BaseAI::ChangeState( long state, const AIEvent &ev )
	{
		assert( HasState( state ) );
		ChangeState( state, EventTransition( CurState(), m_States[state], ev ) );
	}

	bool BaseAI::HasState( long state )
	{
		return m_States[state] != NULL;
	}

	void BaseAI::SendEvent( const AIEvent &event )
	{
		OnEvent( event );
        AIState *state = CurState();
        if( state != NULL )
        {
		    state->ReceiveEvent( this, event );
        }
	}

	AIState *BaseAI::CurState()
	{
		return m_Machine->CurState();
	}

    long BaseAI::CurStateType() 
    {
        AIState *state = CurState();
        return state == NULL ? ST_INVALID : state->Type();
    }

	long BaseAI::ToShapeState( long aiState )
	{
		long ret = -1;
		switch( aiState )
		{
		case ST_HANGUP:
			ret = CShape::STATE_HANGUP;
			break;

		case ST_PEACE:
			ret = CShape::STATE_PEACE;
			break;

		case ST_FIGHT:
			ret = CShape::STATE_FIGHT;
			break;

		case ST_DEAD:
			ret = CShape::STATE_DIED;
			break;

        case ST_AGONAL:
            ret = CShape::STATE_AGONAL;
		}

		return ret;
	}

	AIState *BaseAI::GetState( long type )
	{
		return m_States[type];
	}

	bool BaseAI::HasTarget() const
	{
		return GetTarget() != NULL;
	}

	void BaseAI::SetTarget( const CGUID &targetID, long targetType )
	{
		m_TargetID = targetID;
		m_TargetType = targetType;
	}

	CMoveShape *BaseAI::GetTarget() const
	{
		if( m_TargetID == NULL_GUID )
		{
			return NULL;
		}

		CRegion *region = static_cast<CRegion*>( m_Owner->GetFather() );
        CMoveShape *Target = static_cast<CMoveShape*>( region->FindChildObject( m_TargetType, m_TargetID ) );
        
		return Target;
	}

	bool BaseAI::BeginSkill( stModuParam *moduParam, long x, long y, CMoveShape* target )
	{
		long type = target->GetType();
		const CGUID &targetID = target->GetExID();

		CBaseModule *skill = CNewSkillFactory::CreateModule( moduParam->GetID(), moduParam->GetLv(),
			moduParam, moduParam->GetGlobleID(), moduParam->GetGoodsIndex() );
		if( NULL == skill )
		{
			CNewSkillFactory::RealeaseModuParam( moduParam );
			return false;
		}

        if( moduParam->GetModType() == ModuType_Skill_Passive || 
            moduParam->GetModType()==ModuType_Buff ||
            moduParam->GetModType()==ModuType_DeBuff ||
            moduParam->GetModType()==ModuType_Skill_OnOff ||
            skill->Begin( x, y, type, targetID ) == MR_Modu_End )
        {
            CNewSkillFactory::RealeaseModuParam( moduParam );
            return false;
        }
		else
		{	
			if( moduParam->GetAttack() == eYesAttack )
			{
				m_Owner->SetAttackTarget( targetID, type );
				m_Owner->ProduceEvent( ET_Attack );
			}
            //绑定当前使用技能参数
			m_Owner->ProduceEvent( ET_UseSkill ,0,0,0,0,moduParam);
			m_Owner->AddActiveSkill( moduParam );
			return true;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
    void BaseAI::RunScript( const char *file )
    {
        if( file[0] == '\0' || strcmp( file, "0" ) == 0 )
        {
            return;
        }
        Script::RunEnv env( m_Owner, NULL, static_cast<CRegion*>( m_Owner->GetFather() ) );
        GetInst( ScriptSys ).RunScript( env, file );
    }

	void BaseAI::Stand( unsigned long time )
	{
		Resume( time );
	}

	bool BaseAI::Move( long destX, long destY )
	{
		long curX = m_Owner->GetTileX();
		long curY = m_Owner->GetTileY();
		if( curX == destX && curY == destY )
		{
			Resume( OPER_FAILED_TIME );
			return false;
		}
		long dir = GetLineDir( curX, curY, destX, destY );
		return Move( dir );
	}

	bool BaseAI::OnMove( float destX, float destY )
	{
		if( !m_Owner->CanMove() )
		{
			m_Owner->OnCannotMove();
			return false;
		}
		else
		{
			m_Owner->OnMove( destX, destY );
			return true;
		}
	}

	bool BaseAI::Move( long dir )
	{
		if( !m_Owner->CanMove() )
		{
			Resume( OPER_FAILED_TIME );
			return false;
		}

		long curX = m_Owner->GetTileX();
		long curY = m_Owner->GetTileY();
		long destX = curX;
		long destY = curY;

		float dis = 1.0f;
		bool canMove = GetNextMoveDestCell( dir, (long)dis, destX, destY );
		if( canMove )
		{
			if( GetLineDir( curX, curY, destX, destY ) % 2 != 0 )
			{
				dis *= 1.414f;
			}
			float time = dis / m_Owner->GetSpeed();
			if( m_Owner->GetIsRun() )
			{
				time /= 2.0f;
			}

			float retX = destX + 0.5f;
			float retY = destY + 0.5f;
			m_Owner->OnMove( retX, retY );
			m_Owner->SetPosXY( retX, retY );

			Resume( (unsigned long)time );
		}
		else
		{
			Resume( OPER_FAILED_TIME );
			return false;
		}
		return true;
	}

	bool BaseAI::IsCanMoveTo( CRegion *region, long x, long y ) const
	{
		return region->GetBlock( x, y ) == CRegion::BLOCK_NO && !PHASEMAN.IsPhaseBlock(m_Owner,x,y);
	}

	bool BaseAI::GetNextMoveDestCell( long dir, long dis, long &x, long &y ) const
	{
		CRegion *region = static_cast<CRegion*>( m_Owner->GetFather() );
	
		long curX = m_Owner->GetTileX();
		long curY = m_Owner->GetTileY();
		long destX = curX;
		long destY = curY;

		long i = 0;
		for( i = 0; i < 8; ++ i )
		{
			bool canMove = true;
			long slipDir = _slip_order[dir][i];

            /// 体型0
			for( std::list<POINT>::iterator it = s_listMoveCheckCell[0][slipDir].begin();
				it != s_listMoveCheckCell[0][slipDir].end(); ++ it )
			{
				long cellX = curX + it->x;
				long cellY = curY + it->y;
				if( !IsCanMoveTo( region, cellX, cellY ) )
				{
					canMove = false;
					break;
				}
			}
			if( canMove )
			{
				destX = destX + _dir[slipDir][0];
				destY = destY + _dir[slipDir][1];
				break;
			}
		}

		if( i == 8 )
		{
			// not found any point 
			return false;
		}

		dir = GetLineDir( curX, curY, destX, destY );
		for( i = dis -1; i > 0; -- i )
		{
			destX += _dir[dir][0];
			destY += _dir[dir][1];
			if( !IsCanMoveTo( region, destX, destY ) )
			{
				break;
			}
		}

		x = destX;
		y = destY;
		return true;
	}

    void BaseAI::OnEvent( const AIEvent &event )
    {
        if( m_EventScript.size() > 0 )
        {
            bool focused = false;
            ScriptCaller caller( m_Owner );
            caller.AddParam( "$EventType", event.Type() );
            switch( event.Type() )
            {
            case ET_HURT:
                {
                    const HurtEvent &hev = (const HurtEvent&) event;
                    caller.AddParam( "$Hurt", hev.Hurt );
                    caller.AddParam( "@AttackerID", hev.AttackerId );
                    caller.AddParam( "$AttackerType", hev.AttackerType );
                    focused = true;
                }
                break;

            case ET_KILL:
                {
                    const KillEvent &kev = (const KillEvent&) event;
                    caller.AddParam( "@AttackerID", kev.AttackerId );
                    caller.AddParam( "$AttackerType", kev.AttackerType );
                    focused = true;
                }
                break;

            case ET_INIT:
            case ET_BORN:
            case ET_REBORN:
                focused = true;
                break;
            }
            if( focused )
            {
                caller.Call( m_EventScript.c_str() );
            }
        }
    }
}
