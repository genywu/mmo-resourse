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

int collect_quest_step(lua_State* L)
{
	CQuestStep *self = (CQuestStep*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int quest_step_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestStep",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_new'.",&err);
		return 0;
	}
#endif
	CQuestStep* ret = new CQuestStep();
	tolua_pushusertype(L,(void*)ret,"CQuestStep");
	return 1;
}
int quest_step_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestStep",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_new_local'.",&err);
		return 0;
	}
#endif
	CQuestStep* ret = new CQuestStep();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CQuestStep");
	return 1;
}
int quest_step_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestStep",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_delete'.",&err);
		return 0;
	}
#endif
	CQuestStep* self = (CQuestStep*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

int quest_step_get_discrib(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestStep",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_step_get_discrib'.",&err);
		return 0;
	}
#endif
	CQuestStep* self = (CQuestStep*)tolua_tousertype(L,1,0);
	long lID = self->GetDiscrib();
	tolua_pushnumber(L,lID);
	return 1;
}

void quest_step_reg_types (lua_State* L)
{
	tolua_usertype(L,"CQuestStep");
}

//打开baseobject库
int  quest_step_open(lua_State* L)
{
	tolua_open(L);
	quest_step_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"QuestStep","CQuestStep","",collect_quest_step);
			tolua_beginmodule(L,"QuestStep");
			{
				tolua_function(L,"new",				quest_step_new);
				tolua_function(L,"new_local",		quest_step_new_local);
				tolua_function(L,"delete",			quest_step_delete);

				tolua_function(L,"get_discrib",		quest_step_get_discrib);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}