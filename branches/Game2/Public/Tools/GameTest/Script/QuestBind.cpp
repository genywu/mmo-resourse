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

int collect_quest(lua_State* L)
{
	CQuest *self = (CQuest*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int quest_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuest",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_new'.",&err);
		return 0;
	}
#endif
	CQuest* ret = new CQuest();
	tolua_pushusertype(L,(void*)ret,"CQuest");
	return 1;
}
int quest_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuest",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_new_local'.",&err);
		return 0;
	}
#endif
	CQuest* ret = new CQuest();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CQuest");
	return 1;
}
int quest_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuest",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_delete'.",&err);
		return 0;
	}
#endif
	CQuest* self = (CQuest*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

int quest_get_step(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuest",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_get_step'.",&err);
		return 0;
	}
#endif
	CQuest* self = (CQuest*)tolua_tousertype(L,1,0);
	long lStepID = (long)tolua_tonumber(L,2,0);
	CQuestStep* pStep = (CQuestStep*)self->GetStep(lStepID);
	if( pStep )
		tolua_pushusertype(L,pStep,"CQuestStep");
	else
		tolua_pushboolean(L,false);
	return 1;
}

int quest_get_attribute(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuest",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_get_attribute'.",&err);
		return 0;
	}
#endif
	CQuest* self = (CQuest*)tolua_tousertype(L,1,0);
	CQuestAttribute *pAttri = (CQuestAttribute*)&self->GetAttribute();
	tolua_pushusertype(L,pAttri,"CQuestAttribute");
	return 1;
}

int quest_get_step_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuest",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_get_step_num'.",&err);
		return 0;
	}
#endif
	CQuest* self = (CQuest*)tolua_tousertype(L,1,0);
	long lNum = self->GetStepNum();
	tolua_pushnumber(L,lNum);
	return 1;
}

void quest_reg_types (lua_State* L)
{
	tolua_usertype(L,"CQuest");
}

//打开baseobject库
int  quest_open(lua_State* L)
{
	tolua_open(L);
	quest_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Quest","CQuest","",collect_quest);
			tolua_beginmodule(L,"Quest");
			{
				tolua_function(L,"new",				quest_new);
				tolua_function(L,"new_local",		quest_new_local);
				tolua_function(L,"delete",			quest_delete);

				tolua_function(L,"get_step_num",	quest_get_step_num);
				tolua_function(L,"get_attribute",	quest_get_attribute);
				tolua_function(L,"get_step",		quest_get_step);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}