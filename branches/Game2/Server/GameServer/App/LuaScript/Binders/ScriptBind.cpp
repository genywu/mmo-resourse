/*
** Lua binding: Script
** Generated automatically by tolua++-1.0.92 on 11/17/10 13:31:47.
*/

#include "stdafx.h" //modified by kevin lynx
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Script_open (lua_State* tolua_S);

#include "ScriptCfg.h"
#include "../Script.h"

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
 tolua_usertype(tolua_S,"Script");
 tolua_usertype(tolua_S,"CGUID");
}

/* method: AddInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddInfo00
static int tolua_Script_Script_AddInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,3,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddInfo'", NULL);
#endif
  {
   self->AddInfo(str,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddInfo01
static int tolua_Script_Script_AddInfo01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddInfo'", NULL);
#endif
  {
   self->AddInfo(str,fgColor,bkColor,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_AddInfo00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostRegionInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostRegionInfo00
static int tolua_Script_Script_PostRegionInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,3,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostRegionInfo'", NULL);
#endif
  {
   self->PostRegionInfo(str,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostRegionInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostWorldInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostWorldInfo00
static int tolua_Script_Script_PostWorldInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,3,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostWorldInfo'", NULL);
#endif
  {
   self->PostWorldInfo(str,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostWorldInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostCountryInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostCountryInfo00
static int tolua_Script_Script_PostCountryInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  long country = ((long)  tolua_tonumber(tolua_S,3,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostCountryInfo'", NULL);
#endif
  {
   self->PostCountryInfo(str,country,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostCountryInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayEffectByIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayEffectByIndex00
static int tolua_Script_Script_PlayEffectByIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long index = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayEffectByIndex'", NULL);
#endif
  {
   self->PlayEffectByIndex(index);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayEffectByIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayerTalk of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayerTalk00
static int tolua_Script_Script_PlayerTalk00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayerTalk'", NULL);
#endif
  {
   self->PlayerTalk(str);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayerTalk'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayerTalkByIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayerTalkByIndex00
static int tolua_Script_Script_PlayerTalkByIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long textType = ((long)  tolua_tonumber(tolua_S,2,0));
  long textID = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayerTalkByIndex'", NULL);
#endif
  {
   self->PlayerTalkByIndex(textType,textID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayerTalkByIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PackGlint of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PackGlint00
static int tolua_Script_Script_PackGlint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PackGlint'", NULL);
#endif
  {
   self->PackGlint();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PackGlint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostPlayerInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostPlayerInfo00
static int tolua_Script_Script_PostPlayerInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* str = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostPlayerInfo'", NULL);
#endif
  {
   self->PostPlayerInfo(*playerID,str,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostPlayerInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostPlayerInfoByIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostPlayerInfoByIndex00
static int tolua_Script_Script_PostPlayerInfoByIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long textType = ((long)  tolua_tonumber(tolua_S,3,0));
  long textID = ((long)  tolua_tonumber(tolua_S,4,0));
  unsigned long fgColor = (( unsigned long)  tolua_tonumber(tolua_S,5,0xFFFFFFFF));
  unsigned long bkColor = (( unsigned long)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostPlayerInfoByIndex'", NULL);
#endif
  {
   self->PostPlayerInfoByIndex(*playerID,textType,textID,fgColor,bkColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostPlayerInfoByIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ShowString of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ShowString00
static int tolua_Script_Script_ShowString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long posX = ((long)  tolua_tonumber(tolua_S,2,0));
  long posY = ((long)  tolua_tonumber(tolua_S,3,0));
  const char* str = ((const char*)  tolua_tostring(tolua_S,4,0));
  long isShow = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ShowString'", NULL);
#endif
  {
   self->ShowString(posX,posY,str,isShow);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShowString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeWeather of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeWeather00
static int tolua_Script_Script_ChangeWeather00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeWeather'", NULL);
#endif
  {
   self->ChangeWeather(ID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeWeather'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayEffect of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayEffect00
static int tolua_Script_Script_PlayEffect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long effID = ((long)  tolua_tonumber(tolua_S,2,0));
  long bindPos = ((long)  tolua_tonumber(tolua_S,3,0));
  unsigned char inRgn = (( unsigned char)  tolua_tonumber(tolua_S,4,0));
  long x = ((long)  tolua_tonumber(tolua_S,5,0));
  long y = ((long)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayEffect'", NULL);
#endif
  {
   self->PlayEffect(effID,bindPos,inRgn,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayEffect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayAction of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayAction00
static int tolua_Script_Script_PlayAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned short action = (( unsigned short)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayAction'", NULL);
#endif
  {
   self->PlayAction(action);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayAction'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlaySound of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlaySound00
static int tolua_Script_Script_PlaySound00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* file = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned char toAround = (( unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlaySound'", NULL);
#endif
  {
   self->PlaySound(file,toAround);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlaySound'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Play3DSound of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_Play3DSound00
static int tolua_Script_Script_Play3DSound00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* file = ((const char*)  tolua_tostring(tolua_S,2,0));
  long loop = ((long)  tolua_tonumber(tolua_S,3,0));
  long x = ((long)  tolua_tonumber(tolua_S,4,0));
  long y = ((long)  tolua_tonumber(tolua_S,5,0));
  long z = ((long)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Play3DSound'", NULL);
#endif
  {
   self->Play3DSound(file,loop,x,y,z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Play3DSound'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayBkMusic of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayBkMusic00
static int tolua_Script_Script_PlayBkMusic00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* file = ((const char*)  tolua_tostring(tolua_S,2,0));
  long times = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayBkMusic'", NULL);
#endif
  {
   self->PlayBkMusic(file,times);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayBkMusic'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: StopBkMusic of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_StopBkMusic00
static int tolua_Script_Script_StopBkMusic00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'StopBkMusic'", NULL);
#endif
  {
   self->StopBkMusic();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'StopBkMusic'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddGoodsToShortcutBar of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddGoodsToShortcutBar00
static int tolua_Script_Script_AddGoodsToShortcutBar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsName = ((const char*)  tolua_tostring(tolua_S,2,0));
  long barIndex = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddGoodsToShortcutBar'", NULL);
#endif
  {
   int tolua_ret = (int)  self->AddGoodsToShortcutBar(goodsName,barIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddGoodsToShortcutBar'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddSkillToShortcutBar of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddSkillToShortcutBar00
static int tolua_Script_Script_AddSkillToShortcutBar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long skillID = ((long)  tolua_tonumber(tolua_S,2,0));
  long barIndex = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddSkillToShortcutBar'", NULL);
#endif
  {
   int tolua_ret = (int)  self->AddSkillToShortcutBar(skillID,barIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddSkillToShortcutBar'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: TransformCamera of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_TransformCamera00
static int tolua_Script_Script_TransformCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long mode = ((long)  tolua_tonumber(tolua_S,2,0));
  long param1 = ((long)  tolua_tonumber(tolua_S,3,0));
  long param2 = ((long)  tolua_tonumber(tolua_S,4,0));
  long param3 = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'TransformCamera'", NULL);
#endif
  {
   self->TransformCamera(mode,param1,param2,param3);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TransformCamera'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OperInterface of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OperInterface00
static int tolua_Script_Script_OperInterface00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long mode = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* interfaceName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OperInterface'", NULL);
#endif
  {
   self->OperInterface(mode,interfaceName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OperInterface'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HelpHint of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_HelpHint00
static int tolua_Script_Script_HelpHint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* script = ((const char*)  tolua_tostring(tolua_S,2,0));
  long delTime = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HelpHint'", NULL);
#endif
  {
   self->HelpHint(script,delTime);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HelpHint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InterfaceHint of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_InterfaceHint00
static int tolua_Script_Script_InterfaceHint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* script = ((const char*)  tolua_tostring(tolua_S,2,0));
  long delTime = ((long)  tolua_tonumber(tolua_S,3,0));
  const char* interfaceName = ((const char*)  tolua_tostring(tolua_S,4,0));
  long x = ((long)  tolua_tonumber(tolua_S,5,0));
  long y = ((long)  tolua_tonumber(tolua_S,6,0));
  long w = ((long)  tolua_tonumber(tolua_S,7,0));
  long h = ((long)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InterfaceHint'", NULL);
#endif
  {
   self->InterfaceHint(script,delTime,interfaceName,x,y,w,h);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InterfaceHint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendExpAction of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendExpAction00
static int tolua_Script_Script_SendExpAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long actionID = ((long)  tolua_tonumber(tolua_S,2,0));
  long srcType = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* srcID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long destType = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendExpAction'", NULL);
#endif
  {
   int tolua_ret = (int)  self->SendExpAction(actionID,srcType,*srcID,destType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendExpAction'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendExpAction of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendExpAction01
static int tolua_Script_Script_SendExpAction01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,6,&tolua_err) || !tolua_isusertype(tolua_S,6,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long actionID = ((long)  tolua_tonumber(tolua_S,2,0));
  long srcType = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* srcID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long destType = ((long)  tolua_tonumber(tolua_S,5,0));
  const CGUID* destID = ((const CGUID*)  tolua_tousertype(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendExpAction'", NULL);
#endif
  {
   int tolua_ret = (int)  self->SendExpAction(actionID,srcType,*srcID,destType,*destID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_SendExpAction00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSrcShapeID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSrcShapeID00
static int tolua_Script_Script_GetSrcShapeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSrcShapeID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetSrcShapeID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSrcShapeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDestShapeID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDestShapeID00
static int tolua_Script_Script_GetDestShapeID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDestShapeID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetDestShapeID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDestShapeID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCurRgn00
static int tolua_Script_Script_GetCurRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurRgn'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetCurRgn();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRgnAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRgnAttr00
static int tolua_Script_Script_GetRgnAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRgnAttr'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetRgnAttr(attrName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRgnAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRgnAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetRgnAttr00
static int tolua_Script_Script_SetRgnAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int attrValue = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRgnAttr'", NULL);
#endif
  {
   self->SetRgnAttr(attrName,attrValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRgnAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCellType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCellType00
static int tolua_Script_Script_GetCellType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCellType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetCellType(x,y);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCellType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMapInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMapInfo00
static int tolua_Script_Script_GetMapInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMapInfo'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMapInfo(x,y);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMapInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetBlock of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetBlock00
static int tolua_Script_Script_SetBlock00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  int block = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetBlock'", NULL);
#endif
  {
   self->SetBlock(*regionGuid,x,y,block);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetBlock'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRegionRandomPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRegionRandomPos00
static int tolua_Script_Script_GetRegionRandomPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRegionRandomPos'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetRegionRandomPos();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRegionRandomPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRegionRandomPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRegionRandomPos01
static int tolua_Script_Script_GetRegionRandomPos01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long x = ((long)  tolua_tonumber(tolua_S,3,NULL));
  long y = ((long)  tolua_tonumber(tolua_S,4,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRegionRandomPos'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetRegionRandomPos(*regionGuid,&x,&y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushnumber(tolua_S,(lua_Number)x);
   tolua_pushnumber(tolua_S,(lua_Number)y);
  }
 }
 return 3;
tolua_lerror:
 return tolua_Script_Script_GetRegionRandomPos00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRandomPosInRange of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRandomPosInRange00
static int tolua_Script_Script_GetRandomPosInRange00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long regionId = ((long)  tolua_tonumber(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  int width = ((int)  tolua_tonumber(tolua_S,5,0));
  int height = ((int)  tolua_tonumber(tolua_S,6,0));
  long randX = ((long)  tolua_tonumber(tolua_S,7,NULL));
  long randY = ((long)  tolua_tonumber(tolua_S,8,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRandomPosInRange'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetRandomPosInRange(regionId,x,y,width,height,&randX,&randY);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushnumber(tolua_S,(lua_Number)randX);
   tolua_pushnumber(tolua_S,(lua_Number)randY);
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRandomPosInRange'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLineDir of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetLineDir00
static int tolua_Script_Script_GetLineDir00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int srcX = ((int)  tolua_tonumber(tolua_S,2,0));
  int srcY = ((int)  tolua_tonumber(tolua_S,3,0));
  int destX = ((int)  tolua_tonumber(tolua_S,4,0));
  int destY = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLineDir'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetLineDir(srcX,srcY,destX,destY);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLineDir'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextPosByDir of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetNextPosByDir00
static int tolua_Script_Script_GetNextPosByDir00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int dir = ((int)  tolua_tonumber(tolua_S,2,0));
  int x = ((int)  tolua_tonumber(tolua_S,3,0));
  int y = ((int)  tolua_tonumber(tolua_S,4,0));
  int step = ((int)  tolua_tonumber(tolua_S,5,0));
  int destX = ((int)  tolua_tonumber(tolua_S,6,NULL));
  int destY = ((int)  tolua_tonumber(tolua_S,7,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextPosByDir'", NULL);
#endif
  {
   self->GetNextPosByDir(dir,x,y,step,&destX,&destY);
   tolua_pushnumber(tolua_S,(lua_Number)destX);
   tolua_pushnumber(tolua_S,(lua_Number)destY);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextPosByDir'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RunScriptForAllPlayerInRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RunScriptForAllPlayerInRgn00
static int tolua_Script_Script_RunScriptForAllPlayerInRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* scriptFilePath = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunScriptForAllPlayerInRgn'", NULL);
#endif
  {
   self->RunScriptForAllPlayerInRgn(scriptFilePath);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RunScriptForAllPlayerInRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNormalDupRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetNormalDupRgnID00
static int tolua_Script_Script_GetNormalDupRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long regionId = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNormalDupRgnID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetNormalDupRgnID(regionId);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNormalDupRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeRgn00
static int tolua_Script_Script_ChangeRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long regionId = ((long)  tolua_tonumber(tolua_S,2,0));
  long x = ((long)  tolua_tonumber(tolua_S,3,-1));
  long y = ((long)  tolua_tonumber(tolua_S,4,-1));
  long dir = ((long)  tolua_tonumber(tolua_S,5,0));
  long range = ((long)  tolua_tonumber(tolua_S,6,0));
  long carriageDistance = ((long)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeRgn'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ChangeRgn(regionId,x,y,dir,range,carriageDistance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeDupRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeDupRgn00
static int tolua_Script_Script_ChangeDupRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionId = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long regionType = ((long)  tolua_tonumber(tolua_S,3,0));
  long x = ((long)  tolua_tonumber(tolua_S,4,0));
  long y = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeDupRgn'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ChangeDupRgn(*regionId,regionType,x,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeDupRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurRgnGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCurRgnGUID00
static int tolua_Script_Script_GetCurRgnGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurRgnGUID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetCurRgnGUID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurRgnGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDupRgnPlayerMaxNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDupRgnPlayerMaxNum00
static int tolua_Script_Script_GetDupRgnPlayerMaxNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDupRgnPlayerMaxNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetDupRgnPlayerMaxNum(*regionGuid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDupRgnPlayerMaxNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RunDupRgnScript of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RunDupRgnScript00
static int tolua_Script_Script_RunDupRgnScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* scriptFilename = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunDupRgnScript'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RunDupRgnScript(scriptFilename);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RunDupRgnScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDupRgnMaxNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDupRgnMaxNum00
static int tolua_Script_Script_GetDupRgnMaxNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int regionType = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDupRgnMaxNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetDupRgnMaxNum(regionType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDupRgnMaxNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDupRgnOperFlag of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetDupRgnOperFlag00
static int tolua_Script_Script_SetDupRgnOperFlag00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long operationFlag = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDupRgnOperFlag'", NULL);
#endif
  {
   self->SetDupRgnOperFlag(*regionGuid,operationFlag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDupRgnOperFlag'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDupRgnOperFlag of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDupRgnOperFlag00
static int tolua_Script_Script_GetDupRgnOperFlag00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDupRgnOperFlag'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetDupRgnOperFlag(*regionGuid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDupRgnOperFlag'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamMaterIDByPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamMaterIDByPos00
static int tolua_Script_Script_GetTeamMaterIDByPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamMaterIDByPos'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetTeamMaterIDByPos(pos);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamMaterIDByPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EditDupRgnName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_EditDupRgnName00
static int tolua_Script_Script_EditDupRgnName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* regionName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EditDupRgnName'", NULL);
#endif
  {
   self->EditDupRgnName(*regionGuid,regionName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EditDupRgnName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreatePHGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CreatePHGoods00
static int tolua_Script_Script_CreatePHGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isstring(tolua_S,10,0,&tolua_err) ||
     !tolua_isstring(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* ownerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  int counts = ((int)  tolua_tonumber(tolua_S,4,0));
  int x = ((int)  tolua_tonumber(tolua_S,5,0));
  int y = ((int)  tolua_tonumber(tolua_S,6,0));
  int width = ((int)  tolua_tonumber(tolua_S,7,0));
  int height = ((int)  tolua_tonumber(tolua_S,8,0));
  int dir = ((int)  tolua_tonumber(tolua_S,9,0));
  const char* script = ((const char*)  tolua_tostring(tolua_S,10,0));
  const char* talk = ((const char*)  tolua_tostring(tolua_S,11,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePHGoods'", NULL);
#endif
  {
   self->CreatePHGoods(*ownerGuid,name,counts,x,y,width,height,dir,script,talk);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreatePHGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DeletePHGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DeletePHGoods00
static int tolua_Script_Script_DeletePHGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* ownerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const CGUID* goodGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DeletePHGoods'", NULL);
#endif
  {
   self->DeletePHGoods(*ownerGuid,*goodGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DeletePHGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurPlayerPDupRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCurPlayerPDupRgnID00
static int tolua_Script_Script_GetCurPlayerPDupRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurPlayerPDupRgnID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetCurPlayerPDupRgnID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurPlayerPDupRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurPlayerPHDupRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCurPlayerPHDupRgnID00
static int tolua_Script_Script_GetCurPlayerPHDupRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurPlayerPHDupRgnID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetCurPlayerPHDupRgnID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurPlayerPHDupRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerPDupRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerPDupRgnID00
static int tolua_Script_Script_SetPlayerPDupRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* ownerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerPDupRgnID'", NULL);
#endif
  {
   self->SetPlayerPDupRgnID(*ownerGuid,*regionGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerPDupRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerPHDupRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerPHDupRgnID00
static int tolua_Script_Script_SetPlayerPHDupRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* ownerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerPHDupRgnID'", NULL);
#endif
  {
   self->SetPlayerPHDupRgnID(*ownerGuid,*regionGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerPHDupRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLocalGSID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetLocalGSID00
static int tolua_Script_Script_GetLocalGSID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLocalGSID'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLocalGSID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLocalGSID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeDupRgnByRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeDupRgnByRgnID00
static int tolua_Script_Script_ChangeDupRgnByRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  int regionType = ((int)  tolua_tonumber(tolua_S,3,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  int x = ((int)  tolua_tonumber(tolua_S,5,0));
  int y = ((int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeDupRgnByRgnID'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ChangeDupRgnByRgnID(*playerGuid,regionType,*regionGuid,x,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeDupRgnByRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCallerGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCallerGUID00
static int tolua_Script_Script_GetCallerGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCallerGUID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetCallerGUID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCallerGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CallScriptByGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CallScriptByGUID00
static int tolua_Script_Script_CallScriptByGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* shapeGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* scriptFilename = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CallScriptByGUID'", NULL);
#endif
  {
   self->CallScriptByGUID(*shapeGuid,scriptFilename);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CallScriptByGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearCurGSDupRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ClearCurGSDupRgn00
static int tolua_Script_Script_ClearCurGSDupRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int regionType = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearCurGSDupRgn'", NULL);
#endif
  {
   self->ClearCurGSDupRgn(regionType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearCurGSDupRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddTeamRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddTeamRgnID00
static int tolua_Script_Script_AddTeamRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  int tempRegionId = ((int)  tolua_tonumber(tolua_S,3,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddTeamRgnID'", NULL);
#endif
  {
   self->AddTeamRgnID(*playerGuid,tempRegionId,*regionGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddTeamRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelTeamRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelTeamRgnID00
static int tolua_Script_Script_DelTeamRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  int tempRegionId = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelTeamRgnID'", NULL);
#endif
  {
   self->DelTeamRgnID(*playerGuid,tempRegionId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelTeamRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearTeamRgnID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ClearTeamRgnID00
static int tolua_Script_Script_ClearTeamRgnID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearTeamRgnID'", NULL);
#endif
  {
   self->ClearTeamRgnID(*playerGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearTeamRgnID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRgnStrongPointState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetRgnStrongPointState00
static int tolua_Script_Script_SetRgnStrongPointState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* footholdName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int footholdState = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRgnStrongPointState'", NULL);
#endif
  {
   self->SetRgnStrongPointState(footholdName,footholdState);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRgnStrongPointState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRgnStrongPointState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRgnStrongPointState00
static int tolua_Script_Script_GetRgnStrongPointState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* footholdName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRgnStrongPointState'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetRgnStrongPointState(footholdName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRgnStrongPointState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddGoods00
static int tolua_Script_Script_AddGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddGoods'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddGoods(goodsOriginalName,goodsAmount);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddArmament of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddArmament00
static int tolua_Script_Script_AddArmament00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,3,0));
  int armamentLevel = ((int)  tolua_tonumber(tolua_S,4,0));
  int evilLevel = ((int)  tolua_tonumber(tolua_S,5,0));
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,6,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddArmament'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddArmament(goodsOriginalName,goodsAmount,armamentLevel,evilLevel,playerName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddArmament'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelGoods00
static int tolua_Script_Script_DelGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->DelGoods(goodsOriginalName,goodsAmount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddGoodsToRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddGoodsToRgn00
static int tolua_Script_Script_AddGoodsToRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  double x = ((double)  tolua_tonumber(tolua_S,2,0));
  double y = ((double)  tolua_tonumber(tolua_S,3,0));
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,4,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddGoodsToRgn'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddGoodsToRgn(x,y,goodsOriginalName,goodsAmount);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddGoodsToRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BindGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BindGoods00
static int tolua_Script_Script_BindGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int containerID = ((int)  tolua_tonumber(tolua_S,2,0));
  int containerPos = ((int)  tolua_tonumber(tolua_S,3,0));
  int bindType = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BindGoods'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BindGoods(containerID,containerPos,bindType);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BindGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FrostGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_FrostGoods00
static int tolua_Script_Script_FrostGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int containerID = ((int)  tolua_tonumber(tolua_S,2,0));
  int containerPos = ((int)  tolua_tonumber(tolua_S,3,0));
  int bindType = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FrostGoods'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->FrostGoods(containerID,containerPos,bindType);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FrostGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckGoods00
static int tolua_Script_Script_CheckGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CheckGoods(goodsOriginalName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckGoodsInBank of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckGoodsInBank00
static int tolua_Script_Script_CheckGoodsInBank00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckGoodsInBank'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CheckGoodsInBank(goodsOriginalName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckGoodsInBank'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckSpace of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckSpace00
static int tolua_Script_Script_CheckSpace00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckSpace'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CheckSpace(goodsOriginalName,goodsAmount);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckSpace'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsNum00
static int tolua_Script_Script_GetGoodsNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetGoodsNum();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsFree of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsFree00
static int tolua_Script_Script_GetGoodsFree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsFree'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetGoodsFree();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsFree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEquipIDByPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetEquipIDByPos00
static int tolua_Script_Script_GetEquipIDByPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEquipIDByPos'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetEquipIDByPos(pos);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetEquipIDByPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEquipIDByPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetEquipIDByPos01
static int tolua_Script_Script_GetEquipIDByPos01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEquipIDByPos'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetEquipIDByPos(pos,*playerGuid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetEquipIDByPos00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEquipNameByPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetEquipNameByPos00
static int tolua_Script_Script_GetEquipNameByPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEquipNameByPos'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetEquipNameByPos(pos,*playerGuid);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetEquipNameByPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerGoodsAddonProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerGoodsAddonProperty00
static int tolua_Script_Script_GetPlayerGoodsAddonProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int containerId = ((int)  tolua_tonumber(tolua_S,2,0));
  int pos = ((int)  tolua_tonumber(tolua_S,3,0));
  int addonPropertyIdx = ((int)  tolua_tonumber(tolua_S,4,0));
  int valueId = ((int)  tolua_tonumber(tolua_S,5,0));
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,6,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerGoodsAddonProperty'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetPlayerGoodsAddonProperty(containerId,pos,addonPropertyIdx,valueId,playerName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerGoodsAddonProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerGoodsAddonProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerGoodsAddonProperty00
static int tolua_Script_Script_SetPlayerGoodsAddonProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isstring(tolua_S,7,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int containerId = ((int)  tolua_tonumber(tolua_S,2,0));
  int containerPos = ((int)  tolua_tonumber(tolua_S,3,0));
  int addonPropertyIdx = ((int)  tolua_tonumber(tolua_S,4,0));
  int valueId = ((int)  tolua_tonumber(tolua_S,5,0));
  int value = ((int)  tolua_tonumber(tolua_S,6,0));
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,7,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerGoodsAddonProperty'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetPlayerGoodsAddonProperty(containerId,containerPos,addonPropertyIdx,valueId,value,playerName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerGoodsAddonProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelPlayerGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelPlayerGoods00
static int tolua_Script_Script_DelPlayerGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelPlayerGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->DelPlayerGoods(playerName,goodsOriginalName,goodsAmount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelPlayerGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelUsedGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelUsedGoods00
static int tolua_Script_Script_DelUsedGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int goodsAmount = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelUsedGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->DelUsedGoods(goodsAmount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelUsedGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckUsedGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckUsedGoods00
static int tolua_Script_Script_CheckUsedGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckUsedGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CheckUsedGoods();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckUsedGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUsedGoodsPropertiy of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetUsedGoodsPropertiy00
static int tolua_Script_Script_GetUsedGoodsPropertiy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int addonPropertyId = ((int)  tolua_tonumber(tolua_S,2,0));
  int valueId = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUsedGoodsPropertiy'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetUsedGoodsPropertiy(addonPropertyId,valueId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUsedGoodsPropertiy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetUsedGoodsPropertiy of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetUsedGoodsPropertiy00
static int tolua_Script_Script_SetUsedGoodsPropertiy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int addonPropertyId = ((int)  tolua_tonumber(tolua_S,2,0));
  int valueId = ((int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetUsedGoodsPropertiy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetUsedGoodsPropertiy(addonPropertyId,valueId,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetUsedGoodsPropertiy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BuyGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BuyGoods00
static int tolua_Script_Script_BuyGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* npcName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BuyGoods'", NULL);
#endif
  {
   self->BuyGoods(npcName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BuyGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsAddonProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsAddonProperty00
static int tolua_Script_Script_GetGoodsAddonProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int goodsId = ((int)  tolua_tonumber(tolua_S,2,0));
  int propertyId = ((int)  tolua_tonumber(tolua_S,3,0));
  int valueId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsAddonProperty'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetGoodsAddonProperty(goodsId,propertyId,valueId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsAddonProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FullRefine of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_FullRefine00
static int tolua_Script_Script_FullRefine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FullRefine'", NULL);
#endif
  {
   self->FullRefine();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FullRefine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetWeaponStr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetWeaponStr00
static int tolua_Script_Script_GetWeaponStr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long srcPoint = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetWeaponStr'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetWeaponStr(srcPoint);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetWeaponStr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsDisplayName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsDisplayName00
static int tolua_Script_Script_GetGoodsDisplayName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsDisplayName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetGoodsDisplayName(goodsOriginalName);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsDisplayName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BackToCity of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BackToCity00
static int tolua_Script_Script_BackToCity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BackToCity'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BackToCity();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BackToCity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BeginContainerGoodsOpt of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BeginContainerGoodsOpt00
static int tolua_Script_Script_BeginContainerGoodsOpt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* onFinishScript = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* onPutScript = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned long titleId = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
  unsigned long buttonId = (( unsigned long)  tolua_tonumber(tolua_S,5,0));
  const char* txt = ((const char*)  tolua_tostring(tolua_S,6,0));
  long radius = ((long)  tolua_tonumber(tolua_S,7,0));
  unsigned long maxPushNum = (( unsigned long)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BeginContainerGoodsOpt'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BeginContainerGoodsOpt(onFinishScript,onPutScript,titleId,buttonId,txt,radius,maxPushNum);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BeginContainerGoodsOpt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOptGoodsInContainer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetOptGoodsInContainer00
static int tolua_Script_Script_GetOptGoodsInContainer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long goodsIdx = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOptGoodsInContainer'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetOptGoodsInContainer(goodsIdx);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOptGoodsInContainer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PutOptGoodsInContainer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PutOptGoodsInContainer00
static int tolua_Script_Script_PutOptGoodsInContainer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  unsigned long goodsIdx = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PutOptGoodsInContainer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->PutOptGoodsInContainer(*goodsGuid,goodsIdx);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PutOptGoodsInContainer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveOptGoodsInContainer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RemoveOptGoodsInContainer00
static int tolua_Script_Script_RemoveOptGoodsInContainer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long goodsIdx = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveOptGoodsInContainer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RemoveOptGoodsInContainer(goodsIdx);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveOptGoodsInContainer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RefreshContainerTxt of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RefreshContainerTxt00
static int tolua_Script_Script_RefreshContainerTxt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* txt = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RefreshContainerTxt'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RefreshContainerTxt(txt);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RefreshContainerTxt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RefreshContainerGold of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RefreshContainerGold00
static int tolua_Script_Script_RefreshContainerGold00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long goldAmount = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RefreshContainerGold'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RefreshContainerGold(goldAmount);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RefreshContainerGold'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EndContainerGoodsOpt of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_EndContainerGoodsOpt00
static int tolua_Script_Script_EndContainerGoodsOpt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EndContainerGoodsOpt'", NULL);
#endif
  {
   self->EndContainerGoodsOpt();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EndContainerGoodsOpt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsOriginalName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsOriginalName00
static int tolua_Script_Script_GetGoodsOriginalName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsOriginalName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetGoodsOriginalName(*goodsGuid);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsOriginalName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsAddonPropertyValues of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsAddonPropertyValues00
static int tolua_Script_Script_GetGoodsAddonPropertyValues00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long addonPropertyId = ((long)  tolua_tonumber(tolua_S,3,0));
  long valueId = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsAddonPropertyValues'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetGoodsAddonPropertyValues(*goodsGuid,addonPropertyId,valueId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsAddonPropertyValues'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetGoodsAddonPropertyValues of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetGoodsAddonPropertyValues00
static int tolua_Script_Script_SetGoodsAddonPropertyValues00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long addonPropertyId = ((long)  tolua_tonumber(tolua_S,3,0));
  long valueId = ((long)  tolua_tonumber(tolua_S,4,0));
  long newValue = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetGoodsAddonPropertyValues'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetGoodsAddonPropertyValues(*goodsGuid,addonPropertyId,valueId,newValue);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetGoodsAddonPropertyValues'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ResetGoodsStochastic of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ResetGoodsStochastic00
static int tolua_Script_Script_ResetGoodsStochastic00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ResetGoodsStochastic'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ResetGoodsStochastic(*goodsGuid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ResetGoodsStochastic'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelGoodsByGuid of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelGoodsByGuid00
static int tolua_Script_Script_DelGoodsByGuid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelGoodsByGuid'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->DelGoodsByGuid(*goodsGuid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelGoodsByGuid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddGoodsReturnGuid of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddGoodsReturnGuid00
static int tolua_Script_Script_AddGoodsReturnGuid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* goodsOriginalName = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int goodsAmount = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddGoodsReturnGuid'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->AddGoodsReturnGuid(goodsOriginalName,goodsAmount);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddGoodsReturnGuid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsFrostGoods of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsFrostGoods00
static int tolua_Script_Script_IsFrostGoods00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsFrostGoods'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsFrostGoods(*goodsGuid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsFrostGoods'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUsedGoodsOriginalName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetUsedGoodsOriginalName00
static int tolua_Script_Script_GetUsedGoodsOriginalName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUsedGoodsOriginalName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetUsedGoodsOriginalName();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUsedGoodsOriginalName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUsedGoodsGuid of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetUsedGoodsGuid00
static int tolua_Script_Script_GetUsedGoodsGuid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUsedGoodsGuid'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetUsedGoodsGuid();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUsedGoodsGuid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAddonDesc of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetAddonDesc00
static int tolua_Script_Script_SetAddonDesc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* txt = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAddonDesc'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetAddonDesc(*goodsGuid,txt);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAddonDesc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAddonDesc of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAddonDesc00
static int tolua_Script_Script_GetAddonDesc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* goodsGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAddonDesc'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetAddonDesc(*goodsGuid);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAddonDesc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUsedEquipProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetUsedEquipProperty00
static int tolua_Script_Script_GetUsedEquipProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pidx = ((int)  tolua_tonumber(tolua_S,2,0));
  int vidx = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUsedEquipProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetUsedEquipProperty(pidx,vidx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUsedEquipProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddState00
static int tolua_Script_Script_AddState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  long level = ((long)  tolua_tonumber(tolua_S,3,0));
  unsigned long shape = (( unsigned long)  tolua_tonumber(tolua_S,4,DEF_SHAPE));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddState(ID,level,shape);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RemoveState00
static int tolua_Script_Script_RemoveState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  long level = ((long)  tolua_tonumber(tolua_S,3,0));
  long count = ((long)  tolua_tonumber(tolua_S,4,0));
  unsigned long shape = (( unsigned long)  tolua_tonumber(tolua_S,5,DEF_SHAPE));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveState'", NULL);
#endif
  {
   self->RemoveState(ID,level,count,shape);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddSkill00
static int tolua_Script_Script_AddSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  long level = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddSkill'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddSkill(ID,level);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelSkill00
static int tolua_Script_Script_DelSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  long level = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelSkill'", NULL);
#endif
  {
   self->DelSkill(ID,level);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillLevel of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillLevel00
static int tolua_Script_Script_GetSkillLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillLevel'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillLevel(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillLevel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckStudySkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckStudySkill00
static int tolua_Script_Script_CheckStudySkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckStudySkill'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CheckStudySkill(ID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckStudySkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAllStateNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAllStateNum00
static int tolua_Script_Script_GetAllStateNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAllStateNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetAllStateNum();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAllStateNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAllStateNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAllStateNum01
static int tolua_Script_Script_GetAllStateNum01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* shapeID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long shapeType = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAllStateNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetAllStateNum(*shapeID,shapeType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetAllStateNum00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelDeBuff of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelDeBuff00
static int tolua_Script_Script_DelDeBuff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelDeBuff'", NULL);
#endif
  {
   self->DelDeBuff(shapePtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelDeBuff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelAllBuff of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelAllBuff00
static int tolua_Script_Script_DelAllBuff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelAllBuff'", NULL);
#endif
  {
   self->DelAllBuff(shapePtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelAllBuff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMoveShapeAttrEx of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetMoveShapeAttrEx00
static int tolua_Script_Script_SetMoveShapeAttrEx00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,3,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMoveShapeAttrEx'", NULL);
#endif
  {
   self->SetMoveShapeAttrEx(shapePtr,attrName,attrVal);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMoveShapeAttrEx'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMoveShapeAttrEx of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMoveShapeAttrEx00
static int tolua_Script_Script_GetMoveShapeAttrEx00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMoveShapeAttrEx'", NULL);
#endif
  {
   double tolua_ret = (double)  self->GetMoveShapeAttrEx(shapePtr,attrName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMoveShapeAttrEx'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeMoveShapeAttrEx of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeMoveShapeAttrEx00
static int tolua_Script_Script_ChangeMoveShapeAttrEx00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,3,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeMoveShapeAttrEx'", NULL);
#endif
  {
   self->ChangeMoveShapeAttrEx(shapePtr,attrName,attrVal);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeMoveShapeAttrEx'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddDeBuffToObj of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddDeBuffToObj00
static int tolua_Script_Script_AddDeBuffToObj00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long srcPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  unsigned long destPtr = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddDeBuffToObj'", NULL);
#endif
  {
   self->AddDeBuffToObj(srcPtr,destPtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddDeBuffToObj'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReliveSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ReliveSkill00
static int tolua_Script_Script_ReliveSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ReliveSkill'", NULL);
#endif
  {
   self->ReliveSkill(skillPtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReliveSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPkResult of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPkResult00
static int tolua_Script_Script_GetPkResult00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long srcPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  unsigned long destPtr = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
  long addSub = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPkResult'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPkResult(srcPtr,destPtr,addSub);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPkResult'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetObjectType00
static int tolua_Script_Script_GetObjectType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectType'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetObjectType(shapePtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDistance of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDistance00
static int tolua_Script_Script_GetDistance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long srcPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  unsigned long destPtr = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDistance'", NULL);
#endif
  {
   double tolua_ret = (double)  self->GetDistance(srcPtr,destPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDistance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendSkillList of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendSkillList00
static int tolua_Script_Script_SendSkillList00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendSkillList'", NULL);
#endif
  {
   self->SendSkillList(str);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendSkillList'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEnergyRatio of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetEnergyRatio00
static int tolua_Script_Script_GetEnergyRatio00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long param = ((long)  tolua_tonumber(tolua_S,2,0));
  unsigned char magic = (( unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEnergyRatio'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetEnergyRatio(param,magic);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetEnergyRatio'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DeductEnergy of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DeductEnergy00
static int tolua_Script_Script_DeductEnergy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned char magic = (( unsigned char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DeductEnergy'", NULL);
#endif
  {
   self->DeductEnergy(magic);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DeductEnergy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTarget of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetTarget00
static int tolua_Script_Script_SetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long srcPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  unsigned long destPtr = (( unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTarget'", NULL);
#endif
  {
   self->SetTarget(srcPtr,destPtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetShieldNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetShieldNum00
static int tolua_Script_Script_GetShieldNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long destPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetShieldNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetShieldNum(destPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetShieldNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillParam00
static int tolua_Script_Script_GetSkillParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  long pos = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillParam'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillParam(skillPtr,pos);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillElement of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillElement00
static int tolua_Script_Script_GetSkillElement00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillElement'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillElement(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillElement'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillUseType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillUseType00
static int tolua_Script_Script_GetSkillUseType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillUseType'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillUseType(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillUseType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillModType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillModType00
static int tolua_Script_Script_GetSkillModType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillModType'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillModType(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillModType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStateParamByID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStateParamByID00
static int tolua_Script_Script_GetStateParamByID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* param = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStateParamByID'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetStateParamByID(ID,param);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStateParamByID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStateParamByID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStateParamByID01
static int tolua_Script_Script_GetStateParamByID01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* param = ((const char*)  tolua_tostring(tolua_S,3,0));
  const CGUID* shapeID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long shapeType = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStateParamByID'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetStateParamByID(ID,param,*shapeID,shapeType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetStateParamByID00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStateParamByIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStateParamByIndex00
static int tolua_Script_Script_GetStateParamByIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* param = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStateParamByIndex'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetStateParamByIndex(ID,param);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStateParamByIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStateParamByIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStateParamByIndex01
static int tolua_Script_Script_GetStateParamByIndex01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* param = ((const char*)  tolua_tostring(tolua_S,3,0));
  const CGUID* shapeID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long shapeType = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStateParamByIndex'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetStateParamByIndex(ID,param,*shapeID,shapeType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetStateParamByIndex00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckStateFirer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckStateFirer00
static int tolua_Script_Script_CheckStateFirer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckStateFirer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CheckStateFirer(skillPtr);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckStateFirer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillLv of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillLv00
static int tolua_Script_Script_GetSkillLv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillLv'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillLv(ID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillLv'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReliveInScript of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ReliveInScript00
static int tolua_Script_Script_ReliveInScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  long type = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ReliveInScript'", NULL);
#endif
  {
   self->ReliveInScript(shapePtr,type);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReliveInScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGUIDByParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGUIDByParam00
static int tolua_Script_Script_GetGUIDByParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGUIDByParam'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetGUIDByParam(shapePtr);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGUIDByParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RunScriptByDest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RunScriptByDest00
static int tolua_Script_Script_RunScriptByDest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const char* script = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunScriptByDest'", NULL);
#endif
  {
   self->RunScriptByDest(shapePtr,script);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RunScriptByDest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BeginUseSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BeginUseSkill00
static int tolua_Script_Script_BeginUseSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ID = ((long)  tolua_tonumber(tolua_S,2,0));
  long level = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BeginUseSkill'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BeginUseSkill(ID,level);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BeginUseSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetEventDestObj of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetEventDestObj00
static int tolua_Script_Script_SetEventDestObj00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetEventDestObj'", NULL);
#endif
  {
   self->SetEventDestObj(skillPtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetEventDestObj'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBaseModuleLv of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetBaseModuleLv00
static int tolua_Script_Script_GetBaseModuleLv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBaseModuleLv'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetBaseModuleLv(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBaseModuleLv'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsAddonProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsAddonProperty01
static int tolua_Script_Script_GetGoodsAddonProperty01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  int addonID = ((int)  tolua_tonumber(tolua_S,3,0));
  int pos = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsAddonProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetGoodsAddonProperty(skillPtr,addonID,pos);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetGoodsAddonProperty00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetGoodsPlacePos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetGoodsPlacePos00
static int tolua_Script_Script_GetGoodsPlacePos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  long place = ((long)  tolua_tonumber(tolua_S,3,0));
  long pos = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetGoodsPlacePos'", NULL);
#endif
  {
   self->GetGoodsPlacePos(skillPtr,&place,&pos);
   tolua_pushnumber(tolua_S,(lua_Number)place);
   tolua_pushnumber(tolua_S,(lua_Number)pos);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetGoodsPlacePos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UseGoodsByPlacePos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_UseGoodsByPlacePos00
static int tolua_Script_Script_UseGoodsByPlacePos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long place = ((long)  tolua_tonumber(tolua_S,2,0));
  long pos = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UseGoodsByPlacePos'", NULL);
#endif
  {
   self->UseGoodsByPlacePos(place,pos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UseGoodsByPlacePos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SuccedUseItem of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SuccedUseItem00
static int tolua_Script_Script_SuccedUseItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SuccedUseItem'", NULL);
#endif
  {
   self->SuccedUseItem(skillPtr);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SuccedUseItem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetActModify of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetActModify00
static int tolua_Script_Script_GetActModify00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetActModify'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetActModify(skillPtr);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetActModify'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendUnlockGoodsMsg of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendUnlockGoodsMsg00
static int tolua_Script_Script_SendUnlockGoodsMsg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long place = ((long)  tolua_tonumber(tolua_S,2,0));
  long pos = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendUnlockGoodsMsg'", NULL);
#endif
  {
   self->SendUnlockGoodsMsg(place,pos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendUnlockGoodsMsg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBeneficiaryInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetBeneficiaryInfo00
static int tolua_Script_Script_GetBeneficiaryInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long shapePtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  long type = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBeneficiaryInfo'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetBeneficiaryInfo(shapePtr,&type);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushnumber(tolua_S,(lua_Number)type);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBeneficiaryInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetKillerInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetKillerInfo00
static int tolua_Script_Script_GetKillerInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetKillerInfo'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetKillerInfo(&type);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushnumber(tolua_S,(lua_Number)type);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetKillerInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAttackerInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAttackerInfo00
static int tolua_Script_Script_GetAttackerInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAttackerInfo'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetAttackerInfo(&type);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushnumber(tolua_S,(lua_Number)type);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAttackerInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillValue of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillValue00
static int tolua_Script_Script_GetSkillValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long skillid = ((long)  tolua_tonumber(tolua_S,2,0));
  long lvl = ((long)  tolua_tonumber(tolua_S,3,0));
  std::string name = ((std::string)  tolua_tocppstring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillValue'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillValue(skillid,lvl,name);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSkillAttriValue of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetSkillAttriValue00
static int tolua_Script_Script_GetSkillAttriValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long skillPtr = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  std::string name = ((std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSkillAttriValue'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetSkillAttriValue(skillPtr,name);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSkillAttriValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearSkillAttackType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ClearSkillAttackType00
static int tolua_Script_Script_ClearSkillAttackType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long SkillType = ((long)  tolua_tonumber(tolua_S,2,0));
  long Count = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearSkillAttackType'", NULL);
#endif
  {
   self->ClearSkillAttackType(SkillType,Count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearSkillAttackType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddQuest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddQuest00
static int tolua_Script_Script_AddQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddQuest'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddQuest(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddQuest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMainQuestId of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMainQuestId00
static int tolua_Script_Script_GetMainQuestId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMainQuestId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMainQuestId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMainQuestId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CompleteQuest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CompleteQuest00
static int tolua_Script_Script_CompleteQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CompleteQuest'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CompleteQuest(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CompleteQuest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AbandonQuest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AbandonQuest00
static int tolua_Script_Script_AbandonQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AbandonQuest'", NULL);
#endif
  {
   self->AbandonQuest(questId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AbandonQuest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestState00
static int tolua_Script_Script_GetQuestState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestState'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestState(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetValidQuestAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetValidQuestAmount00
static int tolua_Script_Script_GetValidQuestAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetValidQuestAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetValidQuestAmount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetValidQuestAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UpdateQuestStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_UpdateQuestStep00
static int tolua_Script_Script_UpdateQuestStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UpdateQuestStep'", NULL);
#endif
  {
   self->UpdateQuestStep(questId,stepId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UpdateQuestStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UpdateQuestParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_UpdateQuestParam00
static int tolua_Script_Script_UpdateQuestParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int paramIdx = ((int)  tolua_tonumber(tolua_S,3,0));
  int param = ((int)  tolua_tonumber(tolua_S,4,0));
  int regionId = ((int)  tolua_tonumber(tolua_S,5,0));
  int x = ((int)  tolua_tonumber(tolua_S,6,0));
  int y = ((int)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UpdateQuestParam'", NULL);
#endif
  {
   self->UpdateQuestParam(questId,paramIdx,param,regionId,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UpdateQuestParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UpdateQuestData of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_UpdateQuestData00
static int tolua_Script_Script_UpdateQuestData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int paramIdx = ((int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UpdateQuestData'", NULL);
#endif
  {
   self->UpdateQuestData(questId,paramIdx,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UpdateQuestData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestStepAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestStepAmount00
static int tolua_Script_Script_GetQuestStepAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestStepAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestStepAmount(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestStepAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetParam00
static int tolua_Script_Script_GetParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetParam'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetParam(questId,stepId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestAccParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestAccParam00
static int tolua_Script_Script_GetQuestAccParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestAccParam'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestAccParam(questId,stepId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestAccParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetQuestAccParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetQuestAccParam00
static int tolua_Script_Script_SetQuestAccParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
  int value = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetQuestAccParam'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetQuestAccParam(questId,stepId,idx,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetQuestAccParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddQuestAccParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddQuestAccParam00
static int tolua_Script_Script_AddQuestAccParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
  int value = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddQuestAccParam'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddQuestAccParam(questId,stepId,idx,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddQuestAccParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestRegionID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestRegionID00
static int tolua_Script_Script_GetQuestRegionID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestRegionID'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestRegionID(questId,stepId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestRegionID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetQuestRegionID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetQuestRegionID00
static int tolua_Script_Script_SetQuestRegionID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
  int regionId = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetQuestRegionID'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetQuestRegionID(questId,stepId,idx,regionId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetQuestRegionID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestRegionX of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestRegionX00
static int tolua_Script_Script_GetQuestRegionX00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestRegionX'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestRegionX(questId,stepId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestRegionX'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetQuestRegionX of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetQuestRegionX00
static int tolua_Script_Script_SetQuestRegionX00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
  int x = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetQuestRegionX'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetQuestRegionX(questId,stepId,idx,x);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetQuestRegionX'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestRegionY of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestRegionY00
static int tolua_Script_Script_GetQuestRegionY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestRegionY'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestRegionY(questId,stepId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestRegionY'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetQuestRegionY of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetQuestRegionY00
static int tolua_Script_Script_SetQuestRegionY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int idx = ((int)  tolua_tonumber(tolua_S,4,0));
  int y = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetQuestRegionY'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetQuestRegionY(questId,stepId,idx,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetQuestRegionY'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestAmount00
static int tolua_Script_Script_GetQuestAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestAmount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestChapter of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestChapter00
static int tolua_Script_Script_GetQuestChapter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestChapter'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestChapter();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestChapter'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetQuestChapter of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetQuestChapter00
static int tolua_Script_Script_SetQuestChapter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int num = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetQuestChapter'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetQuestChapter(num);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetQuestChapter'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddFinishedQuestRecord of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddFinishedQuestRecord00
static int tolua_Script_Script_AddFinishedQuestRecord00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddFinishedQuestRecord'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddFinishedQuestRecord(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddFinishedQuestRecord'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelFinishedQuestRecord of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelFinishedQuestRecord00
static int tolua_Script_Script_DelFinishedQuestRecord00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelFinishedQuestRecord'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->DelFinishedQuestRecord(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelFinishedQuestRecord'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestFinishedRecord of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestFinishedRecord00
static int tolua_Script_Script_GetQuestFinishedRecord00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestFinishedRecord'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetQuestFinishedRecord(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestFinishedRecord'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestTextType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestTextType00
static int tolua_Script_Script_GetQuestTextType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestTextType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestTextType(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestTextType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestName00
static int tolua_Script_Script_GetQuestName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetQuestName(questId);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsQuestRecorded of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsQuestRecorded00
static int tolua_Script_Script_IsQuestRecorded00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsQuestRecorded'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsQuestRecorded(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsQuestRecorded'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestType00
static int tolua_Script_Script_GetQuestType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestType(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestIsShare of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestIsShare00
static int tolua_Script_Script_GetQuestIsShare00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestIsShare'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestIsShare(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestIsShare'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestReqSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestReqSkill00
static int tolua_Script_Script_GetQuestReqSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestReqSkill'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetQuestReqSkill(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestReqSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestSexLimit of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestSexLimit00
static int tolua_Script_Script_GetQuestSexLimit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestSexLimit'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetQuestSexLimit(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestSexLimit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestTotePet of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestTotePet00
static int tolua_Script_Script_GetQuestTotePet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestTotePet'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetQuestTotePet(questId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestTotePet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsQuestDisplayed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsQuestDisplayed00
static int tolua_Script_Script_IsQuestDisplayed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsQuestDisplayed'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsQuestDisplayed(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsQuestDisplayed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsQuestRepeated of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsQuestRepeated00
static int tolua_Script_Script_IsQuestRepeated00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsQuestRepeated'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsQuestRepeated(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsQuestRepeated'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsQuestFailOnDead of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsQuestFailOnDead00
static int tolua_Script_Script_IsQuestFailOnDead00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsQuestFailOnDead'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsQuestFailOnDead(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsQuestFailOnDead'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsQuestRepeatedOnFail of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsQuestRepeatedOnFail00
static int tolua_Script_Script_IsQuestRepeatedOnFail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsQuestRepeatedOnFail'", NULL);
#endif
  {
   int tolua_ret = (int)  self->IsQuestRepeatedOnFail(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsQuestRepeatedOnFail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestMinLvLimit of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestMinLvLimit00
static int tolua_Script_Script_GetQuestMinLvLimit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestMinLvLimit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestMinLvLimit(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestMinLvLimit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestMaxLvLimit of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestMaxLvLimit00
static int tolua_Script_Script_GetQuestMaxLvLimit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestMaxLvLimit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestMaxLvLimit(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestMaxLvLimit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestOccupationLimit of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestOccupationLimit00
static int tolua_Script_Script_GetQuestOccupationLimit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int occupationIdx = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestOccupationLimit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestOccupationLimit(questId,occupationIdx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestOccupationLimit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestChapterAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestChapterAmount00
static int tolua_Script_Script_GetQuestChapterAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestChapterAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestChapterAmount(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestChapterAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextQuestId of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetNextQuestId00
static int tolua_Script_Script_GetNextQuestId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextQuestId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetNextQuestId(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextQuestId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPreQuest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPreQuest00
static int tolua_Script_Script_GetPreQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int idx = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPreQuest'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetPreQuest(questId,idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPreQuest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPreQuestAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPreQuestAmount00
static int tolua_Script_Script_GetPreQuestAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPreQuestAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetPreQuestAmount(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPreQuestAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimAMTType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimAMTType00
static int tolua_Script_Script_GetAimAMTType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimAMTType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAimAMTType(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimAMTType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimType00
static int tolua_Script_Script_GetAimType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAimType(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimId of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimId00
static int tolua_Script_Script_GetAimId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAimId(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimAmount of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimAmount00
static int tolua_Script_Script_GetAimAmount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimAmount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAimAmount(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimAmount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimParameIndex of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimParameIndex00
static int tolua_Script_Script_GetAimParameIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimParameIndex'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAimParameIndex(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimParameIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDropFrom of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDropFrom00
static int tolua_Script_Script_GetDropFrom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDropFrom'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetDropFrom(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDropFrom'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDropOdds of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetDropOdds00
static int tolua_Script_Script_GetDropOdds00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDropOdds'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetDropOdds(questId,stepId,aimId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDropOdds'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAimName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAimName00
static int tolua_Script_Script_GetAimName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAimName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetAimName(questId,stepId,aimId);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAimName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPrepareParam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPrepareParam00
static int tolua_Script_Script_GetPrepareParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int aimId = ((int)  tolua_tonumber(tolua_S,4,0));
  int index = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPrepareParam'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetPrepareParam(questId,stepId,aimId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPrepareParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementSum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementSum00
static int tolua_Script_Script_GetQuestEncouragementSum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementSum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestEncouragementSum(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementSum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepEncouragementName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepEncouragementName00
static int tolua_Script_Script_GetStepEncouragementName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int index = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepEncouragementName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetStepEncouragementName(questId,stepId,index);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepEncouragementName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementName00
static int tolua_Script_Script_GetQuestEncouragementName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int index = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetQuestEncouragementName(questId,index);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepEncouragementType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepEncouragementType00
static int tolua_Script_Script_GetStepEncouragementType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int index = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepEncouragementType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepEncouragementType(questId,stepId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepEncouragementType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementType of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementType00
static int tolua_Script_Script_GetQuestEncouragementType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int index = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestEncouragementType(questId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepEncouragementID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepEncouragementID00
static int tolua_Script_Script_GetStepEncouragementID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int index = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepEncouragementID'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepEncouragementID(questId,stepId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepEncouragementID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementID00
static int tolua_Script_Script_GetQuestEncouragementID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int index = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementID'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestEncouragementID(questId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepEncouragementNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepEncouragementNum00
static int tolua_Script_Script_GetStepEncouragementNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
  int index = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepEncouragementNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepEncouragementNum(questId,stepId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepEncouragementNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementNum00
static int tolua_Script_Script_GetQuestEncouragementNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int index = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestEncouragementNum(questId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepEncouragementCell of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepEncouragementCell00
static int tolua_Script_Script_GetStepEncouragementCell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int index = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepEncouragementCell'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepEncouragementCell(questId,index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepEncouragementCell'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestEncouragementCell of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestEncouragementCell00
static int tolua_Script_Script_GetQuestEncouragementCell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestEncouragementCell'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestEncouragementCell(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestEncouragementCell'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepSum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepSum00
static int tolua_Script_Script_GetStepSum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepSum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepSum(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepSum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStepAimNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetStepAimNum00
static int tolua_Script_Script_GetStepAimNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStepAimNum'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStepAimNum(questId,stepId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStepAimNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindRoleIsInSystem of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_FindRoleIsInSystem00
static int tolua_Script_Script_FindRoleIsInSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindRoleIsInSystem'", NULL);
#endif
  {
   int tolua_ret = (int)  self->FindRoleIsInSystem(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindRoleIsInSystem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindRoleStepIsInSystem of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_FindRoleStepIsInSystem00
static int tolua_Script_Script_FindRoleStepIsInSystem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int stepId = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindRoleStepIsInSystem'", NULL);
#endif
  {
   int tolua_ret = (int)  self->FindRoleStepIsInSystem(questId,stepId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindRoleStepIsInSystem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestStateId of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestStateId00
static int tolua_Script_Script_GetQuestStateId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestStateId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetQuestStateId(questId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestStateId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetQuestDisplayText of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetQuestDisplayText00
static int tolua_Script_Script_GetQuestDisplayText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int questId = ((int)  tolua_tonumber(tolua_S,2,0));
  int state = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetQuestDisplayText'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetQuestDisplayText(questId,state);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetQuestDisplayText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamNum00
static int tolua_Script_Script_GetTeamNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetTeamNum();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamNumInRgn of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamNumInRgn00
static int tolua_Script_Script_GetTeamNumInRgn00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamNumInRgn'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetTeamNumInRgn();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamNumInRgn'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamerName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamerName00
static int tolua_Script_Script_GetTeamerName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamerName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetTeamerName(pos);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamerName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamGUID00
static int tolua_Script_Script_GetTeamGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamGUID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetTeamGUID();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerTeamGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerTeamGUID00
static int tolua_Script_Script_GetPlayerTeamGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerTeamGUID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetPlayerTeamGUID(*playerGuid);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerTeamGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsTeamCaptain of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsTeamCaptain00
static int tolua_Script_Script_IsTeamCaptain00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsTeamCaptain'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsTeamCaptain();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsTeamCaptain'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamCaptainPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTeamCaptainPos00
static int tolua_Script_Script_GetTeamCaptainPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamCaptainPos'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetTeamCaptainPos();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamCaptainPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRegionForTeam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetRegionForTeam00
static int tolua_Script_Script_SetRegionForTeam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int type = ((int)  tolua_tonumber(tolua_S,2,0));
  int rgnType = ((int)  tolua_tonumber(tolua_S,3,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long x = ((long)  tolua_tonumber(tolua_S,5,-1));
  long y = ((long)  tolua_tonumber(tolua_S,6,-1));
  long dir = ((long)  tolua_tonumber(tolua_S,7,-1));
  long range = ((long)  tolua_tonumber(tolua_S,8,-1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRegionForTeam'", NULL);
#endif
  {
   long tolua_ret = (long)  self->SetRegionForTeam(type,rgnType,*regionGuid,x,y,dir,range);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRegionForTeam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTeamRegion of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetTeamRegion00
static int tolua_Script_Script_SetTeamRegion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int rgnType = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* regionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  long x = ((long)  tolua_tonumber(tolua_S,4,-1));
  long y = ((long)  tolua_tonumber(tolua_S,5,-1));
  long radius = ((long)  tolua_tonumber(tolua_S,6,3));
  long dir = ((long)  tolua_tonumber(tolua_S,7,-1));
  long range = ((long)  tolua_tonumber(tolua_S,8,2));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTeamRegion'", NULL);
#endif
  {
   self->SetTeamRegion(rgnType,*regionGuid,x,y,radius,dir,range);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTeamRegion'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsTeammatesAroundMe of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsTeammatesAroundMe00
static int tolua_Script_Script_IsTeammatesAroundMe00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  long radius = ((long)  tolua_tonumber(tolua_S,3,3));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsTeammatesAroundMe'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsTeammatesAroundMe(type,radius);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsTeammatesAroundMe'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RunScriptForTeam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RunScriptForTeam00
static int tolua_Script_Script_RunScriptForTeam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* scriptName = ((const char*)  tolua_tostring(tolua_S,2,0));
  long distance = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunScriptForTeam'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RunScriptForTeam(scriptName,distance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RunScriptForTeam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RunScriptForTeam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RunScriptForTeam01
static int tolua_Script_Script_RunScriptForTeam01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* scriptName = ((const char*)  tolua_tostring(tolua_S,2,0));
  long distance = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunScriptForTeam'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RunScriptForTeam(scriptName,distance,*playerGuid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_RunScriptForTeam00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateFaction of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CreateFaction00
static int tolua_Script_Script_CreateFaction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateFaction'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CreateFaction();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateFaction'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OpenApplyBoard of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OpenApplyBoard00
static int tolua_Script_Script_OpenApplyBoard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OpenApplyBoard'", NULL);
#endif
  {
   self->OpenApplyBoard();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OpenApplyBoard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OpenFacIconBoard of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OpenFacIconBoard00
static int tolua_Script_Script_OpenFacIconBoard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OpenFacIconBoard'", NULL);
#endif
  {
   self->OpenFacIconBoard();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OpenFacIconBoard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFactionLevel of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFactionLevel00
static int tolua_Script_Script_GetFactionLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFactionLevel'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetFactionLevel();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFactionLevel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetFactionRecruitState of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetFactionRecruitState00
static int tolua_Script_Script_SetFactionRecruitState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long inPut = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetFactionRecruitState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetFactionRecruitState(inPut);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetFactionRecruitState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFactionIDByPlayerName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFactionIDByPlayerName00
static int tolua_Script_Script_GetFactionIDByPlayerName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,2,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFactionIDByPlayerName'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetFactionIDByPlayerName(playerName);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFactionIDByPlayerName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUinionIDByPlayerName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetUinionIDByPlayerName00
static int tolua_Script_Script_GetUinionIDByPlayerName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,2,NULL));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUinionIDByPlayerName'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetUinionIDByPlayerName(playerName);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUinionIDByPlayerName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsFacMaster of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsFacMaster00
static int tolua_Script_Script_IsFacMaster00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsFacMaster'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsFacMaster();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsFacMaster'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFacTickets of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFacTickets00
static int tolua_Script_Script_GetFacTickets00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* factionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* footholdName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFacTickets'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetFacTickets(*factionGuid,footholdName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFacTickets'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBattleVal of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetBattleVal00
static int tolua_Script_Script_GetBattleVal00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int flag = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBattleVal'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetBattleVal(flag,*guid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBattleVal'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetArmyInvest of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetArmyInvest00
static int tolua_Script_Script_GetArmyInvest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int flag = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetArmyInvest'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetArmyInvest(flag,*guid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetArmyInvest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetJoinBattle of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetJoinBattle00
static int tolua_Script_Script_GetJoinBattle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetJoinBattle'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetJoinBattle(*playerGuid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetJoinBattle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTechLv of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTechLv00
static int tolua_Script_Script_GetTechLv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* factionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long techNo = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTechLv'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetTechLv(*factionGuid,techNo);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTechLv'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFactionName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFactionName00
static int tolua_Script_Script_GetFactionName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* factionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFactionName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetFactionName(*factionGuid);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFactionName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFacBaseBattle of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFacBaseBattle00
static int tolua_Script_Script_GetFacBaseBattle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* factionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* footholdName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFacBaseBattle'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetFacBaseBattle(*factionGuid,footholdName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFacBaseBattle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendBaseInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendBaseInfo00
static int tolua_Script_Script_SendBaseInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* factionGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendBaseInfo'", NULL);
#endif
  {
   self->SendBaseInfo(*factionGuid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendBaseInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFacLevel of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFacLevel00
static int tolua_Script_Script_GetFacLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long type = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFacLevel'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetFacLevel(type);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFacLevel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFacLevel of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFacLevel01
static int tolua_Script_Script_GetFacLevel01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long type = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFacLevel'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetFacLevel(type,*playerGuid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetFacLevel00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMemContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMemContribute00
static int tolua_Script_Script_GetMemContribute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMemContribute'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetMemContribute();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMemContribute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMemContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMemContribute01
static int tolua_Script_Script_GetMemContribute01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerGuid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMemContribute'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetMemContribute(*playerGuid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetMemContribute00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: OpenCreateFactionUi of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OpenCreateFactionUi00
static int tolua_Script_Script_OpenCreateFactionUi00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OpenCreateFactionUi'", NULL);
#endif
  {
   self->OpenCreateFactionUi();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OpenCreateFactionUi'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: KickRegionPlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_KickRegionPlayer00
static int tolua_Script_Script_KickRegionPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* regionID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'KickRegionPlayer'", NULL);
#endif
  {
   self->KickRegionPlayer(*regionID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'KickRegionPlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: KickAllPlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_KickAllPlayer00
static int tolua_Script_Script_KickAllPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'KickAllPlayer'", NULL);
#endif
  {
   self->KickAllPlayer();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'KickAllPlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: KickPlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_KickPlayer00
static int tolua_Script_Script_KickPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'KickPlayer'", NULL);
#endif
  {
   self->KickPlayer(name);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'KickPlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ListOnlineGM of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ListOnlineGM00
static int tolua_Script_Script_ListOnlineGM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ListOnlineGM'", NULL);
#endif
  {
   int tolua_ret = (int)  self->ListOnlineGM();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ListOnlineGM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ListSelencePlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ListSelencePlayer00
static int tolua_Script_Script_ListSelencePlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ListSelencePlayer'", NULL);
#endif
  {
   self->ListSelencePlayer();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ListSelencePlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BanPlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_BanPlayer00
static int tolua_Script_Script_BanPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  long time = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BanPlayer'", NULL);
#endif
  {
   self->BanPlayer(name,time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BanPlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTextRes of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTextRes00
static int tolua_Script_Script_GetTextRes00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* id = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTextRes'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetTextRes(id);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTextRes'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RollRegionInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RollRegionInfo00
static int tolua_Script_Script_RollRegionInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long rgnType = ((long)  tolua_tonumber(tolua_S,2,0));
  long num = ((long)  tolua_tonumber(tolua_S,3,0));
  const char* msg = ((const char*)  tolua_tostring(tolua_S,4,0));
  long fileNum = ((long)  tolua_tonumber(tolua_S,5,0));
  long rgnID = ((long)  tolua_tonumber(tolua_S,6,-1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RollRegionInfo'", NULL);
#endif
  {
   self->RollRegionInfo(rgnType,num,msg,fileNum,rgnID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RollRegionInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RollWorldInfo of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RollWorldInfo00
static int tolua_Script_Script_RollWorldInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long num = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* str = ((const char*)  tolua_tostring(tolua_S,3,0));
  long fileNum = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RollWorldInfo'", NULL);
#endif
  {
   self->RollWorldInfo(num,str,fileNum);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RollWorldInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerRegionEx of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerRegionEx00
static int tolua_Script_Script_ChangePlayerRegionEx00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long rgnType = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* centerPlayer = ((const char*)  tolua_tostring(tolua_S,3,0));
  const CGUID* rgnID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long x = ((long)  tolua_tonumber(tolua_S,5,0));
  long y = ((long)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerRegionEx'", NULL);
#endif
  {
   self->ChangePlayerRegionEx(rgnType,centerPlayer,*rgnID,x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangePlayerRegionEx'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerAttr00
static int tolua_Script_Script_SetPlayerAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerAttr'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetPlayerAttr(attrName,attrVal);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerAttr01
static int tolua_Script_Script_SetPlayerAttr01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerAttr'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetPlayerAttr(attrName,attrVal,*playerID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_SetPlayerAttr00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerAttr00
static int tolua_Script_Script_GetPlayerAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerAttr'", NULL);
#endif
  {
   double tolua_ret = (double)  self->GetPlayerAttr(attrName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerAttr01
static int tolua_Script_Script_GetPlayerAttr01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerAttr'", NULL);
#endif
  {
   double tolua_ret = (double)  self->GetPlayerAttr(attrName,*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerAttr00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerAttr00
static int tolua_Script_Script_ChangePlayerAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerAttr'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ChangePlayerAttr(attrName,attrVal);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangePlayerAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerAttr01
static int tolua_Script_Script_ChangePlayerAttr01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerAttr'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ChangePlayerAttr(attrName,attrVal,*playerID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_ChangePlayerAttr00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerName00
static int tolua_Script_Script_GetPlayerName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetPlayerName();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerName01
static int tolua_Script_Script_GetPlayerName01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetPlayerName(*playerID);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerName00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerID00
static int tolua_Script_Script_GetPlayerID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerID'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetPlayerID(name);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerMoney00
static int tolua_Script_Script_ChangePlayerMoney00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long money = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerMoney'", NULL);
#endif
  {
   self->ChangePlayerMoney(money);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangePlayerMoney'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerMoney01
static int tolua_Script_Script_ChangePlayerMoney01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long money = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerMoney'", NULL);
#endif
  {
   self->ChangePlayerMoney(money,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_ChangePlayerMoney00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerMoney00
static int tolua_Script_Script_GetPlayerMoney00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerMoney'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerMoney();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerMoney'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerMoney01
static int tolua_Script_Script_GetPlayerMoney01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerMoney'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerMoney(*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerMoney00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerMoney00
static int tolua_Script_Script_SetPlayerMoney00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long money = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerMoney'", NULL);
#endif
  {
   self->SetPlayerMoney(money);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerMoney'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerMoney of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerMoney01
static int tolua_Script_Script_SetPlayerMoney01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long money = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerMoney'", NULL);
#endif
  {
   self->SetPlayerMoney(money,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerMoney00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerTicket00
static int tolua_Script_Script_ChangePlayerTicket00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ticket = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerTicket'", NULL);
#endif
  {
   self->ChangePlayerTicket(ticket);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangePlayerTicket'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangePlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangePlayerTicket01
static int tolua_Script_Script_ChangePlayerTicket01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ticket = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangePlayerTicket'", NULL);
#endif
  {
   self->ChangePlayerTicket(ticket,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_ChangePlayerTicket00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerTicket00
static int tolua_Script_Script_GetPlayerTicket00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerTicket'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerTicket();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerTicket'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerTicket01
static int tolua_Script_Script_GetPlayerTicket01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerTicket'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerTicket(*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerTicket00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerTicket00
static int tolua_Script_Script_SetPlayerTicket00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ticket = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerTicket'", NULL);
#endif
  {
   self->SetPlayerTicket(ticket);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerTicket'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerTicket of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerTicket01
static int tolua_Script_Script_SetPlayerTicket01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long ticket = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerTicket'", NULL);
#endif
  {
   self->SetPlayerTicket(ticket,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerTicket00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeBaseSpeed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeBaseSpeed00
static int tolua_Script_Script_ChangeBaseSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeBaseSpeed'", NULL);
#endif
  {
   self->ChangeBaseSpeed();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeBaseSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeBaseSpeed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeBaseSpeed01
static int tolua_Script_Script_ChangeBaseSpeed01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeBaseSpeed'", NULL);
#endif
  {
   self->ChangeBaseSpeed(*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_ChangeBaseSpeed00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerPos00
static int tolua_Script_Script_SetPlayerPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long x = ((long)  tolua_tonumber(tolua_S,2,0));
  long y = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerPos'", NULL);
#endif
  {
   self->SetPlayerPos(x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerPos of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerPos01
static int tolua_Script_Script_SetPlayerPos01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long x = ((long)  tolua_tonumber(tolua_S,2,0));
  long y = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerPos'", NULL);
#endif
  {
   self->SetPlayerPos(x,y,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerPos00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerDir of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerDir00
static int tolua_Script_Script_SetPlayerDir00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long dir = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerDir'", NULL);
#endif
  {
   self->SetPlayerDir(dir);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerDir'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerDir of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerDir01
static int tolua_Script_Script_SetPlayerDir01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long dir = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerDir'", NULL);
#endif
  {
   self->SetPlayerDir(dir,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerDir00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCountry of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCountry00
static int tolua_Script_Script_GetCountry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCountry'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetCountry();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCountry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCountry of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCountry01
static int tolua_Script_Script_GetCountry01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCountry'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetCountry(*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetCountry00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeCountry of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeCountry00
static int tolua_Script_Script_ChangeCountry00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long country = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeCountry'", NULL);
#endif
  {
   self->ChangeCountry(country);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeCountry'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeCountry of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeCountry01
static int tolua_Script_Script_ChangeCountry01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long country = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeCountry'", NULL);
#endif
  {
   self->ChangeCountry(country,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_ChangeCountry00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetContribute00
static int tolua_Script_Script_GetContribute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetContribute'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetContribute();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetContribute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetContribute01
static int tolua_Script_Script_GetContribute01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetContribute'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetContribute(*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetContribute00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetContribute00
static int tolua_Script_Script_SetContribute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long contribute = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetContribute'", NULL);
#endif
  {
   self->SetContribute(contribute);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetContribute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetContribute of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetContribute01
static int tolua_Script_Script_SetContribute01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long contribute = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetContribute'", NULL);
#endif
  {
   self->SetContribute(contribute,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetContribute00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsCharged of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsCharged00
static int tolua_Script_Script_IsCharged00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsCharged'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsCharged();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsCharged'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsCharged of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsCharged01
static int tolua_Script_Script_IsCharged01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsCharged'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsCharged(*playerID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_IsCharged00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetCharged of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetCharged00
static int tolua_Script_Script_SetCharged00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int c = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetCharged'", NULL);
#endif
  {
   self->SetCharged(c);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetCharged'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetCharged of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetCharged01
static int tolua_Script_Script_SetCharged01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int c = ((int)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetCharged'", NULL);
#endif
  {
   self->SetCharged(c,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetCharged00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckOccuLvl of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CheckOccuLvl00
static int tolua_Script_Script_CheckOccuLvl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long occu = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CheckOccuLvl'", NULL);
#endif
  {
   self->CheckOccuLvl(occu);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckOccuLvl'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Relive of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_Relive00
static int tolua_Script_Script_Relive00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  long ratio = ((long)  tolua_tonumber(tolua_S,3,50));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Relive'", NULL);
#endif
  {
   self->Relive(type,ratio);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Relive'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeOccu of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeOccu00
static int tolua_Script_Script_ChangeOccu00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long occu = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeOccu'", NULL);
#endif
  {
   self->ChangeOccu(occu);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeOccu'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AutoMove of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AutoMove00
static int tolua_Script_Script_AutoMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  float x = ((float)  tolua_tonumber(tolua_S,2,0));
  float y = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AutoMove'", NULL);
#endif
  {
   self->AutoMove(x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AutoMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: MovePlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_MovePlayer00
static int tolua_Script_Script_MovePlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,8,&tolua_err) || !tolua_isusertype(tolua_S,8,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,12,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,13,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* src = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long x1 = ((long)  tolua_tonumber(tolua_S,3,0));
  long y1 = ((long)  tolua_tonumber(tolua_S,4,0));
  long x2 = ((long)  tolua_tonumber(tolua_S,5,0));
  long y2 = ((long)  tolua_tonumber(tolua_S,6,0));
  long type = ((long)  tolua_tonumber(tolua_S,7,0));
  const CGUID* dest = ((const CGUID*)  tolua_tousertype(tolua_S,8,0));
  long dx1 = ((long)  tolua_tonumber(tolua_S,9,0));
  long dy1 = ((long)  tolua_tonumber(tolua_S,10,0));
  long dx2 = ((long)  tolua_tonumber(tolua_S,11,0));
  long dy2 = ((long)  tolua_tonumber(tolua_S,12,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'MovePlayer'", NULL);
#endif
  {
   self->MovePlayer(*src,x1,y1,x2,y2,type,*dest,dx1,dy1,dx2,dy2);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MovePlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GodMode of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GodMode00
static int tolua_Script_Script_GodMode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int mode = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GodMode'", NULL);
#endif
  {
   self->GodMode(mode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GodMode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsGM of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsGM00
static int tolua_Script_Script_IsGM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsGM'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsGM(name);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsGM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DurancePlayer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DurancePlayer00
static int tolua_Script_Script_DurancePlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  int flag = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DurancePlayer'", NULL);
#endif
  {
   self->DurancePlayer(name,flag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DurancePlayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetServer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetServer00
static int tolua_Script_Script_GetServer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetServer'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetServer();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetServer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOtherOneInMarriageTeam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetOtherOneInMarriageTeam00
static int tolua_Script_Script_GetOtherOneInMarriageTeam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOtherOneInMarriageTeam'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetOtherOneInMarriageTeam();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOtherOneInMarriageTeam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOtherOneInMarriageTeam of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetOtherOneInMarriageTeam01
static int tolua_Script_Script_GetOtherOneInMarriageTeam01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOtherOneInMarriageTeam'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetOtherOneInMarriageTeam(*playerID);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetOtherOneInMarriageTeam00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerMarriageStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerMarriageStep00
static int tolua_Script_Script_GetPlayerMarriageStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerMarriageStep'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerMarriageStep();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerMarriageStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerMarriageStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerMarriageStep01
static int tolua_Script_Script_GetPlayerMarriageStep01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerMarriageStep'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPlayerMarriageStep(*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerMarriageStep00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerMarriageStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerMarriageStep00
static int tolua_Script_Script_SetPlayerMarriageStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long step = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerMarriageStep'", NULL);
#endif
  {
   self->SetPlayerMarriageStep(step);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerMarriageStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerMarriageStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerMarriageStep01
static int tolua_Script_Script_SetPlayerMarriageStep01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long step = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerMarriageStep'", NULL);
#endif
  {
   self->SetPlayerMarriageStep(step,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerMarriageStep00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerSpouse00
static int tolua_Script_Script_SetPlayerSpouse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* src = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerSpouse'", NULL);
#endif
  {
   self->SetPlayerSpouse(*src);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPlayerSpouse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPlayerSpouse01
static int tolua_Script_Script_SetPlayerSpouse01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* src = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const CGUID* dest = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPlayerSpouse'", NULL);
#endif
  {
   self->SetPlayerSpouse(*src,*dest);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_SetPlayerSpouse00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelPlayerSpouse00
static int tolua_Script_Script_DelPlayerSpouse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelPlayerSpouse'", NULL);
#endif
  {
   self->DelPlayerSpouse();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelPlayerSpouse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelPlayerSpouse01
static int tolua_Script_Script_DelPlayerSpouse01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelPlayerSpouse'", NULL);
#endif
  {
   self->DelPlayerSpouse(*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_DelPlayerSpouse00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerSpouse00
static int tolua_Script_Script_GetPlayerSpouse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerSpouse'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetPlayerSpouse();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerSpouse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerSpouse of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerSpouse01
static int tolua_Script_Script_GetPlayerSpouse01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerSpouse'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetPlayerSpouse(*playerID);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerSpouse00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerSpouseName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerSpouseName00
static int tolua_Script_Script_GetPlayerSpouseName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerSpouseName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetPlayerSpouseName();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayerSpouseName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPlayerSpouseName of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPlayerSpouseName01
static int tolua_Script_Script_GetPlayerSpouseName01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayerSpouseName'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetPlayerSpouseName(*playerID);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPlayerSpouseName00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetElementProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetElementProperty00
static int tolua_Script_Script_GetElementProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long etype = ((long)  tolua_tonumber(tolua_S,2,0));
  long ptype = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetElementProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetElementProperty(etype,ptype);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetElementProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetElementProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetElementProperty01
static int tolua_Script_Script_GetElementProperty01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long etype = ((long)  tolua_tonumber(tolua_S,2,0));
  long ptype = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetElementProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetElementProperty(etype,ptype,*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetElementProperty00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetElementProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetElementProperty00
static int tolua_Script_Script_SetElementProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long etype = ((long)  tolua_tonumber(tolua_S,2,0));
  long ptype = ((long)  tolua_tonumber(tolua_S,3,0));
  long val = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetElementProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->SetElementProperty(etype,ptype,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetElementProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetElementProperty of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetElementProperty01
static int tolua_Script_Script_SetElementProperty01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long etype = ((long)  tolua_tonumber(tolua_S,2,0));
  long ptype = ((long)  tolua_tonumber(tolua_S,3,0));
  long val = ((long)  tolua_tonumber(tolua_S,4,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetElementProperty'", NULL);
#endif
  {
   long tolua_ret = (long)  self->SetElementProperty(etype,ptype,val,*playerID);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_SetElementProperty00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: StudyGenius of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_StudyGenius00
static int tolua_Script_Script_StudyGenius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long id = ((long)  tolua_tonumber(tolua_S,2,0));
  long lvl = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'StudyGenius'", NULL);
#endif
  {
   self->StudyGenius(id,lvl);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'StudyGenius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: StudyGenius of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_StudyGenius01
static int tolua_Script_Script_StudyGenius01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long id = ((long)  tolua_tonumber(tolua_S,2,0));
  long lvl = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'StudyGenius'", NULL);
#endif
  {
   self->StudyGenius(id,lvl,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_StudyGenius00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveGenius of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RemoveGenius00
static int tolua_Script_Script_RemoveGenius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long id = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveGenius'", NULL);
#endif
  {
   self->RemoveGenius(id);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveGenius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveGenius of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RemoveGenius01
static int tolua_Script_Script_RemoveGenius01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long id = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveGenius'", NULL);
#endif
  {
   self->RemoveGenius(id,*playerID);
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_RemoveGenius00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetAttr00
static int tolua_Script_Script_SetAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,4,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAttr'", NULL);
#endif
  {
   self->SetAttr(type,*id,attrName,attrVal);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAttr00
static int tolua_Script_Script_GetAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAttr'", NULL);
#endif
  {
   double tolua_ret = (double)  self->GetAttr(type,*id,attrName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeAttr00
static int tolua_Script_Script_ChangeAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,4,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeAttr'", NULL);
#endif
  {
   self->ChangeAttr(type,*id,attrName,attrVal);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayMotion of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PlayMotion00
static int tolua_Script_Script_PlayMotion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long motion = ((long)  tolua_tonumber(tolua_S,2,0));
  long around = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayMotion'", NULL);
#endif
  {
   self->PlayMotion(motion,around);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayMotion'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ShapeRunScript of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ShapeRunScript00
static int tolua_Script_Script_ShapeRunScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  const char* script = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ShapeRunScript'", NULL);
#endif
  {
   self->ShapeRunScript(type,*id,script);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShapeRunScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAroundPlayerNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAroundPlayerNum00
static int tolua_Script_Script_GetAroundPlayerNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long range = ((long)  tolua_tonumber(tolua_S,2,0));
  long diedFlag = ((long)  tolua_tonumber(tolua_S,3,0));
  long hideFlag = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAroundPlayerNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetAroundPlayerNum(range,diedFlag,hideFlag);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAroundPlayerNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAroundMonsterNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAroundMonsterNum00
static int tolua_Script_Script_GetAroundMonsterNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long range = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAroundMonsterNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetAroundMonsterNum(range);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAroundMonsterNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAroundSameMonsterNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetAroundSameMonsterNum00
static int tolua_Script_Script_GetAroundSameMonsterNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long range = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAroundSameMonsterNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetAroundSameMonsterNum(range);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAroundSameMonsterNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDelTime of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetDelTime00
static int tolua_Script_Script_SetDelTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  long delTime = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDelTime'", NULL);
#endif
  {
   self->SetDelTime(type,*id,delTime);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDelTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ChangeMoveSpeed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ChangeMoveSpeed00
static int tolua_Script_Script_ChangeMoveSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  short speed = ((short)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ChangeMoveSpeed'", NULL);
#endif
  {
   self->ChangeMoveSpeed(type,*id,speed);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ChangeMoveSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMoveSpeed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetMoveSpeed00
static int tolua_Script_Script_SetMoveSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
  short speed = ((short)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMoveSpeed'", NULL);
#endif
  {
   self->SetMoveSpeed(type,*id,speed);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMoveSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrentSpeed of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCurrentSpeed00
static int tolua_Script_Script_GetCurrentSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrentSpeed'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetCurrentSpeed(type,*id);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrentSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsShapeValid of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IsShapeValid00
static int tolua_Script_Script_IsShapeValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsShapeValid'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsShapeValid(type,*id);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsShapeValid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreatePhase of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CreatePhase00
static int tolua_Script_Script_CreatePhase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  float size1 = ((float)  tolua_tonumber(tolua_S,2,0));
  float size2 = ((float)  tolua_tonumber(tolua_S,3,0));
  long trigger = ((long)  tolua_tonumber(tolua_S,4,0));
  const char* scriptfile = ((const char*)  tolua_tostring(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePhase'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CreatePhase(size1,size2,trigger,scriptfile);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreatePhase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ActivePhase of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ActivePhase00
static int tolua_Script_Script_ActivePhase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long etype = ((long)  tolua_tonumber(tolua_S,2,0));
  long lTimer = ((long)  tolua_tonumber(tolua_S,3,0));
  long lExipires = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ActivePhase'", NULL);
#endif
  {
   self->ActivePhase(etype,lTimer,lExipires);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActivePhase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPhasePoint of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPhasePoint00
static int tolua_Script_Script_GetPhasePoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* pName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPhasePoint'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPhasePoint(pName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPhasePoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPhasePoint of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPhasePoint01
static int tolua_Script_Script_GetPhasePoint01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* pName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPhasePoint'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPhasePoint(*id,pName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetPhasePoint00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreatePhaseMonster of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CreatePhaseMonster00
static int tolua_Script_Script_CreatePhaseMonster00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isstring(tolua_S,9,0,&tolua_err) ||
     !tolua_isstring(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* OrgName = ((const char*)  tolua_tostring(tolua_S,2,0));
  int Amount = ((int)  tolua_tonumber(tolua_S,3,0));
  int x_point = ((int)  tolua_tonumber(tolua_S,4,0));
  int y_point = ((int)  tolua_tonumber(tolua_S,5,0));
  int Width = ((int)  tolua_tonumber(tolua_S,6,0));
  int Height = ((int)  tolua_tonumber(tolua_S,7,0));
  int rgnId = ((int)  tolua_tonumber(tolua_S,8,0));
  const char* scriptFileName = ((const char*)  tolua_tostring(tolua_S,9,0));
  const char* talkFileName = ((const char*)  tolua_tostring(tolua_S,10,0));
  int dir = ((int)  tolua_tonumber(tolua_S,11,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePhaseMonster'", NULL);
#endif
  {
   self->CreatePhaseMonster(OrgName,Amount,x_point,y_point,Width,Height,rgnId,scriptFileName,talkFileName,dir);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreatePhaseMonster'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OnPhaseOper of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OnPhaseOper00
static int tolua_Script_Script_OnPhaseOper00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long Oper = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OnPhaseOper'", NULL);
#endif
  {
   long tolua_ret = (long)  self->OnPhaseOper(Oper);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OnPhaseOper'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OnPhaseEvent of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_OnPhaseEvent00
static int tolua_Script_Script_OnPhaseEvent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long eExit = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OnPhaseEvent'", NULL);
#endif
  {
   long tolua_ret = (long)  self->OnPhaseEvent(eExit);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OnPhaseEvent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPhaseEnd of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPhaseEnd00
static int tolua_Script_Script_SetPhaseEnd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPhaseEnd'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetPhaseEnd();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPhaseEnd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPhaseEndScriptFile of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPhaseEndScriptFile00
static int tolua_Script_Script_SetPhaseEndScriptFile00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* Filename = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPhaseEndScriptFile'", NULL);
#endif
  {
   self->SetPhaseEndScriptFile(Filename);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPhaseEndScriptFile'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClosePhase of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ClosePhase00
static int tolua_Script_Script_ClosePhase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* PhaseId = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClosePhase'", NULL);
#endif
  {
   self->ClosePhase(*PhaseId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClosePhase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClosePhase of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ClosePhase01
static int tolua_Script_Script_ClosePhase01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClosePhase'", NULL);
#endif
  {
   self->ClosePhase();
  }
 }
 return 0;
tolua_lerror:
 return tolua_Script_Script_ClosePhase00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPhaseID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPhaseID00
static int tolua_Script_Script_GetPhaseID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPhaseID'", NULL);
#endif
  {
   const CGUID& tolua_ret = (const CGUID&)  self->GetPhaseID();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const CGUID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPhaseID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetShapePhaseID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetShapePhaseID00
static int tolua_Script_Script_GetShapePhaseID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetShapePhaseID'", NULL);
#endif
  {
   const CGUID& tolua_ret = (const CGUID&)  self->GetShapePhaseID();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const CGUID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetShapePhaseID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCostTime of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetCostTime00
static int tolua_Script_Script_GetCostTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* PhaseId = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCostTime'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetCostTime(*PhaseId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCostTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CancelPhaseTimer of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CancelPhaseTimer00
static int tolua_Script_Script_CancelPhaseTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* PhaseId = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CancelPhaseTimer'", NULL);
#endif
  {
   self->CancelPhaseTimer(*PhaseId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CancelPhaseTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PostPhaseMsg of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PostPhaseMsg00
static int tolua_Script_Script_PostPhaseMsg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* PhaseId = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  const char* pMsg = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned long dwColor = (( unsigned long)  tolua_tonumber(tolua_S,4,0));
  unsigned long dwBkColor = (( unsigned long)  tolua_tonumber(tolua_S,5,0));
  long ePos = ((long)  tolua_tonumber(tolua_S,6,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PostPhaseMsg'", NULL);
#endif
  {
   self->PostPhaseMsg(*PhaseId,pMsg,dwColor,dwBkColor,ePos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PostPhaseMsg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreatePet of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_CreatePet00
static int tolua_Script_Script_CreatePet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  long id = ((long)  tolua_tonumber(tolua_S,3,0));
  long isWild = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePet'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CreatePet(type,id,isWild);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreatePet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DelPet of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_DelPet00
static int tolua_Script_Script_DelPet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DelPet'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->DelPet(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DelPet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPetAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetPetAttr00
static int tolua_Script_Script_SetPetAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  double attrVal = ((double)  tolua_tonumber(tolua_S,3,0));
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPetAttr'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetPetAttr(attrName,attrVal,*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPetAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPetAttr of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPetAttr00
static int tolua_Script_Script_GetPetAttr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const char* attrName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPetAttr'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPetAttr(attrName,*guid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPetAttr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ActivePet of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ActivePet00
static int tolua_Script_Script_ActivePet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ActivePet'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ActivePet(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ActivePet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddPetSkill of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_AddPetSkill00
static int tolua_Script_Script_AddPetSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long id = ((long)  tolua_tonumber(tolua_S,3,0));
  long level = ((long)  tolua_tonumber(tolua_S,4,0));
  long pos = ((long)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddPetSkill'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddPetSkill(*guid,id,level,pos);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddPetSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFocusPetGUID of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetFocusPetGUID00
static int tolua_Script_Script_GetFocusPetGUID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFocusPetGUID'", NULL);
#endif
  {
   const CGUID& tolua_ret = (const CGUID&)  self->GetFocusPetGUID();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const CGUID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFocusPetGUID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPetGrade of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPetGrade00
static int tolua_Script_Script_GetPetGrade00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPetGrade'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPetGrade(*guid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPetGrade'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPetDAfterCnt of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetPetDAfterCnt00
static int tolua_Script_Script_GetPetDAfterCnt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPetDAfterCnt'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetPetDAfterCnt(*guid);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPetDAfterCnt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ResetPetTalent of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ResetPetTalent00
static int tolua_Script_Script_ResetPetTalent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ResetPetTalent'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ResetPetTalent(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ResetPetTalent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ResetPetDAfterCnt of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ResetPetDAfterCnt00
static int tolua_Script_Script_ResetPetDAfterCnt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ResetPetDAfterCnt'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ResetPetDAfterCnt(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ResetPetDAfterCnt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UpgradePetDAfterVal of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_UpgradePetDAfterVal00
static int tolua_Script_Script_UpgradePetDAfterVal00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long val = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UpgradePetDAfterVal'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->UpgradePetDAfterVal(*guid,val);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UpgradePetDAfterVal'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ResetPetSavvy of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ResetPetSavvy00
static int tolua_Script_Script_ResetPetSavvy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ResetPetSavvy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ResetPetSavvy(*guid);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ResetPetSavvy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxServantNum of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetMaxServantNum00
static int tolua_Script_Script_GetMaxServantNum00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxServantNum'", NULL);
#endif
  {
   long tolua_ret = (long)  self->GetMaxServantNum();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxServantNum'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IncPetExp of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IncPetExp00
static int tolua_Script_Script_IncPetExp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long val = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IncPetExp'", NULL);
#endif
  {
   self->IncPetExp(*guid,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IncPetExp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IncPetDExp of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_IncPetDExp00
static int tolua_Script_Script_IncPetDExp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long val = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IncPetDExp'", NULL);
#endif
  {
   self->IncPetDExp(*guid,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IncPetDExp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PetLevUp of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_PetLevUp00
static int tolua_Script_Script_PetLevUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* guid = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long newLev = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PetLevUp'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->PetLevUp(*guid,newLev);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PetLevUp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ContinueAI of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ContinueAI00
static int tolua_Script_Script_ContinueAI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long millSecs = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ContinueAI'", NULL);
#endif
  {
   int tolua_ret = (int)  self->ContinueAI(millSecs);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ContinueAI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: MoveToCell of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_MoveToCell00
static int tolua_Script_Script_MoveToCell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long x = ((long)  tolua_tonumber(tolua_S,2,0));
  long y = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'MoveToCell'", NULL);
#endif
  {
   self->MoveToCell(x,y);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MoveToCell'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RandomMove of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_RandomMove00
static int tolua_Script_Script_RandomMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RandomMove'", NULL);
#endif
  {
   self->RandomMove();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RandomMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: MoveByStep of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_MoveByStep00
static int tolua_Script_Script_MoveByStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long direct = ((long)  tolua_tonumber(tolua_S,2,0));
  long step = ((long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'MoveByStep'", NULL);
#endif
  {
   self->MoveByStep(direct,step);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MoveByStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FollowTarget of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_FollowTarget00
static int tolua_Script_Script_FollowTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* targetID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long targetType = ((long)  tolua_tonumber(tolua_S,3,0));
  long time = ((long)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FollowTarget'", NULL);
#endif
  {
   self->FollowTarget(*targetID,targetType,time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FollowTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTarget of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTarget00
static int tolua_Script_Script_GetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long targetType = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTarget'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetTarget(&targetType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushnumber(tolua_S,(lua_Number)targetType);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReceiveMail of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ReceiveMail00
static int tolua_Script_Script_ReceiveMail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* playerID = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ReceiveMail'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ReceiveMail(*playerID);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReceiveMail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SendSysMail of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SendSysMail00
static int tolua_Script_Script_SendSysMail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  unsigned long mailID = (( unsigned long)  tolua_tonumber(tolua_S,2,0));
  const char* writer = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SendSysMail'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SendSysMail(mailID,writer);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendSysMail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReceiveSysMail of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ReceiveSysMail00
static int tolua_Script_Script_ReceiveSysMail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ReceiveSysMail'", NULL);
#endif
  {
   self->ReceiveSysMail();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReceiveSysMail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTauntValue of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTauntValue00
static int tolua_Script_Script_GetTauntValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTauntValue'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetTauntValue(index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTauntValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTauntValue of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_SetTauntValue00
static int tolua_Script_Script_SetTauntValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTauntValue'", NULL);
#endif
  {
   self->SetTauntValue(*id,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTauntValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTauntValue of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTauntValue01
static int tolua_Script_Script_GetTauntValue01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTauntValue'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetTauntValue(*id);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_Script_Script_GetTauntValue00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRandObject of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetRandObject00
static int tolua_Script_Script_GetRandObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRandObject'", NULL);
#endif
  {
   CGUID tolua_ret = (CGUID)  self->GetRandObject(type);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((CGUID)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(CGUID));
     tolua_pushusertype(tolua_S,tolua_obj,"CGUID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRandObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ObjectRunScript of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ObjectRunScript00
static int tolua_Script_Script_ObjectRunScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  long type = ((long)  tolua_tonumber(tolua_S,2,0));
  const char* file = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ObjectRunScript'", NULL);
#endif
  {
   self->ObjectRunScript(type,file);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ObjectRunScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToFirst of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_ForceToFirst00
static int tolua_Script_Script_ForceToFirst00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* id = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToFirst'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ForceToFirst(*id);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToFirst'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTauntSize of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_GetTauntSize00
static int tolua_Script_Script_GetTauntSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTauntSize'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetTauntSize();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTauntSize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: TauntEvents of class  Script */
#ifndef TOLUA_DISABLE_tolua_Script_Script_TauntEvents00
static int tolua_Script_Script_TauntEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Script",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const CGUID",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Script* self = (Script*)  tolua_tousertype(tolua_S,1,0);
  const CGUID* firer = ((const CGUID*)  tolua_tousertype(tolua_S,2,0));
  long ftype = ((long)  tolua_tonumber(tolua_S,3,0));
  const CGUID* owner = ((const CGUID*)  tolua_tousertype(tolua_S,4,0));
  long otype = ((long)  tolua_tonumber(tolua_S,5,0));
  int tvalue = ((int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'TauntEvents'", NULL);
#endif
  {
   self->TauntEvents(*firer,ftype,*owner,otype,tvalue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TauntEvents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Script_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"Script","Script","",NULL);
  tolua_beginmodule(tolua_S,"Script");
   tolua_function(tolua_S,"AddInfo",tolua_Script_Script_AddInfo00);
   tolua_function(tolua_S,"AddInfo",tolua_Script_Script_AddInfo01);
   tolua_function(tolua_S,"PostRegionInfo",tolua_Script_Script_PostRegionInfo00);
   tolua_function(tolua_S,"PostWorldInfo",tolua_Script_Script_PostWorldInfo00);
   tolua_function(tolua_S,"PostCountryInfo",tolua_Script_Script_PostCountryInfo00);
   tolua_function(tolua_S,"PlayEffectByIndex",tolua_Script_Script_PlayEffectByIndex00);
   tolua_function(tolua_S,"PlayerTalk",tolua_Script_Script_PlayerTalk00);
   tolua_function(tolua_S,"PlayerTalkByIndex",tolua_Script_Script_PlayerTalkByIndex00);
   tolua_function(tolua_S,"PackGlint",tolua_Script_Script_PackGlint00);
   tolua_function(tolua_S,"PostPlayerInfo",tolua_Script_Script_PostPlayerInfo00);
   tolua_function(tolua_S,"PostPlayerInfoByIndex",tolua_Script_Script_PostPlayerInfoByIndex00);
   tolua_function(tolua_S,"ShowString",tolua_Script_Script_ShowString00);
   tolua_function(tolua_S,"ChangeWeather",tolua_Script_Script_ChangeWeather00);
   tolua_function(tolua_S,"PlayEffect",tolua_Script_Script_PlayEffect00);
   tolua_function(tolua_S,"PlayAction",tolua_Script_Script_PlayAction00);
   tolua_function(tolua_S,"PlaySound",tolua_Script_Script_PlaySound00);
   tolua_function(tolua_S,"Play3DSound",tolua_Script_Script_Play3DSound00);
   tolua_function(tolua_S,"PlayBkMusic",tolua_Script_Script_PlayBkMusic00);
   tolua_function(tolua_S,"StopBkMusic",tolua_Script_Script_StopBkMusic00);
   tolua_function(tolua_S,"AddGoodsToShortcutBar",tolua_Script_Script_AddGoodsToShortcutBar00);
   tolua_function(tolua_S,"AddSkillToShortcutBar",tolua_Script_Script_AddSkillToShortcutBar00);
   tolua_function(tolua_S,"TransformCamera",tolua_Script_Script_TransformCamera00);
   tolua_function(tolua_S,"OperInterface",tolua_Script_Script_OperInterface00);
   tolua_function(tolua_S,"HelpHint",tolua_Script_Script_HelpHint00);
   tolua_function(tolua_S,"InterfaceHint",tolua_Script_Script_InterfaceHint00);
   tolua_function(tolua_S,"SendExpAction",tolua_Script_Script_SendExpAction00);
   tolua_function(tolua_S,"SendExpAction",tolua_Script_Script_SendExpAction01);
   tolua_function(tolua_S,"GetSrcShapeID",tolua_Script_Script_GetSrcShapeID00);
   tolua_function(tolua_S,"GetDestShapeID",tolua_Script_Script_GetDestShapeID00);
   tolua_function(tolua_S,"GetCurRgn",tolua_Script_Script_GetCurRgn00);
   tolua_function(tolua_S,"GetRgnAttr",tolua_Script_Script_GetRgnAttr00);
   tolua_function(tolua_S,"SetRgnAttr",tolua_Script_Script_SetRgnAttr00);
   tolua_function(tolua_S,"GetCellType",tolua_Script_Script_GetCellType00);
   tolua_function(tolua_S,"GetMapInfo",tolua_Script_Script_GetMapInfo00);
   tolua_function(tolua_S,"SetBlock",tolua_Script_Script_SetBlock00);
   tolua_function(tolua_S,"GetRegionRandomPos",tolua_Script_Script_GetRegionRandomPos00);
   tolua_function(tolua_S,"GetRegionRandomPos",tolua_Script_Script_GetRegionRandomPos01);
   tolua_function(tolua_S,"GetRandomPosInRange",tolua_Script_Script_GetRandomPosInRange00);
   tolua_function(tolua_S,"GetLineDir",tolua_Script_Script_GetLineDir00);
   tolua_function(tolua_S,"GetNextPosByDir",tolua_Script_Script_GetNextPosByDir00);
   tolua_function(tolua_S,"RunScriptForAllPlayerInRgn",tolua_Script_Script_RunScriptForAllPlayerInRgn00);
   tolua_function(tolua_S,"GetNormalDupRgnID",tolua_Script_Script_GetNormalDupRgnID00);
   tolua_function(tolua_S,"ChangeRgn",tolua_Script_Script_ChangeRgn00);
   tolua_function(tolua_S,"ChangeDupRgn",tolua_Script_Script_ChangeDupRgn00);
   tolua_function(tolua_S,"GetCurRgnGUID",tolua_Script_Script_GetCurRgnGUID00);
   tolua_function(tolua_S,"GetDupRgnPlayerMaxNum",tolua_Script_Script_GetDupRgnPlayerMaxNum00);
   tolua_function(tolua_S,"RunDupRgnScript",tolua_Script_Script_RunDupRgnScript00);
   tolua_function(tolua_S,"GetDupRgnMaxNum",tolua_Script_Script_GetDupRgnMaxNum00);
   tolua_function(tolua_S,"SetDupRgnOperFlag",tolua_Script_Script_SetDupRgnOperFlag00);
   tolua_function(tolua_S,"GetDupRgnOperFlag",tolua_Script_Script_GetDupRgnOperFlag00);
   tolua_function(tolua_S,"GetTeamMaterIDByPos",tolua_Script_Script_GetTeamMaterIDByPos00);
   tolua_function(tolua_S,"EditDupRgnName",tolua_Script_Script_EditDupRgnName00);
   tolua_function(tolua_S,"CreatePHGoods",tolua_Script_Script_CreatePHGoods00);
   tolua_function(tolua_S,"DeletePHGoods",tolua_Script_Script_DeletePHGoods00);
   tolua_function(tolua_S,"GetCurPlayerPDupRgnID",tolua_Script_Script_GetCurPlayerPDupRgnID00);
   tolua_function(tolua_S,"GetCurPlayerPHDupRgnID",tolua_Script_Script_GetCurPlayerPHDupRgnID00);
   tolua_function(tolua_S,"SetPlayerPDupRgnID",tolua_Script_Script_SetPlayerPDupRgnID00);
   tolua_function(tolua_S,"SetPlayerPHDupRgnID",tolua_Script_Script_SetPlayerPHDupRgnID00);
   tolua_function(tolua_S,"GetLocalGSID",tolua_Script_Script_GetLocalGSID00);
   tolua_function(tolua_S,"ChangeDupRgnByRgnID",tolua_Script_Script_ChangeDupRgnByRgnID00);
   tolua_function(tolua_S,"GetCallerGUID",tolua_Script_Script_GetCallerGUID00);
   tolua_function(tolua_S,"CallScriptByGUID",tolua_Script_Script_CallScriptByGUID00);
   tolua_function(tolua_S,"ClearCurGSDupRgn",tolua_Script_Script_ClearCurGSDupRgn00);
   tolua_function(tolua_S,"AddTeamRgnID",tolua_Script_Script_AddTeamRgnID00);
   tolua_function(tolua_S,"DelTeamRgnID",tolua_Script_Script_DelTeamRgnID00);
   tolua_function(tolua_S,"ClearTeamRgnID",tolua_Script_Script_ClearTeamRgnID00);
   tolua_function(tolua_S,"SetRgnStrongPointState",tolua_Script_Script_SetRgnStrongPointState00);
   tolua_function(tolua_S,"GetRgnStrongPointState",tolua_Script_Script_GetRgnStrongPointState00);
   tolua_function(tolua_S,"AddGoods",tolua_Script_Script_AddGoods00);
   tolua_function(tolua_S,"AddArmament",tolua_Script_Script_AddArmament00);
   tolua_function(tolua_S,"DelGoods",tolua_Script_Script_DelGoods00);
   tolua_function(tolua_S,"AddGoodsToRgn",tolua_Script_Script_AddGoodsToRgn00);
   tolua_function(tolua_S,"BindGoods",tolua_Script_Script_BindGoods00);
   tolua_function(tolua_S,"FrostGoods",tolua_Script_Script_FrostGoods00);
   tolua_function(tolua_S,"CheckGoods",tolua_Script_Script_CheckGoods00);
   tolua_function(tolua_S,"CheckGoodsInBank",tolua_Script_Script_CheckGoodsInBank00);
   tolua_function(tolua_S,"CheckSpace",tolua_Script_Script_CheckSpace00);
   tolua_function(tolua_S,"GetGoodsNum",tolua_Script_Script_GetGoodsNum00);
   tolua_function(tolua_S,"GetGoodsFree",tolua_Script_Script_GetGoodsFree00);
   tolua_function(tolua_S,"GetEquipIDByPos",tolua_Script_Script_GetEquipIDByPos00);
   tolua_function(tolua_S,"GetEquipIDByPos",tolua_Script_Script_GetEquipIDByPos01);
   tolua_function(tolua_S,"GetEquipNameByPos",tolua_Script_Script_GetEquipNameByPos00);
   tolua_function(tolua_S,"GetPlayerGoodsAddonProperty",tolua_Script_Script_GetPlayerGoodsAddonProperty00);
   tolua_function(tolua_S,"SetPlayerGoodsAddonProperty",tolua_Script_Script_SetPlayerGoodsAddonProperty00);
   tolua_function(tolua_S,"DelPlayerGoods",tolua_Script_Script_DelPlayerGoods00);
   tolua_function(tolua_S,"DelUsedGoods",tolua_Script_Script_DelUsedGoods00);
   tolua_function(tolua_S,"CheckUsedGoods",tolua_Script_Script_CheckUsedGoods00);
   tolua_function(tolua_S,"GetUsedGoodsPropertiy",tolua_Script_Script_GetUsedGoodsPropertiy00);
   tolua_function(tolua_S,"SetUsedGoodsPropertiy",tolua_Script_Script_SetUsedGoodsPropertiy00);
   tolua_function(tolua_S,"BuyGoods",tolua_Script_Script_BuyGoods00);
   tolua_function(tolua_S,"GetGoodsAddonProperty",tolua_Script_Script_GetGoodsAddonProperty00);
   tolua_function(tolua_S,"FullRefine",tolua_Script_Script_FullRefine00);
   tolua_function(tolua_S,"GetWeaponStr",tolua_Script_Script_GetWeaponStr00);
   tolua_function(tolua_S,"GetGoodsDisplayName",tolua_Script_Script_GetGoodsDisplayName00);
   tolua_function(tolua_S,"BackToCity",tolua_Script_Script_BackToCity00);
   tolua_function(tolua_S,"BeginContainerGoodsOpt",tolua_Script_Script_BeginContainerGoodsOpt00);
   tolua_function(tolua_S,"GetOptGoodsInContainer",tolua_Script_Script_GetOptGoodsInContainer00);
   tolua_function(tolua_S,"PutOptGoodsInContainer",tolua_Script_Script_PutOptGoodsInContainer00);
   tolua_function(tolua_S,"RemoveOptGoodsInContainer",tolua_Script_Script_RemoveOptGoodsInContainer00);
   tolua_function(tolua_S,"RefreshContainerTxt",tolua_Script_Script_RefreshContainerTxt00);
   tolua_function(tolua_S,"RefreshContainerGold",tolua_Script_Script_RefreshContainerGold00);
   tolua_function(tolua_S,"EndContainerGoodsOpt",tolua_Script_Script_EndContainerGoodsOpt00);
   tolua_function(tolua_S,"GetGoodsOriginalName",tolua_Script_Script_GetGoodsOriginalName00);
   tolua_function(tolua_S,"GetGoodsAddonPropertyValues",tolua_Script_Script_GetGoodsAddonPropertyValues00);
   tolua_function(tolua_S,"SetGoodsAddonPropertyValues",tolua_Script_Script_SetGoodsAddonPropertyValues00);
   tolua_function(tolua_S,"ResetGoodsStochastic",tolua_Script_Script_ResetGoodsStochastic00);
   tolua_function(tolua_S,"DelGoodsByGuid",tolua_Script_Script_DelGoodsByGuid00);
   tolua_function(tolua_S,"AddGoodsReturnGuid",tolua_Script_Script_AddGoodsReturnGuid00);
   tolua_function(tolua_S,"IsFrostGoods",tolua_Script_Script_IsFrostGoods00);
   tolua_function(tolua_S,"GetUsedGoodsOriginalName",tolua_Script_Script_GetUsedGoodsOriginalName00);
   tolua_function(tolua_S,"GetUsedGoodsGuid",tolua_Script_Script_GetUsedGoodsGuid00);
   tolua_function(tolua_S,"SetAddonDesc",tolua_Script_Script_SetAddonDesc00);
   tolua_function(tolua_S,"GetAddonDesc",tolua_Script_Script_GetAddonDesc00);
   tolua_function(tolua_S,"GetUsedEquipProperty",tolua_Script_Script_GetUsedEquipProperty00);
   tolua_constant(tolua_S,"DEF_SHAPE",DEF_SHAPE);
   tolua_function(tolua_S,"AddState",tolua_Script_Script_AddState00);
   tolua_function(tolua_S,"RemoveState",tolua_Script_Script_RemoveState00);
   tolua_function(tolua_S,"AddSkill",tolua_Script_Script_AddSkill00);
   tolua_function(tolua_S,"DelSkill",tolua_Script_Script_DelSkill00);
   tolua_function(tolua_S,"GetSkillLevel",tolua_Script_Script_GetSkillLevel00);
   tolua_function(tolua_S,"CheckStudySkill",tolua_Script_Script_CheckStudySkill00);
   tolua_function(tolua_S,"GetAllStateNum",tolua_Script_Script_GetAllStateNum00);
   tolua_function(tolua_S,"GetAllStateNum",tolua_Script_Script_GetAllStateNum01);
   tolua_function(tolua_S,"DelDeBuff",tolua_Script_Script_DelDeBuff00);
   tolua_function(tolua_S,"DelAllBuff",tolua_Script_Script_DelAllBuff00);
   tolua_function(tolua_S,"SetMoveShapeAttrEx",tolua_Script_Script_SetMoveShapeAttrEx00);
   tolua_function(tolua_S,"GetMoveShapeAttrEx",tolua_Script_Script_GetMoveShapeAttrEx00);
   tolua_function(tolua_S,"ChangeMoveShapeAttrEx",tolua_Script_Script_ChangeMoveShapeAttrEx00);
   tolua_function(tolua_S,"AddDeBuffToObj",tolua_Script_Script_AddDeBuffToObj00);
   tolua_function(tolua_S,"ReliveSkill",tolua_Script_Script_ReliveSkill00);
   tolua_function(tolua_S,"GetPkResult",tolua_Script_Script_GetPkResult00);
   tolua_function(tolua_S,"GetObjectType",tolua_Script_Script_GetObjectType00);
   tolua_function(tolua_S,"GetDistance",tolua_Script_Script_GetDistance00);
   tolua_function(tolua_S,"SendSkillList",tolua_Script_Script_SendSkillList00);
   tolua_function(tolua_S,"GetEnergyRatio",tolua_Script_Script_GetEnergyRatio00);
   tolua_function(tolua_S,"DeductEnergy",tolua_Script_Script_DeductEnergy00);
   tolua_function(tolua_S,"SetTarget",tolua_Script_Script_SetTarget00);
   tolua_function(tolua_S,"GetShieldNum",tolua_Script_Script_GetShieldNum00);
   tolua_function(tolua_S,"GetSkillParam",tolua_Script_Script_GetSkillParam00);
   tolua_function(tolua_S,"GetSkillElement",tolua_Script_Script_GetSkillElement00);
   tolua_function(tolua_S,"GetSkillUseType",tolua_Script_Script_GetSkillUseType00);
   tolua_function(tolua_S,"GetSkillModType",tolua_Script_Script_GetSkillModType00);
   tolua_function(tolua_S,"GetStateParamByID",tolua_Script_Script_GetStateParamByID00);
   tolua_function(tolua_S,"GetStateParamByID",tolua_Script_Script_GetStateParamByID01);
   tolua_function(tolua_S,"GetStateParamByIndex",tolua_Script_Script_GetStateParamByIndex00);
   tolua_function(tolua_S,"GetStateParamByIndex",tolua_Script_Script_GetStateParamByIndex01);
   tolua_function(tolua_S,"CheckStateFirer",tolua_Script_Script_CheckStateFirer00);
   tolua_function(tolua_S,"GetSkillLv",tolua_Script_Script_GetSkillLv00);
   tolua_function(tolua_S,"ReliveInScript",tolua_Script_Script_ReliveInScript00);
   tolua_function(tolua_S,"GetGUIDByParam",tolua_Script_Script_GetGUIDByParam00);
   tolua_function(tolua_S,"RunScriptByDest",tolua_Script_Script_RunScriptByDest00);
   tolua_function(tolua_S,"BeginUseSkill",tolua_Script_Script_BeginUseSkill00);
   tolua_function(tolua_S,"SetEventDestObj",tolua_Script_Script_SetEventDestObj00);
   tolua_function(tolua_S,"GetBaseModuleLv",tolua_Script_Script_GetBaseModuleLv00);
   tolua_function(tolua_S,"GetGoodsAddonProperty",tolua_Script_Script_GetGoodsAddonProperty01);
   tolua_function(tolua_S,"GetGoodsPlacePos",tolua_Script_Script_GetGoodsPlacePos00);
   tolua_function(tolua_S,"UseGoodsByPlacePos",tolua_Script_Script_UseGoodsByPlacePos00);
   tolua_function(tolua_S,"SuccedUseItem",tolua_Script_Script_SuccedUseItem00);
   tolua_function(tolua_S,"GetActModify",tolua_Script_Script_GetActModify00);
   tolua_function(tolua_S,"SendUnlockGoodsMsg",tolua_Script_Script_SendUnlockGoodsMsg00);
   tolua_function(tolua_S,"GetBeneficiaryInfo",tolua_Script_Script_GetBeneficiaryInfo00);
   tolua_function(tolua_S,"GetKillerInfo",tolua_Script_Script_GetKillerInfo00);
   tolua_function(tolua_S,"GetAttackerInfo",tolua_Script_Script_GetAttackerInfo00);
   tolua_function(tolua_S,"GetSkillValue",tolua_Script_Script_GetSkillValue00);
   tolua_function(tolua_S,"GetSkillAttriValue",tolua_Script_Script_GetSkillAttriValue00);
   tolua_function(tolua_S,"ClearSkillAttackType",tolua_Script_Script_ClearSkillAttackType00);
   tolua_function(tolua_S,"AddQuest",tolua_Script_Script_AddQuest00);
   tolua_function(tolua_S,"GetMainQuestId",tolua_Script_Script_GetMainQuestId00);
   tolua_function(tolua_S,"CompleteQuest",tolua_Script_Script_CompleteQuest00);
   tolua_function(tolua_S,"AbandonQuest",tolua_Script_Script_AbandonQuest00);
   tolua_function(tolua_S,"GetQuestState",tolua_Script_Script_GetQuestState00);
   tolua_function(tolua_S,"GetValidQuestAmount",tolua_Script_Script_GetValidQuestAmount00);
   tolua_function(tolua_S,"UpdateQuestStep",tolua_Script_Script_UpdateQuestStep00);
   tolua_function(tolua_S,"UpdateQuestParam",tolua_Script_Script_UpdateQuestParam00);
   tolua_function(tolua_S,"UpdateQuestData",tolua_Script_Script_UpdateQuestData00);
   tolua_function(tolua_S,"GetQuestStepAmount",tolua_Script_Script_GetQuestStepAmount00);
   tolua_function(tolua_S,"GetParam",tolua_Script_Script_GetParam00);
   tolua_function(tolua_S,"GetQuestAccParam",tolua_Script_Script_GetQuestAccParam00);
   tolua_function(tolua_S,"SetQuestAccParam",tolua_Script_Script_SetQuestAccParam00);
   tolua_function(tolua_S,"AddQuestAccParam",tolua_Script_Script_AddQuestAccParam00);
   tolua_function(tolua_S,"GetQuestRegionID",tolua_Script_Script_GetQuestRegionID00);
   tolua_function(tolua_S,"SetQuestRegionID",tolua_Script_Script_SetQuestRegionID00);
   tolua_function(tolua_S,"GetQuestRegionX",tolua_Script_Script_GetQuestRegionX00);
   tolua_function(tolua_S,"SetQuestRegionX",tolua_Script_Script_SetQuestRegionX00);
   tolua_function(tolua_S,"GetQuestRegionY",tolua_Script_Script_GetQuestRegionY00);
   tolua_function(tolua_S,"SetQuestRegionY",tolua_Script_Script_SetQuestRegionY00);
   tolua_function(tolua_S,"GetQuestAmount",tolua_Script_Script_GetQuestAmount00);
   tolua_function(tolua_S,"GetQuestChapter",tolua_Script_Script_GetQuestChapter00);
   tolua_function(tolua_S,"SetQuestChapter",tolua_Script_Script_SetQuestChapter00);
   tolua_function(tolua_S,"AddFinishedQuestRecord",tolua_Script_Script_AddFinishedQuestRecord00);
   tolua_function(tolua_S,"DelFinishedQuestRecord",tolua_Script_Script_DelFinishedQuestRecord00);
   tolua_function(tolua_S,"GetQuestFinishedRecord",tolua_Script_Script_GetQuestFinishedRecord00);
   tolua_function(tolua_S,"GetQuestTextType",tolua_Script_Script_GetQuestTextType00);
   tolua_function(tolua_S,"GetQuestName",tolua_Script_Script_GetQuestName00);
   tolua_function(tolua_S,"IsQuestRecorded",tolua_Script_Script_IsQuestRecorded00);
   tolua_function(tolua_S,"GetQuestType",tolua_Script_Script_GetQuestType00);
   tolua_function(tolua_S,"GetQuestIsShare",tolua_Script_Script_GetQuestIsShare00);
   tolua_function(tolua_S,"GetQuestReqSkill",tolua_Script_Script_GetQuestReqSkill00);
   tolua_function(tolua_S,"GetQuestSexLimit",tolua_Script_Script_GetQuestSexLimit00);
   tolua_function(tolua_S,"GetQuestTotePet",tolua_Script_Script_GetQuestTotePet00);
   tolua_function(tolua_S,"IsQuestDisplayed",tolua_Script_Script_IsQuestDisplayed00);
   tolua_function(tolua_S,"IsQuestRepeated",tolua_Script_Script_IsQuestRepeated00);
   tolua_function(tolua_S,"IsQuestFailOnDead",tolua_Script_Script_IsQuestFailOnDead00);
   tolua_function(tolua_S,"IsQuestRepeatedOnFail",tolua_Script_Script_IsQuestRepeatedOnFail00);
   tolua_function(tolua_S,"GetQuestMinLvLimit",tolua_Script_Script_GetQuestMinLvLimit00);
   tolua_function(tolua_S,"GetQuestMaxLvLimit",tolua_Script_Script_GetQuestMaxLvLimit00);
   tolua_function(tolua_S,"GetQuestOccupationLimit",tolua_Script_Script_GetQuestOccupationLimit00);
   tolua_function(tolua_S,"GetQuestChapterAmount",tolua_Script_Script_GetQuestChapterAmount00);
   tolua_function(tolua_S,"GetNextQuestId",tolua_Script_Script_GetNextQuestId00);
   tolua_function(tolua_S,"GetPreQuest",tolua_Script_Script_GetPreQuest00);
   tolua_function(tolua_S,"GetPreQuestAmount",tolua_Script_Script_GetPreQuestAmount00);
   tolua_function(tolua_S,"GetAimAMTType",tolua_Script_Script_GetAimAMTType00);
   tolua_function(tolua_S,"GetAimType",tolua_Script_Script_GetAimType00);
   tolua_function(tolua_S,"GetAimId",tolua_Script_Script_GetAimId00);
   tolua_function(tolua_S,"GetAimAmount",tolua_Script_Script_GetAimAmount00);
   tolua_function(tolua_S,"GetAimParameIndex",tolua_Script_Script_GetAimParameIndex00);
   tolua_function(tolua_S,"GetDropFrom",tolua_Script_Script_GetDropFrom00);
   tolua_function(tolua_S,"GetDropOdds",tolua_Script_Script_GetDropOdds00);
   tolua_function(tolua_S,"GetAimName",tolua_Script_Script_GetAimName00);
   tolua_function(tolua_S,"GetPrepareParam",tolua_Script_Script_GetPrepareParam00);
   tolua_function(tolua_S,"GetQuestEncouragementSum",tolua_Script_Script_GetQuestEncouragementSum00);
   tolua_function(tolua_S,"GetStepEncouragementName",tolua_Script_Script_GetStepEncouragementName00);
   tolua_function(tolua_S,"GetQuestEncouragementName",tolua_Script_Script_GetQuestEncouragementName00);
   tolua_function(tolua_S,"GetStepEncouragementType",tolua_Script_Script_GetStepEncouragementType00);
   tolua_function(tolua_S,"GetQuestEncouragementType",tolua_Script_Script_GetQuestEncouragementType00);
   tolua_function(tolua_S,"GetStepEncouragementID",tolua_Script_Script_GetStepEncouragementID00);
   tolua_function(tolua_S,"GetQuestEncouragementID",tolua_Script_Script_GetQuestEncouragementID00);
   tolua_function(tolua_S,"GetStepEncouragementNum",tolua_Script_Script_GetStepEncouragementNum00);
   tolua_function(tolua_S,"GetQuestEncouragementNum",tolua_Script_Script_GetQuestEncouragementNum00);
   tolua_function(tolua_S,"GetStepEncouragementCell",tolua_Script_Script_GetStepEncouragementCell00);
   tolua_function(tolua_S,"GetQuestEncouragementCell",tolua_Script_Script_GetQuestEncouragementCell00);
   tolua_function(tolua_S,"GetStepSum",tolua_Script_Script_GetStepSum00);
   tolua_function(tolua_S,"GetStepAimNum",tolua_Script_Script_GetStepAimNum00);
   tolua_function(tolua_S,"FindRoleIsInSystem",tolua_Script_Script_FindRoleIsInSystem00);
   tolua_function(tolua_S,"FindRoleStepIsInSystem",tolua_Script_Script_FindRoleStepIsInSystem00);
   tolua_function(tolua_S,"GetQuestStateId",tolua_Script_Script_GetQuestStateId00);
   tolua_function(tolua_S,"GetQuestDisplayText",tolua_Script_Script_GetQuestDisplayText00);
   tolua_function(tolua_S,"GetTeamNum",tolua_Script_Script_GetTeamNum00);
   tolua_function(tolua_S,"GetTeamNumInRgn",tolua_Script_Script_GetTeamNumInRgn00);
   tolua_function(tolua_S,"GetTeamerName",tolua_Script_Script_GetTeamerName00);
   tolua_function(tolua_S,"GetTeamGUID",tolua_Script_Script_GetTeamGUID00);
   tolua_function(tolua_S,"GetPlayerTeamGUID",tolua_Script_Script_GetPlayerTeamGUID00);
   tolua_function(tolua_S,"IsTeamCaptain",tolua_Script_Script_IsTeamCaptain00);
   tolua_function(tolua_S,"GetTeamCaptainPos",tolua_Script_Script_GetTeamCaptainPos00);
   tolua_function(tolua_S,"SetRegionForTeam",tolua_Script_Script_SetRegionForTeam00);
   tolua_function(tolua_S,"SetTeamRegion",tolua_Script_Script_SetTeamRegion00);
   tolua_function(tolua_S,"IsTeammatesAroundMe",tolua_Script_Script_IsTeammatesAroundMe00);
   tolua_function(tolua_S,"RunScriptForTeam",tolua_Script_Script_RunScriptForTeam00);
   tolua_function(tolua_S,"RunScriptForTeam",tolua_Script_Script_RunScriptForTeam01);
   tolua_function(tolua_S,"CreateFaction",tolua_Script_Script_CreateFaction00);
   tolua_function(tolua_S,"OpenApplyBoard",tolua_Script_Script_OpenApplyBoard00);
   tolua_function(tolua_S,"OpenFacIconBoard",tolua_Script_Script_OpenFacIconBoard00);
   tolua_function(tolua_S,"GetFactionLevel",tolua_Script_Script_GetFactionLevel00);
   tolua_function(tolua_S,"SetFactionRecruitState",tolua_Script_Script_SetFactionRecruitState00);
   tolua_function(tolua_S,"GetFactionIDByPlayerName",tolua_Script_Script_GetFactionIDByPlayerName00);
   tolua_function(tolua_S,"GetUinionIDByPlayerName",tolua_Script_Script_GetUinionIDByPlayerName00);
   tolua_function(tolua_S,"IsFacMaster",tolua_Script_Script_IsFacMaster00);
   tolua_function(tolua_S,"GetFacTickets",tolua_Script_Script_GetFacTickets00);
   tolua_function(tolua_S,"GetBattleVal",tolua_Script_Script_GetBattleVal00);
   tolua_function(tolua_S,"GetArmyInvest",tolua_Script_Script_GetArmyInvest00);
   tolua_function(tolua_S,"GetJoinBattle",tolua_Script_Script_GetJoinBattle00);
   tolua_function(tolua_S,"GetTechLv",tolua_Script_Script_GetTechLv00);
   tolua_function(tolua_S,"GetFactionName",tolua_Script_Script_GetFactionName00);
   tolua_function(tolua_S,"GetFacBaseBattle",tolua_Script_Script_GetFacBaseBattle00);
   tolua_function(tolua_S,"SendBaseInfo",tolua_Script_Script_SendBaseInfo00);
   tolua_function(tolua_S,"GetFacLevel",tolua_Script_Script_GetFacLevel00);
   tolua_function(tolua_S,"GetFacLevel",tolua_Script_Script_GetFacLevel01);
   tolua_function(tolua_S,"GetMemContribute",tolua_Script_Script_GetMemContribute00);
   tolua_function(tolua_S,"GetMemContribute",tolua_Script_Script_GetMemContribute01);
   tolua_function(tolua_S,"OpenCreateFactionUi",tolua_Script_Script_OpenCreateFactionUi00);
   tolua_function(tolua_S,"KickRegionPlayer",tolua_Script_Script_KickRegionPlayer00);
   tolua_function(tolua_S,"KickAllPlayer",tolua_Script_Script_KickAllPlayer00);
   tolua_function(tolua_S,"KickPlayer",tolua_Script_Script_KickPlayer00);
   tolua_function(tolua_S,"ListOnlineGM",tolua_Script_Script_ListOnlineGM00);
   tolua_function(tolua_S,"ListSelencePlayer",tolua_Script_Script_ListSelencePlayer00);
   tolua_function(tolua_S,"BanPlayer",tolua_Script_Script_BanPlayer00);
   tolua_function(tolua_S,"GetTextRes",tolua_Script_Script_GetTextRes00);
   tolua_function(tolua_S,"RollRegionInfo",tolua_Script_Script_RollRegionInfo00);
   tolua_function(tolua_S,"RollWorldInfo",tolua_Script_Script_RollWorldInfo00);
   tolua_function(tolua_S,"ChangePlayerRegionEx",tolua_Script_Script_ChangePlayerRegionEx00);
   tolua_function(tolua_S,"SetPlayerAttr",tolua_Script_Script_SetPlayerAttr00);
   tolua_function(tolua_S,"SetPlayerAttr",tolua_Script_Script_SetPlayerAttr01);
   tolua_function(tolua_S,"GetPlayerAttr",tolua_Script_Script_GetPlayerAttr00);
   tolua_function(tolua_S,"GetPlayerAttr",tolua_Script_Script_GetPlayerAttr01);
   tolua_function(tolua_S,"ChangePlayerAttr",tolua_Script_Script_ChangePlayerAttr00);
   tolua_function(tolua_S,"ChangePlayerAttr",tolua_Script_Script_ChangePlayerAttr01);
   tolua_function(tolua_S,"GetPlayerName",tolua_Script_Script_GetPlayerName00);
   tolua_function(tolua_S,"GetPlayerName",tolua_Script_Script_GetPlayerName01);
   tolua_function(tolua_S,"GetPlayerID",tolua_Script_Script_GetPlayerID00);
   tolua_function(tolua_S,"ChangePlayerMoney",tolua_Script_Script_ChangePlayerMoney00);
   tolua_function(tolua_S,"ChangePlayerMoney",tolua_Script_Script_ChangePlayerMoney01);
   tolua_function(tolua_S,"GetPlayerMoney",tolua_Script_Script_GetPlayerMoney00);
   tolua_function(tolua_S,"GetPlayerMoney",tolua_Script_Script_GetPlayerMoney01);
   tolua_function(tolua_S,"SetPlayerMoney",tolua_Script_Script_SetPlayerMoney00);
   tolua_function(tolua_S,"SetPlayerMoney",tolua_Script_Script_SetPlayerMoney01);
   tolua_function(tolua_S,"ChangePlayerTicket",tolua_Script_Script_ChangePlayerTicket00);
   tolua_function(tolua_S,"ChangePlayerTicket",tolua_Script_Script_ChangePlayerTicket01);
   tolua_function(tolua_S,"GetPlayerTicket",tolua_Script_Script_GetPlayerTicket00);
   tolua_function(tolua_S,"GetPlayerTicket",tolua_Script_Script_GetPlayerTicket01);
   tolua_function(tolua_S,"SetPlayerTicket",tolua_Script_Script_SetPlayerTicket00);
   tolua_function(tolua_S,"SetPlayerTicket",tolua_Script_Script_SetPlayerTicket01);
   tolua_function(tolua_S,"ChangeBaseSpeed",tolua_Script_Script_ChangeBaseSpeed00);
   tolua_function(tolua_S,"ChangeBaseSpeed",tolua_Script_Script_ChangeBaseSpeed01);
   tolua_function(tolua_S,"SetPlayerPos",tolua_Script_Script_SetPlayerPos00);
   tolua_function(tolua_S,"SetPlayerPos",tolua_Script_Script_SetPlayerPos01);
   tolua_function(tolua_S,"SetPlayerDir",tolua_Script_Script_SetPlayerDir00);
   tolua_function(tolua_S,"SetPlayerDir",tolua_Script_Script_SetPlayerDir01);
   tolua_function(tolua_S,"GetCountry",tolua_Script_Script_GetCountry00);
   tolua_function(tolua_S,"GetCountry",tolua_Script_Script_GetCountry01);
   tolua_function(tolua_S,"ChangeCountry",tolua_Script_Script_ChangeCountry00);
   tolua_function(tolua_S,"ChangeCountry",tolua_Script_Script_ChangeCountry01);
   tolua_function(tolua_S,"GetContribute",tolua_Script_Script_GetContribute00);
   tolua_function(tolua_S,"GetContribute",tolua_Script_Script_GetContribute01);
   tolua_function(tolua_S,"SetContribute",tolua_Script_Script_SetContribute00);
   tolua_function(tolua_S,"SetContribute",tolua_Script_Script_SetContribute01);
   tolua_function(tolua_S,"IsCharged",tolua_Script_Script_IsCharged00);
   tolua_function(tolua_S,"IsCharged",tolua_Script_Script_IsCharged01);
   tolua_function(tolua_S,"SetCharged",tolua_Script_Script_SetCharged00);
   tolua_function(tolua_S,"SetCharged",tolua_Script_Script_SetCharged01);
   tolua_function(tolua_S,"CheckOccuLvl",tolua_Script_Script_CheckOccuLvl00);
   tolua_function(tolua_S,"Relive",tolua_Script_Script_Relive00);
   tolua_function(tolua_S,"ChangeOccu",tolua_Script_Script_ChangeOccu00);
   tolua_function(tolua_S,"AutoMove",tolua_Script_Script_AutoMove00);
   tolua_function(tolua_S,"MovePlayer",tolua_Script_Script_MovePlayer00);
   tolua_function(tolua_S,"GodMode",tolua_Script_Script_GodMode00);
   tolua_function(tolua_S,"IsGM",tolua_Script_Script_IsGM00);
   tolua_function(tolua_S,"DurancePlayer",tolua_Script_Script_DurancePlayer00);
   tolua_function(tolua_S,"GetServer",tolua_Script_Script_GetServer00);
   tolua_function(tolua_S,"GetOtherOneInMarriageTeam",tolua_Script_Script_GetOtherOneInMarriageTeam00);
   tolua_function(tolua_S,"GetOtherOneInMarriageTeam",tolua_Script_Script_GetOtherOneInMarriageTeam01);
   tolua_function(tolua_S,"GetPlayerMarriageStep",tolua_Script_Script_GetPlayerMarriageStep00);
   tolua_function(tolua_S,"GetPlayerMarriageStep",tolua_Script_Script_GetPlayerMarriageStep01);
   tolua_function(tolua_S,"SetPlayerMarriageStep",tolua_Script_Script_SetPlayerMarriageStep00);
   tolua_function(tolua_S,"SetPlayerMarriageStep",tolua_Script_Script_SetPlayerMarriageStep01);
   tolua_function(tolua_S,"SetPlayerSpouse",tolua_Script_Script_SetPlayerSpouse00);
   tolua_function(tolua_S,"SetPlayerSpouse",tolua_Script_Script_SetPlayerSpouse01);
   tolua_function(tolua_S,"DelPlayerSpouse",tolua_Script_Script_DelPlayerSpouse00);
   tolua_function(tolua_S,"DelPlayerSpouse",tolua_Script_Script_DelPlayerSpouse01);
   tolua_function(tolua_S,"GetPlayerSpouse",tolua_Script_Script_GetPlayerSpouse00);
   tolua_function(tolua_S,"GetPlayerSpouse",tolua_Script_Script_GetPlayerSpouse01);
   tolua_function(tolua_S,"GetPlayerSpouseName",tolua_Script_Script_GetPlayerSpouseName00);
   tolua_function(tolua_S,"GetPlayerSpouseName",tolua_Script_Script_GetPlayerSpouseName01);
   tolua_function(tolua_S,"GetElementProperty",tolua_Script_Script_GetElementProperty00);
   tolua_function(tolua_S,"GetElementProperty",tolua_Script_Script_GetElementProperty01);
   tolua_function(tolua_S,"SetElementProperty",tolua_Script_Script_SetElementProperty00);
   tolua_function(tolua_S,"SetElementProperty",tolua_Script_Script_SetElementProperty01);
   tolua_function(tolua_S,"StudyGenius",tolua_Script_Script_StudyGenius00);
   tolua_function(tolua_S,"StudyGenius",tolua_Script_Script_StudyGenius01);
   tolua_function(tolua_S,"RemoveGenius",tolua_Script_Script_RemoveGenius00);
   tolua_function(tolua_S,"RemoveGenius",tolua_Script_Script_RemoveGenius01);
   tolua_function(tolua_S,"SetAttr",tolua_Script_Script_SetAttr00);
   tolua_function(tolua_S,"GetAttr",tolua_Script_Script_GetAttr00);
   tolua_function(tolua_S,"ChangeAttr",tolua_Script_Script_ChangeAttr00);
   tolua_function(tolua_S,"PlayMotion",tolua_Script_Script_PlayMotion00);
   tolua_function(tolua_S,"ShapeRunScript",tolua_Script_Script_ShapeRunScript00);
   tolua_function(tolua_S,"GetAroundPlayerNum",tolua_Script_Script_GetAroundPlayerNum00);
   tolua_function(tolua_S,"GetAroundMonsterNum",tolua_Script_Script_GetAroundMonsterNum00);
   tolua_function(tolua_S,"GetAroundSameMonsterNum",tolua_Script_Script_GetAroundSameMonsterNum00);
   tolua_function(tolua_S,"SetDelTime",tolua_Script_Script_SetDelTime00);
   tolua_function(tolua_S,"ChangeMoveSpeed",tolua_Script_Script_ChangeMoveSpeed00);
   tolua_function(tolua_S,"SetMoveSpeed",tolua_Script_Script_SetMoveSpeed00);
   tolua_function(tolua_S,"GetCurrentSpeed",tolua_Script_Script_GetCurrentSpeed00);
   tolua_function(tolua_S,"IsShapeValid",tolua_Script_Script_IsShapeValid00);
   tolua_function(tolua_S,"CreatePhase",tolua_Script_Script_CreatePhase00);
   tolua_function(tolua_S,"ActivePhase",tolua_Script_Script_ActivePhase00);
   tolua_function(tolua_S,"GetPhasePoint",tolua_Script_Script_GetPhasePoint00);
   tolua_function(tolua_S,"GetPhasePoint",tolua_Script_Script_GetPhasePoint01);
   tolua_function(tolua_S,"CreatePhaseMonster",tolua_Script_Script_CreatePhaseMonster00);
   tolua_function(tolua_S,"OnPhaseOper",tolua_Script_Script_OnPhaseOper00);
   tolua_function(tolua_S,"OnPhaseEvent",tolua_Script_Script_OnPhaseEvent00);
   tolua_function(tolua_S,"SetPhaseEnd",tolua_Script_Script_SetPhaseEnd00);
   tolua_function(tolua_S,"SetPhaseEndScriptFile",tolua_Script_Script_SetPhaseEndScriptFile00);
   tolua_function(tolua_S,"ClosePhase",tolua_Script_Script_ClosePhase00);
   tolua_function(tolua_S,"ClosePhase",tolua_Script_Script_ClosePhase01);
   tolua_function(tolua_S,"GetPhaseID",tolua_Script_Script_GetPhaseID00);
   tolua_function(tolua_S,"GetShapePhaseID",tolua_Script_Script_GetShapePhaseID00);
   tolua_function(tolua_S,"GetCostTime",tolua_Script_Script_GetCostTime00);
   tolua_function(tolua_S,"CancelPhaseTimer",tolua_Script_Script_CancelPhaseTimer00);
   tolua_function(tolua_S,"PostPhaseMsg",tolua_Script_Script_PostPhaseMsg00);
   tolua_function(tolua_S,"CreatePet",tolua_Script_Script_CreatePet00);
   tolua_function(tolua_S,"DelPet",tolua_Script_Script_DelPet00);
   tolua_function(tolua_S,"SetPetAttr",tolua_Script_Script_SetPetAttr00);
   tolua_function(tolua_S,"GetPetAttr",tolua_Script_Script_GetPetAttr00);
   tolua_function(tolua_S,"ActivePet",tolua_Script_Script_ActivePet00);
   tolua_function(tolua_S,"AddPetSkill",tolua_Script_Script_AddPetSkill00);
   tolua_function(tolua_S,"GetFocusPetGUID",tolua_Script_Script_GetFocusPetGUID00);
   tolua_function(tolua_S,"GetPetGrade",tolua_Script_Script_GetPetGrade00);
   tolua_function(tolua_S,"GetPetDAfterCnt",tolua_Script_Script_GetPetDAfterCnt00);
   tolua_function(tolua_S,"ResetPetTalent",tolua_Script_Script_ResetPetTalent00);
   tolua_function(tolua_S,"ResetPetDAfterCnt",tolua_Script_Script_ResetPetDAfterCnt00);
   tolua_function(tolua_S,"UpgradePetDAfterVal",tolua_Script_Script_UpgradePetDAfterVal00);
   tolua_function(tolua_S,"ResetPetSavvy",tolua_Script_Script_ResetPetSavvy00);
   tolua_function(tolua_S,"GetMaxServantNum",tolua_Script_Script_GetMaxServantNum00);
   tolua_function(tolua_S,"IncPetExp",tolua_Script_Script_IncPetExp00);
   tolua_function(tolua_S,"IncPetDExp",tolua_Script_Script_IncPetDExp00);
   tolua_function(tolua_S,"PetLevUp",tolua_Script_Script_PetLevUp00);
   tolua_function(tolua_S,"ContinueAI",tolua_Script_Script_ContinueAI00);
   tolua_function(tolua_S,"MoveToCell",tolua_Script_Script_MoveToCell00);
   tolua_function(tolua_S,"RandomMove",tolua_Script_Script_RandomMove00);
   tolua_function(tolua_S,"MoveByStep",tolua_Script_Script_MoveByStep00);
   tolua_function(tolua_S,"FollowTarget",tolua_Script_Script_FollowTarget00);
   tolua_function(tolua_S,"GetTarget",tolua_Script_Script_GetTarget00);
   tolua_function(tolua_S,"ReceiveMail",tolua_Script_Script_ReceiveMail00);
   tolua_function(tolua_S,"SendSysMail",tolua_Script_Script_SendSysMail00);
   tolua_function(tolua_S,"ReceiveSysMail",tolua_Script_Script_ReceiveSysMail00);
   tolua_function(tolua_S,"GetTauntValue",tolua_Script_Script_GetTauntValue00);
   tolua_function(tolua_S,"SetTauntValue",tolua_Script_Script_SetTauntValue00);
   tolua_function(tolua_S,"GetTauntValue",tolua_Script_Script_GetTauntValue01);
   tolua_function(tolua_S,"GetRandObject",tolua_Script_Script_GetRandObject00);
   tolua_function(tolua_S,"ObjectRunScript",tolua_Script_Script_ObjectRunScript00);
   tolua_function(tolua_S,"ForceToFirst",tolua_Script_Script_ForceToFirst00);
   tolua_function(tolua_S,"GetTauntSize",tolua_Script_Script_GetTauntSize00);
   tolua_function(tolua_S,"TauntEvents",tolua_Script_Script_TauntEvents00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_Script (lua_State* tolua_S) {
 return tolua_Script_open(tolua_S);
};
#endif

