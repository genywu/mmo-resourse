///
/// @file TauntedList.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "TauntedList.h"
#include "TauntEvents.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"
#include "../Monster.h"
#include "../Player.h"

#define TRANVERSE_MONSTERS( m, it, exp ) \
    assert( m_owner ); \
    CServerRegion *rgn = dynamic_cast<CServerRegion*>( m_owner->GetFather() ); \
    for( MonsterListT::iterator it = m_monsters.begin(); \
        it != m_monsters.end(); ) \
    { \
        CMonster *m = rgn->FindMonsterByID( *it ); \
        if( !m ) \
        { \
            it = m_monsters.erase( it ); \
        } \
        else \
        { \
            exp \
            ++ it; \
        } \
    }

void TauntedList::OnAdded( const CGUID &id )
{
    if( !Exist( id ) )
    {
        m_monsters.push_back( id );
    }
}

void TauntedList::OnRemoved( const CGUID &id )
{
    MonsterListT::iterator it = std::find( m_monsters.begin(), m_monsters.end(), id );
    if( it != m_monsters.end() )
    {
        m_monsters.erase( it );
    }
}

void TauntedList::OnRescued( const CGUID &rescuer, long type, long hp )
{
    TRANVERSE_MONSTERS( monster, it, 
        TauntEvents events( monster->GetTauntHandler() );
        events.Rescue( rescuer, type, hp );
        )
}

void TauntedList::OnHelped( const CGUID &rescuer, long type, int value )
{
    TRANVERSE_MONSTERS( monster, it, 
        TauntEvents events( monster->GetTauntHandler() );
        events.Help( rescuer, type, value );
        )
}

bool TauntedList::Exist( const CGUID &id )
{
    return std::find( m_monsters.begin(), m_monsters.end(), id ) != m_monsters.end();
}

TauntedList *QueryTauntedList( CMonster *self, const CGUID &id, long type )
{
    CServerRegion *rgn = (CServerRegion*) self->GetFather();
    CMoveShape *shape = (CMoveShape*) rgn->FindChildObject( type, id );
    if( !shape ) return NULL;
    if( shape->GetType() == TYPE_PLAYER )
    {
        CPlayer *player = (CPlayer*) shape;
        return player->GetTauntedList();
    }
    // TODO: add pets, monsters, etc.
    return NULL;
}
