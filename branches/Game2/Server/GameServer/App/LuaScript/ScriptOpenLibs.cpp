///
/// @file ScriptOpenLibs.cpp
/// @brief Open all libraries in lua.
///
#include "stdafx.h"
#include "ScriptSys.h"
#include "lua.hpp"
#include "Funcs/VarListFuncs.h"
#include "Funcs/BaseFunc.h"
#include "Funcs/RgnFunc.h"
#include "Funcs/ClientFunc.h"
#include "Funcs/PlayerFunc.h"
#include "Funcs/OrganFunc.h"
#include "Funcs/MonsterFunc.h"
#include "Binders/GUIDbind.h"
#include "Binders/ScriptBind.h"
#include "Binders/BaseBind.h"
#include "Binders/PlayerBind.h"

void ScriptSys::OpenLibs()
{
	luaL_openlibs( m_MainState );
	ScriptFuncs::OpenVarListLibs( m_MainState );
	ScriptFuncs::OpenBaseLibs( m_MainState );
	ScriptFuncs::OpenRgnLibs( m_MainState );
	ScriptFuncs::OpenClientLibs( m_MainState );
	ScriptFuncs::OpenPlayerLibs( m_MainState );
	ScriptFuncs::OpenOrganLibs( m_MainState );
	ScriptFuncs::OpenMonsterLibs( m_MainState );
	tolua_GUID_open( m_MainState );
	tolua_Script_open( m_MainState );
	tolua_Base_open( m_MainState );
	tolua_Player_open( m_MainState );
	// other custorm libs...
}

