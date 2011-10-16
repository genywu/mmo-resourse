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
#include "..\GameTest\PlayerInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_playerinfo(lua_State* L)
{
	CPlayerInfo *self = (CPlayerInfo*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int playerinfo_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayerInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'playerinfo_new'.",&err);
		return 0;
	}
#endif
	CPlayerInfo* ret = new CPlayerInfo();
	tolua_pushusertype(L,(void*)ret,"CPlayerInfo");
	return 1;
}

int playerinfo_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayerInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'playerinfo_new_local'.",&err);
		return 0;
	}
#endif
	CPlayerInfo* ret = new CPlayerInfo();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CPlayerInfo");
	return 1;
}

int playerinfo_get_packetid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayerInfo",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'playerinfo_get_packetid'.",&err);
		return 0;
	}
#endif
	CPlayerInfo* self = (CPlayerInfo*)tolua_tousertype(L,1,0);
	long iMark = (long)tolua_tonumber(L,2,0);
	long lPacketid = self->GetPacketId(iMark);
	tolua_pushnumber(L,lPacketid);
	return 1;
}

int playerinfo_get_goods_pos(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayerInfo",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isusertype(L,3,"CGameClient",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'playerinfo_get_goods_pos'.",&err);
		return 0;
	}
#endif
	CPlayerInfo* self = (CPlayerInfo*)tolua_tousertype(L,1,0);
	long iMark = (long)tolua_tonumber(L,2,0);
	CGameClient* pSelClient = (CGameClient*)tolua_tousertype(L,3,0);
	BYTE lPos = self->GetGoodsPos(iMark,pSelClient);
	tolua_pushnumber(L,lPos);
	return 1;
}

void playerinfo_reg_types (lua_State* L)
{
	tolua_usertype(L,"CPlayerInfo");
}

//打开baseobject库
int  playerinfo_open(lua_State* L)
{
	tolua_open(L);
	playerinfo_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"PlayerInfo","CPlayerInfo","",collect_playerinfo);
			tolua_beginmodule(L,"PlayerInfo");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",playerinfo_new);
				tolua_function(L,"new_local",playerinfo_new_local);
				tolua_function(L,"get_packet_id",playerinfo_get_packetid);
				tolua_function(L,"get_goods_pos",playerinfo_get_goods_pos);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;

}