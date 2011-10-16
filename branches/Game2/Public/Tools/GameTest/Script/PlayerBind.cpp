//=============================================================================
/**
*  file: PlayerBind.cpp
*
*  Brief:为CPlayer类产生的扩展脚本函数
*
*  Authtor:wangqiao
*	
*	Date:2007-7-25
*/
//=============================================================================

#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "..\App\Player.h"
#include "..\App\ClientRegion.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"
#include "../App/IncrementShop.h"

#include "../App/SkillMgr.h"
#include "../App/MailMgr.h"
#include "../App/PetMgr.h"
#include "../App/EquipMgr.h"
#include "../App/GoodsMgr.h"
#include "../App/DepotMgr.h"
#include "../App/Task.h"
#include "../App/PacketMgr.h"
#include "../App/TeamMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_player(lua_State* L)
{
	CPlayer *self = (CPlayer*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}
//new
int player_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_new'.",&err);
		return 0;
	}
#endif
	CPlayer* ret = new CPlayer();
	tolua_pushusertype(L,(void*)ret,"CPlayer");
	return 1;	
}

int player_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_new'.",&err);
		return 0;
	}
#endif
	CPlayer* ret = new CPlayer();
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CPlayer");
	return 1;
}
//delete
int player_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_new'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	delete self;
	self = NULL;
	return 0;
}

int player_get_money_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_occupation'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lType = (long)tolua_tonumber(L,2,0);
	if( lType == PEI_WALLET )
	{
		const CGUID& guid = self->GetPlayerMoneyGuid();
		tolua_pushusertype(L,(void*)&guid,"CGUID");
	}
	else if( lType == PEI_SILVERWALLET )
	{
		const CGUID& guid = self->GetPlayerSilverGuid();
		tolua_pushusertype(L,(void*)&guid,"CGUID");
	}
	else if(lType == eDCT_Gold)
	{
		const CGUID& guid = self->GetGoodsMgr()->GetDepotMgr()->GetDepotMoneyGuid();
		tolua_pushusertype(L,(void*)&guid,"CGUID");
	}
	//else if(lType == eDCT_Gem )
	//{
	//	const CGUID& guid = self->GetGoodsMgr()->GetDepotMgr()->GetDepotSilverMoneyGuid();
	//	tolua_pushusertype(L,(void*)&guid,"CGUID");
	//}
	else
	{
		tolua_pushboolean(L,false);
	}
	return 1;
} 


//职业
int player_get_occupation(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_occupation'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lOccupation = self->GetOccupation();
	tolua_pushnumber(L,lOccupation);
	return 1;
}


//性别
int player_get_sex(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_sex'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lSex = self->GetSex();
	tolua_pushnumber(L,lSex);
	return 1;
}

//国家ID
int player_get_country(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_country'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	BYTE byCountry = self->GetCountry();
	tolua_pushnumber(L,byCountry);
	return 1;
}


//解码部分
int player_decord_from_data_block(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isboolean(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_decord_from_data_block'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DBReadSet* db = (DBReadSet*)tolua_touserdata(L,2,0);
	bool b = tolua_toboolean(L,3,0)==0?false:true;
	self->DecordFromDataBlock(*db,b);
	return 0;
}


//得到任务
int player_get_quest(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_quest'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	list<tagRoleRecord*>* questlist = self->GetTaskMgr()->GetQuestList();
	list<tagRoleRecord*>::iterator it  = questlist->begin();
	for(; it != questlist->end(); it++)
	{
		tagRoleRecord* cord = (*it);
		tolua_pushuserdata(L,(void*)cord);
		return 1;
	}
	tolua_pushboolean(L,false);
	return 1;
}
//得到任务变量
int player_get_role_wparam(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_role_wparam'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	tagRoleRecord* cord = (tagRoleRecord*)tolua_touserdata(L,2,0);
	int index = (int)tolua_tonumber(L,3,0);
	if( index >0 && index <ROLE_PRAME_NUM)
	{
		tolua_pushnumber(L,cord->m_Param[index].m_wParam);
	}
	else
		tolua_pushboolean(L,false);
	return 1;
}
//得到任务
int player_get_quest_step(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_quest_step'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	tagRoleRecord* cord = (tagRoleRecord*)tolua_touserdata(L,2,0);
	tolua_pushnumber(L,cord->m_cRoleStep);
	return 1;
}
// 通过id判断是否存在 [12/25/2008 Square]
int player_is_role_being_by_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_is_role_being_by_id'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long id = (long)tolua_tonumber(L,2,0);
	BOOL bBeing = self->GetTaskMgr()->IsRoleBeingByID(id);
	tolua_pushboolean(L,bBeing);
	return 1;
}
//得到任务
int player_get_quest_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_quest_id'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	tagRoleRecord* cord = (tagRoleRecord*)tolua_touserdata(L,2,0);
	tolua_pushnumber(L,cord->m_lRoleID);
	return 1;
}
//得到指定ORG装备
int player_get_equip(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isstring(L,3,0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_equip'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long index = (long)tolua_tonumber(L,2,0);
	char* strEquipOrg = (char*)tolua_tostring(L,3,0);
	CGoods* pGoods = self->GetGoodsMgr()->GetEquipMgr()->GetEquip(index);
	CEquipMgr* pp = self->GetGoodsMgr()->GetEquipMgr();
	if( pGoods )
	{
		//	if (strEquipOrg)
		//	{
		//		if (!strcmp(pGoods->GetOrginName(),strEquipOrg))
		tolua_pushboolean(L,true);
		//		else
		//			tolua_pushboolean(L,false);
	}
	else
		tolua_pushboolean(L,false);	
	return 1;
}
//得到快捷栏ID
int player_find_item(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_find_item'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	int iIndex = self->GetGoodsMgr()->GetPacketMgr()->FindCote();
	if( iIndex != -1)
		tolua_pushnumber(L,iIndex+PEI_PACK1);
	else
		tolua_pushboolean(L,false);
	return 1;
}

int player_find_empty_item(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_find_empty_item'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	int iIndex = self->GetGoodsMgr()->GetPacketMgr()->FindEmptyCote();
	if( iIndex != -1)
		tolua_pushnumber(L,iIndex+PEI_PACK1);
	else
		tolua_pushboolean(L,false);
	return 1;
}

int player_get_subpack_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_subpack_num'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	int iIndex = self->GetGoodsMgr()->GetPacketMgr()->GetCoteGoodsAmount();
	tolua_pushnumber(L,iIndex);
	return 1;
}


int player_get_upgrade_exp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_upgrade_exp'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(long)self->GetUpgradeExp());
	return 1;
}

int player_get_money(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_money'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD num = self->GetPlayerMoneyNum();
	tolua_pushnumber(L,num);
	return 1;
}

int player_get_silver_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_silver_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	CGUID num = self->GetPlayerSilverGuid();
	tolua_pushusertype(L,(void*)&num,"CGUID");
	return 1;
}

int player_get_silvermoney(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_silvermoney'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD num = self->GetPlayerSilverMoneyNum();
	tolua_pushnumber(L,num);
	return 1;
}

int player_get_hp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_hp'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD num = self->GetHp();
	tolua_pushnumber(L,num);
	return 1;
}

int player_get_mp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_mp'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD num = self->GetMp();
	tolua_pushnumber(L,num);
	return 1;
}

int player_get_exp(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_exp'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD num = self->GetExp();
	tolua_pushnumber(L,num);
	return 1;
}

int player_get_level(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_level'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lLevel= (long)self->GetLevel();
	tolua_pushnumber(L,lLevel);
	return 1;
}

int player_set_level(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_level'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	WORD lLevel = (WORD)tolua_tonumber(L,2,0);
	self->SetLevel(lLevel);
	return 0;
}

// 
// int player_get_r(lua_State* L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
// 		!tolua_isstring(L,2,0,&err)	||
// 		!tolua_isnoobj(L,3,&err) )
// 	{
// 		tolua_error(L,"#ferror in function 'player_get_level'.",&err);
// 		return 0;
// 	}
// #endif
// 	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
// 	long lLevel= (long)self->GetLevel();
// 	tolua_pushnumber(L,lLevel);
// 	return 1;
// }


int player_get_goods_by_pos(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_goods_by_pos'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	long lContainerID = (long)tolua_tonumber(L,2,0);
	long lPos = (long)tolua_tonumber(L,3,0);
	CGoods* pGoods = pPlayer->GetGoodsMgr()->GetGoodsByPos(lContainerID,lPos);
	if( pGoods )
	{
		tolua_pushusertype(L,pGoods,"CGoods");
	}
	else
	{
		tolua_pushboolean(L,false);
	}
	return 1;
}
//得到物品位置
int player_get_taggoods_pos(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_pos'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long pos = ptagGoods->lPos;
	tolua_pushnumber(L,pos);
	return 1;
}
//得到物品
int player_clear_container(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_clear_container'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	long lSocketID = (long)tolua_tonumber(L,2,0);
	pPlayer->GetGoodsMgr()->GetPacketMgr()->ClearAllGoods(lSocketID);
	return 0;
}
//得到物品
int player_get_taggoods_goods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_id'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
	}
	else{
		CGoods* pGoods = ptagGoods->pGoods;
		tolua_pushusertype(L,pGoods,"CGoods");
	}
	return 1;
}
//得到物品guid
int player_get_taggoods_id(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_id'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	const CGUID & guid = ptagGoods->pGoods->GetExID();
	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}
//得到物品存放容器id
int player_get_taggoods_type(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_type'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long type = ptagGoods->lType;
	tolua_pushnumber(L,type);
	return 1;
}
//得到物品数量
int player_get_taggoods_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_num'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long lNum = ptagGoods->lNum;
	tolua_pushnumber(L,lNum);
	return 1;
}

/*
=========================================================================
函数功能: 获得指定物品链表数量
by:LGR 2010.10.27
=========================================================================
*/
int player_get_goods_list_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_num'.",&err);
		return 0;
	}
#endif
	CPlayer* pplayer = (CPlayer*)tolua_touserdata(L,1,NULL);
	int i = tolua_tonumber(L,2,0);

	tolua_pushnumber(L,pplayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(i)->size());

	return 1;
}




//得到物品原始名
int player_get_taggoods_name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_name'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	char* strOrgName = (char*)ptagGoods->pGoods->GetOrginName();
	tolua_pushstring(L,strOrgName);
	return 1;
}

int player_get_taggoods_goodsname(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_taggoods_goodsname'.",&err);
		return 0;
	}
#endif
	tagGoods* ptagGoods = (tagGoods*)tolua_touserdata(L,2,0);
	if( ptagGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	tolua_pushstring(L, ptagGoods->pGoods->GetName());
	return 1;
}

//通过原始名删除物品
int player_delete_goods_by_orgname(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_delete_goods_by_orgname'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	char* strOrgName = (char*)tolua_tostring(L,2,0);
	long  lSocketID = (long)tolua_tonumber(L,3,0);
	for(int i=0;i<COTE_NUM;i++)
	{
		list<tagGoods>* goodsList = self->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(i);
		list<tagGoods>::iterator it = goodsList->begin();
		for( ;it != goodsList->end(); it++)
		{
			if( !strcmp(it->pGoods->GetOrginName(),strOrgName))
			{
				char str[128];
				sprintf(str,"OtherTalk(%d,\'//DelGoods(\"%s\",1);\')",lSocketID,strOrgName);
				CScriptSys::getInstance()->PerformString(str);
			}
		}
	}
	return 0;
}
//通过容器ID获取该容器是否装满
int player_goods_list_full(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_goods_list_full'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	int iContainerID = (int)tolua_tonumber(L,2,0);

	long lSize = 0;
	if( iContainerID >= eDCT_Primary && iContainerID <= eDCT_Secondary4 )
	{
		//		hash_map<CGUID,tagGoods,hash_guid_compare>* goodslist = pPlayer->GetDepotGoodsList((eDEPOT_PACK_ID)iContainerID);
		//		lSize = goodslist->size();
	}
	else
	{
		//		list<tagGoods>* goodsList = pPlayer->GetContainerGoodsList(iContainerID);
		// 		lSize = goodsList->size();
	}
	bool b = false;

	lSize = pPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(iContainerID)->size();

	iContainerID += 1;

	if( iContainerID == PEI_PACKET )
	{
		if(lSize == 20)
			b = true;
	}
	else if( iContainerID == PEI_PACK1 || iContainerID == PEI_PACK2 || iContainerID == PEI_PACK3 || iContainerID == PEI_PACK4 || iContainerID == PEI_PACK5 )
	{
		if( lSize == 8 )
			b = true;
	}
	else if( iContainerID == eDCT_Primary )
	{
		if( lSize == 42)
			b = true;
	}
	else if( iContainerID == eDCT_Secondary )
	{
		if( lSize == 4 )
			b = true;
	}
	else if( iContainerID ==eDCT_Secondary1 || iContainerID ==eDCT_Secondary2 ||
		iContainerID == eDCT_Secondary3 || iContainerID ==eDCT_Secondary4 )
	{
		if( lSize == 8 )
			b = true;
	}
	tolua_pushboolean(L,b);
	return 1;
}

//通过容器ID获取该容器物品链表
int player_get_medal_list(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_goods_list'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	vector<DWORD> indexlist = pPlayer->GetMedalList();
	// 	long lsize = indexlist.size();
	// 	int i=1;
	// 	for(;i<=lsize;i++)
	// 	{
	// 		//if( i==20)
	// 		//	break;
	// 		DWORD dwIndex = indexlist[i-1];
	// 		tolua_pushnumber(L,dwIndex);
	// 	}
	return 0;
}
//通过容器ID获取该容器物品链表
int player_get_goods_list(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_goods_list'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	int iContainerID = (int)tolua_tonumber(L,2,0);

	// 	long lNum = 0;
	// 	if( iContainerID >= eDCT_Primary && iContainerID <= eDCT_Secondary4 )
	// 	{
	// //		hash_map<CGUID,tagGoods,hash_guid_compare>* goodslist = pPlayer->GetDepotGoodsList((eDEPOT_PACK_ID)iContainerID);
	// // 		hash_map<CGUID,tagGoods,hash_guid_compare>::iterator it = goodslist->begin();
	// // 		for( ; it != goodslist->end(); it++)
	// // 		{
	// // 			lNum++;
	// // 			tolua_pushuserdata(L,(void*)&(it->second));
	// // 		}
	// 	}
	// 	else
	// 	{
	// 		list<tagGoods>* goodsList = pPlayer->GetContainerGoodsList(iContainerID);
	// 		if( goodsList)
	// 		{
	// 			list<tagGoods>::iterator it = goodsList->begin();
	// 			for(; it != goodsList->end();it++)
	// 			{
	// 				lNum++;
	// 				tolua_pushuserdata(L,(void*)&(*it));
	// 			}
	// 		}
	// 	}
	return 0;
}
int player_is_can_trade(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_is_can_trade'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer	 = (CPlayer*)tolua_tousertype(L, 1, 0);
	//	bool b = pPlayer->IsCanTrade();
	/*	tolua_pushboolean(L,b);*/
	return 1;
}


//得到装备容器数组
//**********************************************88
// by tanhaowen
int player_get_equiparray(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'player_get_equiparray'.",&err);
		return 0;
	}
#endif

	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	int iContainerID = (int)tolua_tonumber(L,2,0);

	CEquipMgr* pEquipMgr = pPlayer->GetGoodsMgr()->GetEquipMgr();
	if( pEquipMgr == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	CGoods *goodsArray = pEquipMgr->GetEquipInfo();

	if( goodsArray)
	{
		tolua_pushuserdata(L,(void*)&(*goodsArray));
	}
	return 1;
}

//得到物品数量
int player_get_equiparray_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_equiparray_num'.",&err);
		return 0;
	}
#endif
	CGoods* equipGoods = (CGoods*)tolua_touserdata(L,2,0);
	if( equipGoods == NULL)
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long lNum = equipGoods->GetNum();
	tolua_pushnumber(L,lNum);
	return 1;
}
//得到物品在装备栏中的位置
//得到物品数量
int player_get_equiparray_pos(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_equiparray_pos'.",&err);
		return 0;
	}
#endif
	CGoods* equipGoods = (CGoods*)tolua_touserdata(L,2,0);
	long num = (long)tolua_tonumber(L, 3, 0);
	if( equipGoods )
	{
		tolua_pushnumber(L,num);
		return 1;
	}
	else
	{
		tolua_pushboolean(L,false);
		return 1;
	}
}
//*******************************************
//通过容器id得到该容器的一个空位置
int player_get_empty_pos(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_empty_pos'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	int iContainerID = (int)tolua_tonumber(L,2,0);
	DWORD dwPos = pPlayer->GetGoodsMgr()->GetPacketMgr()->GetContainerEmptyPos(iContainerID);
	if (dwPos < 5)
		tolua_pushboolean(L,false);
	else
		tolua_pushboolean(L,true);
	return 1;
}




//得到子背包物品链表
int player_get_item_packet(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_item_packet'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	int nIndex = (int)tolua_tostring(L,2,0);
	// 	if( nIndex>=0 && nIndex <COTE_NUM)
	// 	{
	// 		tagCote* ptagCote = pPlayer->GetCoteGoods(nIndex);
	// 		if( ptagCote->pGoods)
	// 		{
	// 			tolua_pushuserdata(L,(void*)ptagCote);
	// 			return 1;
	// 		}
	// 	}
	// 	tolua_pushboolean(L,false);
	return 0;
}
//得到技能列表
int player_get_skill_list(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error e;
	if(!tolua_isusertype(L,1,"CPlayer",0,&e)||
		!tolua_isnoobj(L,2,0))
	{
		tolua_error(L,"error in function 'player_get_skill_list'.",&e);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetSkillMgr() == NULL )
		return 0;

	list<tagPlayerSkill> listSkill = self->GetSkillMgr()->GetSkillList();
	size_t iSize = listSkill.size();
	DWORD dwTimes(0);
	if( iSize > 0 )
	{
		list<tagPlayerSkill>::iterator it = listSkill.begin();
		for(;it!=listSkill.end();++it)
		{
			DWORD dwID = it->dwID;
			if( dwID > 10010)
			{
				++dwTimes;
				tolua_pushnumber(L,dwID);
			}
		}
		return dwTimes;
	}
	else
	{
		tolua_pushboolean(L,false);
		return 1;
	}
}
//获取单个技能
int player_get_skill_info(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if (!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"$ferror in function 'player_get_skill_info'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	DWORD dwSkillID = (DWORD)tolua_tonumber(L,2,0);
	if( self->GetSkillMgr() == NULL )	return 0;

	tagPlayerSkill *pSkill = self->GetSkillMgr()->GetSkill(dwSkillID);
	if (pSkill == NULL )
		tolua_pushboolean(L,false);
	else
		tolua_pushboolean(L,true);
	return 1;
}

//通过原始名得到一个物品
int player_get_taggoods_by_orgname(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_get_taggoods_by_orgname'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	char* strName = (char*)tolua_tostring(L,2,0);

	tagGoods* ptagGoods = pPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsByOrgName(strName);
	if (ptagGoods)
	{
		tolua_pushusertype(L,(void*)ptagGoods,"CGoods");
		return 1;
	}
	tolua_pushboolean(L,false);
	return 1;
}
int player_set_father(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isusertype(L,2,"CBaseObject",0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_set_father'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	CBaseObject* pObj = (CBaseObject*)tolua_tousertype(L,2,0);
	pPlayer->SetFather(pObj);
	return 0;
}

int player_get_talkbox_text(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_get_talkbox_text'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	char* strText = (char*)pPlayer->GetNpcTalkBoxText();
	// 	tolua_pushstring(L,pPlayer->GetNpcTalkBoxText());
	return 0;
}

int player_get_talk_scriptguid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_get_talk_scriptguid'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID & guid = pPlayer->GetTalkScriptGuid();
	// 	tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 0;
}

//MartySa 
int player_set_talk_scriptguid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isnumber(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_set_talk_scriptguid'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	long lNumber = (long)tolua_tonumber(L, 2, 0);
	if ( lNumber == 0)
	{
		//	  pPlayer->SetTalkScriptGuid(CGUID::GUID_INVALID);
	}

	return 0;
}

int player_contain_subpack(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if( !tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)	||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"$error in function 'player_contain_subpack'.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	//	bool b = pPlayer->IsContainSubPack();
	/*	tolua_pushboolean(L,b);*/
	return 1;
}

//组队
// tanhaowen
//获取玩家是否为队长
int player_team_getIsLeader(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L, 1, "CPlayer", 0, &err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L, 3, &err) )
	{
		tolua_error(L,"#ferror in function 'player_team_getIsLeader'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	bool b = self->GetTeamMgr()->IsTeamLeader();
	tolua_pushboolean(L,b);
	//	bool b = self->IsTeamLeader();
	//	tolua_pushboolean(L, b);
	return 1;
}
//获取玩家是否为队内成员
int player_team_getIsMate(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L, 1, "CPlayer", 0, &err) ||
		!tolua_isusertype(L, 2, "CGUID",   0, &err) ||
		!tolua_isnoobj(L, 3, &err) )
	{
		tolua_error(L,"#ferror in function 'player_team_getIsMate.",&err);
		return 0;
	}
#endif
	CPlayer* self	  = (CPlayer*)tolua_tousertype(L,1,0);
	const CGUID &guid = *((CGUID*)tolua_tousertype(L, 2, 0)); 
	//	bool b		      = self->IsTeammate(guid);
	//	tolua_pushboolean(L, b);
	return 1;
}
//  [12/25/2008 Square]
int player_get_team_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'player_get_team_guid.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	//	const CGUID& guid = pPlayer->GetTeamID();
	// 	if( guid == CGUID::GUID_INVALID )
	// 		tolua_pushboolean(L,false);
	// 	else
	// 		tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 0;
}
int player_is_recruiting(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'player_is_recruiting.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	bool b = pPlayer->GetTeamMgr()->IsRecruitingTeam();
	tolua_pushboolean(L,b);
	return 1;
}

int player_get_requester_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'player_get_requester_guid.",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	const CGUID& guid = pPlayer->GetTeamMgr()->GetTeamRequesterGuid();
	if (guid == CGUID::GUID_INVALID)
		tolua_pushboolean(L,false);
	else
		tolua_pushusertype(L,(void*)&guid,"CGUID");
	return 1;
}

//获取一个队伍当前的队伍人数
//获取玩家是否为队内成员
int player_team_getmatesize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L, 1, "CPlayer", 0, &err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3, &err) )
	{
		tolua_error(L,"#ferror in function 'player_team_getmatesize.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	long lSize    = self->GetTeammateAmount();
	// 	tolua_pushnumber(L, lSize);
	return 0;
}

//
//获取合成物品结构体对象
int player_get_tagsyntsdatagoods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'player_get_tagsyntsdatagoods'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L, 1, 0);
	// 	tagSynthesizeGoods &taggoods = self->GetSyntsGoods();
	// 
	// 	tolua_pushuserdata(L, (void*)&taggoods);
	return 0;

}
//获取合成物品结构体size
int player_get_tagsyntsdatagoods_size(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#ferror in function 'player_get_tagsyntsdatagoods_size'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L, 1, 0);
	// 	tagSynthesizeGoods &taggoods = self->GetSyntsGoods();
	// 	DWORD dwSize = sizeof(taggoods);
	// 	tolua_pushnumber(L, dwSize);
	return 0;
}
int player_add_compose_stuff(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isusertype(L,2,"CGoods",0,&err)||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err))
	{
		tolua_error(L,"#ferror in function 'player_add_compose_stuff'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	tagGoods* ptagGoods = (tagGoods*)tolua_tousertype(L,2,0);
	DWORD dwIndex = (DWORD)tolua_tonumber(L,3,0);
	// 	if( self && ptagGoods )
	// 	{
	// 		tagSynthesizeGoods &tagsyntsgoods = self->GetSyntsGoods();
	// 		if( dwIndex>=0 && dwIndex < MAX_STUFF_NUM)
	// 		{
	// 			tagsyntsgoods.arrContainerFlag[dwIndex] = ptagGoods->lType;
	// 			tagsyntsgoods.arrGoodsGuid[dwIndex] = ptagGoods->pGoods->GetExID();
	// 			tagsyntsgoods.arrPos[dwIndex] = ptagGoods->lPos;
	// 			tagsyntsgoods.arrNum[dwIndex] = ptagGoods->lNum;
	// 		}
	// 	}
	return 0;
}

//判断是否自己正在被攻击
int get_be_attack(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	//CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//const CGUID &id = self->GetTradeRequestGUID();
	return 1;
}

/////////////////////////////////////2008-10-30玩家自动交易的相关接口
//获得交易发起方的GUID
int get_trade_request_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_trade_request_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID &id = self->GetTradeRequestGUID();
	// 	if( id == CGUID::GUID_INVALID )
	// 		tolua_pushboolean(L,false);
	// 	else
	// 	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 0;
}

int set_trade_request_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'set_trade_request_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	CGUID *id = (CGUID*)tolua_tousertype(L,2,0);
	// 	self->SetTradeRequestGUID(*id);
	return 0;
}

int player_get_trade_page_lock(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_trade_page_lock'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	bool b = self->GetbMeIsLocked();
	// 	tolua_pushboolean(L,b);
	return 1;

}

int player_set_trade_page_lock(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isboolean(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_set_trade_page_lock'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//bool b = (bool)tolua_toboolean(L,2,0);
	//修改:MartySa
	//	self->SetbMeIsLocked( tolua_toboolean(L,2,0) ? true:false );
	return 0;
}


//获得交易的会话ID
int get_trade_trade_sessionid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID &id = self->GetTradeSessionID();
	// 	if( id == CGUID::GUID_INVALID )
	// 		tolua_pushboolean(L,false);
	// 	else
	// 	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 0;
}

//获得我的玩家ID
int get_trade_my_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	const CGUID &id = self->GetMyID();
	//	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 1;
}

//获得你的玩家ID
int get_trade_your_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID &id = self->GetYourID();
	// 	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 1;
}

//获取我的交易的PLUGID
int get_trade_my_plugid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	long plugid = self->GetlMPlugID();
	// 	tolua_pushnumber(L,plugid);
	return 1;
}

//获取你的交易的PLUGID
int get_trade_your_plugid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	long plugid = self->GetlYPlugID();
	//	tolua_pushnumber(L,plugid);
	return 1;
}

//获取对方是否已经锁定
int get_you_islocked(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	bool islocked = self->GetbYouIsLocked();
	//	tolua_pushboolean(L,islocked);
	return 1;
}


//交易是否成功
int get_trade_result(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_attackme_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	bool result = self->GetTradeResult();
	// 	tolua_pushboolean(L,result);
	return 1;
}

//获得商店NPC的GUID
int player_is_trade_request(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_shop_npc_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	bool b = self->IsRequestTrade();
	// 	tolua_pushboolean(L,b);
	return 1;
}
int player_set_trade_request(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isboolean(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_shop_npc_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//bool b = tolua_toboolean(L,2,0);
	//修改:MartySa
	//	self->SetRequestTrade( tolua_toboolean(L,2,0) ? true:false );
	return 0;
}

//获得商店NPC的GUID
int get_shop_npc_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_shop_npc_guid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID &id = self->GetShopNPCGuid();
	// 	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 1;
}

//获得要购买的物品的ID
int get_shop_buy_goodsid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_shop_buy_goodsid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	DWORD goodsid = self->GetBuyGoodsID();
	// 	
	// 	tolua_pushnumber(L,goodsid);

	return 1;
}

//获得要购买的物品的数量
int get_shop_buy_goodsnum(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_shop_buy_goodsnum'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);

	// 	DWORD goodsnum = self->GetEveryGoodsNum();
	// 	
	// 	tolua_pushnumber(L,goodsnum);
	return 1;
}

//获得玩家个人商店的SESSIONID
int get_player_shop_sessionid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_player_shop_sessionid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);

	// 	const CGUID &id = self->GetPlayerShopSessionID();
	// 
	// 	tolua_pushusertype(L,(void*)&id,"CGUID");
	return 1;
}

int player_is_shop_open(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_player_shop_sessionid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	bool bOpen = self->IsPlayerShopOpened();
	// 
	// 	tolua_pushboolean(L,bOpen);
	return 1;
}

//获得玩家个人商店的PLUGID(用于放置卖出物品)
int get_player_shop_plugid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_player_shop_plugid'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	long plugid = self->GetPlayerShopPlugID();
	// 	tolua_pushnumber(L,plugid);
	return 1;
}

int player_LinkMan_RepeatForName(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_LinkMan_RepeatForName'.",&err);
		return 0;
	}
#endif
	CPlayer* self  = (CPlayer*)tolua_tousertype(L,1,0);
	string strName = (string)tolua_tostring(L, 2, 0);
	// 	bool bRepeat   = self->isLinkManRepeatForName(strName);
	// 	tolua_pushboolean(L, bRepeat);
	return 1;
}



//	--邮件相关
//		by tanhaowen  2008.12.2
//添加名字
int player_Mail_AddSendToName(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_AddSendToName'.", &err);
		return 0;
	}
#endif
	CPlayer* self  = (CPlayer*)tolua_tousertype(L,1,0);
	char* szName = (char*)tolua_tostring(L,2,0);
	//	string strName = (string)tolua_tostring(L, 2, 0);
	//	if (!strName.empty())
	if( szName )
	{
		if( self->GetMailMgr() != NULL )
			self->GetMailMgr()->AddSendToPlayerName(szName);
		//		self->MailAddSendToPlayerName(szName);
		return 0;
	}
	else
	{
		tolua_pushboolean(L, false);
		return 1;
	}
}

//添加附件物品
int player_Mail_AddGoods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isusertype(L,2,"tagGoods",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_AddGoods'.", &err);
		return 0;
	}
#endif
	CPlayer* self  = (CPlayer*)tolua_tousertype(L,1,0);
	tagGoods *goods = (tagGoods *)tolua_tousertype(L, 2, 0);
	if (NULL != goods)
	{
		if( self->GetMailMgr() != NULL )
			self->GetMailMgr()->AddSendToNorGoods(goods);
		return 0;
	}
	else
	{	
		if( self->GetMailMgr() != NULL )
			self->GetMailMgr()->ClearAllSendData();
		tolua_pushboolean(L, false);
		return 1;
	}
}

//发送名字
int player_Mail_SendMsg_Name(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isusertype(L,2,"CMessage",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_SendMsg_Name'.", &err);
		return 0;
	}
#endif
	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	CMessage *msg = (CMessage*)tolua_tousertype(L, 2, 0);
	if (NULL != msg)
	{
		if( self->GetMailMgr() == NULL )
		{
			tolua_pushboolean(L, false);
			return 1;
		}

		if ( self->GetMailMgr()->SendPlayerName(msg))
		{	
			return 0;
		}
		else
		{
			tolua_pushboolean(L, false);
			return 1;
		}

	}
	else
	{	
		tolua_pushboolean(L, false);
		return 1;
	}
}
//发送goods
int player_Mail_SendMsg_goods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isusertype(L,2,"CMessage",0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_SendMsg_goods'.", &err);
		return 0;
	}
#endif
	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	CMessage *msg = (CMessage*)tolua_tousertype(L, 2, 0);
	BYTE type     = (BYTE)tolua_tonumber(L, 3, 0);
	if (NULL != msg)
	{
		if( self->GetMailMgr() == NULL )
		{
			tolua_pushboolean(L,false);
			return 1;
		}

		if (self->GetMailMgr()->SendNorGoods(msg, type))
		{	
			return 0;
		}
		else
		{
			tolua_pushboolean(L, false);
			return 1;
		}

	}
	else
	{	
		tolua_pushboolean(L, false);
		return 1;
	}
}
//获取联系人数量
int player_Mail_Get_SendPlayerSize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_SendPlayerSize'.", &err);
		return 0;
	}
#endif
	CPlayer* self   = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}
	const long size = self->GetMailMgr()->GetSendToPlayerSize();
	tolua_pushnumber(L, size);
	return 1;
}
//获取发送物品数量
int player_Mail_Get_SendGoodsSize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_SendGoodsSize'.", &err);
		return 0;
	}
#endif
	CPlayer* self   = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}
	const long size = self->GetMailMgr()->GetSendGoodsSize();
	tolua_pushnumber(L, size);
	return 1;
}
//获取发送所需要的钱
int player_Mail_Get_SendMoney(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_SendMoney'.", &err);
		return 0;
	}
#endif
	CPlayer* self   = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}
	const long money = self->GetMailMgr()->GetSendMoney();
	tolua_pushnumber(L, money);
	return 1;
}
//根据输入的下标来获取需要阅读邮件的guid
int player_Mail_Get_ReadMailGuid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_ReadMailGuid'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long index    = (long )tolua_tonumber(L, 2, 0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long size     = self->GetMailMgr()->GetMailDataSize();     
	if (index>=0 && index <size)
	{	
		const CGUID &guid = self->GetMailMgr()->GetMailDataGuid(index);
		tolua_pushusertype(L, (void*)&guid, "CGUID");
		return 1;
	}
	else
	{
		return 0;
	}

}
//获取收件箱有多少邮件
int player_Mail_Get_ReceiveMailSize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_ReceiveMailSize'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	long size     = self->GetMailMgr()->GetMailDataSize();     
	tolua_pushnumber(L, size);
	return 1;
}

//清空收件箱保存数据
int player_Mail_ClearMailData(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_ClearMailData'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	if( self->GetMailMgr() == NULL )
	{
		tolua_pushboolean(L,false);
		return 1;
	}

	self->GetMailMgr()->ClearMailData();
	return 0;
}

//根据传入参数随机获取增值商店goods
// By MartySa 2009.2.21
int player_Mail_Get_IncrementGoods(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_IncrementGoods'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lNumber  = (long)tolua_tonumber(L, 2, 0);
	if ( self )
	{
		//获取增值商店物品list
		//vector<tagGoods*> vecGoods = self->GetvecIncrementShopGoods();
		//if ( !vecGoods.empty() && lNumber>0 && lNumber<vecGoods.size() )
		//{
		//	for (int i=0; i!=lNumber; ++i)
		//	{			
		//		tolua_pushusertype(L, (void*)(vecGoods[rand()%(vecGoods.size())]), "tagGoods");
		//	}
		//	return lNumber;
		//}
	}
	return 0;
}


/*
=========================================================================
函数功能:为邮件提供获取玩家背包道具接口
by:LGR   2010.10.27
=========================================================================
*/
int player_Mail_Get_PacketGoodslist(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_Mail_Get_PacketGoodslist'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lNumber  = (long)tolua_tonumber(L, 2, 0);

	if ( self )
	{
		list<tagGoods>* lisGoods = self->GetGoodsMgr()->GetPacketMgr()->GetGoodsList();
		list<tagGoods>::iterator _it =  lisGoods->begin();

		if (lisGoods->empty()  ||
			lNumber< 0 
			)
		{
			tolua_pushboolean(L,false);
			return 1;
		}
		int _GdNum = lisGoods->size()<lNumber?lisGoods->size():lNumber;

		int i = 0;
		for (;_it!=lisGoods->end();_it++)
		{
			i++;
			tolua_pushusertype(L, (void*)(&(*_it)), "tagGoods");
			if (i == _GdNum)
				break;
		}
		return _GdNum;
	}
	else
	{
		tolua_pushboolean( L ,false);
		return 1;
	}
}



//获取和设置玩家当前自身转向
// by MartySa
// 2008.12.17
int player_get_dir(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_dir'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	float dir     = self->GetDirEx();
	tolua_pushnumber(L, dir);
	return 1;
}

int player_set_dir(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_set_dir'.", &err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	float dir     = (float)tolua_tonumber(L, 2, 0);
	self->SetDirEx(dir);

	return 0;
}
//------------------------------------------------------------
// 帮会/家族 [12/20/2008 Square]
//--------
int player_get_family_guid(lua_State*L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err))
	{
		tolua_error(L,"#function error in player_get_family_guid:",&err);
		return 0;
	}
#endif
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	const CGUID& family_guid = pPlayer->GetFamilyGuid();
	// 	if( family_guid == CGUID::GUID_INVALID )
	// 		tolua_pushboolean(L,false);
	// 	else
	// 		tolua_pushusertype(L,(void*)&family_guid,"CGUID");
	return 0;
}
//获取当前已经存在的所有行会的guid
// by MartySa 2008.12.23
int player_get_hmapfamily_guid(lua_State*L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err)||
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#function error in player_get_hmapfamily_guid:",&err);
		return 0;
	}
#endif
	// 	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,1,0);
	// 	hash_map<CGUID,FamilyInfo,hash_guid_compare> *temp = pPlayer->GethmapFamily();
	// 	
	// 	long lSize = temp->size();
	// 	hash_map<CGUID,FamilyInfo,hash_guid_compare>::iterator it = temp->begin();
	// 	for ( ; it!=temp->end(); ++it)
	// 	{
	// 		tolua_pushusertype(L, (void*)&((*it).first), "CGUID");	
	// 	}
	// 	
	// 	return lSize;
	return 0;
}

//随机的获取增值商店中一个随机的商品的物编编号
int get_random_incrementshop_goods_index(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'get_random_incrementshop_goods_index'.",&err);
		return 0;
	}
#endif

	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lShopID = (long)tolua_tonumber(L,2,0);
	long lGoodsListID = (long)tolua_tonumber(L,3,0);

	DWORD dwGoodsIndex = CIncrementShop::GetIncrShop()->RandomOneGoodsIndex(lShopID,lGoodsListID);
	if( dwGoodsIndex == -1 )
		tolua_pushboolean(L,false);
	else
		tolua_pushnumber(L,dwGoodsIndex);
	return 1;
}


// @增值商店是否打开 [11/10/2009 Square]
int PlayerGetIsIncrShopOpen(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'PlayerGetIsIncrShopOpen'.",&err);
		return 0;
	}
#endif
	tolua_pushboolean(L,CIncrementShop::GetIncrShop()->IsOpend());
	return 1;
}


// 小精灵 [12/30/2008 Square]
int player_get_sprite_setup_array(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'sprite_setup_array'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	for(long i=0;i<AutoSupplyTypeNum;i++)
	// 	{
	// 		tagAutoSupplyData* pData = self->GetAutoSupplySetup(i);
	// 		if(pData)
	// 		{
	// 			tolua_pushuserdata(L,(void*)pData);
	// 		}
	// 	}
	return 0;
}
//补给状态
int player_get_supply_state(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_supply_state'.",&err);
		return 0;
	}
#endif
	tagAutoSupplyData* pData = (tagAutoSupplyData*)tolua_touserdata(L,2,0);
	BOOL b = pData->bActive;
	tolua_pushboolean(L,b);
	return 1;
}
//补给物品索引
int player_get_supply_index(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_supply_index'.",&err);
		return 0;
	}
#endif
	tagAutoSupplyData* pData = (tagAutoSupplyData*)tolua_touserdata(L,2,0);
	long lIndex = pData->lGoodsIndex;
	tolua_pushnumber(L,lIndex);
	return 1;
}
//补给百分比
int player_get_supply_percent(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_supply_percent'.",&err);
		return 0;
	}
#endif
	tagAutoSupplyData* pData = (tagAutoSupplyData*)tolua_touserdata(L,2,0);
	long lPercent = pData->lSpringScale;
	tolua_pushnumber(L,lPercent);
	return 1;
}
//得到一个配置项
int player_get_one_supply_setup(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_one_supply_setup'.",&err);
		return 0;
	}
#endif
	CPlayer* pSelf = (CPlayer*)tolua_tousertype(L,1,0);
	long lIndex = (long)tolua_tonumber(L,2,0);
	// 	tagAutoSupplyData* pData = pSelf->GetAutoSupplySetup(lIndex);
	// 	tolua_pushuserdata(L,(void*)pData);
	return 1;
}
// 设置一个补给配置项 [12/30/2008 Square]
int player_set_supply_setup(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnumber(L,4,0,&err) ||
		!tolua_isnoobj(L,5,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_one_supply_setup'.",&err);
		return 0;
	}
#endif
	tagAutoSupplyData* pData = (tagAutoSupplyData*)tolua_touserdata(L,2,0);
	long lType = (long)tolua_tonumber(L,3,0);
	long lValue = (long)tolua_tonumber(L,4,0);
	if(lType==1)
	{
		bool b = lValue == 0?false:true;
		pData->bActive = b;
	}
	else if( lType==2)
	{
		pData->lGoodsIndex = lValue;
	}
	else if( lType==3)
	{
		pData->lSpringScale = lValue;
	}
	return 0;
}

//随机获得一个要学习的副职业生产技能
int get_occuskillid_to_study(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_occuskillid_to_study'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	DWORD OccuSkillIDToStudy = self->GetRandomOccuSkillToStudy();
	//	tolua_pushnumber(L,OccuSkillIDToStudy);
	return 1;
}

//获得本次登陆客户端后玩家已经学得的生产技能的数目
int get_occuskill_have_studied(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_occuskillid_to_study'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	//	int OccuSkillHaveStudied = self->GetOccuSkillHaveStudied();
	//	tolua_pushnumber(L,OccuSkillHaveStudied);
	return 1;
}

//根据解码获得当前PLAYER身上的所有的生产技能的数目
int get_facture_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_occuskillid_to_study'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	// 	DWORD FactureNum = (self->GetPlayerFactureList())->size();
	// 	tolua_pushnumber(L,FactureNum);
	return 1;
}

//DrivClient 根据传入的UI eumu得到当前UI的状态（是否打开）
//by MartySa 2009.2.11
int get_ui_state(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_occuskillid_to_study'.",&err);
		return 0;
	}
#endif
	CPlayer* self = (CPlayer*)tolua_tousertype(L,1,0);
	long lUIType  = (long)tolua_tonumber(L,2,0);
	if ( lUIType<0 || lUIType >=UI_PAGE_MAX_NUM || !self ) return 0;

	//	switch( lUIType )
	//	{
	// 		case UI_PAGE_SKILL:
	// 			tolua_pushboolean(L, self->GetSkillPageState());
	// 			break;
	// 		case UI_PAGE_SUBBACK:
	// 			tolua_pushboolean(L, self->GetSubBackUIPageState());
	// 			break;
	// 		case UI_PAGE_CHARACTER:
	// 			tolua_pushboolean(L, self->GetCharacterPageState());
	// 			break;
	// 		case UI_PAGE_DEPOT:
	// 			tolua_pushboolean(L, self->GetDepotUIPageState());
	// 			break;
	// 		case UI_PAGE_FRIEND:
	// 			tolua_pushboolean(L, self->GetFriendsUIPageState());
	// 			break;
	// 		case UI_PAGE_ITEM:
	// 			tolua_pushboolean(L, self->GetItemPageState());
	// 			break;
	// 		case UI_PAGE_MAIL:
	// 			tolua_pushboolean(L, self->GetMailPageState());
	// 			break;
	// 		case UI_PAGE_TEAM:
	// 			tolua_pushboolean(L, self->GetTeamPageState());
	// 			break;
	//		default:
	//			break;
	//	}
	return 1;
}

//获取HotKey上已经Icon数目
int get_hotkey_use_num(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_hotkey_use_num'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	//	tagPPro *pProperty = &(self->GetPlayerProperty());
	// 	long lUseNum = 0;
	// 	for (long i=0; i!=24; ++i)
	// 	{
	// 		if ( pProperty->dwHotKey[i] > 0)
	// 		{
	// 			lUseNum++;
	// 		}
	// 	}
	// 	
	// 	tolua_pushnumber(L, lUseNum);
	return 0;
}

int get_selected_guid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_selected_guid'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	//	CGUID *guid   = &(self->GetSelectedShapeGuid());

	//	tolua_pushusertype(L, (void*)(guid), "CGUID");
	return 1;
}

int set_selected_guid_invalid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'set_selected_guid_invalid'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	long lnum = (long)tolua_tonumber(L,2,0);
	if ( self && lnum == 0)
	{
		//		self->SetSelectedShapeGuid(CGUID::GUID_INVALID);
	}

	return 0;
}

//取得玩家当前身上背包的物品总数
int get_pack_goods_sum(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'get_pack_goods_sum'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);

	if ( self )
	{	
		//		long lNum = self->GetPlayerPackGoodsSum();
		//		tolua_pushnumber(L, lNum);
	}

	return 1;
}

//	--	宠物
//	传入一个bool，当true表示取得激活的宠物列表
int player_get_pet_active_list(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'get_pack_goods_sum'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	long lReturnNum(0);
	if( self->GetPetMgr() == NULL )	return 0;

	map<CGUID,CPet*> * mapActivePet = self->GetPetMgr()->GetActivePetList();
	if( mapActivePet != NULL)
	{
		map<CGUID,CPet*>::iterator it = mapActivePet->begin();
		for(; it != mapActivePet->end(); it++)
		{
			//CPet* pPet = dynamic_cast<CPet*>((*it).second);
			//if( pPet != NULL )
			//{
			//	tolua_pushusertype(L,(void*)pPet,"CPet");
			//	lReturnNum++;
			//}
		}
	}
	return lReturnNum;
}

int PlayerGetBouns(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_IsAnswerQuestion'.",&err);
		return 0;
	}
#endif
	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);
	long lSocket = (long)tolua_tonumber(L,2,0);
	self->QuestGetBouns(lSocket);
	return 0;
}

//答题目 add by MartySa 2009.6.15
int player_get_IsAnswerQuestion(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_get_IsAnswerQuestion'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);	

	if ( self )
	{	
		//		tolua_pushboolean(L, self->GetIsAnswerQuestion());
		//		return 1;
	}

	return 0;
}

int player_set_IsAnswerQuestion(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isboolean(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'player_set_IsAnswerQuestion'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);	

	if ( self )
	{	
		//		self->SetIsAnswerQuestion( tolua_toboolean(L, 2, 0) ? true : false );
	}

	return 0;
}

int player_mapsocket(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CPlayer",0,&err) ||
		!tolua_isnumber(L,2,0,&err))
	{
		tolua_error(L,"#ferror in function 'player_mapsocket'.",&err);
		return 0;
	}
#endif

	CPlayer *self = (CPlayer*)tolua_tousertype(L,1,0);	
	long socketid = (long)tolua_tonumber(L,2,0);
	if ( self )
	{	
		CMessage::MapPlayerIDSocket(self->GetExID(), socketid);
	}

	return 0;
}


//
//////////////////////////////////////////////////////////////////////////
void player_reg_types (lua_State* L)
{
	tolua_usertype(L,"CPlayer");
}

void taggoods_reg_types(lua_State* L)
{
	tolua_usertype(L,"tagGoods");
}
//打开baseobject库
int  player_open(lua_State* L)
{
	tolua_open(L);

	player_reg_types(L);
	taggoods_reg_types(L);

	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Player","CPlayer","CMoveShape",collect_player);
			tolua_beginmodule(L,"Player");
			{
				//注册CPlayer类的函数
				tolua_function(L,"new",			player_new);
				tolua_function(L,"new_local",	player_new_local);
				tolua_function(L,".call",		player_new_local);
				tolua_function(L,"delete",		player_delete);

				tolua_function(L,"father",		player_set_father);
				tolua_function(L,"get_money_guid",	player_get_money_guid);
				//Player 基本属性
				tolua_variable(L,"occu",		player_get_occupation,	NULL);
				tolua_variable(L,"level",		player_get_level,		player_set_level);
				tolua_variable(L,"sex",			player_get_sex,			NULL);
				tolua_variable(L,"exp",			player_get_exp,			NULL);
				tolua_variable(L,"hp",			player_get_hp,			NULL);
				tolua_variable(L,"mp",			player_get_mp,			NULL);
				tolua_variable(L,"upgrade_exp",	player_get_upgrade_exp, NULL);
				tolua_variable(L,"country",		player_get_country,		NULL);
				tolua_variable(L,"dir",			player_get_dir,			player_set_dir);

				tolua_variable(L,"contains_subpack",	player_contain_subpack,		NULL);
				//解码部分
				tolua_function(L,"decord_from_data_block",player_decord_from_data_block);
				// 小精灵 [12/30/2008 Square]
				tolua_function(L,"sprite_setup_array",	player_get_sprite_setup_array);
				tolua_function(L,"supply_state",		player_get_supply_state);
				tolua_function(L,"supply_index",		player_get_supply_index);
				tolua_function(L,"supply_percent",		player_get_supply_percent);
				tolua_function(L,"set_supply_setup",	player_set_supply_setup);
				tolua_function(L,"get_one_supply_setup",	player_get_one_supply_setup);
				/*
				一些c++中的Player相关的 enum
				*/
				//装备常量
				tolua_constant(L,"EQUIP_HEAD",		EQUIP_HEAD);
				tolua_constant(L,"EQUIP_NECKLACE",	EQUIP_NECKLACE);
				tolua_constant(L,"EQUIP_WING",		EQUIP_WING);
				tolua_constant(L,"EQUIP_BODY",		EQUIP_BODY);
				tolua_constant(L,"EQUIP_WAISTBAND",	EQUIP_WAISTBAND);
				tolua_constant(L,"EQUIP_GLOVE",		EQUIP_GLOVE);
				tolua_constant(L,"EQUIP_SHOES",		EQUIP_SHOES);
				tolua_constant(L,"EQUIP_HEADGEAR",	EQUIP_HEADGEAR);
				tolua_constant(L,"EQUIP_FROCK",		EQUIP_FROCK);
				tolua_constant(L,"EQUIP_LRING",		EQUIP_LRING);
				tolua_constant(L,"EQUIP_RRING",		EQUIP_RRING);
				tolua_constant(L,"EQUIP_MEDAL1",	EQUIP_MEDAL1);
				tolua_constant(L,"EQUIP_MEDAL2",	EQUIP_MEDAL2);
				tolua_constant(L,"EQUIP_MEDAL3",	EQUIP_MEDAL3);
				tolua_constant(L,"EQUIP_FAIRY",		EQUIP_FAIRY);
				tolua_constant(L,"EQUIP_WEAPON",	EQUIP_WEAPON);
				tolua_constant(L,"EQUIP_WEAPON2",	EQUIP_WEAPON2);
				tolua_constant(L,"EQUIP_PIFENG",	EQUIP_PIFENG);
				tolua_constant(L,"EQUIP_HORSE",		EQUIP_HORSE);
				tolua_constant(L,"EQUIP_SUM",		EQUIP_SUM);
				/*
				新的lua接口
				*/
				//TalkBox
				tolua_variable(L,"script_guid",				player_get_talk_scriptguid,	player_set_talk_scriptguid);
				tolua_variable(L,"talkbox_text",			player_get_talkbox_text,	NULL);
				//任务变量
				tolua_variable(L,"role_wparam",				player_get_role_wparam,NULL);
				//获取任务
				tolua_function(L,"get_quest",				player_get_quest);
				// 通过id判断一个任务是否存在 [12/25/2008 Square]
				tolua_function(L,"is_role_being_by_id",		player_is_role_being_by_id);
				//tolua_function(L,"quest_step",	player_get_quest_step);
				tolua_function(L,"quest_id",				player_get_quest_id);
				//获取武器栏位上的Goods
				tolua_function(L,"get_equip",				player_get_equip);
				//快捷栏
				tolua_function(L,"find_item",				player_find_item);
				tolua_function(L,"find_empty_item",			player_find_empty_item);
				tolua_variable(L,"subpack_num",				player_get_subpack_num,		NULL);
				//得到指定容器的物品链表
				tolua_function(L,"get_medal_list",			player_get_medal_list);
				tolua_function(L,"get_goods_list",			player_get_goods_list);
				tolua_function(L,"goods_list_full",			player_goods_list_full);

				//得到指定容器的空格子
				tolua_function(L,"get_empty_pos",			player_get_empty_pos);
				//获取背包物品
				tolua_function(L,"get_item_packet",			player_get_item_packet);
				tolua_function(L,"get_goods_by_orgname",	player_get_taggoods_by_orgname);
				tolua_function(L,"get_goods_by_pos",		player_get_goods_by_pos);
				//tagGoods
				tolua_function(L,"taggoods_id",				player_get_taggoods_id);//,				NULL);
				tolua_function(L,"taggoods_pos",			player_get_taggoods_pos);//,			NULL);		//所在容器 位置
				tolua_function(L,"taggoods_type",			player_get_taggoods_type);//,			NULL);		//所在容器 类型
				tolua_function(L,"taggoods_num",			player_get_taggoods_num);
				tolua_function(L,"taggoodslist_num",			player_get_goods_list_num);
				
				tolua_function(L,"taggoods_orgname",		player_get_taggoods_name);
				tolua_function(L,"taggoods_goods",			player_get_taggoods_goods);
				//add by MartySa
				tolua_function(L,"taggoods_goodsname",		player_get_taggoods_goodsname);
				//
				tolua_function(L,"clear_container",			player_clear_container);
				//
				tolua_function(L,"delete_goods_by_orgname",	player_delete_goods_by_orgname);
				//技能
				tolua_function(L,"get_skill_list",          player_get_skill_list);
				tolua_function(L,"get_skill",               player_get_skill_info);
				//组队消息 
				//by MartySa 
				tolua_variable(L,"is_team_leader",			player_team_getIsLeader,	NULL);
				tolua_function(L,"is_team_mate",			player_team_getIsMate);
				tolua_variable(L,"tm_size",					player_team_getmatesize,	NULL);	//获取当前队伍成员个数	
				// 新增队伍接口 [12/25/2008 Square]
				tolua_variable(L,"team_id",		player_get_team_guid,	NULL);
				tolua_variable(L,"is_recruit",	player_is_recruiting,	NULL);
				tolua_variable(L,"requester_id",	player_get_requester_guid,	NULL);
				//合成物品
				tolua_variable(L,"compose_stuff",			player_get_tagsyntsdatagoods,		NULL);			//得到合成结构体
				tolua_variable(L,"compose_stuff_size",		player_get_tagsyntsdatagoods_size,	NULL);		//得到结构体size
				tolua_function(L,"add_compose_stuff",		player_add_compose_stuff);						//添加物品到合成结构体

				//防守反击
				tolua_variable(L,"get_be_attack",			get_be_attack,NULL);
				//玩家交易
				tolua_variable(L,"is_can_trade",			player_is_can_trade,NULL);
				tolua_variable(L,"trade_request",			player_is_trade_request,player_set_trade_request);
				tolua_variable(L,"get_trade_request_guid",	get_trade_request_guid,set_trade_request_guid);
				tolua_variable(L,"get_trade_trade_sessionid",get_trade_trade_sessionid,NULL);
				tolua_variable(L,"get_trade_my_guid",		get_trade_my_guid,NULL);
				tolua_variable(L,"get_trade_your_guid",		get_trade_your_guid,NULL);
				tolua_variable(L,"get_trade_my_plugid",		get_trade_my_plugid,NULL);
				tolua_variable(L,"get_trade_your_plugid",	get_trade_your_plugid,NULL);
				tolua_variable(L,"get_you_islocked",		get_you_islocked,NULL);
				tolua_variable(L,"get_trade_result",		get_trade_result,NULL);
				tolua_variable(L,"trade_page_lock",			player_get_trade_page_lock,player_set_trade_page_lock);
				//npc商店交易
				tolua_variable(L,"get_shop_npc_guid",		get_shop_npc_guid,NULL);
				tolua_variable(L,"get_shop_buy_goodsid",	get_shop_buy_goodsid,NULL);
				tolua_variable(L,"get_shop_buy_goodsnum",	get_shop_buy_goodsnum,NULL);
				//玩家个人商店交易
				tolua_variable(L,"is_shop_open",			player_is_shop_open,	NULL);
				tolua_variable(L,"get_player_shop_sessionid",get_player_shop_sessionid,NULL);
				tolua_variable(L,"get_player_shop_plugid",get_player_shop_plugid,NULL);
				//添加好友
				//通过传入的名字来查找是否该玩家已经添加过这个人物，从而用来判断Lua下是否该发送相关AutoMsg
				//player_LinkMan_RepeatForName
				tolua_function(L,"link_man_repeat",			player_LinkMan_RepeatForName);
				//mail send
				tolua_function(L,"mail_add_sendname",		player_Mail_AddSendToName);
				tolua_function(L,"mail_add_sendgoods",		player_Mail_AddGoods);
				tolua_function(L,"mail_send_name",			player_Mail_SendMsg_Name);
				tolua_function(L,"mail_send_goods",			player_Mail_SendMsg_goods);				
				tolua_function(L,"mail_get_readid",			player_Mail_Get_ReadMailGuid);
				tolua_function(L,"mail_clear_maildata",		player_Mail_ClearMailData);
				//获取增值商店物品
				tolua_function(L,"mail_get_incrementgoods",	player_Mail_Get_IncrementGoods);
				//获取背包物品
				tolua_function(L,"mail_get_packetgoodslist",	player_Mail_Get_PacketGoodslist);

				tolua_variable(L,"mail_get_sendnamesize",	player_Mail_Get_SendPlayerSize,NULL);
				tolua_variable(L,"mail_get_sendgoodssize",	player_Mail_Get_SendGoodsSize,NULL);
				tolua_variable(L,"mail_get_sendmoney",		player_Mail_Get_SendMoney,NULL);
				tolua_variable(L,"mail_get_remailsize",		player_Mail_Get_ReceiveMailSize,NULL);

				//处理S2C_USEITEM
				//tolua_function(L,"player_useitem_result",player_useitem_result);
				//帮会,.家族
				tolua_variable(L,"family_guid",		player_get_family_guid,		NULL);
				//player_get_hmapfamily_guid
				tolua_function(L,"allfamily_guid",	player_get_hmapfamily_guid);
				//增值商店
				tolua_variable(L,"is_incr_shop_open",		PlayerGetIsIncrShopOpen,	NULL);
				tolua_function(L,"random_one_goods_index", get_random_incrementshop_goods_index);
				//副职业生产技能的学习
				tolua_variable(L,"get_occuskillid_to_study",get_occuskillid_to_study,NULL);
				tolua_variable(L,"get_occuskill_have_studied",get_occuskill_have_studied,NULL);
				tolua_variable(L,"get_facture_num",get_facture_num,NULL);

				//for DriveClient 界面当前状态				
				tolua_function(L,"get_ui_state",			get_ui_state);		//获取当前状态
				// ... 获取当前HotKey上已经Icon数目 get_hotkey_use_num
				tolua_variable(L,"get_hotkey_use_num", get_hotkey_use_num, NULL);
				// ... 获取当前DrivePlayer选中的Monster Guid
				//get_selected_guid
				tolua_variable(L,"selected_guid", get_selected_guid, set_selected_guid_invalid);
				// ... 获取当前玩家身上所有物品的总数（没包含仓库）
				//get_pack_goods_sum
				tolua_variable(L,"get_goods_sum", get_pack_goods_sum, NULL);

				//	--	宠物
				tolua_function(L,"get_pet_active_list",	player_get_pet_active_list);

				// 答题
				tolua_variable(L,"anti_cheat_question", player_get_IsAnswerQuestion, player_set_IsAnswerQuestion);
				//	领奖
				tolua_function(L,"get_bouns",			PlayerGetBouns);

				tolua_function(L,"mapsocket", player_mapsocket );
			}
			tolua_endmodule(L);
		}
	}
	tolua_endmodule(L);
	return 1;
}