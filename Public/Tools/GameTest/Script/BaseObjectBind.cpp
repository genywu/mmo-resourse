
//=============================================================================
/**
 *  file: BaseObjectBind.cpp
 *
 *  Brief:为CBaseObject类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
#include "..\App\BaseObject.h"
#include "..\App\Monster.h"
#include "..\App\Goods.h"
#include "..\App\Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_baseobject(lua_State* L)
{
	CBaseObject *self = (CBaseObject*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int baseobject_gettype(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CBaseObject",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'baseobject_gettype'.",&err);
		return 0;
	}
#endif
	CBaseObject *self = (CBaseObject*)tolua_tousertype(L,1,0);
	if( self == NULL )
		return 0;

	long lType = (long)self->GetType();
	tolua_pushnumber(L,lType);
	return 1;
}

int baseobject_change_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CBaseObject",0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'baseobject_change_type'.",&err);
		return 0;
	}
#endif
	CBaseObject* pObject = (CBaseObject*)tolua_tousertype(L,1,0);
	if( pObject )
	{
		long lType = (long)tolua_tonumber(L,2,0);
		switch(lType)
		{
		case	TYPE_MONSTER:
			{
				CMonster* pMonster = dynamic_cast<CMonster*>(pObject);
				tolua_pushusertype(L,pMonster,"CMonster");
			}
			return 1;
		case	TYPE_GOODS:
			{
				CGoods* pGoods = dynamic_cast<CGoods*>(pObject);
				tolua_pushusertype(L,pGoods,"CGoods");
			}
			return 1;
		case	TYPE_PLAYER:
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
				tolua_pushusertype(L,pPlayer,"CPlayer");
			}
			return 1;
		default:
			tolua_pushboolean(L,false);
			break;
		}
	}
	return 1;
}
//====================================================================================================================================
//得到ID
int baseobject_getid(lua_State* L)
{
#ifdef _DEBUG
 	tolua_Error err;
 	if(!tolua_isusertype(L,1,"CBaseObject",0,&err)||
 		!tolua_isstring(L,2,0,&err)||
 		!tolua_isnoobj(L,3,&err))
 	{
 		tolua_error(L,"#ferror in function 'baseobject_getid'.",&err);
 		return 0;
 	}
#endif
	CBaseObject *self = (CBaseObject*)tolua_tousertype(L,1,0);									//10.10注释
	if( self != NULL )
	{
		const CGUID& guid = self->GetExID();
		tolua_pushusertype(L,(void*)&guid,"CGUID");
		return 1;
	}
	return 0;
}

//名字
int baseobject_getname(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CBaseObject",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'baseobject_getname'.",&err);
		return 0;
	}
#endif
	CBaseObject *self = (CBaseObject*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		tolua_pushstring(L,self->GetName());
		return 1;
	}
	return 0;
}

int baseobject_setname(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CBaseObject",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"&ferror paramter in funciton 'baseobject_setname'.",&err);
		return 0;
	}
#endif
	CBaseObject *self = (CBaseObject *)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		char* strName = (char*)tolua_tostring(L,2,0);
		if( strName != NULL )
			self->SetName(strName);
	}
	return 0;
}

// int baseobject_add_object(lua_State* L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isusertype(L,1,"CBaseObject",0,&err) ||
// 		!tolua_isusertype(L,2,"CBaseObject",0,&err) ||
// 		!tolua_isnoobj(L,3,&err))
// 	{
// 		tolua_error(L,"&ferror paramter in funciton 'baseobject_add_object'.",&err);
// 		return 0;
// 	}
// #endif
// 	CBaseObject *self = (CBaseObject *)tolua_tousertype(L,1,0);
// 	if( self != NULL )
// 	{
// 		CBaseObject *pObject = (CBaseObject *)tolua_tousertype(L,2,0);
// 		if( pObject != NULL )
// 			self->AddObject(pObject);
// 	}
// 	return 0;
// }
// int baseobject_remove_object(lua_State* L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isusertype(L,1,"CBaseObject",0,&err) ||
// 		!tolua_isusertype(L,2,"CBaseObject",0,&err) ||
// 		!tolua_isnoobj(L,3,&err))
// 	{
// 		tolua_error(L,"&ferror paramter in funciton 'baseobject_add_object'.",&err);
// 		return 0;
// 	}
// #endif
// 	CBaseObject *self = (CBaseObject *)tolua_tousertype(L,1,0);
// 	if( self != NULL )
// 	{
// 		CBaseObject *pObject = (CBaseObject *)tolua_tousertype(L,2,0);
// 		if( pObject != NULL )
// 		{
// 			self->RemoveObject(pObject);
// 		}
// 	}
// 	return 0;
// }

// int baseobject_recursive_find_object(lua_State* L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(	!tolua_isusertype(L,1,"CBaseObject",0,&err) ||
// 		!tolua_isnumber(L,2,0,&err) ||
// 		!tolua_isusertype(L,3,"CGUID",0,&err) ||
// 		!tolua_isnoobj(L,4,&err))
// 	{
// 		tolua_error(L,"&ferror paramter in funciton 'baseobject_recursive_find_object'.",&err);
// 		return 0;
// 	}
// #endif
// 	CBaseObject *self = (CBaseObject *)tolua_tousertype(L,1,0);
// 	if( self != NULL )
// 	{
// 		long lType = (long)tolua_tonumber(L,2,0); 
// 		CGUID* guid = (CGUID*)tolua_tousertype(L,3,0);
// 		if( guid != NULL )
// 		{
// 			CBaseObject* base =	self->RecursiveFindObject(lType,*guid);
// 			if( base != NULL )
// 			{
// 				tolua_pushusertype(L,base,"CBaseObject");
// 			}
// 			else
// 			{
// 				tolua_pushboolean(L,false);
// 			}
// 			return 1;
// 		}
// 	}
// 	return 0;
// }

void baseobject_reg_types (lua_State* L)
{
	tolua_usertype(L,"CBaseObject");
}



//打开baseobject库
int  baseobject_open(lua_State* L)
{
	tolua_open(L);
	baseobject_reg_types(L);
	//tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"BaseObject","CBaseObject","",collect_baseobject);
			tolua_beginmodule(L,"BaseObject");
			{
				//==类型转换
				tolua_function(L,"change_type",		baseobject_change_type);
				//注册CBaseObject类的函数
				tolua_variable(L,"id",				baseobject_getid,				NULL);
				tolua_variable(L,"type",			baseobject_gettype,				NULL);
				tolua_variable(L,"name",			baseobject_getname,				baseobject_setname);

// 				tolua_function(L,"add_object",				baseobject_add_object);
// 				tolua_function(L,"remove_object",			baseobject_remove_object);
// 				tolua_function(L,"recursive_find_object",	baseobject_recursive_find_object);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}