///
/// @file VarListFuncs.cpp
/// @brief 操作脚本变量的函数
///
#include "stdafx.h"
#include "VarListFuncs.h"
#include "lua.hpp"
#include "../ScriptSys.h"
#include "../ScriptUtils.h"
#include "../../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../../Public/ServerPublic/Server/Common/VarListUtils.h"

namespace ScriptFuncs
{
	///
	/// 检查当前脚本运行实例是否有某个脚本列表
	///
	/// ret = HasVarList( operType )
	/// @param operType: VT_SCRIPT脚本本身，只在技能脚本里有效；
	/// VT_SRCSHAPE源对象；VT_DESTSHAPE目标对象；
	/// VT_GLOBAL全局变量
	/// @return 1表示存在
	///
	static int HasVarList( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		int ret = 1;
		long operType = (long) tolua_tonumber( L, 1, 0 );
		if( script == NULL || script->GetVarList( operType ) == NULL )
		{
			ret = 0;	
		}
		lua_pushboolean( L, ret );
		return 1;
	}

	///
	/// 添加一个脚本变量到指定变量列表里
	///
	/// ret = AddVar( operType, "$varName", initVal, count =1 )
	/// @param operType: VT_SCRIPT, VT_SRCSHAPE, VT_DESTSHAPE
	/// @param varName 变量名
	/// @param initVal 初始值
	/// @param count [可选]数组长度，非数组为1
	///
	static int AddVar( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		long operType = (long) tolua_tonumber( L, 1, 0 );
		VariableList *varList = ( script != NULL ? script->GetVarList( operType ) : NULL );
		bool ret = false;
		if( operType == Script::VT_GLOBAL )
		{
			// not support global variable list
			varList = NULL;
		}
		if( varList != NULL )
		{
			ret = true;
			long count = (long) tolua_tonumber( L, 4, 1 );
			const char *varName = TOLUA_TOSTRING( varName, L, 2, "" );
			long varType = VarList::VarType( varName );
			if( varType == VariableList::VAR_NUM )
			{
				VariableList::Number val = (VariableList::Number) tolua_tonumber( L, 3, 0 );
				varList->AddVar( varName, val, count );
			}
			else if( varType == VariableList::VAR_STRING )
			{
				const char *val = TOLUA_TOSTRING( val, L, 3, "" ); 
				varList->AddVar( varName, tString( val ), count );
			}
			else if( varType == VariableList::VAR_GUID )
			{
				const CGUID *val = TOLUA_TOGUID( val, L, 3, &NULL_GUID );
                varList->AddVar( varName, *val, count );
			}
			else
			{
				// argument error
				ret = false;
			}
		}
		else
		{
			// pop the arguments.
			int top = lua_gettop( L );
			lua_settop( L, top - 3 );
		}

		lua_pushboolean( L, ret ? 1 : 0 );
		return 1;
	}

	///
	/// 移除指定列表里的某个脚本变量
	///
	/// RemoveVar( operType, "$varName" )
	/// @param operType:VT_SCRIPT, VT_SRCSHAPE, VT_DESTSHAPE
	/// @param varName 变量名
	///
	static int RemoveVar( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		long operType = (long) tolua_tonumber( L, 1, 0 );
		VariableList *varList = ( script != NULL ? script->GetVarList( operType ) : NULL );
		if( operType == Script::VT_GLOBAL )
		{
			// not support global variable list
			varList = NULL;
		}
		const char *varName = TOLUA_TOSTRING( varName, L, 2, "" );
		if( varList != NULL )
		{
			varList->RemoveVar( varName );
		}
		return 0;
	}

	///
	/// 取得指定列表里某个变量的值
	///
	/// luaVar = GetVar( operType, "$varName", [pos] )
	/// @param operType:VT_SCRIPT, VT_SRCSHAPE, VT_DESTSHAPE, VT_GLOBAL
	/// @param varName 变量名
	/// @param pos [可选] 数组索引，从0开始
	/// @return 变量值，如果变量不存在，则需要与各类型对应的非法值做比较：
	/// 字符串可与""比较，数字与INVALID_NUM比较，GUID与 CGUID::GUID_INVALID 做比较
	///
	static int GetVar( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		long operType = (long) tolua_tonumber( L, 1, 0 );
		VariableList *varList = ( script != NULL ? script->GetVarList( operType ) : NULL );
		long index = (long) tolua_tonumber( L, 3, 0 );
		const char *varName = TOLUA_TOSTRING( varName, L, 2, "" );
		long varType = VarList::VarType( varName );
		if( varList == NULL )
		{
			lua_pushnumber( L, -1 );
			return 1;
		}
		if( varType == VariableList::VAR_NUM )
		{
			VariableList::Number val = varList->GetVarValue<VariableList::Number>( varName, index );
			lua_pushnumber( L, (lua_Number) val );
		}
		else if( varType == VariableList::VAR_STRING )
		{
			const tString &val = varList->GetVarValue<tString>( varName, index );
			lua_pushstring( L, val.Valid() ? val.Str() : "" );
		}
		else if( varType == VariableList::VAR_GUID )
		{
			const CGUID &val = varList->GetVarValue<CGUID>( varName, index );
            Script::PushGCGUID( L, val );
		}
		else
		{
			// argument error.
			lua_pushnumber( L, -1 );
		}
		return 1;
	}

	///
	/// 设置指定列表里某个变量的值
	///
	/// ret = SetVar( operType, "$varName", val, pos )
	/// @param operType:VT_SCRIPT, VT_SRCSHAPE, VT_DESTSHAPE, VT_GLOBAL
	/// @param varName 变量名
	/// @param val 变量值
	/// @param pos [可选]数组索引
	///
	static int SetVar( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		long operType = (long) tolua_tonumber( L, 1, 0 );
		VariableList *varList = ( script != NULL ? script->GetVarList( operType ) : NULL );
		bool ret = false;
		if( varList != NULL )
		{
			ret = true;
			long index = (long) tolua_tonumber( L, 4, 0 );
			const char *varName = TOLUA_TOSTRING( varName, L, 2, "" );
			long varType = VarList::VarType( varName );
			if( varType == VariableList::VAR_NUM )
			{
				VariableList::Number val = (VariableList::Number) tolua_tonumber( L, 3, 0 );
				varList->SetVarValue( varName, val, index );
				if( operType == Script::VT_GLOBAL )
				{
					ScriptUtils::UpdateValToWS( varName, val, index );
				}
			}
			else if( varType == VariableList::VAR_STRING )
			{
				const char *val = TOLUA_TOSTRING( val, L, 3, "" ); 
				varList->SetVarValue( varName, tString( val ), index );
				if( operType == Script::VT_GLOBAL )
				{
					ScriptUtils::UpdateValToWS( varName, tString( val ), index );
				}
			}
			else if( varType == VariableList::VAR_GUID )
			{
				const CGUID *val = TOLUA_TOGUID( val, L, 3, &NULL_GUID ); 
				varList->SetVarValue( varName, *val, index );
				if( operType == Script::VT_GLOBAL )
				{
					ScriptUtils::UpdateValToWS( varName, *val, index );
				}
			}
			else
			{
				// argument error
				ret = false;
			}
		}
		else
		{
			// pop the arguments.
			int top = lua_gettop( L );
			lua_settop( L, top - 3 );
		}

		lua_pushboolean( L, ret ? 1 : 0 );
		return 1;
	}

	void OpenVarListLibs( lua_State *L )
	{
		lua_register( L, "HasVarList", HasVarList );
		lua_register( L, "AddVar", AddVar );
		lua_register( L, "RemoveVar", RemoveVar );
		lua_register( L, "SetVar", SetVar );
		lua_register( L, "GetVar", GetVar );

		// some constants.
		lua_pushinteger( L, Script::VT_SCRIPT );
		lua_setglobal( L, "VT_SCRIPT" );
		lua_pushinteger( L, Script::VT_SRCSHAPE );
		lua_setglobal( L, "VT_SRCSHAPE" );
		lua_pushinteger( L, Script::VT_DESTSHAPE );
		lua_setglobal( L, "VT_DESTSHAPE" );
		lua_pushinteger( L, Script::VT_REGION );
		lua_setglobal( L, "VT_REGION" );
		lua_pushinteger( L, Script::VT_GLOBAL );
		lua_setglobal( L, "VT_GLOBAL" );

		lua_pushnumber( L, (lua_Number) Private::TypeTrait<VariableList::Number>::InvalidVal() );
		lua_setglobal( L, "INVALID_NUM" );
	}
}
