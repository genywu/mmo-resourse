/*
** Lua binding: Player
** Generated automatically by tolua++-1.0.92 on 11/17/10 13:31:47.
*/

#include "stdafx.h" //modified by kevin lynx
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Player_open (lua_State* tolua_S);

#include "ScriptCfg.h"
#include "../Funcs/PlayerFunc.h"
using namespace ScriptFuncs;

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* Open function */
TOLUA_API int tolua_Player_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Player (lua_State* tolua_S) {
 return tolua_Player_open(tolua_S);
};
#endif

