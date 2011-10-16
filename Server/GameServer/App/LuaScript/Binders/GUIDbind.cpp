/*
** Lua binding: GUID
** Generated automatically by tolua++-1.0.92 on 11/17/10 13:31:47.
*/

#include "stdafx.h" //modified by kevin lynx
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_GUID_open (lua_State* tolua_S);

#include "ScriptCfg.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_CGUID (lua_State* tolua_S)
{
 CGUID* self = (CGUID*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CGUID");
}

/* method: new of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_new00
static int tolua_GUID_CGUID_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CGUID* tolua_ret = (CGUID*)  Mtolua_new((CGUID)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"CGUID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_new00_local
static int tolua_GUID_CGUID_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   CGUID* tolua_ret = (CGUID*)  Mtolua_new((CGUID)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_delete00
static int tolua_GUID_CGUID_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CGUID* self = (CGUID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_new01
static int tolua_GUID_CGUID_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   CGUID* tolua_ret = (CGUID*)  Mtolua_new((CGUID)(str));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"CGUID");
  }
 }
 return 1;
tolua_lerror:
 return tolua_GUID_CGUID_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_new01_local
static int tolua_GUID_CGUID_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   CGUID* tolua_ret = (CGUID*)  Mtolua_new((CGUID)(str));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_GUID_CGUID_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator== of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID__eq00
static int tolua_GUID_CGUID__eq00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const CGUID",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const CGUID* self = (const CGUID*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* other = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator=='", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->operator==(*other);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.eq'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: tostring of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_tostring00
static int tolua_GUID_CGUID_tostring00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const CGUID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const CGUID* self = (const CGUID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'tostring'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->tostring();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'tostring'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsInvalid of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_IsInvalid00
static int tolua_GUID_CGUID_IsInvalid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CGUID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CGUID* self = (CGUID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsInvalid'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsInvalid();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsInvalid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateGUID of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_GUID_CGUID_CreateGUID00
static int tolua_GUID_CGUID_CreateGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"CGUID",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CGUID* guid = ((CGUID*)  tolua_tousertype(tolua_S,2,0));
  {
   bool tolua_ret = (bool)  CGUID::CreateGUID(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: GUID_INVALID of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_get_CGUID_GUID_INVALID
static int tolua_get_CGUID_GUID_INVALID(lua_State* tolua_S)
{
   tolua_pushusertype(tolua_S,(void*)&CGUID::GUID_INVALID,"CGUID");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: GUID_INVALID of class  CGUID */
#ifndef TOLUA_DISABLE_tolua_set_CGUID_GUID_INVALID
static int tolua_set_CGUID_GUID_INVALID(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"CGUID",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  CGUID::GUID_INVALID = *((CGUID*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_GUID_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"CGUID","CGUID","",tolua_collect_CGUID);
  #else
  tolua_cclass(tolua_S,"CGUID","CGUID","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"CGUID");
   tolua_function(tolua_S,"new",tolua_GUID_CGUID_new00);
   tolua_function(tolua_S,"new_local",tolua_GUID_CGUID_new00_local);
   tolua_function(tolua_S,".call",tolua_GUID_CGUID_new00_local);
   tolua_function(tolua_S,"delete",tolua_GUID_CGUID_delete00);
   tolua_function(tolua_S,"new",tolua_GUID_CGUID_new01);
   tolua_function(tolua_S,"new_local",tolua_GUID_CGUID_new01_local);
   tolua_function(tolua_S,".call",tolua_GUID_CGUID_new01_local);
   tolua_function(tolua_S,".eq",tolua_GUID_CGUID__eq00);
   tolua_function(tolua_S,"tostring",tolua_GUID_CGUID_tostring00);
   tolua_function(tolua_S,"IsInvalid",tolua_GUID_CGUID_IsInvalid00);
   tolua_function(tolua_S,"CreateGUID",tolua_GUID_CGUID_CreateGUID00);
   tolua_variable(tolua_S,"GUID_INVALID",tolua_get_CGUID_GUID_INVALID,tolua_set_CGUID_GUID_INVALID);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_GUID (lua_State* tolua_S) {
 return tolua_GUID_open(tolua_S);
};
#endif

