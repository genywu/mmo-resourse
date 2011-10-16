#include "StdAfx.h"
#include "PhaseList.h"
#include "PhasePro.h"
#include "PhaseOtherDef.h"
#include "../Player.h"
#include "../Monster.h"
#include "../StateAI/AIEventSender.h"
  
PhaseList::PhaseList(PhasePro*  pro)
{
    m_PhasePro = pro;
    Release();
}

PhaseList::~PhaseList()
{
    Release();
}

void PhaseList::Release()
{
    /// 清空所有的怪物
    std::list<CMonster*>::iterator itr = m_MonsterList.begin();
    for (  ; itr != m_MonsterList.end() ; itr++ )
    {
        if( !(*itr) )  continue;
        CMessage msg(MSG_S2C_RGN_DELSHAPE);
        msg.Add( (*itr)->GetType() );
        msg.Add( (*itr)->GetExID() );
        msg.Add( static_cast<long>(0) );
        msg.SendToAround( (*itr) );
        m_PhasePro->m_Place.pRegion->DelShapeToAround((*itr));      
        (*itr)->AddDelEvent(0);
    }

    /// 清空玩家列表
    for( MPIER it(m_PlayerList.begin());it!= m_PlayerList.end();it++)
    {
         for ( std::list<CPlayer*>::iterator lit (it->second.begin()) ; lit != (it->second).end() ; lit++ )
         {
              if( !(*lit) )  continue;
              (*lit)->SetPhaseID(NULL_GUID);
              if( (*lit)->GetState() == CShape::STATE_AGONAL )
              {
                  /// 濒死状态改变
                  AI_EVENT_SENDER( (*lit)->GetAI() ).WhenPlayerLeavePhase();
              }
         }
         it->second.clear();
    }
    
    m_PlayerList.clear();
    m_MonsterList.clear();

    Log4c::Trace(ROOT_MODULE, "清除相位{ %s }所有的玩家标示和怪物", m_PhasePro->GetID().tostring() );
}

void   PhaseList::AddPlayer(ePhaseRest e, CPlayer* player)
{
    m_PlayerList[e].push_back( player );
    player->SetPhaseID( m_PhasePro->GetID() );
    player->SetPhaseRest( e );

   SendToAddWS(MSG_S2W_PHASESYSTEM,eUpdateWSPhaseID,player->GetExID(),m_PhasePro->GetID(),e);

   SendToClientPlayer(MSG_S2C_PHASE_OPER,S2C_EnterPhase,player->GetExID(),m_PhasePro,e);
   
   SendToClinetAround(MSG_S2C_PHASE_OPER,S2C_ChangeState,m_PhasePro->GetID(),player);
}

void   PhaseList::AddMonster(CMonster* Monster)
{
    if( !Monster )  return ;

    m_MonsterList.push_back( Monster );
    Monster->SetPhaseID( m_PhasePro->GetID() );
}

void   PhaseList::RemovePlayer(ePhaseRest e,CPlayer* player)
{
    if ( !player )  return ;
    std::list<CPlayer*>::iterator itr = find( m_PlayerList[e].begin() , m_PlayerList[e].end() ,player );
    if ( itr != m_PlayerList[e].end() )
    {
        m_PlayerList[e].erase( itr );
        player->SetPhaseID(NULL_GUID);
        /// 濒死状态改变
        if( player->GetState() == CShape::STATE_AGONAL )
            AI_EVENT_SENDER( player->GetAI() ).WhenPlayerLeavePhase();

       SendToRemoveWS( MSG_S2W_PHASESYSTEM , eDeleteWSPhaseID ,player->GetExID() );

       SendToClientPlayer(MSG_S2C_PHASE_OPER,S2C_LeavePhase,player->GetExID(),m_PhasePro,e);

       SendToClinetAround(MSG_S2C_PHASE_OPER,S2C_ChangeState,m_PhasePro->GetID(),player);
    }
}

void   PhaseList::RemoveMonster(CMonster* Monster)
{
    if( !Monster ) return ;

    std::list<CMonster*>::iterator itr = find( m_MonsterList.begin() , m_MonsterList.end() ,Monster );
    if ( itr != m_MonsterList.end() )
    {
        m_MonsterList.erase( itr );
        Monster->SetPhaseID(NULL_GUID);
    }
}

long    PhaseList::GetPhaseMonsterNum() const
{
    return  (long)m_MonsterList.size();
}


long    PhaseList::GetPhasePlayerNum(ePhaseRest e) const
{
    /// 清空玩家列表
    MPlayer::const_iterator it( m_PlayerList.find(e) );
    if ( it != m_PlayerList.end() )
    {
        return (long)it->second.size();
    }
    return 0;
}


void      PhaseList::PostPhaseMsg(const char* pMsg,ulong dwColor, ulong dwBkColor,long ePos)
{
    hash_set<CGUID,hash_guid_compare> PlayerID;PlayerID.clear();

    for( MPIER it(m_PlayerList.begin());it!= m_PlayerList.end();it++)
    {
        for ( std::list<CPlayer*>::iterator lit (it->second.begin()) ; lit != (it->second).end() ; lit++ )
        {   
            if( !(*lit) )  continue;
            PlayerID.insert( (*lit)->GetExID() ) ;
        }
    }
    CMessage msg( MSG_S2C_OTHER_ADDINFO );
   // msg.Add( dwColor);
    //msg.Add( dwBkColor );
    msg.Add( (BYTE)ePos );
    msg.Add( pMsg );
    msg.SendToPlayers(PlayerID);
}

long     PhaseList::PhaseExitEvent()
{
    /// 清空玩家列表
    for( MPIER it(m_PlayerList.begin());it!= m_PlayerList.end();it++)
    {
        for ( std::list<CPlayer*>::iterator lit (it->second.begin()) ; lit != (it->second).end() ; lit++ )
        {
            if( !(*lit) )  continue;
            m_PhasePro->RunScript(*lit,eExit,eSc_Event);
        }
    }
    return 0;
}