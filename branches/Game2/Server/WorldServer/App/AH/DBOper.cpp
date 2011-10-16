///
/// @file DBOper.cpp
///
#include "stdafx.h"
#include "OrderMgr.h"
#include "../DBEntity/Entity.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"

#define AH_PLAYERID "PlayerID"
#define AH_AGENTORDER "AgentOrders"

namespace AH
{
    void OrderMgr::LoadFromDB( int ret, CEntityGroup *entity )
    {
        if( ret != S_OK || entity == NULL )
        {
            return;
        }
        CEntityManager::DBEntityComponentMapItr entityMapItr = 
			GetGame().GetEntityManager()->GetBaseEntityMap().find( "[agentordersgroup]" );
		if( entityMapItr == GetGame().GetEntityManager()->GetBaseEntityMap().end() )
        {
            return;
        }
        typedef std::map<CGUID, CBaseEntity*> EntityGroupTable;
        CEntityGroup *varGroup = entityMapItr->second.begin()->second;
        if( varGroup == NULL )
        {
            return;
        }
        EntityGroupTable &entityTable = varGroup->GetEntityGroupMap();
        for( EntityGroupTable::iterator it = entityTable.begin(); 
            it != entityTable.end(); ++ it )
        {
            varGroup = (CEntityGroup*)it->second;
            CGUID playerID;
            varGroup->GetGuidAttr( AH_PLAYERID, playerID );

            long size = varGroup->GetBufSize( AH_AGENTORDER );
            if( size <= 0 )
            {
                continue;
            }
            Buffer buf( size, 0 );
            varGroup->GetBufAttr( AH_AGENTORDER, &buf[0], size );
            AgentOrder *order = CheckAgentOrder( playerID );
            order->Decode( buf );
        }
        m_DBLoaded = true;
    }

    void OrderMgr::SaveToDB( CEntityGroup *entity )
    {
        if( entity == NULL )
        {
            return;
        }
        entity->ReleaseChilds();
        for( AgentOrderTableT::iterator it = m_AgentOrders.begin();
            it != m_AgentOrders.end(); ++ it )
        {
            CGUID guid;
			CGUID::CreateGUID( guid );
			CEntity *entityChild = (CEntity*)GetGame().GetEntityManager()->NewBaseEntity( "[agentorders]", guid );

            entityChild->SetGuidAttr( AH_PLAYERID, it->first );
            AgentOrder *order = it->second;
            Buffer buf;
            order->Encode( buf );
            entityChild->SetBufAttr( AH_AGENTORDER, &buf[0], (long) buf.size() );

            entity->AddChild( entityChild );
        }
    }

    void OrderMgr::SaveAll()
    {
        GetGame().GetEntityManager()->CreateSaveAgentOrdersSession();
    }
}

