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
#include "..\App\Goods.h"
#include "../GameTest/resource.h"		// 主符号
#include "..\GameTest\RegionGoodsInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_region_goods(lua_State* L)
{
	CGoods *self = (CGoods*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int region_goods_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CRegionGoodsInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'region_goods_new'.",&err);
		return 0;
	}
#endif
	CRegionGoodsInfo* ret = new CRegionGoodsInfo();
	tolua_pushusertype(L,(void*)ret,"CRegionGoodsInfo");
	return 1;
}

int region_goods_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CRegionGoodsInfo",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'region_goods_new_local'.",&err);
		return 0;
	}
#endif
	CRegionGoodsInfo* ret = new CRegionGoodsInfo();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CRegionGoodsInfo");
	return 1;
}

int region_goods_get_shape_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CRegionGoodsInfo",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'region_goods_get_guid'.",&err);
		return 0;
	}
#endif
	CRegionGoodsInfo* self = (CRegionGoodsInfo*)tolua_tousertype(L,1,0);
	long iMark = (long)tolua_tonumber(L,2,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,3,0);
	*guid = self->GetShapeGuid(iMark);
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

int region_goods_get_shape_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CRegionGoodsInfo",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'region_goods_get_shape_type'.",&err);
		return 0;
	}
#endif
	CRegionGoodsInfo* self = (CRegionGoodsInfo*)tolua_tousertype(L,1,0);
	long iMark = (long)tolua_tonumber(L,2,0);
	long lType = self->GetShapeType(iMark);
	tolua_pushnumber(L,lType);
	return 1;
}

void region_goods_reg_types (lua_State* L)
{
	tolua_usertype(L,"CRegionGoodsInfo");
}

//打开baseobject库
int  region_goods_open(lua_State* L)
{
	tolua_open(L);
	region_goods_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"RegionGoodsInfo","CRegionGoodsInfo","",collect_region_goods);
			tolua_beginmodule(L,"RegionGoodsInfo");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",region_goods_new);
				tolua_function(L,"new_local",region_goods_new_local);
				tolua_function(L,"get_guid",region_goods_get_shape_guid);
				tolua_function(L,"get_type",region_goods_get_shape_type);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}