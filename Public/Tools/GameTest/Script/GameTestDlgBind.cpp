//=============================================================================
/**
 *  file: GameTestDialog.cpp
 *
 *  Brief:为CGameTestDlg类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================


#include "stdafx.h"

#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/GameTestDlg.h"
#include "../App/AutoDriveClient/AutoDriveClient.h"
#include "../Script/MapScriptSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int dialog_get_display_info(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_get_display_info'.",&err);
		return 0;
	}
#endif
	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
	CTestInfo* pInfo = self->GetTestInfo();
	tolua_pushusertype(L,(void*)pInfo,"CDisplayInfo");
	return 1;
}

int dialog_get_test_script_dlg(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_get_test_script_dlg'.",&err);
		return 0;
	}
#endif
	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
	CTestScript* pScirpt = self->GetTestScript();
	tolua_pushusertype(L,(void*)pScirpt,"CTestScript");
	return 1;
}

int dialog_create_client(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_create_client'.",&err);
		return 0;
	}
#endif
	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
	long socketid = (long)tolua_tonumber(L,2,0);
	self->CreateClient(socketid);
	return 0;
}

int dialog_get_client(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_get_client'.",&err);
		return 0;
	}
#endif
	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
	long socketid = (long)tolua_tonumber(L,2,0);
	void* client = self->GetGameClient(socketid);
	if ( client )
		tolua_pushusertype(L,client,"CGameClient");
	else
		tolua_pushboolean(L,false);
		
	return 1;
}

int dialog_delete_client(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_delete_client'.",&err);
		return 0;
	}
#endif
	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
	long socketid = (long)tolua_tonumber(L,2,0);
	self->DeleteGameClient(socketid);
	return 0;
}

//
//Auto Drive Client 相关绑定
// by MartySa 2009.2.1
// int dialog_get_driveclient(lua_State *L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
// 		!tolua_isstring(L,2,0,&err) ||
// 		!tolua_isnoobj(L,3,&err) )
// 	{
// 		tolua_error(L,"#ferror in function 'dialog_get_driveclient'.",&err);
// 		return 0;
// 	}
// #endif
// 	CGameTestDlg *self = (CGameTestDlg*)tolua_tousertype(L,1,0);
// 	CDriveClient *driverclient = self->GetDriveClientManager();
// 	if (driverclient)
// 	{
// 		tolua_pushusertype(L, (void*)driverclient, "CDriveClient");
// 	}
// 	else
// 	{
// 		tolua_pushboolean(L, false);
// 	}
// 	return 1;
// }

//int dialog_create_drivegameclient(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(!tolua_isusertype(L,1,"CDriveClient",0,&err) ||
//		!tolua_isstring(L,2,0,&err) ||
//		!tolua_isnoobj(L,3,&err) )
//	{
//		tolua_error(L,"#ferror in function 'dialog_create_drivegameclient'.",&err);
//		return 0;
//	}
//#endif
//	CDriveClient *self = (CDriveClient*)tolua_tousertype(L, 1, 0);
//	string strCDkey    = (string)tolua_tostring(L, 2, 0);
//	//self->_CreateGameClient(strCDkey);
//	return 0;
//}

int dialog_get_drivegameclient(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
		!tolua_isusertype(L,2,"CDriveClient",0,&err) ||
		!tolua_isstring(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_get_drivegameclient'.",&err);
		return 0;
	}
#endif
	CDriveClient *self		= (CDriveClient*)tolua_tousertype(L,2,0);
	string strCDkey         = (string)tolua_tostring(L,3,0);
	CGameClient *gameclient = self->_FindGameClient(strCDkey);
	if (gameclient != NULL)
	{
		tolua_pushusertype(L, (void*)gameclient, "CGameClient");
	}
	else
	{
		tolua_pushboolean(L, false);
	}
	return 1;
}

int dialog_delete_drivegameclient(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CDriveClient",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_delete_drivegameclient'.",&err);
		return 0;
	}
#endif
	CDriveClient *self = (CDriveClient*)tolua_tousertype(L,1,0);
	string strCDkey	   = (string)tolua_tostring(L,2,0);
	self->_DeleteGameClient(strCDkey);
	return 0;
}

int dialog_get_mapfile_manager(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGameTestDlg",0,&err) ||
		!tolua_isusertype(L,2,"CDriveClient",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'dialog_get_mapfile_manager'.",&err);
		return 0;
	}
#endif
	CDriveClient *self = (CDriveClient*)tolua_tousertype(L,2,0);
	CMemServer	 *manager = self->GetMemServerManager();
	if ( manager )
	{
		tolua_pushusertype(L, (void*)manager, "CMemServer");
	}
	else
	{
		tolua_pushboolean(L, false);
	}
	return 1;
}

//注册类别
void dialog_reg_types (lua_State* L)
{
	tolua_usertype(L, "CGameTestDlg");
	tolua_usertype(L, "CDriveClient");
	tolua_usertype(L, "CGameClient");
	tolua_usertype(L, "CMemServer");
}

//打开Dialog库
int  dialog_open(lua_State* L)
{
	tolua_open(L);
	dialog_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Dialog","CGameTestDlg","",NULL);
			tolua_beginmodule(L,"Dialog");
			{
				tolua_variable(L,"display_info",			dialog_get_display_info,		NULL);
				tolua_variable(L,"scirpt_dlg",				dialog_get_test_script_dlg,		NULL);

				tolua_function(L,"create_client",			dialog_create_client);
				tolua_function(L,"get_client",				dialog_get_client);
				tolua_function(L,"delete_client",			dialog_delete_client);

				//for auto drive
			//	tolua_variable(L,"get_driveclient",			dialog_get_driveclient,		NULL);
				//tolua_function(L,"create_drivegameclient",	dialog_create_drivegameclient);
				tolua_function(L,"get_drivegameclient",		dialog_get_drivegameclient);
				tolua_function(L,"delete_drivegameclient",	dialog_delete_drivegameclient);
				tolua_function(L,"get_mapfile_manager",	    dialog_get_mapfile_manager);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}