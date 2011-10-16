
#include "stdafx.h"
#include "../App/CCDKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_cdkey(lua_State* L)
{
	CCDKey *self = (CCDKey*)tolua_touserdata(L,1,0);
	delete self;
	return 0;
}

int cdkey_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCDKey",0,&err) ||
		!tolua_isstring(L,2,0,&err )||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_new'.",&err);
		return 0;
	}
#endif
	char* strCDKey = (char*)tolua_tostring(L,2,0);
	CCDKey* ret = new CCDKey(strCDKey);
	tolua_pushusertype(L,(void*)ret,"CCDKey");
	return 1;
}

int cdkey_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCDKey",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_new_local'.",&err);
		return 0;
	}
#endif
	char * strCDKey = (char*)tolua_tostring(L,2,0);
	CCDKey* ret = new CCDKey(strCDKey);
	tolua_pushusertype_and_takeownership(L,(void*)ret,"CCDKey");
	return 1;
}

int cdkey_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CCDKey",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'client_delete'.",&err);
		return 0;
	}
#endif
	CCDKey* self = (CCDKey*)tolua_tousertype(L,1,0);
	delete self;
	return 0;
}

int cdkey_add_data(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CCDKey",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_add_data'.",&err);
		return 0;
	}
#endif
	CCDKey* slef = (CCDKey*)tolua_tousertype(L,1,0);
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,2,0);
	long lType = (long)tolua_tonumber(L,3,0);
	slef->AddPlayerData(pPlayer,(CCDKey::eSaveDataType)lType);
	return 0;
}


int cdkey_check_data(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CCDKey",0,&err) ||
		!tolua_isusertype(L,2,"CPlayer",0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_check_data'.",&err);
		return 0;
	}
#endif
	CCDKey* slef = (CCDKey*)tolua_tousertype(L,1,0);
	CPlayer* pPlayer = (CPlayer*)tolua_tousertype(L,2,0);
	long lType = (long)tolua_tonumber(L,3,0);
	slef->CheckPlayerData(pPlayer,(CCDKey::eSaveDataType)lType);
	return 0;
}

int cdkey_find_cdkey(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCDKey",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_is_add_data'.",&err);
		return 0;
	}
#endif
	char * strCDKey = (char*)tolua_tostring(L,2,0);
	bool b = CCDKey::FindCDKey(strCDKey) == 0? false:true;
	tolua_pushboolean(L,b);
	return 1;
}

int cdkey_add_cdkey(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCDKey",0,&err) ||
		!tolua_isusertype(L,2,"CCDKey",0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_add_cdkey'.",&err);
		return 0;
	}
#endif
	CCDKey* pCDKey = (CCDKey*)tolua_tousertype(L,2,0);
	CCDKey::AddCDKey(pCDKey);
	return 0;
}

int cdkey_get_cdkey(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CCDKey",0,&err) ||
		!tolua_isstring(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_get_cdkey'.",&err);
		return 0;
	}
#endif
	const char* strCDKey = (char*)tolua_tostring(L,2,0);
	CCDKey* pCDKey = CCDKey::GetCDKey(strCDKey);
	if(pCDKey)
		tolua_pushusertype(L,(void*)pCDKey,"CCDKey");
	else

		tolua_pushboolean(L,false);
	return 1;
}

//-----------------------------------------------------------------------------------------
// 对玩家身上的邮件进行数据保存和核对函数bind
// by MartySa 2009.1.6
//-----------------------------------------------------------------------------------------
int cdkey_svae_maildata(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,3,"CPlayer",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_svae_maildata'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L,3, 0);
	pCDKey->SavaMailData(pPlayer);
	return 0;
}

//AddSaveMailDataByTxt
//int cdkey_add_savemaildata_by_txt(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(
//		!tolua_isusertype(L,1,"CCDKey",0,&err)||
//		!tolua_isusertype(L,2,"CCDKey",0,&err)||
//		!tolua_isusertype(L,3,"CPlayer",0,&err)||
//		!tolua_isnoobj(L,4,&err) )
//	{
//		tolua_error(L,"#ferror in function 'cdkey_add_savemaildata_by_txt'.",&err);
//		return 0;
//	}
//#endif
//	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
//	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L, 3, 0);
//	
//	pCDKey->AddSaveMailDataByTxt(pPlayer, pCDKey->m_strCDKey.c_str());
//	return 0;
//}

//CheckMailData
//int cdkey_check_maildata(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(
//		!tolua_isusertype(L,1,"CCDKey",0,&err)||
//		!tolua_isusertype(L,2,"CCDKey",0,&err)||
//		!tolua_isusertype(L,3,"CPlayer",0,&err)||
//		!tolua_isnoobj(L,4,&err) )
//	{
//		tolua_error(L,"#ferror in function 'cdkey_check_maildata'.",&err);
//		return 0;
//	}
//#endif
//	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
//	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L, 3, 0);
//	
//	pCDKey->CheckMailData(pPlayer, pCDKey->m_strCDKey.c_str());
//	return 0;
//}

//clear maildata
int cdkey_clear_maildata(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(
		!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,2,"CCDKey",0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_clear_maildata'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	pCDKey->ClearAllSaveMailData();
	return 0;
}

//int cdkey_delete_maildata_by_guid(lua_State* L)
//{
//#ifdef _DEBUG
//	tolua_Error err;
//	if(
//		!tolua_isusertype(L,1,"CCDKey",0,&err)||
//		!tolua_isusertype(L,2,"CCDKey",0,&err)||
//		!tolua_isusertype(L,3,"CPlayer",0,&err)||
//		!tolua_isusertype(L,4,"CGUID",0,&err)||
//		!tolua_isnoobj(L,5,&err) )
//	{
//		tolua_error(L,"#ferror in function 'cdkey_delete_maildata_by_guid'.",&err);
//		return 0;
//	}
//#endif
//	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
//	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L, 3, 0);
//	const CGUID guid = *(CGUID*)tolua_tousertype(L, 4, 0);
//	pCDKey->DeleteSaveMailData(pPlayer, guid);
//	return 0;
//}

//-----------------------------------------------------------------------------------------
// 对玩家身上的好友进行数据保存和核对bind
// by MartySa 2009.1.12
//-----------------------------------------------------------------------------------------

int cdkey_svae_savelinkman(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,3,"CPlayer",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_svae_savelinkman'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L,3, 0);
	pCDKey->SaveLinkManPanleGroup(pPlayer);
	return 0;
}

//
int cdkey_clear_savelinkman(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(
		!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,2,"CCDKey",0,&err)||
		!tolua_isusertype(L,3,"CPlayer",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_clear_savelinkman'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L, 3, 0);
	pCDKey->ClearLinkManAllSavePanleGroup(pPlayer);
	return 0;
}

//-----------------------------------------------------------------------------------------
// 对玩家身上的行会进行数据保存和核对函数bind
// by MartySa 2009.1.6
//-----------------------------------------------------------------------------------------
int cdkey_svae_svaefaction(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,3,"CPlayer",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_svae_faction'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L,3, 0);
	pCDKey->SaveFaction(pPlayer);
	return 0;
}

//
int cdkey_clear_svaefaction(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(
		!tolua_isusertype(L,1,"CCDKey",0,&err)||
		!tolua_isusertype(L,2,"CCDKey",0,&err)||
		!tolua_isusertype(L,3,"CPlayer",0,&err)||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'cdkey_clear_svaefaction'.",&err);
		return 0;
	}
#endif
	CCDKey *pCDKey   = (CCDKey*)tolua_tousertype(L, 2, 0);
	CPlayer *pPlayer = (CPlayer*)tolua_tousertype(L, 3, 0);
	pCDKey->ClearSaveFaction(pPlayer);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
void cdkey_reg_types (lua_State* L)
{
	tolua_usertype(L,"CCDKey");
}
//打开baseobject库
int  cdkey_open(lua_State* L)
{
	tolua_open(L);
	cdkey_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"CDKey","CCDKey","",collect_cdkey);
			tolua_beginmodule(L,"CDKey");
			{
				//册CPlayer类的函数
				tolua_function(L,"new",			cdkey_new);
				tolua_function(L,"new_local",	cdkey_new_local);
				tolua_function(L,".call",		cdkey_new_local);
				tolua_function(L,"delete",		cdkey_delete);
				//
				tolua_function(L,"get_cdkey",	cdkey_get_cdkey);
				tolua_function(L,"find_cdkey",	cdkey_find_cdkey);
				tolua_function(L,"add_cdkey",	cdkey_add_cdkey);

				tolua_function(L,"add_data",	cdkey_add_data);
				tolua_function(L,"check_data",	cdkey_check_data);
				
				//	邮件保存和核对
				//cdkey_svae_maildata
				tolua_function(L,"save_maildata",	cdkey_svae_maildata);	
				//cdkey_clear_maildata
				tolua_function(L,"clear_maildata",	cdkey_clear_maildata);
					
				//cdkey_add_savemaildata_by_txt
				//tolua_function(L,"add_maildata",	cdkey_add_savemaildata_by_txt);
				//cdkey_check_maildata
				//tolua_function(L,"check_maildata",	cdkey_check_maildata);
				
				//cdkey_delete_maildata_by_guid
				//tolua_function(L,"delete_maildata",	cdkey_delete_maildata_by_guid);
				//
				// 好友保存和核对
				//cdkey_svae_savelinkman
				tolua_function(L,"save_linkman",	cdkey_svae_savelinkman);
				//cdkey_clear_savelinkman
				tolua_function(L,"clear_linkman",	cdkey_clear_savelinkman);

				// 行会保存和核对
				// cdkey_svae_svaefaction
				tolua_function(L,"save_faction",	cdkey_svae_svaefaction);
				// cdkey_clear_savelinkman
				tolua_function(L,"clear_faction",	cdkey_clear_savelinkman);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}