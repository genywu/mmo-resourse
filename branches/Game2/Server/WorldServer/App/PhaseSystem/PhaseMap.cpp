#include "StdAfx.h"
#include "PhaseMap.h"
#include "PhaseOtherDef.h"
#include "../Player.h"
#include "../../Net/Message.h"

void  PhaseMap::AddPhaseTable(const CGUID& PlayerID,const PhaseFace& PhaseId)
{
    m_GuidTable.insert( std::make_pair(PlayerID,PhaseId) );
    Log4c::Trace(ROOT_MODULE,
        "Add Map PlayerID (%s) PhaseId (%s) PhaseRest(%d),Player Phase Sum %d  ",
        PlayerID.tostring() , PhaseId.guid.tostring() , PhaseId.Rest,m_GuidTable.size() );
}

const CGUID&  PhaseMap::GetPhaseID(const CGUID& PlayerID)
{
    GIter itr = m_GuidTable.find( PlayerID );
    if ( itr != m_GuidTable.end() )
    {
        return itr->second.guid;
    }
    return NULL_GUID;
}
const ePhaseRest PhaseMap::GetPhaseRest(const CGUID& PlayerID)
{
    GIter itr = m_GuidTable.find( PlayerID );
    if ( itr != m_GuidTable.end() )
    {
        return itr->second.Rest;
    }
    return ePRest;
}

void   PhaseMap::ClearPlayerID(const CGUID& PlayerID)
{
    GIter itr = m_GuidTable.find( PlayerID );
    if ( itr != m_GuidTable.end() )
        m_GuidTable.erase( itr );
   Log4c::Trace(ROOT_MODULE, "Remove %s PlayerID Player Phase Sum %d  ",PlayerID.tostring(),m_GuidTable.size());
}

void    PhaseMap::OnMessage(CMessage* pMsg )
{
    S2WPhaseOper  Oper = (S2WPhaseOper)pMsg->GetLong();

    switch( Oper )
    {
    case eUpdateWSPhaseID:
        {
            CGUID PlayerId,PhaseId;
            pMsg->GetGUID( PlayerId );
            pMsg->GetGUID( PhaseId  );
            long  Rest = pMsg->GetLong();
            PhaseFace  phase(PhaseId,(ePhaseRest)Rest);
            AddPhaseTable( PlayerId, phase );
        }
        break;

    case eDeleteWSPhaseID:
        {
            CGUID PlayerId;
            pMsg->GetGUID( PlayerId );
            ClearPlayerID(PlayerId);
        }
        break;
    }
}