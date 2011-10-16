//=============================================================================
/**
 *  file: ClientRegionBind.cpp
 *
 *  Brief:为CClientRegion类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
#include "..\App\ClientRegion.h"
#include "..\App\GameClient.h"
#include "..\App\Shape.h"
#include "..\App\Goods.h"
#include "..\App\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//自动回收
int collect_client_region(lua_State* L)
{
	CClientRegion *self = (CClientRegion*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

//new,手动回收
int client_region_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_new'.",&err);
		return 0;
	}
#endif
	CClientRegion* ret = new CClientRegion();
	tolua_pushusertype(L,(void*)ret,"CClientRegion");
	return 1;
}

//new_local ,自动回收
int client_region_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_new'.",&err);
		return 0;
	}
#endif
	CClientRegion* ret = new CClientRegion();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CClientRegion");
	return 1;
}

//手动删除
int client_region_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_new'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//添加goods
 int client_region_add_goods(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isusertype(L,2,"CGoods",0,&err) ||
 		!tolua_isnumber(L,3,0,&err)||
 		!tolua_isnumber(L,4,0,&err)||
 		!tolua_isnoobj(L,5,&err) )
 	{
 		tolua_error(L,"#ferror in function 'client_region_add_goods'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
 	if( self != NULL )
 	{
 		CGoods* pGoods = (CGoods*)tolua_tousertype(L,2,0);
 		if( pGoods != NULL )
 		{
 			DWORD dwAimContainerPosition = (DWORD)tolua_tonumber(L,3,0);
 			long lAimContainerExtendID = (long)tolua_tonumber(L,4,0);
 			self->AddGoods(pGoods,dwAimContainerPosition,lAimContainerExtendID);
 		}
 	}
 	return 0;
 }
//查找object
int clientregion_find_child_object(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isusertype(L,3,"CGUID",0,&err)||
		!tolua_isnoobj(L,4,&err))
	{
		tolua_error(L,"&ferror paramter in funciton 'clientregion_find_child_object'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion *)tolua_tousertype(L,1,0);
	if( self != NULL)
	{
		long lType = (long)tolua_tonumber(L,2,0);
		CGUID* guid = (CGUID *)tolua_tousertype(L,3,0);
		if( guid != NULL )
		{
			CBaseObject* pObject = self->FindChildObject(lType,*guid);
			if( pObject!=NULL)
			{
				string strType;
				if(lType == TYPE_PLAYER)
					strType = "CPlayer";
				else if( lType == TYPE_MONSTER )
					strType = "CMonster";
				else if( lType == TYPE_GOODS )
					strType = "CGoods";
				else if( lType == TYPE_COLLECTION )
				{
					tolua_pushusertype(L,pObject,"CShape");
					return 1;
				}
				else
				{
					tolua_pushboolean(L,false);
					return 1;
				}
				tolua_pushusertype(L,pObject,strType.c_str());
				return 1;
			}
			else
			{
				tolua_pushboolean(L,false);
				return 1;
			}
		}
	}
	tolua_pushboolean(L,false);
	return 1;
}
//添加object
 int clientregion_add_object(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isusertype(L,2,"CBaseObject",0,&err) ||
 		!tolua_isnoobj(L,3,&err))
 	{
 		tolua_error(L,"&ferror paramter in funciton 'clientregion_add_object'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion *)tolua_tousertype(L,1,0);
 	if( self != NULL )
 	{
 		CBaseObject *pObject = (CBaseObject *)tolua_tousertype(L,2,0);
 		if( pObject != NULL )
 		{
 			self->AddObject(pObject);
 		}
 	}
 	return 0;
 }
//删除goods
 int client_region_remove_goods(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isusertype(L,2,"CGUID",0,&err) ||
 		!tolua_isnumber(L,3,0,&err)||
 		!tolua_isnumber(L,4,0,&err)||
 		!tolua_isnumber(L,5,0,&err)||
 		!tolua_isnoobj(L,6,&err) )
 	{
 		tolua_error(L,"#ferror in function 'client_region_remove_goods'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
 	if( self != NULL )
 	{
 		CGUID* SourceObjID = (CGUID*)tolua_tousertype(L,2,0);
 		if( SourceObjID && (*SourceObjID) != CGUID::GUID_INVALID)
 		{
 			DWORD dwSourceContainerPos = (DWORD)tolua_tonumber(L,3,0);
 			long lSourceContainerExtendID = (long)tolua_tonumber(L,4,0);
 			DWORD dwSourceAmount = (DWORD)tolua_tonumber(L,5,0);
 			CGoods* pGoods = (CGoods*)self->RemoveGoods(*SourceObjID,dwSourceContainerPos,lSourceContainerExtendID,dwSourceAmount);
 			if( pGoods)
 			{
 				tolua_pushusertype(L,(void*)pGoods,"CGoods");
 				return 1;
 			}
 		}
 	}
 	tolua_pushboolean(L,false);
 	return 1;
 }
//删除object
 int client_region_remove_object(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isusertype(L,2,"CBaseObject",0,&err) ||
 		!tolua_isnoobj(L,3,&err) )
 	{
 		tolua_error(L,"#ferror in function 'client_region_remove_object'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
 	if( self != NULL )
 	{
 		CBaseObject* pObject = (CBaseObject*)tolua_tousertype(L,2,0);
 		if( pObject != NULL )
 		{
 			self->RemoveObject(pObject);
 		}
 	}
 	return 0;
 }
//删除所有object
 int client_region_delete_all_object(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isnoobj(L,2,&err))
 	{
 		tolua_error(L,"&ferror paramter in funciton 'client_region_delete_all_object'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion *)tolua_tousertype(L,1,0);
 	if( self != NULL )
 		self->DeleteAllChildObject();
 	return 0;
 }
//GetShape
 int clientregion_get_shape(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isnumber(L,2,0,&err)||
 		!tolua_isnumber(L,3,0,&err)||
 		!tolua_isnoobj(L,4,&err) )
 	{
 		tolua_error(L,"#ferror in function 'clientregion_get_shape'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
 	if( self != NULL )
 	{
 		long lDestX = (long)tolua_tonumber(L,2,0);
 		long lDestY = (long)tolua_tonumber(L,3,0);
 		CShape* pShape = self->GetShape(lDestX,lDestY);
 		if ( pShape != NULL)
 			tolua_pushusertype(L,(void*)pShape,"CShape");
 	}
 	tolua_pushboolean(L,false);
 	return 1;
 }

 int clientregion_get_shape_by_showname(lua_State* L)
 {
 #ifdef _DEBUG
 	tolua_Error err;
 	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
 		!tolua_isnumber(L,2,0,&err)||
 		!tolua_isnumber(L,3,0,&err)||
 		!tolua_isnoobj(L,4,&err) )
 	{
 		tolua_error(L,"#ferror in function 'clientregion_get_shape'.",&err);
 		return 0;
 	}
 #endif
 	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
 	char* szShowName = (char*)tolua_tostring(L,2,0);
 
 	CShape* pShape = self->GetShapeByShowName(szShowName);
 	if( pShape )
 	{
 		tolua_pushusertype(L,pShape,"CShape");
 	}
 	else
 	{
 		tolua_pushboolean(L,false);
 	}
 	return 1;
 }
int clientregion_get_shape_by_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_shape_by_type'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	CPlayer* pSelf = (CPlayer*)tolua_tousertype(L,2,0);
	OBJECT_TYPE eType = (OBJECT_TYPE)(long)tolua_tonumber(L,3,0);
	CShape* pShape = self->GetShapeByType(eType,pSelf);
	if(pShape!=NULL)
		tolua_pushusertype(L,(void*)pShape,"CShape");
	else
		tolua_pushboolean(L,false);
	return 1;
}

int clientregion_get_nearest_shape_by_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_nearest_shape_by_type'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	CPlayer* pSelf = (CPlayer*)tolua_tousertype(L,2,0);
	OBJECT_TYPE eType = (OBJECT_TYPE)(long)tolua_tonumber(L,3,0);
	CShape* pShape = self->GetNearestShapeByType(eType,pSelf);
	if(pShape!=NULL)
		tolua_pushusertype(L,(void*)pShape,"CShape");
	else
		tolua_pushboolean(L,false);
	return 1;
}





//(x,y,size)区域内是否有shape
int clientregion_get_space_shape(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnumber(L,4,0,&err)||
		!tolua_isnumber(L,5,0,&err)||
		!tolua_isusertype(L,6,"CShape",0,&err)||
		!tolua_isnoobj(L,7,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_space_shape'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		long x = (long)tolua_tonumber(L,2,0);
		long y = (long)tolua_tonumber(L,3,0);
		long size = (long)tolua_tonumber(L,4,0);
		OBJECT_TYPE eType = (OBJECT_TYPE)(long)tolua_tonumber(L,5,0);
		CShape* pShape = (CShape*)tolua_tousertype(L,6,"CShape");
		if( pShape != NULL )
		{
			CShape* pFindShape = (CShape*)self->GetShapeFromSpace(x,y,size,eType,pShape);
			if ( pFindShape != NULL )
			{
				tolua_pushusertype(L,(void*)pFindShape,"CShape");
				return 1;
			}
		}
	}
	tolua_pushboolean(L,false);
	return 1;
}
//(x,y)是否阻挡
int clientregion_get_block(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_block'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		int lDestX = (int)tolua_tonumber(L,2,0);
		int lDestY = (int)tolua_tonumber(L,3,0);
		byte byBlock = self->GetBlock(lDestX,lDestY);
		if( byBlock == CRegion::BLOCK_NO ||
			byBlock == CRegion::BLOCK_AIM ||
			byBlock == CRegion::BLOCK_SHAPE )

			tolua_pushboolean(L,false);
		else
			tolua_pushboolean(L,true);
		return 1;
	}
	tolua_pushboolean(L,false);
	return 1;
}
//件物品
int clientregion_get_goods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_goods'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		CGoods* pGoods = self->GetRegionGoods();
		if( pGoods != NULL)
			tolua_pushusertype(L,pGoods,"CGoods");
	}
	tolua_pushboolean(L,false);
	return 1;
}
//得到一个obejct
int clientregion_get_aim_shape(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err)	||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_aim_shape'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,2,0);
		if( pPlayer != NULL )
		{
			CShape* pShape = self->GetAimShape(pPlayer);
			if( pShape != NULL )
			{
				tolua_pushusertype(L,pShape,"CShape");
				return 1;
			}
		}
	}
	tolua_pushboolean(L,false);
	return 1;
}
//通过显示名得到一个对象
int clientregion_find_object_by_name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_find_object_by_name'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	char* strName = (char*)tolua_tostring(L,2,0);
	if( self != NULL && strName != NULL)	
	{
		CShape* pShape = self->GetShapeByShowName(strName);
		if( pShape != NULL )
		{
			tolua_pushusertype(L,pShape,"CShape");
			return 1;
		}
	}
	tolua_pushboolean(L,false);
	return 1;
}

int clientregion_find_trade_answer(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_find_trade_answer'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,2,0);
	CPlayer* pFindPlayer = self->FindTradeAnswer(pPlayer);
	if( pFindPlayer )
		tolua_pushusertype(L,pFindPlayer,"CPlayer");
	else
		tolua_pushboolean(L,false);
	return 1;
}

//宽
int region_get_width(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'region_get_width'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		long Width = self->GetWidth();
		tolua_pushnumber(L,Width);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}
//高
int region_get_hetght(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'region_get_height'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
	{
		long Height = self->GetHeight();
		tolua_pushnumber(L,Height);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
int client_region_get_region_type(lua_State* L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_get_region_type'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL)
		tolua_pushnumber(L,(lua_Number)self->GetRegionType());
	else
		tolua_pushboolean(L,false);
	return 1;
}

int client_region_set_region_type(lua_State* L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_set_region_type'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL)
		self->SetRegionType((long)tolua_tonumber(L,2,0));
	return 0;
}

int client_region_get_resource_id(lua_State* L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_get_resource_id'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
		tolua_pushnumber(L,(lua_Number)self->GetResourceID());
	else
		tolua_pushboolean(L,false);
	return 1;
}

int client_region_set_resource_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_set_resource_id'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	if( self != NULL )
		self->SetResourceID((long)tolua_tonumber(L,2,0));
	return 0;
}

int client_region_get_exp_scale(lua_State* L)
{
	#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_get_exp_scale'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(lua_Number)self->GetExpScale());
	return 1;
}

int clientregion_get_tax_rate(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_tax_rate'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(lua_Number)self->GetTaxRate());
	return 1;
}

int clientregion_get_war_regiontype(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_war_regiontype'.",&err);
		return 0;
	}
#endif
	CClientRegion* self = (CClientRegion*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(lua_Number)self->GetWarRegionType());
	return 1;
}
int client_region_set_exp_scale(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_set_exp_scale'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	self->SetExpScale((float)tolua_tonumber(L,2,0));
	return 0;
}

int client_region_load(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_region_load'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	BOOL bRet = self->Load();
	tolua_pushboolean(L,bRet);
	return 1;
}
//  [12/25/2008 Square]
int clientregion_get_object_by_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;

	if(!tolua_isusertype(L,1,"CClientRegion",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'clientregion_get_object_by_type'.",&err);
		return 0;
	}
#endif
	CClientRegion *self = (CClientRegion*)tolua_tousertype(L,1,0);
	long lType = (long)tolua_tonumber(L,2,0);
	list<CBaseObject*>* pList = self->GetObjectList();
	list<CBaseObject*>::iterator it = pList->begin();

	long lAmount = 0;
	for(;it != pList->end();it++)
	{
		CShape* pShape = (CShape*)(*it);
		if(pShape&&pShape->GetType() == lType)
		{
			switch(lType)
			{
			case TYPE_PLAYER:
				tolua_pushusertype(L,(void*)(*it),"CPlayer");
				break;
			case TYPE_GOODS:
				tolua_pushusertype(L,(void*)(*it),"CGoods");
				break;
			case TYPE_MONSTER:
				tolua_pushusertype(L,(void*)(*it),"CMonster");
				break;
			default:
				continue;
			}
			if (lAmount<20.00)
			{
				lAmount++;
			}
		}
	}
	return lAmount;
}
//===========================================================
void client_region_reg_types (lua_State* L)
{
	tolua_usertype(L,"CClientRegion");
}

//打开baseobject库
int  client_region_open(lua_State* L)
{
	tolua_open(L);
	client_region_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"ClientRegion","CClientRegion","CBaseObject",collect_client_region);
			tolua_beginmodule(L,"ClientRegion");
			{
				//CClientRegion类的函数
				tolua_function(L,"new",			client_region_new);
				tolua_function(L,"new_local",	client_region_new_local);
				tolua_function(L,".call",		client_region_new_local);
				tolua_function(L,"delete",		client_region_delete);
				//物品管理函数
				tolua_function(L,"find_child_object",		clientregion_find_child_object);
 				tolua_function(L,"add_object",				clientregion_add_object);
 				tolua_function(L,"add_goods",				client_region_add_goods);
 				tolua_function(L,"remove_goods",			client_region_remove_goods);
 				tolua_function(L,"remove_object",			client_region_remove_object);
 				tolua_function(L,"delete_all_object",		client_region_delete_all_object);
// 				//  [12/25/2008 Square]
 				tolua_function(L,"get_object_list_by_type",	clientregion_get_object_by_type);
// 				//
 				tolua_function(L,"get_shape_by_type",	clientregion_get_shape_by_type);
				tolua_function(L,"get_nearest_shape_by_type",	clientregion_get_nearest_shape_by_type);
 				tolua_function(L,"get_goods",		clientregion_get_goods);
 				tolua_function(L,"get_block",		clientregion_get_block);				//一个点(x,y)上是否是阻挡
 				tolua_function(L,"get_shape",		clientregion_get_shape);				//一个点(x,y)上是否有shape
 				tolua_function(L,"space_shape",		clientregion_get_space_shape);			//一个点(x,y,size)上的size区域内是否有shape
 				tolua_function(L,"get_aim_object",	clientregion_get_aim_shape);			//得到场景上的一个对象
 				tolua_function(L,"get_object_by_name",	clientregion_find_object_by_name);
 				tolua_function(L,"find_trade_answer",	clientregion_find_trade_answer);

				//场景变量
				tolua_variable(L,"region_type",		client_region_get_region_type,	client_region_set_region_type);
				tolua_variable(L,"resource_id",		client_region_get_resource_id,	client_region_set_resource_id);
				tolua_variable(L,"exp_scale",		client_region_get_exp_scale,		client_region_set_exp_scale);
				tolua_variable(L,"tax",				clientregion_get_tax_rate,		NULL);
				tolua_variable(L,"war_regiontype",	clientregion_get_war_regiontype,	NULL);
				tolua_function(L,"load",				client_region_load);
				tolua_variable(L,"width",				region_get_width,					NULL);
				tolua_variable(L,"height",			region_get_hetght,				NULL);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}