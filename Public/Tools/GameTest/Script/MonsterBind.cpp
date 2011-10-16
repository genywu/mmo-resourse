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
#include "..\App\Monster.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_monster(lua_State* L)
{
	CMonster *self = (CMonster*)tolua_touserdata(L,1,0);
	//delete self;
	//MartySa 更改
	SAFE_DELETE(self);
	return 0;
}

int monster_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CMonster",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_new'.",&err);
		return 0;
	}
#endif
	CMonster* ret = new CMonster();
	tolua_pushusertype(L,(void*)ret,"CMonster");
	return 1;
}

int monster_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CMonster",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_new_local'.",&err);
		return 0;
	}
#endif
	CMonster* ret = new CMonster();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CMonster");
	return 1;
}

int monster_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_delete'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	//delete self;
	//MartySa 更改
	SAFE_DELETE(self);
	return 0;
}

//int monster_decord_from_byte_array(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
//		!tolua_isuserdata(L,2,0,&err) ||
//		!tolua_isnumber(L,3,0,&err) ||
//		!tolua_isboolean(L,4,0,&err)||
//		!tolua_isnoobj(L,5,&err) )
//	{
//		tolua_error(L,"#ferror in function 'monster_decord_from_byte_array'.",&err);
//		return 0;
//	}
//#endif
//	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
//	BYTE* byte = (BYTE*)tolua_touserdata(L,2,0);
//	long pos = (long)tolua_tonumber(L,3,0);
//	bool b = tolua_toboolean(L,4,0)==0?false:true;
//	self->DecordFromByteArray(byte,pos,b);
//	return 0;
//}

int monster_decord_change_property(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_decord_change_property'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	BYTE* byte = (BYTE*)tolua_touserdata(L,2,0);
	long pos = (long)tolua_tonumber(L,3,0);
	self->DecordChangeProperty(byte,pos);
	return 0;
}

/////////////////////////	Monster	/////////////////////////////////////////////////
int monster_get_hp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_get_hp'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(long)self->GetHp());
	return 1;
}
int monster_set_hp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isstring(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_set_hp'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	self->SetHp((DWORD)tolua_tonumber(L,2,0));
	return 0;
}

int monster_get_monster_max_hp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_get_monster_max_hp'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(long)self->GetMaxHp());
	return 1;
}

//int monster_set_monster_max_hp(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
//		!tolua_isnumber(L,2,0,&err) ||
//		!tolua_isnoobj(L,3,&err) )
//	{
//		tolua_error(L,"#ferror in function 'monster_set_monster_max_hp'.",&err);
//		return 0;
//	}
//#endif
//	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
//	self->SetMaxHp((DWORD)tolua_tonumber(L,2,0));
//	return 1;
//}

/////////////////////////	NPC		/////////////////////////////////////////////////

int monster_set_is_can_talk(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_set_is_can_talk'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	WORD wTalk = (WORD)tolua_tonumber(L,2,0);
	self->SetIsCanTalk(wTalk);
	return 0;
}

int monster_is_can_talk(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMonster",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'monster_is_can_talk'.",&err);
		return 0;
	}
#endif
	CMonster* self = (CMonster*)tolua_tousertype(L,1,0);
	//bool bTalk = self->IsCanTalk()==1?true:false;      //注释\修改 By MartySa 2009.7.6
	if ( self )
	{	
		tolua_pushboolean(L, self->IsCanTalk() ? true : false);
		return 1;
	}
	
	return 0;
}

void monster_reg_types (lua_State* L)
{
	tolua_usertype(L,"CMonster");
}

//打开baseobject库
int  monster_open(lua_State* L)
{
	tolua_open(L);
	monster_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Monster","CMonster","CMoveShape",collect_monster);
			tolua_beginmodule(L,"Monster");
			{
				//基础属性
				tolua_variable(L,"talk", monster_is_can_talk,	NULL);
				//册CPlayer类的函数
				tolua_function(L,"new",monster_new);
				tolua_function(L,"new_local",monster_new_local);
				tolua_function(L,".call",monster_new_local);
				tolua_function(L,"delete",monster_delete);
//				tolua_function(L,"decord_from_byte_array",monster_decord_from_byte_array);
				tolua_function(L,"decord_change_property",monster_decord_change_property);

				//基本属性
				tolua_variable(L,"hp",monster_get_hp,		monster_set_hp);
				//tolua_variable(L,"maxhp",monster_get_monster_max_hp,monster_set_monster_max_hp);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}