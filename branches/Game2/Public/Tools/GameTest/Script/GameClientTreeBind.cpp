//=============================================================================
/**
 *  file: GameClientTreeBind.cpp
 *
 *  Brief:为CTreeGameClient类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-7-25
 */
//=============================================================================


#include "stdafx.h"
#include <afxcmn.h>

#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/TreeGameClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int game_client_tree_insert_client_item(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CTreeGameClient",0,&err) ||
		!tolua_isusertype(L,2,"CGameClient",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'game_client_tree_insert_client_item'.",&err);
		return 0;
	}
#endif
//	return 0;
	CTreeGameClient *self = (CTreeGameClient*)tolua_tousertype(L,1,0);
	CGameClient* pGameClient = (CGameClient*)tolua_tousertype(L,2,0);
	self->InsertClientItem(pGameClient);
	return 0;
}

int game_client_tree_delete_client_item(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CTreeGameClient",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'game_client_tree_delete_client_item'.",&err);
		return 0;
	}
#endif
//	return 0;
	CTreeGameClient *self = (CTreeGameClient*)tolua_tousertype(L,1,0);
	long lClientID = (long)tolua_tonumber(L,2,0);
	self->DelClientItem(lClientID);
	return 0;
}

void game_client_tree_reg_types (lua_State* L)
{
	tolua_usertype(L,"CTreeGameClient");
}

//打开Dialog库
int  game_client_tree_open(lua_State* L)
{
	tolua_open(L);
	game_client_tree_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"GameClientTree","CTreeGameClient","",NULL);
			tolua_beginmodule(L,"GameClientTree");
			{
				tolua_function(L,"insert_client_item",	game_client_tree_insert_client_item);
				tolua_function(L,"delete_client_item",	game_client_tree_delete_client_item);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}