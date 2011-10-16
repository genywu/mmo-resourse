//=============================================================================
/**
*  file: ShapeBind.cpp
*
*  Brief:为CShape类产生的扩展脚本函数
*
*  Authtor:wangqiao
*	
*	Date:2007-7-25
*/
//=============================================================================
#include "stdafx.h"
#include "..\App\Shape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_shape(lua_State* L)
{
	CShape *self = (CShape*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
//	region_id
int shape_get_region_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'shape_get_region_id'.",&err);
		return 0;
	}
#endif
	const char* str= tolua_tostring(L,2,0);
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(lua_Number)self->GetRegionID());
	return 1;
}
int shape_set_region_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_set_region_id'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long lRegionID = (long)tolua_tonumber(L,2,0);
	self->SetRegionID(lRegionID);
	return 0;
}
// pos_x
int shape_get_posx(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'shape_get_posx'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long posx = (long)self->GetPosX();
	tolua_pushnumber(L,posx+0.5f);
	return 1;
}
// Set_posXY
int shape_set_posxy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_set_posxy'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long posx = (long)tolua_tonumber(L,2,0);
	long posy = (long)tolua_tonumber(L,3,0);
	self->SetPosXY(posx + 0.5f,posy + 0.5f);
	return 0;
}
// posy
int shape_get_posy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'shape_get_posy'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long posy = (long)self->GetPosY();
	tolua_pushnumber(L,posy+0.5f);
	return 1;
}
// shape_distance
int shape_distance(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isusertype(L,2,"CShape",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_distance'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	CShape* pShape = (CShape*)tolua_tousertype(L,2,0);
	long lDis = self->Distance(pShape);
	tolua_pushnumber(L,(lua_Number)lDis);
	return 1;
}
//解码
//int shape_decord_from_byte_array(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
//		!tolua_isusertype(L,2,"CShape",0,&err) ||
//		!tolua_isnumber(L,3,0,&err)||
//		!tolua_isboolean(L,4,0,&err)||
//		!tolua_isnoobj(L,5,&err) )
//	{
//		tolua_error(L,"#ferror in function 'shape_distance'.",&err);
//		return 0;
//	}
//#endif
//	CShape* self = (CShape*)tolua_tousertype(L,1,0);
//	BYTE* byte = (BYTE*)tolua_touserdata(L,2,0);
//	long pos = (long)tolua_tonumber(L,3,0);
//	bool b = tolua_toboolean(L,4,0)==0?false:true;
//	self->DecordFromByteArray(byte,pos,b);
//	return 1;
//}
// tile 格子
int shape_get_tiley(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'shape_get_tiley'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long lTileY = (long)self->GetTileY();
	tolua_pushnumber(L,lTileY);
	return 1;
}
int shape_get_tilex(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_get_tilex'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long lTileX = (long)self->GetTileX();
	tolua_pushnumber(L,lTileX);
	return 1;
}

void shape_reg_types (lua_State* L)
{
	tolua_usertype(L,"CShape");
}
//获取当前玩家方向
//	by	 tanhaowen
int shap_get_edir(lua_State *L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_get_edir'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	long dir = (long)self->GetDir();
	tolua_pushnumber(L, dir);
	return 1;
}

//设置当前玩家方向
int shap_set_edir(lua_State *L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'shape_set_edir'.",&err);
		return 0;
	}
#endif
	CShape *self = (CShape*)tolua_tousertype(L,1,0);
	CShape::eDIR dir = (CShape::eDIR)(long)tolua_tonumber(L, 2, 0);
	self->SetDir(dir);
	return 0;
}

//打开baseobject库
int  shape_open(lua_State* L)
{
	tolua_open(L);
	shape_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Shape","CShape","CBaseObject",collect_shape);
			tolua_beginmodule(L,"Shape");
			{
				//注册CShape变量
				tolua_variable(L,"regionid",shape_get_region_id,shape_set_region_id);
				tolua_variable(L,"posx",	shape_get_posx,		NULL);
				tolua_variable(L,"posy",	shape_get_posy,		NULL);
				tolua_function(L,"pos_xy",	shape_set_posxy);
				tolua_variable(L,"tilex",	shape_get_tilex,NULL);
				tolua_variable(L,"tiley",	shape_get_tiley,NULL);
				tolua_function(L,"distance",shape_distance);
				//新bind 方向
				// tanhaowen
				//获取玩家方向 和 设置玩家方向
				tolua_variable(L,"dir", shap_get_edir, shap_set_edir);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}