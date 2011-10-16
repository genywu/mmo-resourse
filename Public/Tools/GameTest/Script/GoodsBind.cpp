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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_goods(lua_State* L)
{
	CGoods *self = (CGoods*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int goods_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGoods",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_new'.",&err);
		return 0;
	}
#endif
	CGoods* ret = new CGoods();
	tolua_pushusertype(L,(void*)ret,"CGoods");
	return 1;
}

int goods_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CGoods",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_new_local'.",&err);
		return 0;
	}
#endif
	CGoods* ret = new CGoods();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CGoods");
	return 1;
}

int goods_can_up_grade(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_can_up_grade'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	bool l = self->CanUpgrade();
	tolua_pushnumber(L,l);
	return 1;
}

int goods_get_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_num'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	long num = (long)self->GetNum();
	tolua_pushnumber(L,num);
	return 1;
}

int goods_get_effect_val(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_num'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	CGoods::GOODS_ADDON_PROPERTIES eIndex = (CGoods::GOODS_ADDON_PROPERTIES)(long)tolua_tonumber(L,2,0);
	if( eIndex>=0 && eIndex<CGoods::GAP_AMOUNT)
	{
		long lValue = self->GetEffectVal(eIndex,1);
		tolua_pushnumber(L,lValue);
		return 1;
	}
	tolua_pushboolean(L,false);
	return 1;
}

int goods_get_index(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_index'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	DWORD index = (DWORD)self->GetIndex();
	tolua_pushnumber(L,index);
	return 1;
}


int goods_get_name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_name'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	const char* name = self->GetName();//GetBaseProperty()->strName.c_str();
	tolua_pushstring(L,name);
	return 1;
}

int goods_set_index(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnumber(L,4,0,&err)||
		!tolua_isnoobj(L,5,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_set_index'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	long index = (long)tolua_tonumber(L,2,0);
	self->SetIndex(index);
	return 0;
}

int goods_set_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnumber(L,4,0,&err)||
		!tolua_isnoobj(L,5,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_set_num'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	long num = (long)tolua_tonumber(L,2,0);
	self->SetNum(num);
	return 0;
}

int goods_decord_from_byte_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isboolean(L,4,0,&err)||
		!tolua_isnoobj(L,5,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_decord_from_byte_array'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	BYTE* byte = (BYTE*)tolua_touserdata(L,2,0);
	long pos = (long)tolua_tonumber(L,3,0);
	bool b = tolua_toboolean(L,4,0)==0?false:true;
	self->DecordFromByteArray(byte,pos,b);
	return 0;
}

int goods_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_delete'.",&err);
		return 0;
	}
#endif
	CGoods* self = (CGoods*)tolua_tousertype(L,1,0);
	delete self;
	return 0;
}

//得到物品基本类型
int goods_get_basetype(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_basetype'.",&err);
		return 0;
	}
#endif
	CGoods* pGoods = (CGoods*)tolua_tousertype(L,1,0);
	DWORD dwType = pGoods->GetProperty()->dwType;
	tolua_pushnumber(L,dwType);
	return 1;
}
//得到物品原始名
int goods_get_orgName(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_get_orgName'.",&err);
		return 0;
	}
#endif
	CGoods* pGoods = (CGoods*)tolua_tousertype(L,1,0);
	string strOrgName = pGoods->GetProperty()->strOrginName;
	tolua_pushstring(L,strOrgName.c_str());
	return 1;
}
//判断物品是否可叠加
//  by tanhaowen
//	  2008.10.29
int goods_is_pile(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CGoods",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'goods_is_pile'.",&err);
		return 0;
	}
#endif
	CGoods *pGoods = (CGoods*)tolua_tousertype(L,1,0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L,2,0);
	bool ispile = CGoods::isGoodsCanAccumulate(pGoods, pPlayer);			//调用判断是否可叠加函数
	tolua_pushboolean(L, ispile);
	return 1;
}
void goods_reg_types (lua_State* L)
{
	tolua_usertype(L,"CGoods");
}

//打开baseobject库
int  goods_open(lua_State* L)
{
	tolua_open(L);
	goods_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Goods","CGoods","CMoveShape",collect_goods);
			tolua_beginmodule(L,"Goods");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",				goods_new);
				tolua_function(L,"new_local",		goods_new_local);
				tolua_function(L,"delete",			goods_delete);
				tolua_function(L,"decord_from_byte_array",		goods_decord_from_byte_array);
				tolua_function(L,"can_up_grade",				goods_can_up_grade);	
				//goods_is_pile
				//  by tanhaowen
				tolua_function(L,"is_pile",			goods_is_pile);
				
				//基本属性
				tolua_variable(L,"goodstype",		goods_get_basetype,	NULL);
				tolua_variable(L,"orgname",			goods_get_orgName,	NULL);
				tolua_variable(L,"index",			goods_get_index,	NULL);
				tolua_variable(L,"num",				goods_get_num,		goods_set_num);	
				//
				tolua_function(L,"effect_val",		goods_get_effect_val);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}