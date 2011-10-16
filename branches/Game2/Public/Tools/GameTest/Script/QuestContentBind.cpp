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

int collect_questcontent(lua_State* L)
{
	CQuestContent *self = (CQuestContent*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int questcontent_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_new'.",&err);
		return 0;
	}
#endif
	CQuestContent* ret = new CQuestContent();
	tolua_pushusertype(L,(void*)ret,"CQuestContent");
	return 1;
}
int questcontent_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_new_local'.",&err);
		return 0;
	}
#endif
	CQuestContent* ret = new CQuestContent();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CQuestContent");
	return 1;
}
int questcontent_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CQuestContent",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_delete'.",&err);
		return 0;
	}
#endif
	CQuestContent* self = (CQuestContent*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

int questcontent_get_quest(lua_State *L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_get_quest'.",&err);
		return 0;
	}
#endif
	CQuestContent* self = (CQuestContent*)tolua_tousertype(L,1,0);
	long index= (long)tolua_tonumber(L,2,0);
	CQuestContent::tagQuestContent* pQuest = self->GetQuest(index);
	if( pQuest)
		tolua_pushusertype(L,pQuest,"CQuestContent");
	else
		tolua_pushboolean(L,false);
	return 1;
}

int questcontent_get_regionid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_get_regionid'.",&err);
		return 0;
	}
#endif
	CQuestContent::tagQuestContent* self = (CQuestContent::tagQuestContent*)tolua_tousertype(L,2,0);
	if( self )
	{
		tolua_pushnumber(L,self->lRegionID);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}

int questcontent_get_posx(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_get_posx'.",&err);
		return 0;
	}
#endif
	CQuestContent::tagQuestContent* self = (CQuestContent::tagQuestContent*)tolua_tousertype(L,2,0);
	if( self )
	{
		tolua_pushnumber(L,self->lPosX);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}
int questcontent_get_posy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_get_posy'.",&err);
		return 0;
	}
#endif
	CQuestContent::tagQuestContent* self = (CQuestContent::tagQuestContent*)tolua_tousertype(L,2,0);
	if( self )
	{
		tolua_pushnumber(L,self->lPosY);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}
int questcontent_get_name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CQuestContent",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'questcontent_get_name'.",&err);
		return 0;
	}
#endif
	CQuestContent::tagQuestContent* self = (CQuestContent::tagQuestContent*)tolua_tousertype(L,2,0);
	if( self )
	{
		tolua_pushstring(L,self->strName.c_str());
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}

void questcontent_reg_types (lua_State* L)
{
	tolua_usertype(L,"CQuestContent");
}

//打开baseobject库
int  questcontent_open(lua_State* L)
{
	tolua_open(L);
	questcontent_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"QuestContent","CQuestContent","",collect_questcontent);
			tolua_beginmodule(L,"QuestContent");
			{
				tolua_function(L,"new",				questcontent_new);
				tolua_function(L,"new_local",		questcontent_new_local);
				tolua_function(L,"delete",			questcontent_delete);

				tolua_function(L,"get_quest",		questcontent_get_quest);
				tolua_function(L,"get_regionid",	questcontent_get_regionid);//,		NULL);
				tolua_function(L,"get_posx",		questcontent_get_posx);//,		NULL);
				tolua_function(L,"get_posy",		questcontent_get_posy);//,		NULL);
				tolua_function(L,"get_name",		questcontent_get_name);//,		NULL);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}