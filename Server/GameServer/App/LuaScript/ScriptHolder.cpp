///
/// @file ScriptHolder.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "ScriptHolder.h"
#include "lua.hpp"

#define SCRIPT_TABLE "Scripts"
#define G_TABLE (LUA_REGISTRYINDEX)

bool ScriptHolder::Init()
{
	lua_pushliteral( m_luaState, SCRIPT_TABLE );
	lua_newtable( m_luaState );
	lua_settable( m_luaState, G_TABLE );
	return true;
}

bool ScriptHolder::Add( const char *file, const char *buf, size_t size )
{
	if( !GetScriptTable( m_luaState ) ) // [script table]
	{
		return false;
	}
	int index = lua_gettop( m_luaState );
	lua_pushstring( m_luaState, file ); // [script table, string]
	int ret = luaL_loadbuffer( m_luaState, buf, size, file ); // [script table, string, chunk]
	if( ret != 0 )
	{
		const char *errMsg = lua_tostring( m_luaState, -1 );
		//PutoutLog( "ScriptErr", LT_ERROR, "Script compile error: %d: %s.", ret, 
		//	errMsg == NULL ? "Unknown" : errMsg );
        Log4c::Error(ROOT_MODULE,"%-15s Script compile error: %d: %s.",__FUNCTION__, ret, errMsg == NULL ? "Unknown" : errMsg );
		// clear the stack
		lua_pop( m_luaState, 3 );
		return false;
	}
	lua_settable( m_luaState, index ); // script_table[file] = file_chunk
	// pop the script table
	lua_pop( m_luaState, 1 );
	return true;
}

bool ScriptHolder::Get( const char *file )
{
	return Get( file, m_luaState );
}

bool ScriptHolder::Get( const char *file, lua_State *L )
{
	if( !GetScriptTable( L ) )
	{
		return false;
	}
	int index = lua_gettop( L );
	lua_pushstring( L, file );
	lua_gettable( L, index );
	if( lua_isnil( L, -1 ) )
	{
		// remove the nil value and the script table from the stack
		lua_pop( L, 2 );
		return false;
	}
	// pop the script table from the stack
	lua_remove( L, index );
	return true;
}

bool ScriptHolder::GetScriptTable( lua_State *L )
{
	lua_pushliteral( L, SCRIPT_TABLE );
	lua_gettable( L, G_TABLE );
	if( lua_isnil( L, -1 ) )
	{
		// remove the nil value from the stack
		lua_pop( L, 1 );
		return false;
	}
	return true;
}
