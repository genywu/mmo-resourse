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

int collect_quest_system(lua_State* L)
{
	CQuestSystem *self = (CQuestSystem*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int quest_system_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestSystem",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_system_new'.",&err);
		return 0;
	}
#endif
	CQuestSystem* ret = new CQuestSystem;
	tolua_pushusertype(L,(void*)ret,"CQuestSystem");
	return 1;
}
int quest_system_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestSystem",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_system_new_local'.",&err);
		return 0;
	}
#endif
	CQuestSystem* ret = new CQuestSystem;
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CQuestSystem");
	return 1;
}
int quest_system_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestSystem",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_system_delete'.",&err);
		return 0;
	}
#endif
	CQuestSystem* self = (CQuestSystem*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

int quest_system_get_quest(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestSystem",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'quest_system_get_quest'.",&err);
		return 0;
	}
#endif
	CQuestSystem* self = (CQuestSystem*)tolua_tousertype(L,1,0);
	long lQuestID = (long)tolua_tonumber(L,2,0);
	CQuest* pQuest = (CQuest*)self->GetQuest(lQuestID);
	if( pQuest)
	{
		tolua_pushusertype(L,pQuest,"CQuest");
	}
	else
	{
		tolua_pushboolean(L,false);
	}
	return 1;
}

void quest_system_reg_types (lua_State* L)
{
	tolua_usertype(L,"CQuestSystem");
}

//打开baseobject库
int  quest_system_open(lua_State* L)
{
	tolua_open(L);
	quest_system_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"QuestSystem","CQuestSystem","",collect_quest_system);
			tolua_beginmodule(L,"QuestSystem");
			{
				tolua_function(L,"new",			quest_system_new);
				tolua_function(L,"new_local",	quest_system_new_local);
				tolua_function(L,"delete",		quest_system_delete);
				tolua_function(L,"get_quest",	quest_system_get_quest);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}