#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "..\GameTest\SkillInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int skillinfo_is_being(lua_State*L)
{
#ifdef _DEBUG
	tolua_Error err;
	if (!tolua_isusertype(L,1,"CSkillInfo",0,&err) ||
		!tolua_isuserdata(L,2,0,&err)||
		!tolua_isnoobj(L,3,0))
	{
		tolua_error(L,"#ferror in function 'skillinfo_is_being'.",&err);
		return 0;
	}
#endif
	CSkillInfo* self = (CSkillInfo*)tolua_tousertype(L,1,0);
	CPlayer::tagSkill* pSkill = (CPlayer::tagSkill*)tolua_touserdata(L,2,0);
	bool b = self->IsBeing(pSkill);
	tolua_pushboolean(L,b);
	return 1;
}
void skillinf_reg_type(lua_State*L)
{
	tolua_usertype(L,"CSkillInfo");
}
//打开baseobject库
int skillinfo_open(lua_State* L)
{
	tolua_open(L);
	skillinf_reg_type(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"SkillInfo","CSkillInfo","",NULL);
			tolua_beginmodule(L,"PlayerInfo");
			{
				//册CPlayer类的函数
//				tolua_function(L,"new",playerinfo_new);
//				tolua_function(L,"new_local",playerinfo_new_local);
//				tolua_function(L,"get_packet_id",playerinfo_get_packetid);
//				tolua_function(L,"get_goods_pos",playerinfo_get_goods_pos);
			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;

}