//=============================================================================
/**
 *  file: DisplayInfoBind.cpp
 *
 *  Brief:为CTestInfo类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
//#include <afxcmn.h>

#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/TestInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int display_info_get_game_client_tree(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CDisplayInfo",0,&err) )
	{
		tolua_error(L,"#ferror in function 'display_info_get_game_client_tree'.",&err);
		return 0;
	}
#endif

	CTestInfo* self = (CTestInfo*)tolua_tousertype(L,1,0);
	CTreeGameClient* pTree = self->GetTreeGameClient();
	tolua_pushusertype(L,(void*)pTree,"CTreeGameClient");
	return 1;
}

int display_info_get_select_game_client(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CDisplayInfo",0,&err) )
	{
		tolua_error(L,"#ferror in function 'display_info_get_select_game_client'.",&err);
		return 0;
	}
#endif

	CTestInfo* self = (CTestInfo*)tolua_tousertype(L,1,0);
	CGameClient* pGameClient = self->GetSelGameClient();
	tolua_pushusertype(L,(void*)pGameClient,"CGameClient");
	return 1;
}

int display_info_output_game_info(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CDisplayInfo",0,&err) ||
		!tolua_isstring(L,2,0,&err) )
	{
		tolua_error(L,"#ferror in function 'display_info_output_game_info'.",&err);
		return 0;
	}
#endif
	CTestInfo* self = (CTestInfo*)tolua_tousertype(L,1,0);
	const char* strInfo = (char*)tolua_tostring(L,2,0);
	self->OutputGameInfo(strInfo);
	return 0;
}

void display_info_reg_types (lua_State* L)
{
	tolua_usertype(L,"CDisplayInfo");
}

//打开Dialog库
int  display_info_open(lua_State* L)
{
	tolua_open(L);
	display_info_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"DisplayInfo","CDisplayInfo","",NULL);
			tolua_beginmodule(L,"DisplayInfo");
			{
				tolua_variable(L,"client_tree",					display_info_get_game_client_tree,		NULL);
				tolua_variable(L,"select_client",				display_info_get_select_game_client,	NULL);
				tolua_function(L,"output_game_info",			display_info_output_game_info);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}