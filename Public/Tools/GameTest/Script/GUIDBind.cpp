#include "stdafx.h"

#include "../Net/Message.h"
//#include "../../nets/netGT/message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_game_guID(lua_State* L)
{
	CGUID* guid = (CGUID*)tolua_touserdata(L,1,0);
	SAFE_DELETE(guid);
	return 0;
}

int game_guID_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",1,&err) ||
		!tolua_isnoobj(L,2,&err) )
		
	{
		tolua_error(L,"#ferror in function 'guId_new'.",&err);
		return 0;
	}
#endif 
	CGUID* guid = new CGUID();
	tolua_pushusertype(L,(void*)guid,"CGUID");
	return 1;
}

int game_guID_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) || 
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'guID_new_local'.",&err);
		return 0;
	}
#endif
	CGUID* guid = new CGUID();
	tolua_pushusertype_and_takeownership(L,(void*)guid,"CGUID");
	return 1;
}

int game_guID_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGUID",0,&err) ||
	//	!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'guID_delete'.",&err);
		return 0;
	}
#endif
	CGUID* guid = (CGUID*)tolua_tousertype(L,1,0);
	delete guid;
	return 0;
}

int game_guID_Initialize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'guID_Initialize'.",&err);
		return 0;
	}
#endif
	CGUID* guid = (CGUID*)tolua_tousertype(L,2,0);
	guid->Initialize();
	return 1;
}

int game_create_guID(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CGUID",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'create_guID'.",&err);
		return 0;
	}			
#endif
	CGUID* luaGuid = (CGUID*)tolua_tousertype(L,1,0);;
	CGUID::CreateGUID(*luaGuid);
	tolua_pushusertype(L,(void*)luaGuid,"CGUID");
	return 1;
}

int game_guid_tostring(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'game_guid_tostring'.",&err);
		return 0;
	}
#endif
	CGUID* guid = (CGUID*)tolua_tousertype(L,2,0);;
	static char str[50]="";
	(*guid).tostring(str);
	tolua_pushstring(L,str);
	return 1;
}

int game_guid_is_invalid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'game_guid_is_invalid'.",&err);
		return 0;
	}
#endif
	CGUID* guid = (CGUID*)tolua_tousertype(L,2,0);
	bool ret = false;
	if((*guid) == CGUID::GUID_INVALID)
	{
		ret = true;
	}
	tolua_pushboolean(L,ret);
	return 1;
}

int game_compare_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isusertype(L,3,"CGUID",0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'game_compare_guid'.",&err);
		return 0;
	}
#endif
	CGUID guid1 = *(CGUID*)tolua_tousertype(L,2,0);
	CGUID guid2 = *(CGUID*)tolua_tousertype(L,3,0);
	bool b = false;
	if( guid1==guid2 )
	{
		b = true;
	}
	tolua_pushboolean(L,b);
	return 1;
}


int guid_memcpy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isusertype(L,3,"CGUID",0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'game_compare_guid'.",&err);
		return 0;
	}
#endif
	CGUID* guid1 = (CGUID*)tolua_tousertype(L,2,0);
	CGUID* guid2 = (CGUID*)tolua_tousertype(L,3,0);
	*guid1 = *guid2;
	tolua_pushusertype(L,(void*)guid1,"CGUID");
	return 0;
}

int game_guid_get_invalid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGUID",0,&err)||
//		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'game_guid_get_invalid'.",&err);
		return 0;
	}
#endif
	tolua_pushusertype(L,(void*)&(CGUID::GUID_INVALID),"CGUID");
	return 1;
}

void guID_reg_types(lua_State* L)
{
	tolua_usertype(L,"CGUID");
}

int  guID_open(lua_State* L)
{
	tolua_open(L);
	guID_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Guid","CGUID","",collect_game_guID);
			tolua_beginmodule(L,"Guid");
			{
				tolua_function(L,"GUID_INVALID",	game_guid_get_invalid);
				//×¢²águidÀàµÄluaº¯Êý	
				tolua_function(L,"new",			game_guID_new);
				tolua_function(L,"new_local",	game_guID_new_local);	
				tolua_function(L,".call",		game_guID_new_local);
				tolua_function(L,"delete",		game_guID_delete);
				tolua_function(L,"guID_Initialize",		game_guID_Initialize);
				tolua_function(L,"create",				game_create_guID);
				tolua_function(L,"guid_tostring",		game_guid_tostring);

				tolua_function(L,"guid_is_invalid",		game_guid_is_invalid);
				tolua_function(L,"compare_guid",		game_compare_guid);
				tolua_function(L,"guid_memcpy",			guid_memcpy);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}