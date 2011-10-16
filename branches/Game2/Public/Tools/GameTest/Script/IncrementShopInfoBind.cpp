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
#include "..\GameTest\IncrementShopInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_incre_shop(lua_State* L)
{
	CIncrementShopInfo *self = (CIncrementShopInfo*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int incre_shop_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CIncrementShopInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'incre_shop_new'.",&err);
		return 0;
	}
#endif
	CIncrementShopInfo* ret = new CIncrementShopInfo();
	tolua_pushusertype(L,(void*)ret,"CIncrementShopInfo");
	return 1;
}

int incre_shop_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CIncrementShopInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'incre_shop_new_local'.",&err);
		return 0;
	}
#endif
	CIncrementShopInfo* ret = new CIncrementShopInfo();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CIncrementShopInfo");
	return 1;
}

int incre_shop_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CIncrementShopInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'incre_shop_delete'.",&err);
		return 0;
	}
#endif
	CIncrementShopInfo* self = (CIncrementShopInfo*)tolua_tousertype(L,1,0);
	delete self;
	return 0;
}

int incre_shop_set_shop_goods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CIncrementShopInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'incre_shop_set_shop_goods'.",&err);
		return 0;
	}
#endif
	CIncrementShopInfo* self = (CIncrementShopInfo*)tolua_tousertype(L,1,0);
	vector<CIncrementShopInfo::tagShopGoods>* vec = (vector<CIncrementShopInfo::tagShopGoods>*)tolua_touserdata(L,2,0);
	self->SetShopGoods(*vec);
	return 1;
}

void increment_shop_reg_types (lua_State* L)
{
	tolua_usertype(L,"CIncrementShopInfo");
}

//打开baseobject库
int  increment_shop_open(lua_State* L)
{
	tolua_open(L);
	increment_shop_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"IncreShop","CIcrementShopInfo","CMoveShape",collect_incre_shop);
			tolua_beginmodule(L,"IncreShop");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",incre_shop_new);
				tolua_function(L,"new_local",incre_shop_new_local);
				tolua_function(L,"delete",incre_shop_delete);

				tolua_function(L,"set_shop_goods",incre_shop_set_shop_goods);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}