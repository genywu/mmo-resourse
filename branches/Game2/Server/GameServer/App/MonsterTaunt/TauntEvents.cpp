///
/// @file TauntEvents.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "TauntEvents.h"
#include "TauntHandler.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../Player.h"
#include "../Monster.h"

void TauntEvents::Searched( const CGUID &target, long type )
{
    if( m_handler.Exist( target ) ) return;
    // TODO: add taunt value formula here.
    int value = 1;
    m_handler.AddItem( target, type, value );
    // his team.
    if( type == TYPE_PLAYER )
    {
        AddTeammate( target );
    } 
}

void TauntEvents::AddTeammate( const CGUID &target )
{
    CPlayer *player = GetGame()->FindPlayer( target );
    assert( player );
    const CGUID &teamID = player->GetTeamID();
    if( teamID == NULL_GUID ) return; // no team
    GSTeam *team = GetOrganiCtrl()->GetGameTeamByID( teamID );
    std::list<CGUID> memList;
    team->GetOnServerTeamMember( memList );
    for( std::list<CGUID>::iterator it = memList.begin();
        it != memList.end(); ++ it )
    {
        if( *it == target ) continue;
        // TODO: check range here.
        CPlayer *player = GetGame()->FindPlayer( *it );
        if( player && m_handler.GetOwner()->RealDistance( player ) < 10 )
        {
            // TODO: add taunt value formula here.
            int value = 1;
            if( !m_handler.Exist( player->GetExID() ) )
            {
                m_handler.AddItem( player->GetExID(), TYPE_PLAYER, value );
            }
        }
    } // for
}

void TauntEvents::Attacked( const CGUID &attacker, long type, long hurt )
{
    // TODO: add taunt value formula here.
    int value = 2;
    if( m_handler.Exist( attacker ) )
    {
        // increase the taunt value.
        m_handler.IncValue( attacker, type, value );
    }
    else
    {
        // add a new item.
        m_handler.AddItem( attacker, type, value );
    }
    // add his teammates.
    if( type == TYPE_PLAYER )
    {
        AddTeammate( attacker );
    }
}

void TauntEvents::Rescue( const CGUID &rescuer, long type, long validHp )
{
    // exist ?
    if( m_handler.Exist( rescuer ) )
    {
        // increase taunt value.
        // TODO: add taunt value formula here.
        int value = 1;
        m_handler.IncValue( rescuer, type, value );
    }
    else
    {
        // add a new taunt item.
        // TODO: add taunt value formula here.
        int value = 1;
        m_handler.AddItem( rescuer, type, value );
    }
}

void TauntEvents::Help( const CGUID &helper, long type, int value )
{
    // exist ?
    if( m_handler.Exist( helper ) )
    {
        // increase taunt value.
        m_handler.IncValue( helper, type, value );
    }
    else
    {
        // add a new taunt item.
        m_handler.AddItem( helper, type, value );
    }
}

void TauntEvents::TargetDied( const CGUID &target, long type )
{
    // remove taunt item.
    m_handler.RemoveItem( target );
}

void TauntEvents::TargetLost( const CGUID &target, long type )
{
    // remove taunt item.
    m_handler.RemoveItem( target );
}
