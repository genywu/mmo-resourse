//=============================================================================
/**
 *  file: ScriptDialogBind.cpp
 *
 *  Brief:为CTestScirpt类产生的扩展脚本函数
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"

#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/TestScript.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int test_script_dlg_output_script_info (lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CTestScript",0,&err) ||
		!tolua_isstring(L,2,0,&err) )
	{
		tolua_error(L,"#ferror in function 'test_script_dlg_output_script_info'.",&err);
		return 0;
	}
#endif
	CTestScript* self = (CTestScript*)tolua_tousertype(L,1,0);
	const char* strInfo = (char*)tolua_tostring(L,2,0);
	self->OutputScriptInfo(strInfo);
	return 0;
}

void test_script_dlg_reg_types (lua_State* L)
{
	tolua_usertype(L,"CTestScript");
}

//打开Dialog库
int  test_script_dlg_open(lua_State* L)
{
	tolua_open(L);
	test_script_dlg_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"TestScriptDlg","CTestScript","",NULL);
			tolua_beginmodule(L,"TestScriptDlg");
			{
				tolua_function(L,"output_script_info",test_script_dlg_output_script_info);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}