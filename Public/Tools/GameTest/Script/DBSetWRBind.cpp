
//=============================================================================
/**
*  file: DBSetWR.cpp
*
*  Brief:结构体 tagDataBlockReadSet/tagDataBlockWriteSet 产生的扩展脚本函数
*
*  Authtor:liaolin
*	
*	Date:2008/12/5
*/
//=============================================================================

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------
//	read
//------------------------------------------------------
int collect_db_read_set(lua_State* L)
{
	DBReadSet* self = (DBReadSet*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}
int db_read_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBReadSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_read_new'.",&err);
		return 0;
	}
#endif
	DBReadSet* self = new DBReadSet;
	tolua_pushusertype(L,(void*)self,"DBReadSet");
	return 1;
}
int db_read_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBReadSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_read_new_local'.",&err);
		return 0;
	}
#endif
	DBReadSet* self = new DBReadSet;
	tolua_pushusertype_and_takeownership(L,(void*)self,"DBReadSet");
	return 1;
}
int db_read_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBReadSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_read_delete'.",&err);
		return 0;
	}
#endif
	DBReadSet* self = (DBReadSet*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}
void db_read_reg_type(lua_State* L)
{
	tolua_usertype(L,"DBReadSet");
}
//------------------------------------------------------
//	write
//------------------------------------------------------
int collect_db_write_set(lua_State* L)
{
	DBWriteSet* self = (DBWriteSet*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}
int db_write_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBWriteSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_write_new'.",&err);
		return 0;
	}
#endif
	DBWriteSet* self = new DBWriteSet;
	tolua_pushusertype(L,self,"DBWriteSet");
	return 1;
}
int db_write_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBWriteSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_write_new_local'.",&err);
		return 0;
	}
#endif
	DBWriteSet* self = new DBWriteSet;
	tolua_pushusertype_and_takeownership(L,(void*)self,"DBWriteSet");
	return 1;
}
int db_write_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"DBWriteSet",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'db_write_delete'.",&err);
		return 0;
	}
#endif
	DBWriteSet* self = (DBWriteSet*)tolua_tousertype(L,1,0);
	SAFE_DELETE(self);
	return 0;
}
void db_write_reg_type(lua_State* L)
{
	tolua_usertype(L,"DBWriteSet");
}

//////////////////////////////////////////////////////////////////////////
//打开DBWriteSet库
int db_write_open(lua_State* L)
{
	tolua_open(L);
	db_write_reg_type(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"DBWrite","DBWriteSet","",collect_db_write_set);
			tolua_beginmodule(L,"DBWrite");
			{
				tolua_function(L,"new",			db_write_new);
				tolua_function(L,"new_local",	db_write_new_local);
				tolua_function(L,"delete",		db_write_delete);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}
//打开DBReadSet库
int db_read_open(lua_State* L)
{
	tolua_open(L);
	db_read_reg_type(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"DBRead","DBReadSet","",collect_db_write_set);
			tolua_beginmodule(L,"DBRead");
			{
				tolua_function(L,"new",			db_read_new);
				tolua_function(L,"new_local",	db_read_new_local);
				tolua_function(L,"delete",		db_read_delete);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}