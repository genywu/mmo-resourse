#pragma once              //┃  //┃
#ifndef __LUALTCFUN_H__   //┃  //┃
#define __LUALTCFUN_H__   //┃  //┃
//━━━━━━━━━━━━━┛  //┃
//文件名称：LuaLTCFun.h         //┃
//功能描述：Lua到C函数声明      //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// LUA返回值错误
#define LUA_RET_ERROR   -1

// LTC函数注册数组最大个数
#define LTC_FUN_REG_ARRAY_MAX_COUTN 4096

// LTC函数
// LUA函数调用发生错误时的处理函数名字
#define LUA_ERROR_DISPOSE_FUN_NAME "LTCErrorMsgDispose"

// 初始化LUA中的LTC函数
BOOL InitLuaLTCFun(VOID* Parameter);

// LUA函数中的LTC函数的声明

// Lua错误处理函数
INT LUACALL LuaErrorMsgDispose(lua_State *L);

// 全局LTC函数注册数组
extern luaL_Reg g_RegLTCArray[LTC_FUN_REG_ARRAY_MAX_COUTN]; 

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛