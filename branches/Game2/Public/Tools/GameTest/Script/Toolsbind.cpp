//=============================================================================
/**
 *  file: ToolsBind.cpp
 *
 *  Brief:为Tools函数产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum eLuaTestType{
	OtherTest = 1,
	LoginTest,
	CreatePlayerTest,
	DeletePlayerTest,
	CreateGoodsTest,
	QuitGameTest,
	RestartTest,
	ChangeRegionTest,
	ChangeServerTest,
	OneGroupServerPlayerMaxNum,
	OneServerPlayerMaxNum,
	OneGroupServerMonsterMaxNum,
	OneServerMonsterMaxNum,
	ReLoginTest,
	MoveTest,
	ChangeRgnPrushGoods,
	AtkTest,
	FindRoad,
	AutoAtkTest,
};

int HandleGoodsOutInfo(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'HandleGoodsOutInfo'.",&err);
		return 0;
	}
#endif
	const char* strCDKey = tolua_tostring(L,1,0);
	const char* strOutInfo = tolua_tostring(L,2,0);

//	PutHandleGoodsInfo(strCDKey,strOutInfo);
	return 0;
}
int outputinfo(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'outputinfo'.",&err);
		return 0;
	}
#endif
	const char* str = tolua_tostring(L,1,0);
	PutDebugString(str);
	return 0;
}

int PutDebugStringToFile(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'outputinfo'.",&err);
		return 0;
	}
#endif
	const char* strName = tolua_tostring(L,1,0);
	const char* str = tolua_tostring(L,2,0);
	PutStringToFile(strName,str);
	return 0;
}

int outputGoodsIinfo(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'outputGoodsIinfo'.",&err);
		return 0;
	}
#endif
	const char* str = tolua_tostring(L,1,0);
//	PutGoodsAddOrDelInfo(str);
	return 0;
}

int output_test_info(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_decord_from_byte_array'.",&err);
		return 0;
	}
#endif
	const char* str = tolua_tostring(L,1,0);
//	PutDebugString(str);
	return 0;
}
int output_container_info(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'output_container_info'.",&err);
		return 0;
	}
#endif
	const char* str = tolua_tostring(L,1,0);
//	OutputContainerInfo(str);
	return 0;
}

int get_cur_tick_count(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'get_cur_tick_count'.",&err);
		return 0;
	}
#endif
	DWORD dwCount = GetCurTickCount();
	tolua_pushnumber(L,dwCount);
	return 1;
}

int get_char_from_byte_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstringarray(L,1,0,0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_char_from_byte_array'.",&err);
		return 0;
	}
#endif
	BYTE* pByte = (BYTE*)tolua_tostring(L,1,0);
	long l = (long)tolua_tonumber(L,2,0);
	const char *lType = (char*)_GetCharFromByteArray(pByte,l);
	tolua_pushstring(L,lType);
	return 1;
}

int get_long_from_byte_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstringarray(L,1,0,0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_long_from_byte_array'.",&err);
		return 0;
	}
#endif
	BYTE* pByte = (BYTE*)tolua_tostring(L,1,0);
	long l = (long)tolua_tonumber(L,2,0);
	long lExtendID = _GetLongFromByteArray(pByte,l);
	tolua_pushnumber(L,lExtendID);
	return 1;
}

int get_dword_from_byte_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstringarray(L,1,0,0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_dword_from_byte_array'.",&err);
		return 0;
	}
#endif
	BYTE* pByte = (BYTE*)tolua_tostring(L,1,0);
	long l = (long)tolua_tonumber(L,2,0);
	DWORD dwAmount= _GetDwordFromByteArray(pByte,l);
	tolua_pushnumber(L,dwAmount);
	return 1;
}

int random(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnumber(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_decord_from_byte_array'.",&err);
		return 0;
	}
#endif
	int nMax = (int)tolua_tonumber(L,1,0);
	int n = random(nMax);
	tolua_pushnumber(L,(lua_Number)n);
	return 1;
}

int size_of(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isuserdata(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'size_of'.",&err);
		return 0;
	}
#endif
	int size = sizeof(tolua_touserdata(L,1,0));
	tolua_pushnumber(L,(lua_Number)size);
	return 1;
}
int ReadString(lua_State* L)
{
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isnumber(L,1,0,&err) ||
// 		!tolua_isnumber(L,2,0,&err) ||
// 		!tolua_isnoobj(L,3,&err) )
// 	{
// 		tolua_error(L,"#ferror in function 'ReadString'.",&err);
// 		return 0;
// 	}
// #endif
// 	long lTypeID = (long)tolua_tonumber(L,1,0);
// 	long lDisc = (long)tolua_tonumber(L,2,0);
// 	tolua_pushstring(L,CStringReading::LoadText(lTypeID,lDisc));
	return 1;
}
int Sleep(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnumber(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'Sleep'.",&err);
		return 0;
	}
#endif
	long lTimes = (long)tolua_tonumber(L,1,0);
	Sleep(lTimes);
	return 0;
}



//打开baseobject库
int tools_open(lua_State* L)
{
	tolua_open(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{

		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_function(L,"sleep",			 Sleep);
			tolua_function(L,"read_string",		 ReadString);
			tolua_function(L,"outputinfo",		 outputinfo);
			tolua_function(L,"putstring_tofile", PutDebugStringToFile);
			tolua_function(L,"containerinfo",	 output_container_info);
			tolua_function(L,"handlegoodsinfo",  HandleGoodsOutInfo);
			tolua_function(L,"goodsinfo",		 outputGoodsIinfo);
			tolua_function(L,"output_test_info", output_test_info);
			tolua_function(L,"random",			 random);
			tolua_function(L,"size_of",			 size_of);
			tolua_function(L,"cur_tick_count",	 get_cur_tick_count);

			tolua_function(L,"get_char_from_byte_array",get_char_from_byte_array);
			tolua_function(L,"get_long_from_byte_array",get_long_from_byte_array);
			tolua_function(L,"get_dword_from_byte_array",get_dword_from_byte_array);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}