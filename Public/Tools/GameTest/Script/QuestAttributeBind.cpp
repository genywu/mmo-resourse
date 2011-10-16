//=============================================================================
/**
*  file: QuestSysBind.cpp
*
*  Brief:为CQuestSystem类产生的扩展脚本函数
*
*  Author:square
*	
*	Date:2009/10/9
*/
//=============================================================================
#include "stdafx.h"
#include "../App/QuestSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_quest_attribute(lua_State* L)
{
	CQuestAttribute *self = (CQuestAttribute*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int quest_attribute_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestAttribute",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_new'.",&err);
		return 0;
	}
#endif
	CQuestAttribute* ret = new CQuestAttribute();
	tolua_pushusertype(L,(void*)ret,"CQuestAttribute");
	return 1;
}
int quest_attribute_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestAttribute",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_attribute_new_local'.",&err);
		return 0;
	}
#endif
	CQuestAttribute* ret = new CQuestAttribute();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CQuestAttribute");
	return 1;
}
int quest_attribute_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestAttribute",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_attribute_delete'.",&err);
		return 0;
	}
#endif
	CQuestAttribute* self = (CQuestAttribute*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

int quest_attribute_get_texttype_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestAttribute",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_attribute_get_texttype_id'.",&err);
		return 0;
	}
#endif
	CQuestAttribute* self = (CQuestAttribute*)tolua_tousertype(L,1,0);
	long lID = self->GetTextTypeID();
	tolua_pushnumber(L,lID);
	return 1;
}

void quest_attribute_reg_types (lua_State* L)
{
	tolua_usertype(L,"CQuestAttribute");
}

//打开baseobject库
int  quest_attribute_open(lua_State* L)
{
	tolua_open(L);
	quest_attribute_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"QuestAttribute","CQuestAttribute","",collect_quest_attribute);
			tolua_beginmodule(L,"QuestAttribute");
			{
				tolua_function(L,"new",				quest_attribute_new);
				tolua_function(L,"new_local",		quest_attribute_new_local);
				tolua_function(L,"delete",			quest_attribute_delete);
				tolua_function(L,"get_texttype_id",	quest_attribute_get_texttype_id);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}