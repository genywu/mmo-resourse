///
/// @file ScriptTauntFunc.cpp
///
#include "stdafx.h"
#include "../Script.h"
#include "../ScriptSys.h"
#include "../../Monster.h"
#include "../../Player.h"
#include "../../MonsterTaunt/TauntHandler.h"
#include "../../MonsterTaunt/TauntedList.h"


#define NULL_RET ;
#define CHECK_MONSTER( ret ) \
    CMonster *monster = dynamic_cast<CMonster*>( m_RunEnv.srcShape ); \
    if( monster == NULL ) return ret

int Script::GetTauntValue( int index )
{
    CHECK_MONSTER( -1 );
    TauntHandler &handler = monster->GetTauntHandler();
    return handler.Value( index );
}

int Script::GetTauntValue( const CGUID &id )
{
    CHECK_MONSTER( -1 );
    TauntHandler &handler = monster->GetTauntHandler();
    return handler.Value( id );
}

void Script::SetTauntValue( const CGUID &id, int value )
{
    CHECK_MONSTER( NULL_RET );
    TauntHandler &handler = monster->GetTauntHandler();
    handler.SetValue( id, value );
}

CGUID Script::GetRandObject( long type )
{
    CHECK_MONSTER( NULL_GUID );
    TauntHandler &handler = monster->GetTauntHandler();
    const TauntHandler::TauntListT &taunts = handler.QueryList();
    std::vector<CGUID> retIDs;
    for( TauntHandler::TauntListT::const_iterator it = taunts.begin();
        it != taunts.end(); ++ it )
    {
        const TauntHandler::TauntItem &item = *it;
        if( item.target.type == type )
        {
            retIDs.push_back( item.target.id );
        }
    }
    if( retIDs.size() == 0 ) return NULL_GUID;
    int i = random( (int)retIDs.size() );
    return retIDs[i];
}

void Script::ObjectRunScript( long type, const char *file )
{
    CHECK_MONSTER( NULL_RET );
    CServerRegion *rgn = dynamic_cast<CServerRegion*>( monster->GetFather() );
    TauntHandler &handler = monster->GetTauntHandler();
    const TauntHandler::TauntListT &taunts = handler.QueryList();
    for( TauntHandler::TauntListT::const_iterator it = taunts.begin();
        it != taunts.end(); ++ it )
    {
        const TauntHandler::TauntItem &item = *it;
        if( item.target.type == type )
        {
            CMoveShape *obj = (CMoveShape*) rgn->FindChildObject( type, item.target.id );
            Script::RunEnv env( obj, NULL, rgn );
            GetInst( ScriptSys ).RunScript( env, file );
        }
    }
}

bool Script::ForceToFirst( const CGUID &id )
{
    CHECK_MONSTER( false );
    TauntHandler &handler = monster->GetTauntHandler();
    return handler.ForceToBest( id );
}

int Script::GetTauntSize()
{
    CHECK_MONSTER( 0 );
    TauntHandler &handler = monster->GetTauntHandler();
    return (int) handler.Size();
}

void Script::TauntEvents( const CGUID &firer, long ftype, const CGUID &owner, long otype, int tvalue )
{
    if( otype == TYPE_PLAYER )
    {
        CPlayer *player = GetGame()->FindPlayer( owner );
        if( player )
        {
            player->GetTauntedList()->OnHelped( firer, ftype, tvalue );
        }
    }
    // TODO: add pet type.
}
