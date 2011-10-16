/*
** Lua binding: Base
** Generated automatically by tolua++-1.0.92 on 11/17/10 13:31:47.
*/

#include "stdafx.h" //modified by kevin lynx
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Base_open (lua_State* tolua_S);

#include "ScriptCfg.h"
#include "../Funcs/BaseFunc.h"
using namespace ScriptFuncs;

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: ARGB */
#ifndef TOLUA_DISABLE_tolua_Base_ARGB00
static int tolua_Base_ARGB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char a = (( unsigned char)  tolua_tonumber(tolua_S,1,0));
  unsigned char r = (( unsigned char)  tolua_tonumber(tolua_S,2,0));
  unsigned char g = (( unsigned char)  tolua_tonumber(tolua_S,3,0));
  unsigned char b = (( unsigned char)  tolua_tonumber(tolua_S,4,0));
  {
   unsigned long tolua_ret = ( unsigned long)  ARGB(a,r,g,b);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ARGB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: RegisterTimer */
#ifndef TOLUA_DISABLE_tolua_Base_RegisterTimer00
static int tolua_Base_RegisterTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* file = ((const char*)  tolua_tostring(tolua_S,1,0));
  long userData = ((long)  tolua_tonumber(tolua_S,2,0));
  long trigerTime = ((long)  tolua_tonumber(tolua_S,3,0));
  long interTime = ((long)  tolua_tonumber(tolua_S,4,0));
  {
   long tolua_ret = (long)  RegisterTimer(file,userData,trigerTime,interTime);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RegisterTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: UnRegisterTimer */
#ifndef TOLUA_DISABLE_tolua_Base_UnRegisterTimer00
static int tolua_Base_UnRegisterTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  long timerID = ((long)  tolua_tonumber(tolua_S,1,0));
  long userData = ((long)  tolua_tonumber(tolua_S,2,0));
  {
   UnRegisterTimer(timerID,userData);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnRegisterTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Base_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"ARGB",tolua_Base_ARGB00);
  tolua_function(tolua_S,"RegisterTimer",tolua_Base_RegisterTimer00);
  tolua_function(tolua_S,"UnRegisterTimer",tolua_Base_UnRegisterTimer00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Base (lua_State* tolua_S) {
 return tolua_Base_open(tolua_S);
};
#endif

