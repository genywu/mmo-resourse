///
/// @file ScriptSys.h
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "lua.hpp"
#include "ScriptSys.h"
#include "Script.h"
#include "Funcs/GlobalScriptTimer.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"

#define THREAD_TABLE "ThreadTable"
#define NOTIFY_ERR( ret, L, s ) if( ret != LUA_YIELD && ret != 0 ) PrintError( L, ret, s )
#define STATE_KEY( state ) (ulong) (state)

static void PrintError( lua_State *L, int ret, const char *file )
{
	const char *s = NULL;
	if( ret != -1 )
	{
		s = lua_tostring( L, -1 );
        //PutoutLog( "ScriptErr", LT_ERROR, "Lua execute error: %d: %s.", ret, 
        //    s == NULL ? "Unknown" : s );	
        Log4c::Error(ROOT_MODULE,"%-15s Lua execute error: %d: %s.",__FUNCTION__,ret, s == NULL ? "Unknown" : s );	
	}
    else
    {
        //PutoutLog( "ScriptErr", LT_ERROR, "Lua execute error: Chunk [%s] does NOT exist",
        //    file );
        Log4c::Error(ROOT_MODULE,"%-15s Lua execute error: Chunk [%s] does NOT exist",__FUNCTION__, file );
    }
}

ScriptSys::ScriptSys()
{
	m_Holder = NULL;
	m_MainState = NULL;
	m_GeneralVarList = NULL;
}

bool ScriptSys::Init()
{
	m_MainState = lua_open();
	m_Holder = new ScriptHolder( m_MainState );
	m_Holder->Init();
    CreateThreadTable();

	OpenLibs();
    int ret = RunCustomScript();
    NOTIFY_ERR( ret, m_MainState, "" );

	m_GeneralVarList = new VariableList();

	return true;
}

void ScriptSys::Release()
{
	CGlobalScriptTimer::Release();
	SAFE_DELETE( m_GeneralVarList );
	SAFE_DELETE( m_Holder );
	Script::clear();
	lua_close( m_MainState );
	m_Scripts.clear();
	m_State2ScriptTable.clear();
}

#define CHECK_STR(s) if(s == NULL || s[0] == '\0') return false
#define tolower( src, dst ) strcpy( dst, src ), strlwr( dst )

bool ScriptSys::RunScript( const Script::RunEnv &env, const char *_file )
{
    CHECK_STR(_file);
    char file[512];
    tolower( _file, file );
	int stack = lua_gettop( m_MainState );
	Script *s = NewScript( env );
	int ret = s->RunScript( file );
	NOTIFY_ERR( ret, s->QueryState(), file );
	if( ret != LUA_YIELD )
	{
		DelScript( s );
	}
	int stackPost = lua_gettop( m_MainState );
	assert( stack == stackPost );
	return ret == LUA_YIELD || ret == 0;
}

bool ScriptSys::RunStr( const Script::RunEnv &env, const char *str )
{
    CHECK_STR(str);
	int stack = lua_gettop( m_MainState );
	Script *s = NewScript( env );
	int ret = s->RunStr( str );
	NOTIFY_ERR( ret, s->QueryState(), "" );
	if( ret != LUA_YIELD )
	{
		DelScript( s );
	}
	int stackPost = lua_gettop( m_MainState );
	assert( stack == stackPost );
	return ret == LUA_YIELD || ret == 0;
}

bool ScriptSys::ResumeScript( const CGUID &id, int retCount )
{
	Script *s = GetScript( id );
	if( s != NULL )
	{
		return ResumeScript( s, retCount );
	}
	return false;
}

bool ScriptSys::ResumeScript( Script *s, int retCount )
{
	int ret = s->Resume( retCount );
	NOTIFY_ERR( ret, s->QueryState(), "" );
	if( ret != LUA_YIELD )
	{
		DelScript( s );
	}
	return ret == LUA_YIELD || ret == 0;
}

Script *ScriptSys::NewScript( const Script::RunEnv &env )
{
	Script *s = Script::create();
	// create a child state from the parent state.
	lua_State *state = lua_newthread( m_MainState );
    NewThread( state );
    lua_pop( m_MainState, 1 );

	// generate an id for the new script.
	CGUID scriptID;
	CGUID::CreateGUID( scriptID );
	// initiate the script.
	s->Init( scriptID, state );
	s->InitEnv( env );
	// store the new script
	m_Scripts[s->ID()] = s;
	// insert into the lua_State->Script table.
	m_State2ScriptTable[STATE_KEY( state )] = s;
	return s;
}

void ScriptSys::DelScript( Script *s )
{
    DelThread( s->QueryState() );

	m_Scripts.erase( s->ID() );
	m_State2ScriptTable.erase( STATE_KEY( s->QueryState() ) );
	Script::destroy( s );
}

Script *ScriptSys::GetScript( const CGUID &scriptID )
{
	ScriptTableT::iterator it = m_Scripts.find( scriptID );
	return it != m_Scripts.end() ? it->second : NULL;
}

Script *ScriptSys::GetScript( lua_State *L )
{
	State2ScriptTableT::iterator it = m_State2ScriptTable.find( STATE_KEY( L ) );
	return it != m_State2ScriptTable.end() ? it->second : NULL;
}

void ScriptSys::UpdateGeneralVarList( DBReadSet &db )
{
	m_GeneralVarList->RemoveAll();
	VarList::Decode( m_GeneralVarList, db );
}

void ScriptSys::PushDelScript( const CGUID &scriptID )
{
	m_DelScripts.push_back( scriptID );
}

void ScriptSys::PushDelScript( CMoveShape *shape )
{
	for( ScriptTableT::const_iterator it = m_Scripts.begin();
		it != m_Scripts.end(); ++ it )
	{
		const Script::RunEnv &env = it->second->GetEnv();
		if( env.srcShape == shape )
		{
			PushDelScript( it->first );
		}
	}
}

void ScriptSys::DelScripts()
{
	for( DelScriptListT::iterator it = m_DelScripts.begin();
		it != m_DelScripts.end(); ++ it )
	{
		Script *script = GetScript( *it );
		if( script != NULL )
		{
			DelScript( script );
		}
	}
	m_DelScripts.clear();
}

#define GET_THREAD_TABLE( l ) lua_pushliteral( l, THREAD_TABLE ); \
    lua_gettable( l, LUA_REGISTRYINDEX )

void ScriptSys::CreateThreadTable()
{
	lua_pushliteral( m_MainState, THREAD_TABLE );
	lua_newtable( m_MainState );
	lua_settable( m_MainState, LUA_REGISTRYINDEX );
}

void ScriptSys::NewThread( lua_State *thread )
{
    GET_THREAD_TABLE( m_MainState );
    assert( !lua_isnil( m_MainState, -1 ) );
    lua_pushnumber( m_MainState, STATE_KEY( thread ) );
    lua_pushvalue( m_MainState, -3 );
    lua_settable( m_MainState, -3 );
    lua_pop( m_MainState, 1 );
}

void ScriptSys::DelThread( lua_State *thread )
{
    GET_THREAD_TABLE( m_MainState );
    assert( !lua_isnil( m_MainState, -1 ) );
    lua_pushnumber( m_MainState, STATE_KEY( thread ) );
    lua_pushnil( m_MainState );
    lua_settable( m_MainState, -3 );
    lua_pop( m_MainState, 1 );
}
