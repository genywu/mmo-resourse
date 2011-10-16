//=============================================================================
/**
 *  file: MonsterBind.cpp
 *
 *  Brief:为CMonster类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================
#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "..\GameTest\PlayerTrade.h"
#include "..\App\Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_player_trade(lua_State* L)
{
	CPlayerTrade *self = (CPlayerTrade*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int player_trade_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_trade_new'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* ret = new CPlayerTrade();
	tolua_pushusertype(L,(void*)ret,"CPlayerTrade");
	return 1;
}

int player_trade_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_trade_new_local'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* ret = new CPlayerTrade();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CPlayerTrade");
	return 1;
}

int player_trade_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_trade_delete'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* self = (CPlayerTrade*)tolua_tousertype(L,1,0);
	delete self;
	return 0;
}


int player_trade_open_trade_page(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err)||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnumber(L,4,0,&err)||
		!tolua_isusertype(L,5,"CGUID",0,&err)||
		!tolua_isnumber(L,6,0,&err)||
		!tolua_isnoobj(L,7,&err) )
	{
		tolua_error(L,"#ferror in function 'player_update_property'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* pTrade = (CPlayerTrade*)tolua_tousertype(L,1,0);
	const CGUID SessionID = *(CGUID*)tolua_tousertype(L,2,0);
	const CGUID MyID	= *(CGUID*)tolua_tousertype(L,3,0);
	long lMPlugID = (long)tolua_tonumber(L,4,0);
	const CGUID YouID = *(CGUID*)tolua_tousertype(L,5,0);
	long lYPlugID = (long)tolua_tonumber(L,6,0);
	pTrade->OpenTradePage(SessionID,MyID,lMPlugID,YouID,lYPlugID);
	return 0;
}

int player_trade_open_trade_dialog(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isusertype(L,2,"CGameClient",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_open_trade_dialog'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* selfTrade = (CPlayerTrade*)tolua_tousertype(L,1,0);
	CGameClient* selfClient = (CGameClient*)tolua_tousertype(L,2,0);
	long bRet = (long)tolua_tonumber(L,3,0);
	if(bRet ==1)
	{
		selfTrade->OpenTradeDialog(selfClient);
	}
	return 0;
}

int player_trade_get_my_plug_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isusertype(L,2,"CGameClient",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_trade_get_my_plug_id'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* selfTrade = (CPlayerTrade*)tolua_tousertype(L,1,0);
	long lMyPulgID = (long)selfTrade->GetMyPlugID();
	tolua_pushnumber(L,lMyPulgID);
	return 1;
}

int player_trade_get_you_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isusertype(L,2,"CGameClient",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_open_trade_dialog'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* selfTrade = (CPlayerTrade*)tolua_tousertype(L,1,0);
	CGUID guid = selfTrade->GetYouID();
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

int player_trade_get_session_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CPlayerTrade",0,&err) ||
		!tolua_isusertype(L,2,"CGameClient",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_trade_get_session_id'.",&err);
		return 0;
	}
#endif
	CPlayerTrade* selfTrade = (CPlayerTrade*)tolua_tousertype(L,1,0);
	const CGUID& guid = selfTrade->GetSessionID();
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

void player_trade_open_reg_types (lua_State* L)
{
	tolua_usertype(L,"CPlayerTrade");
}

//打开baseobject库
int	player_trade_open(lua_State* L)
{
	tolua_open(L);
	player_trade_open_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"PlayerTrade","CPlayerTrade","",collect_player_trade);
			tolua_beginmodule(L,"PlayerTrade");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",player_trade_new);
				tolua_function(L,"new_local",player_trade_new_local);
				tolua_function(L,"delete",player_trade_delete);

				tolua_function(L,"open_trade_page",player_trade_open_trade_page);
				tolua_function(L,"open_trade_dialog",player_trade_open_trade_dialog);

				tolua_function(L,"player_trade_get_my_plug_id",player_trade_get_my_plug_id);
				tolua_function(L,"player_trade_get_you_id",player_trade_get_you_id);
				tolua_function(L,"player_trade_get_session_id",player_trade_get_session_id);
				//tolua_function(L,"player_trade_get_goods_id",player_trade_get_goods_id);
				//tolua_function(L,"player_trade_get_goods_guid",player_trade_get_goods_guid);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}