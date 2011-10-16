#include "stdafx.h"
#include "..\Utility\CopyRegionSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_copyregion_setup(lua_State* L)
{
	CCopyRegionSetup* mCopyRegionSetup = (CCopyRegionSetup*)tolua_touserdata(L,1,0);
	delete mCopyRegionSetup;
	return 0;
}

int copyregion_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCopyRegionSetup",1,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'copyregion_new'.",&err);
		return 0;
	}
#endif 
	CCopyRegionSetup* region = new CCopyRegionSetup;
	tolua_pushusertype(L,(void*)region,"CCopyRegionSetup");
	return 1;
}

int copyregion_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCopyRegionSetup",0,&err) || 
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'copyregion_new_local'.",&err);
		return 0;
	}
#endif
	CCopyRegionSetup* region = new CCopyRegionSetup();
	tolua_pushusertype_and_takeownership(L,(void*)region,"CCopyRegionSetup");
	return 1;
}

int copyregion_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCopyRegionSetup",0,&err) ||
		!tolua_isusertype(L,2,"CCopyRegionSetup",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'copyregion_delete'.",&err);
		return 0;
	}
#endif
	CCopyRegionSetup* r = (CCopyRegionSetup*)tolua_tousertype(L,2,0);
	delete r;
	return 0;
}

int decord_copyregion_from_byte_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CCopyRegionSetup",0,&err) ||
		!tolua_isuserdata(L,2,0,&err)||
		!tolua_isnumber(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'decord_copyregion_from_byte_array'.",&err);
		return 0;
	}
#endif
	CCopyRegionSetup* self = (CCopyRegionSetup*)tolua_tousertype(L,1,0);
	BYTE* byte = (BYTE*)tolua_touserdata(L,2,0);
	long pos = (long)tolua_tonumber(L,3,0);
	self->DecordFromByteArray(byte,pos);
	return 0;
}

void copyregion_setup(lua_State* L)
{
	tolua_usertype(L,"CCopyRegionSetup");
}

int copyregion_open(lua_State* L)
{
	tolua_open(L);
	copyregion_setup(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"CopyRegionSetup","CCopyRegionSetup","",collect_copyregion_setup);
			tolua_beginmodule(L,"CopyRegionSetup");
			{
				//×¢²águidÀàµÄluaº¯Êý	
				tolua_function(L,"new",copyregion_new);
				tolua_function(L,"new_local",copyregion_new_local);	
				tolua_function(L,"delete",copyregion_delete);
				tolua_function(L,"decord_from_byte_array",decord_copyregion_from_byte_array);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}