#include "Precompiled.h"
#include "MsLuaLTCFun.h"

BOOL InitLuaLTCFun(VOID*)
{
    return TRUE;
}

// LUA错误消息处理函数
INT LUACALL LuaErrorMsgDispose(lua_State *L)
{
    _ENTER_FUN_C;

    CONST ACHAR* strErrorMsg = luaL_checkstring(L, 1);
    if (strErrorMsg)
    {
        ::MessageBoxA(NULL, strErrorMsg, "LUA错误", MB_OK);
    }
    lua_pushnumber(L, 0);
    return 1;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return LUA_RET_ERROR;
}

luaL_Reg g_RegLTCArray[LTC_FUN_REG_ARRAY_MAX_COUTN] = 
{
    {LUA_ERROR_DISPOSE_FUN_NAME, LuaErrorMsgDispose},
};