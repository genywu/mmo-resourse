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
#include "..\App\other\GoodsList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_goodslist(lua_State* L)
{
	CGoodsList *self = (CGoodsList*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int goodslist_vec_tagEffect_begin(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_vec_tagEffect_begin'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>* eff = (vector<CGoodsList::tagEffect>*)tolua_touserdata(L,1,0);
	vector<CGoodsList::tagEffect>::iterator begin = eff->begin();
	tolua_pushuserdata(L,(void*)&begin);
	return 1;
}

int goodslist_vec_tagEffect_end(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_vec_tagEffect_end'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>* eff = (vector<CGoodsList::tagEffect>*)tolua_touserdata(L,1,0);
	tolua_pushuserdata(L,(void*)&eff->end());
	return 1;
}

int goodslist_vec_tagEffect_next(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_vec_tagEffect_next'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>::iterator* begin = (vector<CGoodsList::tagEffect>::iterator*)tolua_touserdata(L,1,0);
	vector<CGoodsList::tagEffect>::iterator next = *begin;
	tolua_pushuserdata(L,(void*)&next);
	return 1;
}

int goodslist_get_tagEffect_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_get_tagEffect_type'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>::iterator* begin = (vector<CGoodsList::tagEffect>::iterator*)tolua_touserdata(L,1,0);
	tolua_pushnumber(L,(*begin)->wType);
	return 1;
}

int goodslist_set_tagEffect_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_set_tagEffect_type'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>::iterator* begin = (vector<CGoodsList::tagEffect>::iterator*)tolua_touserdata(L,1,0);
	(*begin)->wType = (WORD)tolua_tonumber(L,2,0);
	return 0;
}

int goodslist_get_tagEffect_value1(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_get_tagEffect_value1'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>::iterator* begin = (vector<CGoodsList::tagEffect>::iterator*)tolua_touserdata(L,1,0);
	tolua_pushnumber(L,(*begin)->dwValue1);
	return 1;
}

int goodslist_set_tagEffect_value1(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_set_tagEffect_value1'.",&err);
		return 0;
	}
#endif
	vector<CGoodsList::tagEffect>::iterator* begin = (vector<CGoodsList::tagEffect>::iterator*)tolua_touserdata(L,1,0);
	(*begin)->dwValue1 = (DWORD)tolua_tonumber(L,2,0);
	return 0;
}

int goodslist_get_property_by_index(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGoodsList",0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'goodslist_get_property_by_index'.",&err);
		return 0;
	}
#endif
	DWORD dwIndex = (DWORD)tolua_tonumber(L,2,0);
	CGoodsList::tagGoods2* pTagGoods = CGoodsList::GetProperty(dwIndex);
	tolua_pushuserdata(L,(void*)&(*pTagGoods));
	return 1;
}

void goodslist_reg_types (lua_State* L)
{
	tolua_usertype(L,"CGoodsList");
}

//打开baseobject库
int  goodslist_open(lua_State* L)
{
	tolua_open(L);
	goodslist_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"GoodsList","CGoodsList","",NULL);
			tolua_beginmodule(L,"GoodsList");
			{
				//册CPlayer类的函数
//				tolua_function(L,"vec_tagEffect_begin",goodslist_vec_tagEffect_begin);
//				tolua_function(L,"vec_tagEffect_end",goodslist_vec_tagEffect_end);
//				tolua_function(L,"vec_tagEffect_next",goodslist_vec_tagEffect_next);

//				tolua_variable(L,"tagEffect_type",goodslist_get_tagEffect_type,goodslist_set_tagEffect_type);
//				tolua_variable(L,"tagEffect_value1",goodslist_get_tagEffect_value1,goodslist_set_tagEffect_value1);
				// tagGoods2 
//				tolua_function(L,"get_property_by_index",goodslist_get_property_by_index);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}