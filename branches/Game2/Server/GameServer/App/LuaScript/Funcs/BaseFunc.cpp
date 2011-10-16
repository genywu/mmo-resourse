///
/// @file BaseFunc.cpp
/// @brief 全局基础支持函数
/// @see BaseFunc.h
///
#include "stdafx.h"
#include "BaseFunc.h"
#include "lua.hpp"
#include "../ScriptSys.h"
#include "GlobalScriptTimer.h"

class Script;
namespace ScriptFuncs
{
	///
	/// 获取Script表，很多函数位于Script表内，在使用这些函数之前需要先取得Script表本身
	///
	/// script = GetScript()
	/// @return Script表
	///
	static int GetScript( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		tolua_pushusertype( L, script, "Script" );
		return 1;
	}

	/// 
	/// 运行指定脚本，新的运行实例与当前运行实例的脚本环境相同
	///
	/// ret = Call( scriptFile )
	/// @param scriptFile 脚本文件名
	/// @return 返回非0表示成功
	///
	static int Call( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		int ret = 0;
		if( script != NULL )
		{
			const char *file = luaL_checkstring( L, -1 );
			if( file != NULL )
			{
				// the same run env.
				ret = GetInst( ScriptSys ).RunScript( script->GetEnv(), file ) ? 1:0;
			}
		}
		lua_pushnumber( L, (lua_Number)ret );
		return 1;
	}

	///
	/// 让当前脚本挂起并等待一定时间
	///
	/// Wait( millSecs )
	/// @param millSecs 等待的时间（毫秒）
	///
	static int Wait( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		if( script == NULL )
		{
			return 0;
		}
		long millSecs = (long) tolua_tonumber( L, 1, 0 );
		CGlobalScriptTimer::GetInstance()->RegisterWaitScript( script->ID(), millSecs );		
		return lua_yield( L, 0 );
	}

	void OpenBaseLibs( lua_State *L )
	{
		lua_register( L, "GetScript", GetScript );
		lua_register( L, "Call", Call );
		lua_register( L, "Wait", Wait );

		lua_pushnumber( L, TYPE_PLAYER );
		lua_setglobal( L, "TYPE_PLAYER" );
		lua_pushnumber( L, TYPE_MONSTER );
		lua_setglobal( L, "TYPE_MONSTER" );

		lua_pushnumber( L, Script::EXEC_ERROR );
		lua_setglobal( L, "EXEC_ERROR" );
		lua_pushnumber( L, Script::EXEC_FAIL );
		lua_setglobal( L, "EXEC_FAIL" );
		lua_pushnumber( L, Script::EXEC_SUCCESS );
		lua_setglobal( L, "EXEC_SUCCESS" );
	}

	ulong ARGB( byte a, byte r, byte g, byte b )
	{
		return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
	}

	long RegisterTimer( const char *file, long userData, long trigerTime, long interTime )
	{
		return CGlobalScriptTimer::GetInstance()->RegisterTimer( trigerTime, interTime, file, userData );
	}

	void UnRegisterTimer( long timerID, long userData )
	{
		CGlobalScriptTimer::GetInstance()->UnRegisterTimer( userData, timerID );
	}
}


