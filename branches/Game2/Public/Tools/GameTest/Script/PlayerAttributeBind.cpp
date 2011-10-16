#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/TestInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_player_attribute(lua_State* L)
{
	//CGUID* guid = (CGUID*)tolua_touserdata(L,1,0);
	//delete guid;
	return 0;
}	

int player_AttributeTo_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertable(L,1,"CTestInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_AttributeTo_guid'.",&err);
		return 0;
	}
#endif 
	CTestInfo* playerAttribute = new CTestInfo();
	const CGUID* guid = playerAttribute->GetPlayerAttributeToGUID();
	tolua_pushusertype(L,(void*)guid,"CGUID");
	return 1;
}

int player_AttributeTo_socketid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CTestInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_AttributeTo_socketid'.",&err);
		return 0;
	}
#endif 
	CTestInfo* playerAttribute = new CTestInfo();
	long lSocketID = (long)playerAttribute->GetPlayerAttributeToSocketID();
	tolua_pushnumber(L,(long)lSocketID);
	return 1;
}

int player_AttributeTo_name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CTestInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_AttributeTo_name'.",&err);
		return 0;
	}
#endif 
	CTestInfo* playerAttribute = new CTestInfo();
	const char* name = playerAttribute->GetPlayerAttributeToName();
	tolua_pushstring(L,name);
	return 1;
}

void player_attribute_types(lua_State* L)
{
	tolua_usertype(L,"CTestInfo");
}

int player_Atb_open(lua_State* L)
{
	tolua_open(L);
	player_attribute_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"PlayerAtb","CTestInfo","",collect_player_attribute);
			tolua_beginmodule(L,"PlayerAtb");
			{
				//注册guid类的lua函数	
				tolua_function(L,"get_playerAtbTo_guid",player_AttributeTo_guid);
				tolua_function(L,"get_playerAtbTo_name",player_AttributeTo_name);
				tolua_function(L,"get_playerAtbTo_socketid",player_AttributeTo_socketid);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}