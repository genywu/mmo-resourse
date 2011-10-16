#include "StdAfx.h"
#include ".\luafunction.h"
#include "../LuaLib/tolua++.h"
//extern "C" { // 这是个C++程序, 所以要extern "C",
//  // 因为lua的头文件都是C格式的
//#include "lua.h"
//#include "lualib.h"
//#include "lauxlib.h"
//}
/* the Lua interpreter */
lua_State* L;

CLuaFunction::CLuaFunction(void)
{
}

CLuaFunction::~CLuaFunction(void)
{
}

CLuaFunction* CLuaFunction::AddGoods(CString OriginName,int Num,int lLevel)
{
	/* the function name */
	lua_getglobal(L, "AddGoods");

	/* the first argument */
	tolua_pushstring(L,OriginName);
	tolua_pushnumber(L,Num);
	tolua_pushnumber(L,lLevel);
	///* call the function with 2
	//arguments, return 1 result */
	//lua_call(L, 2, 1);

	///* get the result */
	//sum = (int)lua_tonumber(L, -1);
	//lua_pop(L, 1);

	//return sum;
	return NULL;

}