//=============================================================================
/**
 *  file: ToolsBind.cpp
 *
 *  Brief:为MonsterInfo函数产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "..\GameTest\MonsterInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int monsterinfo_get_monster_id(lua_State* L)
             {
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
	//	!tolua_isusertype(L,2,"CMessage",0,&err)||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_monster_id'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
//	CMessage* SelRow = (CMessage*)tolua_tousertype(L,2,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,3,0);
	*guid = self->GetMonsterId(SelRow);
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}
int monsterinfo_get_player_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_player_id'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,3,0);
	*guid = self->GetPlayerId(SelRow);
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

int monsterinfo_get_npc_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_npc_id'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,3,0);
	*guid = self->GetNpcId(SelRow);
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

int monsterinfo_get_monster_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_monster_type'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
	tolua_pushnumber(L,self->GetMonsterType(SelRow));
	return 1;
}

int monsterinfo_get_npc_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_npc_type'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
	tolua_pushnumber(L,self->GetNpcType(SelRow));
	return 1;
}

int monsterinfo_get_player_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonsterInfo",0,&err)||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monsterinfo_get_player_type'.",&err);
		return 0;
	}
#endif
	CMonsterInfo* self = (CMonsterInfo*)tolua_tousertype(L,1,0);
	long SelRow = (long)tolua_tonumber(L,2,0);
	tolua_pushnumber(L,self->GetPlayerType(SelRow));
	return 1;
}

void monsterinfo_reg_types (lua_State* L)
{
	tolua_usertype(L,"CMonsterInfo");
}

//打开baseobject库
int monsterinfo_open(lua_State* L)
{
	tolua_open(L);
	monsterinfo_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"MsInfo","CMonsterInfo","",NULL);
			tolua_beginmodule(L,"MsInfo");
			{
				tolua_function(L,"get_monster_id",monsterinfo_get_monster_id);
				tolua_function(L,"get_player_id",monsterinfo_get_player_id);
				tolua_function(L,"get_npc_id",monsterinfo_get_npc_id);
				tolua_function(L,"get_monster_type",monsterinfo_get_monster_type);
				tolua_function(L,"get_npc_type",monsterinfo_get_npc_type);
				tolua_function(L,"get_player_type",monsterinfo_get_player_type);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}