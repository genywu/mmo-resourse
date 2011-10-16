///
/// @file Script.cpp
///
#include "stdafx.h"
#include "Script.h"
#include "lua.hpp"
#include "ScriptSys.h"
#include "../Player.h"

Script::Script()
{
	Reset();
}

bool Script::Init( const CGUID &id, lua_State *state )
{
	m_ID = id;
	m_luaState = state;
	return true;
}

void Script::InitEnv( const RunEnv &env )
{
	m_RunEnv = env;
}

void Script::Reset()
{
	m_ID = NULL_GUID;
	m_luaState = NULL;
	memset( &m_RunEnv, 0, sizeof( m_RunEnv ) );
}

int Script::RunScript( const char *file )
{
	int ret = -1;
	if( GetInst( ScriptSys ).GetHolder().Get( file, m_luaState ) )
	{
		PreRun();
		ret = lua_resume( m_luaState, 0 ); //lua_pcall( m_luaState, 0, 0, 0 );
	}
	return ret;	
}

int Script::RunStr( const char *str )
{
	int ret = luaL_loadbuffer( m_luaState, str, strlen( str ), "lua_string" );
	if( ret == 0 )
	{
		PreRun();
		ret = lua_resume( m_luaState, 0 );
	}
	return ret;
}

int Script::Resume( int retCount )
{
	return lua_resume( m_luaState, retCount );
}

bool Script::CanDelete( int ret ) const
{
	return ret != LUA_YIELD;
}

VariableList *Script::GetVarList( long type )
{
	switch( type )
	{
	case VT_SCRIPT:
		return m_RunEnv.varList;

	case VT_SRCSHAPE:
		{
			if( m_RunEnv.srcShape != NULL )
			{
				return m_RunEnv.srcShape->GetVariableList();
			}
		}
		break;

	case VT_DESTSHAPE:
		{
			if( m_RunEnv.destShape != NULL )
			{
				return m_RunEnv.destShape->GetVariableList();
			}
		}
		break;

	case VT_REGION:
		{
			if( m_RunEnv.region != NULL )
			{
				//TODO
				return NULL;//m_RunEnv.region->GetVariableList();
			}
		}
		break;

	case VT_GLOBAL:
		{
			return GetInst( ScriptSys ).GetGeneralVarList();
		}
		break;
	}
	return NULL;
}

void Script::PushRetValue( double val )
{
	lua_pushnumber( m_luaState, (lua_Number) val );
}

void Script::PushRetValue( const char *val )
{
	lua_pushstring( m_luaState, val );
}

void Script::PushRetValue( const CGUID &val )
{
    PushGCGUID( m_luaState, val );
}

void Script::PushGCGUID( lua_State *L, const CGUID &val )
{
	CGUID *valP = (CGUID*) Mtolua_new( CGUID );
	*valP = val;
	tolua_pushusertype( L, valP, "CGUID" );
	tolua_register_gc( L, lua_gettop( L ) );
}

void Script::PreRun()
{
#ifdef _ENABLE_PREDEFINED_
	int funcIndex = lua_gettop( m_luaState );
	int envT = BeginLocalEnv();
	// create some local variables first
	tolua_pushusertype( m_luaState, this, "Script" );
	lua_setfield( m_luaState, envT, "_Script" );

	EndLocalEnv( envT, funcIndex );
#endif
}

// create { __index=G, __newindex=_G} table
// the table is on the top of the lua stack
static void CreateEnvMetatable( lua_State *L, bool hasNewIndex )
{
	lua_newtable( L );
	int t = lua_gettop( L );
	lua_pushstring( L, "__index" );
	lua_getfield( L, LUA_GLOBALSINDEX, "_G" );
	lua_settable( L, t );
	if( hasNewIndex ) // set __newindex metamethod ?
	{
		lua_pushstring( L, "__newindex" );
		lua_getfield( L, LUA_GLOBALSINDEX, "_G" );
		lua_settable( L, t );
	}
}

int Script::BeginLocalEnv()
{
	// create the env table
	lua_newtable( m_luaState );
	int envT = lua_gettop( m_luaState );

	// create {__index=_G} table
	CreateEnvMetatable( m_luaState, false );

	// set the metatable for the env table
	lua_setmetatable( m_luaState, envT );
	return envT;
}

void Script::EndLocalEnv( int envT, int funcIndex )
{
	// copy the env table
	lua_pushvalue( m_luaState, envT );
	
	// set the env, it will pop the env table 
	lua_setfenv( m_luaState, envT );

	// set "__newindex" metamethod
	CreateEnvMetatable( m_luaState, true );
	lua_setmetatable( m_luaState, envT );

	// set a new env
	lua_setfenv( m_luaState, envT );
}

CPlayer *Script::FindPlayer( const CGUID &playerID )
{
	return dynamic_cast<CPlayer*>( FindShape( TYPE_PLAYER, playerID ) );
}

CPlayer *Script::FindPlayer( const char *name )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL && ( name == NULL || strcmp( player->GetName(), name ) == 0 ) )
	{
		return player;
	}
	player = dynamic_cast<CPlayer*>( m_RunEnv.destShape );
	if( player != NULL && ( name == NULL || strcmp( player->GetName(), name ) == 0 ) )
	{
		return player;
	}
	return GetGame()->FindPlayer( name );
}

CMoveShape *Script::FindShape( long type, const CGUID &id )
{
	// default value
	if( id == NULL_GUID )
	{
		return m_RunEnv.srcShape;
	}
	if( m_RunEnv.srcShape != NULL &&
		m_RunEnv.srcShape->GetType() == type && 
		m_RunEnv.srcShape->GetExID() == id )
	{
		return m_RunEnv.srcShape;
	}
	CMoveShape *shape = GetGame()->FindShape( (OBJECT_TYPE) type, id );
	return shape;
}
