//=============================================================================
/**
 *  file: MoveShapeBind.cpp
 *
 *  Brief:为CMoveShape类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================
#include "stdafx.h"
#include "..\App\MoveShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_moveshape(lua_State* L)
{
	CMoveShape *self = (CMoveShape*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
int moveshape_get_hp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_get_hp'.",&err);
		return 0;
	}
#endif
	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
	DWORD dwHp = self->GetHp();
	tolua_pushnumber(L,dwHp);
	return 1;
}
//
int moveshape_set_destxy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_set_destxy'.",&err);
		return 0;
	}
#endif
	CMoveShape* self = (CMoveShape*)tolua_tousertype(L,1,0);
	float x = (float)tolua_tonumber(L,2,0);
	float y = (float)tolua_tonumber(L,3,0);
	self->SetDestXY(x,y);
	return 0;
}

int moveshape_get_died(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_get_died'.",&err);
		return 0;
	}
#endif
	CMoveShape* self = (CMoveShape*)tolua_tousertype(L,1,0);
	bool b = (self->GetState() == CShape::STATE_DIED);
	tolua_pushboolean(L,b);
	return 1;
}
int moveshape_set_beginxy(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CShape",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_set_beginxy'.",&err);
		return 0;
	}
#endif
	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
	self->SetBeginXY((float)tolua_tonumber(L,2,0),(float)tolua_tonumber(L,3,0));
	return 0;
}

int moveshape_get_destX(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_get_destX'.",&err);
		return 0;
	}
#endif
	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
	float destX = (float)self->GetDestX();
	tolua_pushnumber(L,destX);
	return 1;
}
int moveshape_get_destY(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_get_destY'.",&err);
		return 0;
	}
#endif
	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
	float destY = (float)self->GetDestY();
	tolua_pushnumber(L,destY);
	return 1;
}
//移动
int moveshape_get_distance(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err)||
		!tolua_isusertype(L,2,"CShape",0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'moveshape_get_distance'.",&err);
		return 0;
	}
#endif
	CMoveShape* pShape = (CMoveShape*)tolua_tousertype(L,1,0);
	CShape* pAimObject = (CShape*)tolua_tousertype(L,2,0);
	long lDistance = pShape->Distance(pAimObject);
	tolua_pushnumber(L,lDistance);
	return 1;
}

//获取当前移动模式
// by  tanhaowen
//	10.20
//int moveshape_get_isrun(lua_State* L)
//{
//	#ifdef _DEBUG
//	tolua_Error err;
//	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
//		!tolua_isstring(L,2,0,&err)||
//		!tolua_isnoobj(L,3,&err) )
//	{
//		tolua_error(L,"#ferror in function 'moveshape_get_isrun'.",&err);
//		return 0;
//	}
//#endif
//	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
//	bool isrun = self->GetIsRun();
//	tolua_pushboolean(L,isrun);
//	return 1;	
//}
//设置当前移动模式
// 
int moveshape_set_isrun(lua_State* L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) ||
		!tolua_isboolean(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_set_isrun'.",&err);
		return 0;
	}
#endif
	CMoveShape *self = (CMoveShape*)tolua_tousertype(L,1,0);
	bool bTemp = false;
	if(tolua_toboolean(L,2,0))
	{
		bTemp = true;
	}
	else
	{
		bTemp = false;
	}
//	self->SetIsRun(bTemp);
	return 0;
}

//移动
int moveshape_is_atk(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'moveshape_is_atk'.",&err);
		return 0;
	}
#endif
	CMoveShape* pShape = (CMoveShape*)tolua_tousertype(L,1,0);
	bool bIsAtk		   = pShape->GetShapeIsAtk();
	tolua_pushboolean(L, bIsAtk);
	return 1;
}

int moveshape_request_move(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err))
	{
		tolua_error(L,"#ferror in function 'moveshape_request_move'.",&err);
		return 0;
	}
#endif
	CMoveShape *shape = (CMoveShape*)tolua_tousertype(L,1,0);
    float destX = (float) tolua_tonumber( L, 2, 0 );
    float destY = (float) tolua_tonumber( L, 3, 0 );
    shape->RequestMove( destX, destY ); 
	return 0;
}

int moveshape_update_move(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMoveShape",0,&err) )
	{
		tolua_error(L,"#ferror in function 'moveshape_update_move'.",&err);
		return 0;
	}
#endif
	CMoveShape *shape = (CMoveShape*)tolua_tousertype(L,1,0);
    bool arrive = shape->UpdateMove();
    tolua_pushboolean(L, arrive ? 1 : 0 );
	return 1;
}


////////////////////////////////
void moveshape_reg_types (lua_State* L)
{
	tolua_usertype(L,"CMoveShape");
}

//打开baseobject库
int  moveshape_open(lua_State* L)
{
	tolua_open(L);
	moveshape_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"MoveShape","CMoveShape","CShape",collect_moveshape);
			tolua_beginmodule(L,"MoveShape");
			{
				//基本属性
				tolua_variable(L,"hp",		moveshape_get_hp,		NULL);
				tolua_variable(L,"destx",	moveshape_get_destX,	NULL);
				tolua_variable(L,"desty",	moveshape_get_destY,	NULL);
				tolua_function(L,"dest_xy",	moveshape_set_destxy);
				tolua_variable(L,"is_died", moveshape_get_died,		NULL);

				tolua_function(L,"begin_xy",		moveshape_set_beginxy);
				tolua_function(L,"get_distance",	moveshape_get_distance);

                tolua_function(L, "request_move", moveshape_request_move );
                tolua_function(L, "update_move", moveshape_update_move );
				//tolua_variable(L,"isrun",	moveshape_get_isrun, moveshape_set_isrun);
				//过去当前shape是否受到攻击
				// By MartySa 2009.2.9 for DriveClient
				//moveshape_is_atk
				tolua_variable(L,"is_atk",	moveshape_is_atk, NULL);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}