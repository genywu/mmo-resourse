//=============================================================================
/**
*  file: MessageBind.cpp
*
*  Brief:为CMessage类产生的扩展脚本函数
*
*  Authtor:wangqiao
*	
*	Date:2007-7-25
*/
//=============================================================================
#include "stdafx.h"
#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/GameTestDlg.h"

#include "../Net/Message.h"
#include "../App/Player.h"
#include "../Script/ScriptSys.h"
#include "..\Utility\TestMsgType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int collect_message(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_delete'.",&err);
		return 0;
	}
#endif
	CMessage *self = (CMessage*)tolua_touserdata(L,1,0);
	CBaseMessage::FreeMsg(self);
	return 0;
}

int message_new(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_new'.",&err);
		return 0;
	}
#endif
	long lType = (long)tolua_tonumber(L,2,0);
	CMessage *pMsg = (CMessage*)CBaseMessage::AllocMsg();
	pMsg->Reset();
	pMsg->Init(lType);
	tolua_pushusertype(L,(void*)pMsg, "CMessage");
	return 1;
}

int message_new_local(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertable(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_new_local'.",&err);
		return 0;
	}
#endif
	long lType = (long)tolua_tonumber(L,2,0);
	CMessage* pMsg = (CMessage*)CBaseMessage::AllocMsg();
	pMsg->Reset();
	pMsg->Init(lType);
	tolua_pushusertype_and_takeownership(L,(void*)pMsg,"CMessage");
	return 1;
}

int message_delete(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_delete'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	CBaseMessage::FreeMsg(self);


	return 0;
}


//添加数据
int message_addnum(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_addnum'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);

	long number = (long)tolua_tonumber(L,2,0);
	long size = (long)tolua_tonumber(L,3,0);

	if(size == 1)
		self->Add(static_cast<BYTE>(number));
	else if(size == 2)
		self->Add(static_cast<short>(number));
	else if(size == 3)
		self->Add(static_cast<float>(number));
	else if(size == 4)
		self->Add(static_cast<long>(number));
	else if(size == 8)
		self->Add(static_cast<LONG64>(number));	
	return 0;

}
int message_addstring(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_addstring'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	const char* str = tolua_tostring(L,2,0);

	if ( self != NULL )
	{
		self->Add(str);
	}	

	return 0;
}
int message_adddata(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_adddata'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	BYTE* data = (BYTE*)tolua_touserdata(L,2,0);
	int size = (int)tolua_tonumber(L,3,0);
	self->AddEx(data,size);
	return 0;
}

int message_adddata_string(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_adddata_string'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	char* data = (char*)tolua_tostring(L,2,0);
	int size = (int)tolua_tonumber(L,3,0);
	self->AddEx((void*)data,size);
	return 0;
}

int message_adddata_number(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_adddata_number'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	lua_Number data = (lua_Number)tolua_tonumber(L,2,0);
	int size = (int)tolua_tonumber(L,3,0);
	self->AddEx((void*)&data,size);
	return 0;
}

int message_add_ex(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		//!tolua_isuserdata(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_add_ex'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	const char* strValueType = tolua_tostring(L,2,0);
	void* pValue = NULL; //(void*)tolua_touserdata(L,3,0);
	if( !strcmp(strValueType,"char") )
	{
		char c = *(char*)pValue;
	}
	else if( !strcmp(strValueType,"BYTE") )
	{
		BYTE c = *(BYTE*)pValue;
	}
	else if( !strcmp(strValueType,"short") )
	{
		short c = *(short*)pValue;
	}
	else if( !strcmp(strValueType,"WORD") )
	{
		WORD c = *(WORD*)pValue;
	}
	else if( !strcmp(strValueType,"int") )
	{
		int c = *(int*)pValue;
	}
	else if( !strcmp(strValueType,"UINT") )
	{
		UINT c = *(UINT*)pValue;
	}
	else if( !strcmp(strValueType,"long") )
	{
		long l = (long)tolua_tonumber(L,3,0);
		pValue = (void*)&l;
	}
	else if( !strcmp(strValueType,"DWORD") )
	{
		DWORD c = *(DWORD*)pValue;
	}
	else if( !strcmp(strValueType,"float") )
	{
		float c = *(float*)pValue;
	}
	else if( !strcmp(strValueType,"md5") )
	{
		//	md5 c = *(md5*)pValue;
	}
	else if( !strcmp(strValueType,"string") )
	{
		string c = *(string*)pValue;
	}

	return 0;
}


//消息里边添加需要加密的数据的接口
int message_addmd5data(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isstring(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_addencryptdata'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	const char* str = tolua_tostring(L,2,0);		
	unsigned char strMD5[16]="";
	TMD5 tempMD5;
	tempMD5.MessageDigest((unsigned char *)str,(unsigned int)strlen(str),strMD5);
	self->AddEx(strMD5,16);
	return 0;
}
int message_get_read_db(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_get_read_db'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	DBReadSet * readDB = new DBReadSet;
	self->GetDBReadSet(*readDB);
	tolua_pushuserdata(L,readDB);
	return 1;
}

int message_delete_read_db(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isuserdata(L,2,0,&err)||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_delete_read_db'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	DBReadSet * readDB = (DBReadSet*)tolua_touserdata(L,2,0);
	if(readDB)
		SAFE_DELETE(readDB);
	return 0;
}

//得数据
int message_getnum(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getnum'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	int size = (int)tolua_tonumber(L,2,0);
	lua_Number number = 0;// Lua_Number ：double
	if(size == 1)
		number = self->GetByte();
	else if(size == 2)
		number = self->GetShort();
	else if(size == 4)
		number = self->GetLong();
	else if(size == 8)
		number = (double)self->GetLONG64();
	tolua_pushnumber(L,number);
	return 1;
}
int message_getstring(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getstring'.",&err);
		return 0;
	}
#endif
	char str[256]="";
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	self->GetStr(str,256);
	tolua_pushstring(L,str);
	return 1;
}
int message_get_db(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isuserdata(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'message_get_db'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	BYTE* data = (BYTE*)tolua_touserdata(L,2,0);
	int size = (int)tolua_tonumber(L,3,0);
	self->GetEx(data,size);
	return 0;
}

int message_gettype(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_gettype'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	long lType = (long)self->GetType();
	tolua_pushnumber(L,(lua_Number)lType);
	return 1;
}

int message_getsize(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getsize'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	long lSize = (long)self->GetSize();
	tolua_pushnumber(L,(lua_Number)lSize);
	return 1;
}

int message_getbigtype(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getbigtype'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	long lType = self->GetType();
	lType = lType - (lType&0x00000ff);
	tolua_pushnumber(L,(lua_Number)lType);
	return 1;
}

int message_getsocketid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getsocketid'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	tolua_pushnumber(L,(lua_Number)self->GetSocketID());
	return 1;
}

int message_set_socketid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0 ,&err)  ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_set_socketid'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	long socketid = (long)tolua_tonumber(L,2,0);
	self->SetSocketID(socketid);
	return 0;
}

int message_sendbysocket(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_sendbysocket'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	if( self->GetMsgData().size() == 0 || !self ) return 0;
	long socketid = (long)tolua_tonumber(L,2,0);
	self->SendBySocket(socketid);
	return 0;
}

int message_sendbyplayer(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_sendbyplayer'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	const CGUID* playerid = (CGUID*)tolua_tousertype(L,2,0);
	self->SendByPlayer(*playerid);
	return 0;
}

int OnScriptMessage(lua_State* L,CMessage* pMsg)
{
	lua_pushvalue(L,LUA_GLOBALSINDEX);
	lua_pushstring(L, "OnMessage");
	lua_rawget(L,-2);
	tolua_pushusertype(L,(void*)pMsg,"CMessage");
	int nRet = lua_pcall(L,1,1,0);
	//
	if(nRet != 0)
	{
		char strInfo[512]="";
		sprintf(strInfo,"error in function <OnScriptMessage>(ErroNum:%d,info:%s)",nRet,tolua_tostring(L,-1,0));
		PutDebugString(strInfo);
		lua_pop(L,2);
		return 0;
	}
	nRet = (int)tolua_tonumber(L,-1,0);
	lua_pop(L,1);
	return nRet;
}

int message_getguid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getguid'.",&err);
		return 0;
	}
#endif 
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,2,0);
	self->GetGUID(*guid);
	return 0;
}
int message_addguid(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err)	 ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'message_addguid'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	CGUID* guid = (CGUID*)tolua_tousertype(L,2,0);

	if ( &(*guid) != NULL )
	{
		self->Add(*guid);
	}
	return 0;
}

int message_reset(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isusertype(L,1,"CMessage",0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'message_reset'.",&err);
		return 0;
	}
#endif
	CMessage* self = (CMessage*)tolua_tousertype(L,1,0);
	self->Reset();
	return 0;
}

//For MappingFile 
// by MartySa 2009.2.3
//发送一个消息数据给内存映射文件
// int message_sendtoclient(lua_State* L)
// {
// #ifdef _DEBUG
// 	tolua_Error err;
// 	if(!tolua_isusertype(L,1,"CMessage",0,&err) ||
// 		!tolua_isnumber(L,2,0,&err) ||
// 		!tolua_isnoobj(L,3,&err) )
// 	{
// 		tolua_error(L,"#ferror in function 'message_sendtoclient'.",&err);
// 		return 0;
// 	}
// #endif
// 	CMessage *self = (CMessage*)tolua_tousertype(L,1,0);
// 	unsigned uID   = (unsigned)tolua_tonumber(L,2,0);
// 	tolua_pushboolean(L, self->SendToClient(uID));
// 	return 1;
// }

//得到随机消息头，供随机消息功能使用
int message_getradmsghead(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'message_getradmsghead'.",&err);
		return 0;
	}
#endif
	tolua_pushnumber(L,MsgRad[random(1680)]);
	return 1;
}



void message_reg_types (lua_State* L)
{
	tolua_usertype(L,"CMessage");
	tolua_usertype(L,"CMemServer");
}

//打开baseobject库
int  message_open(lua_State* L)
{
	tolua_open(L);
	message_reg_types(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			tolua_cclass(L,"Message","CMessage","",collect_message);
			tolua_beginmodule(L,"Message");
			{
				//注册CMessage构造、析构类的函数
				tolua_function(L,"new",message_new);
				tolua_function(L,"new_local",message_new_local);
				tolua_function(L,".call",message_new_local);
				tolua_function(L,"delete",message_delete);

				//注册函数
				tolua_function(L,"set_socketid",message_set_socketid);
				tolua_function(L,"reset",message_reset);

				tolua_function(L,"get_big_type",message_getbigtype);
				tolua_function(L,"get_type",message_gettype);
				tolua_function(L,"get_size",message_getsize);
				tolua_function(L,"get_socketid",message_getsocketid);
				tolua_function(L,"get_guid",message_getguid);
				tolua_function(L,"get_num",message_getnum);
				tolua_function(L,"get_string",message_getstring);
				tolua_function(L,"get_db",message_get_db);

				tolua_function(L,"get_read_db",message_get_read_db);
				tolua_function(L,"del_read_db",message_delete_read_db);

				tolua_function(L,"send_bysocket",message_sendbysocket);
				tolua_function(L,"send_byplayer",message_sendbyplayer);


				tolua_function(L,"add_guid",message_addguid);				//	10.15+
				tolua_function(L,"add_num",message_addnum);
				tolua_function(L,"add_string",message_addstring);
				tolua_function(L,"add_data",message_adddata);
				tolua_function(L,"add_data_string", message_adddata_string);
				tolua_function(L,"add_data_number", message_adddata_number);
				tolua_function(L,"add_md5_data",message_addmd5data);

				tolua_function(L,"add_ex",		message_add_ex);

				// for MappingFile
				//	by MartySa 2009.2.3
				//tolua_function(L,"send_to_client",		message_sendtoclient);

				tolua_function(L,"get_rad_msghead",		message_getradmsghead);

			}
			tolua_endmodule(L);
		}
		tolua_endmodule(L);

		tolua_module(L,"MsgType",0);
		tolua_beginmodule(L,"MsgType");
		{
			tolua_constant(L,"BASEMSG_Socket_Close",BASEMSG_Socket_Close);
			//注册消息类型常量
			//MSG_C2L start
			tolua_constant(L,"MSG_C2L",MSG_C2L);			
			tolua_constant(L,"MSG_C2L_LOG",MSG_C2L_LOG);
			tolua_constant(L,"MSG_C2L_LOG_QUESTCDKEY",MSG_C2L_LOG_QUESTCDKEY);
			tolua_constant(L,"MSG_C2L_LOG_QUEST_PLAYERBASELIST",MSG_C2L_LOG_QUEST_PLAYERBASELIST);
			tolua_constant(L,"MSG_C2L_LOG_QUEST_PLAYERDATA",MSG_C2L_LOG_QUEST_PLAYERDATA);
			tolua_constant(L,"MSG_C2L_LOG_QUESTCREATE",MSG_C2L_LOG_QUESTCREATE);
			tolua_constant(L,"MSG_C2L_LOG_QUESTDELETE",MSG_C2L_LOG_QUESTDELETE);
			tolua_constant(L,"MSG_C2L_LOG_QUESTRESTORE",MSG_C2L_LOG_QUESTRESTORE);
			tolua_constant(L,"MSG_C2L_LOG_QUESTEXIT",MSG_C2L_LOG_QUESTEXIT);

			//	client ---> GameClient
			tolua_constant(L,"MSG_C2S",MSG_C2S);
			tolua_constant(L,"MSG_C2S_LOG",MSG_C2S_LOG);
			tolua_constant(L,"MSG_C2S_LOG_PLAYEREXITGAME",MSG_C2S_LOG_PLAYEREXITGAME);
			tolua_constant(L,"MSG_C2S_LOG_PLAYERENTER",MSG_C2S_LOG_PLAYERENTER);

			tolua_constant(L,"MSG_C2S_RGN",MSG_C2S_RGN);				//Y
			tolua_constant(L,"MSG_C2S_RGN_ENTER",MSG_C2S_RGN_ENTER);	//Yes
			tolua_constant(L,"MSG_C2S_RGN_EXIT",MSG_C2S_RGN_EXIT);		//Y								//退出场景

			tolua_constant(L,"MSG_C2S_SHAPE",MSG_C2S_SHAPE);					
			tolua_constant(L,"MSG_C2S_SHAPE_CHANGEDIR",MSG_C2S_SHAPE_CHANGEDIR);//Y						// 改变方向
			tolua_constant(L,"MSG_C2S_SHAPE_SETPOS",MSG_C2S_SHAPE_SETPOS);		//Y						// 瞬移
			tolua_constant(L,"MSG_C2S_SHAPE_MOVE",MSG_C2S_SHAPE_MOVE);
			tolua_constant(L,"MSG_C2S_SHAPE_STOPMOVE",MSG_C2S_SHAPE_STOPMOVE);
			tolua_constant(L,"MSG_C2S_SHAPE_CHANGEMOVEMODE",MSG_C2S_SHAPE_CHANGEMOVEMODE);
			tolua_constant(L,"MSG_C2S_SHAPE_QUESTINFO",MSG_C2S_SHAPE_QUESTINFO);//Y						// 主动向服务器请求shape的信息
			tolua_constant(L,"MSG_C2S_SHAPE_EMOTION",MSG_C2S_SHAPE_EMOTION);	//No Function						// 表情动作

			tolua_constant(L,"MSG_C2S_PLAYER",MSG_C2S_PLAYER);
			//		tolua_constant(L,"MSG_C2S_PLAYER_ADDPOINT",MSG_C2S_PLAYER_ADDPOINT);			//N	f					// 加点
			tolua_constant(L,"MSG_C2S_PLAYER_QUEST_RELIVE",MSG_C2S_PLAYER_QUEST_RELIVE);	//N	f			// 请求复活
			tolua_constant(L,"MSG_C2S_PLAYER_TOUCHNPC",MSG_C2S_PLAYER_TOUCHNPC);			//Y					// 触发NPC
			tolua_constant(L,"MSG_C2S_PLAYER_USEITEM",MSG_C2S_PLAYER_USEITEM);				//Y					// client到gameserver验证一个消耗道具的使用
			tolua_constant(L,"MSG_C2S_PLAYER_PKSWITCH",MSG_C2S_PLAYER_PKSWITCH);			//No Function					// 切换PK开关
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADELOCK",MSG_C2S_PLAYER_TRADELOCK);			//Y			//进入锁定状态
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADEREQUEST",MSG_C2S_PLAYER_TRADEREQUEST);	//Y										//交易请求
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADERESPONSE",MSG_C2S_PLAYER_TRADERESPONSE);	//Y									//客户端同意或拒绝交易请求
			//tolua_constant(L,"MSG_C2S_PLAYER_PUTGOODSTOTRADEPAGE",MSG_C2S_PLAYER_PUTGOODSTOTRADEPAGE);			//No Function				//客户端放一个物品到交易展示区
			//tolua_constant(L,"MSG_C2S_PLAYER_FETCHGOODSFROMTRADEPAGE",MSG_C2S_PLAYER_FETCHGOODSFROMTRADEPAGE);	//N	F			//玩家从展示区拿出一个物品
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADECHANGEMONEY",MSG_C2S_PLAYER_TRADECHANGEMONEY);				//N	f	//客户端请求调整金钱
			//tolua_constant(L,"MSG_C2S_PLAYER_WAITFORTRADE",MSG_C2S_PLAYER_WAITFORTRADE);						//N	f		//客户端通知进入交易等待状态
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADECANCEL",MSG_C2S_PLAYER_TRADECANCEL);							//Y		//客户端请求取消交易
			//tolua_constant(L,"MSG_C2S_PLAYER_HELPREQUEST",MSG_C2S_PLAYER_HELPREQUEST);							//N	f	// 玩家请求帮助
			////			tolua_constant(L,"MSG_C2S_PLAYER_UpdateDisplayHeadPiece",MSG_C2S_PLAYER_UpdateDisplayHeadPiece);	//N f			//更新是否显示头盔信息
			////			tolua_constant(L,"MSG_C2S_PLAYER_CHANGE_QUEST_TIMEREQUEST",MSG_C2S_PLAYER_CHANGE_QUEST_TIMEREQUEST);//Y-->未调用
			//tolua_constant(L,"MSG_C2S_PLAYER_TRADING",MSG_C2S_PLAYER_TRADING);									//Y		// 玩家确定后点交易时发送

			tolua_constant(L,"MSG_C2S_OTHER",MSG_C2S_OTHER);						//Y
			tolua_constant(L,"MSG_C2S_OTHER_TALK",MSG_C2S_OTHER_TALK);				//Y
			tolua_constant(L,"MSG_C2S_OTHER_SENDTALKBOXSELECT",MSG_C2S_OTHER_SENDTALKBOXSELECT);	// n ScriptID					// 客户端返回TalkBox的选择
			tolua_constant(L,"MSG_C2S_OTHER_VIEWGOODSLINK",MSG_C2S_OTHER_VIEWGOODSLINK); //n f
			tolua_constant(L,"MSG_C2S_OTHER_CONNECTGAMESERVER",MSG_C2S_OTHER_CONNECTGAMESERVER); //Y
			tolua_constant(L,"MSG_C2S_OTHER_CHANGENAME",MSG_C2S_OTHER_CHANGENAME);	//n f
			tolua_constant(L,"MSG_C2S_OTHER_QUESTNPCLIST",MSG_C2S_OTHER_QUESTNPCLIST);//n f
			tolua_constant(L,"MSG_C2S_OTHER_TALK_WORLD",MSG_C2S_OTHER_TALK_WORLD);	//y
			tolua_constant(L,"MSG_C2S_OTHER_TALK_COUNTRY",MSG_C2S_OTHER_TALK_COUNTRY);//Y
			//			tolua_constant(L,"MSG_C2S_OTHER_REQUEST_LOANTIMELIMIT",MSG_C2S_OTHER_REQUEST_LOANTIMELIMIT);	//n f			// 请求还款期限
			tolua_constant(L,"MSG_C2S_OTHER_QUESTGOD",MSG_C2S_OTHER_QUESTGOD);	//n f					// 请求天神降临
			tolua_constant(L,"MSG_C2S_OTHER_PING", MSG_C2S_OTHER_PING);	

			tolua_constant(L,"MSG_C2S_GOODS",MSG_C2S_GOODS);
			tolua_constant(L,"MSG_C2S_FETCHGOOD",MSG_C2S_FETCHGOOD);	//n f						//player取一个物品
			tolua_constant(L,"MSG_C2S_PUTDOWNGOOD",MSG_C2S_PUTDOWNGOOD);//n f						//player放下一个物品
			tolua_constant(L,"MSG_C2S_CHANGEGOOD",MSG_C2S_CHANGEGOOD);//n f								//player调换物品位置
			tolua_constant(L,"MSG_C2S_DROPGOOD",MSG_C2S_DROPGOOD);//n f										//player请求手上物品
			tolua_constant(L,"MSG_C2S_PICKGOOD",MSG_C2S_PICKGOOD);//n f										//player请求拣物品
			tolua_constant(L,"MSG_C2S_DROPMONEY",MSG_C2S_DROPMONEY);//n f							
			tolua_constant(L,"MSG_C2S_PICKMONEY",MSG_C2S_PICKMONEY);//n f							
			tolua_constant(L,"MSG_C2S_SETHOTKEY",MSG_C2S_SETHOTKEY);	// n write							//player请求设置HOTKEY
			tolua_constant(L,"MSG_C2S_CLEARHOTKEY",MSG_C2S_CLEARHOTKEY);// n write									//取消热键设置
			tolua_constant(L,"MSG_C2S_EXCHANGEHOTKEY",MSG_C2S_EXCHANGEHOTKEY);// n f									//替换热键设置
			tolua_constant(L,"MSG_C2S_VIEWEQUIP",MSG_C2S_VIEWEQUIP);// n  f										//查看玩家装备
			tolua_constant(L,"MSG_C2S_CLOSEGOODSUPGRADE",MSG_C2S_CLOSEGOODSUPGRADE);//n f
			tolua_constant(L,"MSG_C2S_GOODSUPGRADE_PUT",MSG_C2S_GOODSUPGRADE_PUT);//n f								//放置一个物品到升级界面
			tolua_constant(L,"MSG_C2S_GOODSUPGRADE_FETCH",MSG_C2S_GOODSUPGRADE_FETCH);//n f								//从升级界面中取出一个物品
			tolua_constant(L,"MSG_C2S_GOODSUPGRADE_CHANGE",MSG_C2S_GOODSUPGRADE_CHANGE);//n f							//在升级界面中交换一个物品
			tolua_constant(L,"MSG_C2S_GOODSUPGRADE_UPGRADE",MSG_C2S_GOODSUPGRADE_UPGRADE);//n f                           //client请求升级一个物品
			tolua_constant(L,"MSG_C2S_ENCHASE_OPEN_HOLE",MSG_C2S_ENCHASE_OPEN_HOLE);//n write
			tolua_constant(L,"MSG_C2S_ENCHASE_CLOSE_HOLE",MSG_C2S_ENCHASE_CLOSE_HOLE);//n write
			tolua_constant(L,"MSG_C2S_ENCHASE_ENCHASECARD",MSG_C2S_ENCHASE_ENCHASECARD);
			tolua_constant(L,"MSG_C2S_ENCHASE_REMOVECARD",MSG_C2S_ENCHASE_REMOVECARD);
			tolua_constant(L,"MSG_C2S_ENCHASE_ENDSESSION",MSG_C2S_ENCHASE_ENDSESSION);
			tolua_constant(L,"MSG_C2S_GOODS_CONTAINER",MSG_C2S_GOODS_CONTAINER);	//n f							// 物品容器返回

			//! GoodsExManage应用
			tolua_constant(L,"MSG_C2S_GOODS_CLOSE_EX_WND",MSG_C2S_GOODS_CLOSE_EX_WND);
			tolua_constant(L,"MSG_C2S_GOODS_DISASSEMBLE_PREVIEW",MSG_C2S_GOODS_DISASSEMBLE_PREVIEW);
			tolua_constant(L,"MSG_C2S_GOODS_DISASSEMBLE_DO",MSG_C2S_GOODS_DISASSEMBLE_DO);
			tolua_constant(L,"MSG_C2S_GOODS_SYNTHESIZE_PREVIEW",MSG_C2S_GOODS_SYNTHESIZE_PREVIEW);
			tolua_constant(L,"MSG_C2S_GOODS_SYNTHESIZE_DO",MSG_C2S_GOODS_SYNTHESIZE_DO);
			tolua_constant(L,"MSG_C2S_GOODS_BIND",MSG_C2S_GOODS_BIND);
			tolua_constant(L,"MSG_C2S_GOODS_FROST",MSG_C2S_GOODS_FROST);
			tolua_constant(L,"MSG_C2S_GOODS_UPGRADE_VIEW",MSG_C2S_GOODS_UPGRADE_VIEW);
			tolua_constant(L,"MSG_C2S_GOODS_UPGRADE_DO",MSG_C2S_GOODS_UPGRADE_DO);
			//			tolua_constant(L,"MSG_C2S_GOODS_STUFF_SYNTHESIZE",MSG_C2S_GOODS_STUFF_SYNTHESIZE);
			//			tolua_constant(L,"MSG_C2S_GOODS_PLUG_EVIL_VIEW",MSG_C2S_GOODS_PLUG_EVIL_VIEW);
			tolua_constant(L,"MSG_C2S_GOODS_PLUG_EVIL_DO",MSG_C2S_GOODS_PLUG_EVIL_DO);
			//			tolua_constant(L,"MSG_C2S_GOODS_EVIL_BACKOUT",MSG_C2S_GOODS_EVIL_BACKOUT);
			tolua_constant(L,"MSG_C2S_DEPOT_INPUT_PWD",MSG_C2S_DEPOT_INPUT_PWD);
			tolua_constant(L,"MSG_C2S_DEPOT_REQUEST_CLOSE",MSG_C2S_DEPOT_REQUEST_CLOSE);
			tolua_constant(L,"MSG_C2S_DEPOT_REQUEST_CHANGE_PWD",MSG_C2S_DEPOT_REQUEST_CHANGE_PWD);
			tolua_constant(L,"MSG_C2S_DEPOT_REQUEST_BUY_SUBPACK",MSG_C2S_DEPOT_REQUEST_BUY_SUBPACK);

			tolua_constant(L,"MSG_C2S_DEPOT",MSG_C2S_DEPOT);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_C2S_DEPOT_PUTDOWN",MSG_C2S_DEPOT_PUTDOWN);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_FETCH",MSG_C2S_DEPOT_FETCH);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_CHANGE",MSG_C2S_DEPOT_CHANGE);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_SAVEMONEY",MSG_C2S_DEPOT_SAVEMONEY);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_FETCHMONEY",MSG_C2S_DEPOT_FETCHMONEY);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_CHECKPASSWORD",MSG_C2S_DEPOT_CHECKPASSWORD);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_CHANGEPASSWORD",MSG_C2S_DEPOT_CHANGEPASSWORD);//n f
			tolua_constant(L,"MSG_C2S_DEPOT_CLOSE",MSG_C2S_DEPOT_CLOSE);
			//end
			//MSG_C2S_SHOP start
			tolua_constant(L,"MSG_C2S_SHOP",MSG_C2S_SHOP);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_C2S_SHOP_BUY",MSG_C2S_SHOP_BUY);			//Y							//player请求买东西
			tolua_constant(L,"MSG_C2S_SHOP_SELL",MSG_C2S_SHOP_SELL);		//Y		//player卖一个物品回商店
			tolua_constant(L,"MSG_C2S_SHOP_BUYBACK",MSG_C2S_SHOP_BUYBACK);	//NPC商店回购
			tolua_constant(L,"MSG_C2S_SHOP_REPAIR",MSG_C2S_SHOP_REPAIR);	//Y								//在商店中修理一个物品
			tolua_constant(L,"MSG_C2S_SHOP_REPAIRALL",MSG_C2S_SHOP_REPAIRALL); //Y	//商店中修理全部物品
			tolua_constant(L,"MSG_C2S_SHOP_DURUPDATE",MSG_C2S_SHOP_DURUPDATE);	//N								//耐久更新
			tolua_constant(L,"MSG_C2S_SHOP_CLOSE",MSG_C2S_SHOP_CLOSE);	//n f										//关闭个人商店
			tolua_constant(L,"MSG_C2S_NPCSHOP_OPEN",MSG_C2S_NPCSHOP_OPEN);		// n f	//打开NPC商店（临时使用）
			tolua_constant(L,"MSG_C2S_BUSINESS_BUY",MSG_C2S_BUSINESS_BUY);
			tolua_constant(L,"MSG_C2S_BUSINESS_SELL",MSG_C2S_BUSINESS_SELL);
			tolua_constant(L,"MSG_C2S_BUSINESS_INVEST",MSG_C2S_BUSINESS_INVEST);	//商业贸易点投资

            tolua_constant(L,"MSG_C2S_QUEST_PLAYERRANKS",MSG_C2S_QUEST_PLAYERRANKS);
			//end
			//MSG_C2S_TEAM start
			tolua_constant(L,"MSG_C2S_TEAM",MSG_C2S_TEAM);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_C2S_TEAM_QUERY_JOIN",MSG_C2S_TEAM_QUERY_JOIN);	//Y							//邀请加入
			tolua_constant(L,"MSG_C2S_TEAM_QUERY_JOIN_BYNAME",MSG_C2S_TEAM_QUERY_JOIN_BYNAME);	//N	f						//通过名字邀请加入队伍
			tolua_constant(L,"MSG_C2S_TEAM_ANSWER_JOIN",MSG_C2S_TEAM_ANSWER_JOIN);			//Y						//回复邀请加入
			tolua_constant(L,"MSG_C2S_TEAM_CHANGE_LEADER",MSG_C2S_TEAM_CHANGE_LEADER);		//	Y						//改变队长
			tolua_constant(L,"MSG_C2S_TEAM_KICK_PLAYER",MSG_C2S_TEAM_KICK_PLAYER);				//Y					//踢人(队长可以踢所有人，队员可以踢自己)
			tolua_constant(L,"MSG_C2S_TEAM_SetGoods",MSG_C2S_TEAM_SetGoods);				//Y					//修改物品分配设置
			tolua_constant(L,"MSG_C2S_TEAM_CHAT",MSG_C2S_TEAM_CHAT);				//		Y					//队聊
			tolua_constant(L,"MSG_C2S_TEAM_RECRUIT",MSG_C2S_TEAM_RECRUIT);					//Y					//招募控制
			tolua_constant(L,"MSG_C2S_TEAM_BeRecruited",MSG_C2S_TEAM_BeRecruited);			//Y					//应征
			tolua_constant(L,"MSG_C2S_TEAM_TEAMATE_LEAVE",MSG_C2S_TEAM_TEAMATE_LEAVE);			//Y						//离开队伍
			tolua_constant(L,"MSG_C2S_TEAM_TEAMATE_JOIN",MSG_C2S_TEAM_TEAMATE_JOIN);				//N	F				
			tolua_constant(L,"MSG_C2S_TEAM_DESTORY",MSG_C2S_TEAM_DESTORY);			//N	F
			tolua_constant(L,"MSG_C2S_TEAM_CHANGE_ALLOCATION_SCHEME",MSG_C2S_TEAM_CHANGE_ALLOCATION_SCHEME);//N	F
			//end
			//MSG_C2S_LINKMAN
			// 新添加 添加好友
			// by tanhaowen
			//  2008.10.23
			tolua_constant(L,"MSG_C2S_LINKMAN", MSG_C2S_LINKMAN);
			//------------------------------------------------------
			tolua_constant(L,"MSG_C2S_LINKMAN_ADD", MSG_C2S_LINKMAN_ADD);						//A 向 S 申请添加联系人 B
			tolua_constant(L,"MSG_C2S_LINKMAN_DEL", MSG_C2S_LINKMAN_DEL);						//A 向 S 请求删除联系人 B
			tolua_constant(L,"MSG_C2S_LINKMAN_MOVE", MSG_C2S_LINKMAN_MOVE);						//A 向 S 请求移动联系人 B
			tolua_constant(L,"MSG_C2S_LINKMAN_RESPONSE_ADD", MSG_C2S_LINKMAN_RESPONSE_ADD);		//B 向 S 回应 A 的添加请求
			//end
			//MSG_C2S_MAIL
			// 新添加 邮件
			tolua_constant(L,"MSG_C2S_MAIL", MSG_C2S_MAIL);
			//------------------------------------------------------
			tolua_constant(L,"MSG_C2S_MAIL_SEND",		     MSG_C2S_MAIL_SEND);			//! 发信			
			tolua_constant(L,"MSG_C2S_MAIL_RECEIVER",	     MSG_C2S_MAIL_RECEIVER);		//! 收信					
			tolua_constant(L,"MSG_C2S_MAIL_READ",		     MSG_C2S_MAIL_READ);			//！读邮件			
			tolua_constant(L,"MSG_C2S_MAIL_GET_GOODS",       MSG_C2S_MAIL_GET_GOODS);		//! 从邮件中取物品到背包
			tolua_constant(L,"MSG_C2S_MAIL_GET_GOLDS",       MSG_C2S_MAIL_GET_GOLDS);		//! 从邮件中取金币
			tolua_constant(L,"MSG_C2S_MAIL_DELETE",		     MSG_C2S_MAIL_DELETE);			//! 删除邮件
			tolua_constant(L,"MSG_C2S_MAIL_REJECT",		     MSG_C2S_MAIL_REJECT);			//! 拒收邮件
			tolua_constant(L,"MSG_C2S_MAIL_AGREED_PAY",	     MSG_C2S_MAIL_AGREED_PAY);		//! 同意付费
			//tolua_constant(L,"MSG_C2S_MAIL_AGREED_PRESENT",  MSG_C2S_MAIL_AGREED_PRESENT);	//! 同意赠送
			//end

			//MSG_C2S_ANTICHEAT
			// 客户端答题
			tolua_constant(L,"MSG_C2S_ANTICHEAT", MSG_C2S_ANTICHEAT);
			//------------------------------------------------------
			tolua_constant(L,"MSG_C2S_AC_ANSWER", MSG_C2S_AC_ANSWER);
			//end
			//MSG_C2S_SKILL start
			tolua_constant(L,"MSG_C2S_SKILL",MSG_C2S_SKILL);
			tolua_constant(L,"MSG_C2S_SKILL_USE",MSG_C2S_SKILL_USE);										// 使用技能
			tolua_constant(L,"MSG_C2S_SKILL_USE_END",MSG_C2S_SKILL_USE_END);									// 使用技能结束，有些技能松开快捷键的时候发送
			tolua_constant(L,"MSG_C2S_SKILL_STUDY",MSG_C2S_SKILL_STUDY);									// 学习技能
			tolua_constant(L,"MSG_C2S_SKILL_STUDY_BEGIN",MSG_C2S_SKILL_STUDY_BEGIN);								// 向服务器发送打开技能学习导师页面
			tolua_constant(L,"MSG_C2S_DOCCUSKILL_USE",MSG_C2S_DOCCUSKILL_USE);									// 使用副职业技能
			tolua_constant(L,"MSG_C2S_OCCUSKILL_STUDY_BEGIN",MSG_C2S_OCCUSKILL_STUDY_BEGIN);							// 向服务器发送打开副职业技能学习页面
			tolua_constant(L,"MSG_C2S_OCCUSKILL_STUDY",MSG_C2S_OCCUSKILL_STUDY);								// 向服务器发送学习副职业技能消息
			//MSG_C2S_PALYERSHOP	start]
			tolua_constant(L,"MSG_C2S_PLAYERSHOP",MSG_C2S_PLAYERSHOP);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_CREATE",MSG_C2S_PLAYERSHOP_CREATE);		//Y						//请求创建个人商店
			//tolua_constant(L,"MSG_C2S_PLAYERSHOP_GOODSPRICE",MSG_C2S_PLAYERSHOP_GOODSPRICE);//N	f
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_BUYITEM",MSG_C2S_PLAYERSHOP_BUYITEM);		//Y						//购买商品
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_OPENFORBUSINESS",MSG_C2S_PLAYERSHOP_OPENFORBUSINESS);	//Vector 未全部实现					//开张
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_CLOSEDOWN",MSG_C2S_PLAYERSHOP_CLOSEDOWN);	//Y						//关张
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_LOOKGOODS",MSG_C2S_PLAYERSHOP_LOOKGOODS);	//Y						//察看商品
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_CLOSE",MSG_C2S_PLAYERSHOP_CLOSE);		//Y								//关闭个人商店
			tolua_constant(L,"MSG_C2S_PLAYERSHOP_QUIT",MSG_C2S_PLAYERSHOP_QUIT);			//Y					//购买者离开个人商店
			//tolua_constant(L,"MSG_C2S_PLAYERSHOP_REMOVE",MSG_C2S_PLAYERSHOP_REMOVE);	//N							//从个人商店上移除物品时的消息
			//end;
			// 容器
			tolua_constant(L,"MSG_C2S_CONTAINER",MSG_C2S_CONTAINER);		//N
			tolua_constant(L,"MSG_C2S_CONTAINER_OBJECT_MOVE",MSG_C2S_CONTAINER_OBJECT_MOVE); //Y
			tolua_constant(L,"MSG_C2S_CONTAINER_OBJECT_UNLOCK",MSG_C2S_CONTAINER_OBJECT_UNLOCK);

			// 宠物				//N
			tolua_constant(L,"MSG_C2S_PET",MSG_C2S_PET);						//N
			tolua_constant(L,"MSG_C2S_PET_CHANGE_MODE",MSG_C2S_PET_CHANGE_MODE);	//N							//改变宠物当前模式
			tolua_constant(L,"MSG_C2S_PET_CHANGE_ACTION",MSG_C2S_PET_CHANGE_ACTION);	//N							//改变宠物当前动作

			// 点卡寄售 c->s				//N
			tolua_constant(L,"MSG_C2S_CARD",MSG_C2S_CARD);					
			tolua_constant(L,"MSG_C2S_CARD_SELL_REQUEST",MSG_C2S_CARD_SELL_REQUEST);	//N							// 持卡者请求寄售点卡
			tolua_constant(L,"MSG_C2S_CARD_BUY_REQUEST",MSG_C2S_CARD_BUY_REQUEST);  	//N							// 持卡者请求购买点卡
			tolua_constant(L,"MSG_C2S_CARD_CANCEL_REQUEST",MSG_C2S_CARD_CANCEL_REQUEST);	//N						// 取消寄售点卡
			tolua_constant(L,"MSG_C2S_CARD_LIST_REQUEST",MSG_C2S_CARD_LIST_REQUEST);		//N						//请求服务器当前出售的点卡列表
			tolua_constant(L,"MSG_C2S_CARD_SALECOSTS_REQUEST",MSG_C2S_CARD_SALECOSTS_REQUEST);//n							//请求服务器当前出售的点卡手续费 
			tolua_constant(L,"MSG_C2S_CARD_CHARGE_REQUEST",MSG_C2S_CARD_CHARGE_REQUEST);		//n					// 请求充值

			// 国家
			tolua_constant(L,"MSG_C2S_COUNTRY",MSG_C2S_COUNTRY);	//N
			// 增值商店
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP",MSG_C2S_INCREMENTSHOP);
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_OPEN",MSG_C2S_INCREMENTSHOP_OPEN);
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_BUY",MSG_C2S_INCREMENTSHOP_BUY);										//player请求买东西
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_CLOSE",MSG_C2S_INCREMENTSHOP_CLOSE);									//关闭个人商店
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_AFFICHE_REQUEST",MSG_C2S_INCREMENTSHOP_AFFICHE_REQUEST);							// 公告窗口开
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_REPAY",MSG_C2S_INCREMENTSHOP_REPAY);									// 偿还借贷
			tolua_constant(L,"MSG_C2S_INCREMENTSHOP_GETLOG_REQUEST",MSG_C2S_INCREMENTSHOP_GETLOG_REQUEST);							// 请求增值日志
			tolua_constant(L,"MSG_C2S_STATE",MSG_C2S_STATE);
			tolua_constant(L,"MSG_C2S_STATE_END",MSG_C2S_STATE_END);											//状态结束

			////////////MSG_L2C start
			tolua_constant(L,"MSG_L2C",MSG_L2C);
			///MSG_L2C_LOG start
			tolua_constant(L,"MSG_L2C_LOG",MSG_L2C_LOG);
			tolua_constant(L,"MSG_L2C_LOG_ANSWERCDKEY",MSG_L2C_LOG_ANSWERCDKEY);
			tolua_constant(L,"MSG_L2C_LOG_ANSWER_PLAYERBASELIST",MSG_L2C_LOG_ANSWER_PLAYERBASELIST);
			tolua_constant(L,"MSG_L2C_LOG_ANSWER_PLAYERDATA",MSG_L2C_LOG_ANSWER_PLAYERDATA);
			tolua_constant(L,"MSG_L2C_LOG_ANSWERCREATE",MSG_L2C_LOG_ANSWERCREATE);
			tolua_constant(L,"MSG_L2C_LOG_ANSWERDELETE",MSG_L2C_LOG_ANSWERDELETE);
			tolua_constant(L,"MSG_L2C_LOG_ANSWERRESTORE",MSG_L2C_LOG_ANSWERRESTORE);
			tolua_constant(L,"MSG_L2C_LOG_UPDATEQUEUENUM",MSG_L2C_LOG_UPDATEQUEUENUM);
			tolua_constant(L,"MSG_L2C_LOG_SENDACCOUNTINFO",MSG_L2C_LOG_SENDACCOUNTINFO);
			tolua_constant(L,"MSG_L2C_LOG_UPDATEWORLDSERVERINFO",MSG_L2C_LOG_UPDATEWORLDSERVERINFO);
			///MSG_L2C_LOG end
			////////////MSG_L2C end

			////////////MSG_S2C start
			tolua_constant(L,"MSG_S2C",MSG_S2C);
			///MSG_S2C_LOG start
			tolua_constant(L,"MSG_S2C_LOG",MSG_S2C_LOG);
			tolua_constant(L,"MSG_S2C_LOG_ANSWER_PLAYERDATA",MSG_S2C_LOG_ANSWER_PLAYERDATA);
			tolua_constant(L,"MSG_S2C_LOG_GAMESERVERTIME",MSG_S2C_LOG_GAMESERVERTIME);
			tolua_constant(L,"MSG_S2C_LOG_SEQUENCESTRING",MSG_S2C_LOG_SEQUENCESTRING);
			//			tolua_constant(L,"MSG_S2C_LOG_PLAYER_ONLINE",MSG_S2C_LOG_PLAYER_ONLINE);
			//			tolua_constant(L,"MSG_S2C_LOG_PLAYER_OFFLINE",MSG_S2C_LOG_PLAYER_OFFLINE);

			///MSG_S2C_RGN start
			tolua_constant(L,"MSG_S2C_RGN",MSG_S2C_RGN);
			tolua_constant(L,"MSG_S2C_RGN_ENTER",MSG_S2C_RGN_ENTER);			//Y
			tolua_constant(L,"MSG_S2C_RGN_ADDSHAPE",MSG_S2C_RGN_ADDSHAPE);		//Y
			tolua_constant(L,"MSG_S2C_RGN_ADDSUMMONEDSHAPE",MSG_S2C_RGN_ADDSUMMONEDSHAPE);							// 召唤的Shape加入场景
			tolua_constant(L,"MSG_S2C_RGN_DELSHAPE",MSG_S2C_RGN_DELSHAPE);		//Y							// 有对象离开场景
			tolua_constant(L,"MSG_S2C_RGN_CHANGE",MSG_S2C_RGN_CHANGE);			//Y							// 服务器命令玩家切换场景
			tolua_constant(L,"MSG_S2C_RGN_CHANGESERVER",MSG_S2C_RGN_CHANGESERVER);	//Y **								// 服务器命令玩家切换场景服务器
			tolua_constant(L,"MSG_S2C_RGN_CHANGENAME",MSG_S2C_RGN_CHANGENAME);
			tolua_constant(L,"MSG_S2C_RGN_ANSWER_CREATE",MSG_S2C_RGN_ANSWER_CREATE);
			tolua_constant(L,"MSG_S2C_RGN_ANSWER_DELETE",MSG_S2C_RGN_ANSWER_DELETE);
			///MSG_S2C_RGN end

			///MSG_S2C_SHAPE start
			tolua_constant(L,"MSG_S2C_SHAPE",MSG_S2C_SHAPE);
			tolua_constant(L,"MSG_S2C_SHAPE_CHANGEDIR",MSG_S2C_SHAPE_CHANGEDIR);	//Y							// 改变方向
			tolua_constant(L,"MSG_S2C_SHAPE_CHECKPOS",MSG_S2C_SHAPE_CHECKPOS);									// 坐标效验
			tolua_constant(L,"MSG_S2C_SHAPE_SETPOS",MSG_S2C_SHAPE_SETPOS);			//Y						// 瞬移
			tolua_constant(L,"MSG_S2C_SHAPE_FORCEMOVE",MSG_S2C_SHAPE_FORCEMOVE);	//Y+N						// 强制移动
			//
			tolua_constant(L,"MSG_S2C_SHAPE_MOVE",MSG_S2C_SHAPE_MOVE);
			tolua_constant(L,"MSG_S2C_SHAPE_STOPMOVE",MSG_S2C_SHAPE_STOPMOVE);
			tolua_constant(L,"MSG_S2C_SHAPE_CHANGEMOVEMODE",MSG_S2C_SHAPE_CHANGEMOVEMODE);
			tolua_constant(L,"MSG_S2C_SHAPE_MOVETILE",MSG_S2C_SHAPE_MOVETILE);		//N							// 服务器发来消息移动一格
			tolua_constant(L,"MSG_S2C_SHAPE_CHANGESTATE",MSG_S2C_SHAPE_CHANGESTATE);//Y								// 改变状态
			//
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_LOCK",MSG_S2C_SHAPE_ATK_LOCK);			//N						// 指示锁定目标
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_BEGIN",MSG_S2C_SHAPE_ATK_BEGIN);		//N						// 服务器通知客户端开始攻击
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_DIED",MSG_S2C_SHAPE_ATK_DIED);			//	N Y							// 死亡
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_REJECT",MSG_S2C_SHAPE_ATK_REJECT);	//N							// 攻击指令被拒绝
			tolua_constant(L,"MSG_S2C_SHAPE_FIGHTSTATE",MSG_S2C_SHAPE_FIGHTSTATE);//N								// 通知client进入或退出战斗状态
			tolua_constant(L,"MSG_S2C_SHAPE_SINSTATE",MSG_S2C_SHAPE_SINSTATE);	//Y								// 通知client进入或退出犯罪状态
			tolua_constant(L,"MSG_S2C_SHAPE_Refresh",MSG_S2C_SHAPE_Refresh);	//	Y							// 刷新建筑状态 
			tolua_constant(L,"MSG_S2C_SHAPE_STIFFEN",MSG_S2C_SHAPE_STIFFEN);//N									// 僵直状态
			tolua_constant(L,"MSG_S2C_SHAPE_EMOTION",MSG_S2C_SHAPE_EMOTION);		//Y							// 僵直状态
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_DAMAGE",MSG_S2C_SHAPE_ATK_DAMAGE);	//N					//普通受伤消息
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_BREAK",MSG_S2C_SHAPE_ATK_BREAK);	//N							// 被攻击
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_FULLMISS",MSG_S2C_SHAPE_ATK_FULLMISS);	//N							// 闪避
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_PARRY",MSG_S2C_SHAPE_ATK_PARRY);//N								// 招架
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_BLOCK",MSG_S2C_SHAPE_ATK_BLOCK);	//N							// 格挡
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_RESIST",MSG_S2C_SHAPE_ATK_RESIST);	//N							// 抵抗
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_REBOUND",MSG_S2C_SHAPE_ATK_REBOUND); //N							    // 反弹
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_SUCK",MSG_S2C_SHAPE_ATK_SUCK);//N									// 吸收
			tolua_constant(L,"MSG_S2C_SHAPE_RELIVE",MSG_S2C_SHAPE_RELIVE);		//N							// 复活一个玩家
			tolua_constant(L,"MSG_S2C_SHAPE_CHANGE_PROPERTIES",MSG_S2C_SHAPE_CHANGE_PROPERTIES);	//N					// 更新可移动物体的属性
			tolua_constant(L,"MSG_S2C_SHAPE_ATK_LOSE",MSG_S2C_SHAPE_ATK_LOSE);					//丢失
			///MSG_S2C_SHAPE end

			///MSG_S2C_PLAYER start
			tolua_constant(L,"MSG_S2C_PLAYER",MSG_S2C_PLAYER);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_PLAYER_RESUME",MSG_S2C_PLAYER_RESUME);	//Y								// 恢复HP、MP、YP。。。
			tolua_constant(L,"MSG_S2C_PLAYER_ADDPOINT",MSG_S2C_PLAYER_ADDPOINT);	//N							// 加点
			tolua_constant(L,"MSG_S2C_PLAYER_ANSWER_RELIVE",MSG_S2C_PLAYER_ANSWER_RELIVE);	//N						// 恢复复活
			tolua_constant(L,"MSG_S2C_PLAYER_ADDEXP",MSG_S2C_PLAYER_ADDEXP);	//Y								// 增加经验值
			tolua_constant(L,"MSG_S2C_PLAYER_LEVELUP",MSG_S2C_PLAYER_LEVELUP);									// 升级
			tolua_constant(L,"MSG_S2C_PLAYER_HITLEVELUPGRADE",MSG_S2C_PLAYER_HITLEVELUPGRADE);// N							// 连击数升级
			tolua_constant(L,"MSG_S2C_PLAYER_SYNC_HITS",MSG_S2C_PLAYER_SYNC_HITS);		//Y						// 同步连击数
			tolua_constant(L,"MSG_S2C_PLAYER_SETLEVEL",MSG_S2C_PLAYER_SETLEVEL);//Y
			tolua_constant(L,"MSG_S2C_PLAYER_USEITEM",MSG_S2C_PLAYER_USEITEM);	//Y								// gameserver向客户端返回消耗道具的使用结果
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGEMAXATK",MSG_S2C_PLAYER_CHANGEMAXATK);	//Y						// 消耗性道具提升最大攻击力
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGEATKSPEED",MSG_S2C_PLAYER_CHANGEATKSPEED);//N							// 消耗性道具提升攻击速度
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGEDEF",MSG_S2C_PLAYER_CHANGEDEF);//Y								// 消耗性道具提升防守能力
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGEELEMENTMODIFY",MSG_S2C_PLAYER_CHANGEELEMENTMODIFY);	//N					// 消耗性道具改变技能攻击系数
			tolua_constant(L,"MSG_S2C_PLAYER_SYNC_PKCOUNT",MSG_S2C_PLAYER_SYNC_PKCOUNT);	//Y						// 同步杀人数
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADEREQUEST",MSG_S2C_PLAYER_TRADEREQUEST);	//Y						//GS转发交易请求给客户
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADEBEGIN",MSG_S2C_PLAYER_TRADEBEGIN);		//Y						//交易开始
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADELOCK",MSG_S2C_PLAYER_TRADELOCK);	//N
			//tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_TO_LOCKSTATE",MSG_S2C_PLAYER_CHANGE_TO_LOCKSTATE);	//N					//退回交易锁定状态
			//tolua_constant(L,"MSG_S2C_PLAYER_PUTGOODSTOTRADEPAGE",MSG_S2C_PLAYER_PUTGOODSTOTRADEPAGE);	//N					//服务器返回放一个物品到展示区的结果
			//tolua_constant(L,"MSG_S2C_PLAYER_GETGOODSFROMTRADEPAGE",MSG_S2C_PLAYER_GETGOODSFROMTRADEPAGE);	//N				//服务器推一个物品信息给对方玩家
			//tolua_constant(L,"MSG_S2C_PLAYER_FETCHGOODSFORMTRADEPAGE",MSG_S2C_PLAYER_FETCHGOODSFORMTRADEPAGE);//N					//服务器返回从展示区拿一个物品的相关结果
			//tolua_constant(L,"MSG_S2C_PLAYER_DELGOODSFROMTRADEPAGE",MSG_S2C_PLAYER_DELGOODSFROMTRADEPAGE);	//N				//服务器通知对方一个物品从交易区中删除
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADECHANGEMONEY",MSG_S2C_PLAYER_TRADECHANGEMONEY);//N						//服务器返回调整金钱的相关结果
			//tolua_constant(L,"MSG_S2C_PLAYER_WAITFORTRADE",MSG_S2C_PLAYER_WAITFORTRADE);	//N
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADING",MSG_S2C_PLAYER_TRADING);//N									// 玩家点交易时发送此消息
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADECANCEL",MSG_S2C_PLAYER_TRADECANCEL);	//N							//服务器通知玩家对方已经取消交易
			//tolua_constant(L,"MSG_S2C_PLAYER_TRADERESULT",MSG_S2C_PLAYER_TRADERESULT);		//N						//交易的最终结果
			//			tolua_constant(L,"MSG_S2C_PLAYER_ADDFRIENDREQUEST",MSG_S2C_PLAYER_ADDFRIENDREQUEST);	//N					//GameServer转发好友添加消息给被请求玩家
			//			tolua_constant(L,"MSG_S2C_PLAYER_ADDFRIENDRESPONSE",MSG_S2C_PLAYER_ADDFRIENDRESPONSE);	//Y					//GameServer转发被请求玩家的响应结果给发出请求的那个玩家
			//			tolua_constant(L,"MSG_S2C_PLAYER_DELFRIEND",MSG_S2C_PLAYER_DELFRIEND);		//N						//gameserver向client返回删除一个好友的结果
			tolua_constant(L,"MSG_S2C_PLAYER_HELPRESPONSE",MSG_S2C_PLAYER_HELPRESPONSE);//Y							// 相应玩家帮助
			tolua_constant(L,"MSG_S2C_PLAYER_ADDSKILL",MSG_S2C_PLAYER_ADDSKILL);		//Y						// 添加技能
			tolua_constant(L,"MSG_S2C_PLAYER_DELSKILL",MSG_S2C_PLAYER_DELSKILL);		//	Y					// 删除技能
			tolua_constant(L,"MSG_S2C_PLAYER_SETATTACKRANGE",MSG_S2C_PLAYER_SETATTACKRANGE);//N				
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_EQUIPMENT",MSG_S2C_PLAYER_CHANGE_EQUIPMENT);//Y
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_PROPERTIES",MSG_S2C_PLAYER_CHANGE_PROPERTIES);	//Y					//玩家改变属性
			//			tolua_constant(L,"MSG_S2C_PLAYER_UpdateDisplayHeadPiece",MSG_S2C_PLAYER_UpdateDisplayHeadPiece);//Y					//更新玩家是否显示头盔信息
			tolua_constant(L,"MSG_S2C_PLAYER_AUTOMOVE",MSG_S2C_PLAYER_AUTOMOVE);	//Y							//自动移动到指定的点
			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_CONTRIBUTE",MSG_S2C_PLAYER_CHANGE_CONTRIBUTE);	//Y					// 国家贡献改变
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_LOAN",MSG_S2C_PLAYER_CHANGE_LOAN);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_LOANMAX",MSG_S2C_PLAYER_CHANGE_LOANMAX);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_LOANTIME",MSG_S2C_PLAYER_CHANGE_LOANTIME);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_QUEST_ON",MSG_S2C_PLAYER_CHANGE_QUEST_ON);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_QUEST_TIMEBEGIN",MSG_S2C_PLAYER_CHANGE_QUEST_TIMEBEGIN);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_QUEST_TIMECLEAR",MSG_S2C_PLAYER_CHANGE_QUEST_TIMECLEAR);//Y
			//			tolua_constant(L,"MSG_S2C_PLAYER_CHANGE_QUEST_TIMERESPONSE",MSG_S2C_PLAYER_CHANGE_QUEST_TIMERESPONSE);//Y
			tolua_constant(L,"MSG_S2C_PLAYER_FORCE_TARGET",MSG_S2C_PLAYER_FORCE_TARGET);
			///MSG_S2C_PLAYER end
			//MSG_S2C_OTHER
			tolua_constant(L,"MSG_S2C_OTHER",MSG_S2C_OTHER);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_OTHER_TALK",MSG_S2C_OTHER_TALK);		//Y							// 对话消息
			tolua_constant(L,"MSG_S2C_OTHER_FACTIONTALK",MSG_S2C_OTHER_FACTIONTALK);//N								// 帮会聊天
			tolua_constant(L,"MSG_S2C_OTHER_BROADCAST",MSG_S2C_OTHER_BROADCAST);//Y								// 广播
			tolua_constant(L,"MSG_S2C_OTHER_AddTopInfo",MSG_S2C_OTHER_AddTopInfo);		//N						// 添加顶端提示信息
			tolua_constant(L,"MSG_S2C_OTHER_OPENTALKBOX",MSG_S2C_OTHER_OPENTALKBOX);	//N							// 让客户端弹出TalkBox
			tolua_constant(L,"MSG_S2C_OTHER_ADDINFO",MSG_S2C_OTHER_ADDINFO);		//N							// 在玩家对话框加入一条信息
			tolua_constant(L,"MSG_S2C_OTHER_ADDSYSTEMINFO",MSG_S2C_OTHER_ADDSYSTEMINFO);	//N						// 在屏幕上方加入一条信息
			tolua_constant(L,"MSG_S2C_OTHER_GAMEMESSAGE",MSG_S2C_OTHER_GAMEMESSAGE);	//N							// 弹出消息对话框
			//			tolua_constant(L,"MSG_S2C_OTHER_PING",MSG_S2C_OTHER_PING);		//N								// 服务器定时检测Client是否存在线，client返回同样消息
			tolua_constant(L,"MSG_S2C_OTHER_SENDOBJECTPROPERTIY",MSG_S2C_OTHER_SENDOBJECTPROPERTIY);//Y						// 接受Shape的所有属性
			tolua_constant(L,"MSG_S2C_OTHER_CHANGESHAPEPROPERTIY",MSG_S2C_OTHER_CHANGESHAPEPROPERTIY);	//Y					// 改变Shape的一个属性
			tolua_constant(L,"MSG_S2C_OTHER_SETSHAPEPROPERTIY",MSG_S2C_OTHER_SETSHAPEPROPERTIY);	//Y					// 设置Shape的一个属性
			tolua_constant(L,"MSG_S2C_OTHER_VIEWGOODSLINK",MSG_S2C_OTHER_VIEWGOODSLINK);	//Y		
			tolua_constant(L,"MSG_S2C_OTHER_SCRIPTTIMERRUN",MSG_S2C_OTHER_SCRIPTTIMERRUN);	//N						// 脚本计时器运行
			tolua_constant(L,"MSG_S2C_OTHER_CHANGENAME",MSG_S2C_OTHER_CHANGENAME);		//Y						// 接受Gs转发Ws更名结果
			tolua_constant(L,"MSG_S2C_OTHER_CHANGENAME_OPENUI",MSG_S2C_OTHER_CHANGENAME_OPENUI);//Y						// 启动更名窗口
			tolua_constant(L,"MSG_S2C_OTHER_ADDPLAYERINFO",MSG_S2C_OTHER_ADDPLAYERINFO);//Y
			tolua_constant(L,"MSG_S2C_OTHER_SHOWREADMEWINDOW",MSG_S2C_OTHER_SHOWREADMEWINDOW);//N
			tolua_constant(L,"MSG_S2C_OTHER_ANSWERNPCLIST",MSG_S2C_OTHER_ANSWERNPCLIST);	//N						// 返回NPC列表给客户端
			tolua_constant(L,"MSG_S2C_OTHER_TALK_WORLD",MSG_S2C_OTHER_TALK_WORLD);		//Y						// 世界喊话
			tolua_constant(L,"MSG_S2C_OTHER_TALK_COUNTRY",MSG_S2C_OTHER_TALK_COUNTRY);	//Y							// 国家喊话
			tolua_constant(L,"MSG_S2C_OTHER_TALK_NOTIFY",MSG_S2C_OTHER_TALK_NOTIFY);	//Y							// 国家通知
			//			tolua_constant(L,"MSG_S2C_OTHER_RESPONSE_LOANTIMELIMIT",MSG_S2C_OTHER_RESPONSE_LOANTIMELIMIT);	//N				// 返回还款期限
			//end
			///MSG_S2C_GOODS start
			tolua_constant(L,"MSG_S2C_GOODS",MSG_S2C_GOODS);
			tolua_constant(L,"MSG_S2C_FETCHGOOD",MSG_S2C_FETCHGOOD);	//N									//GAMESERVER返回取一个物品的结果
			tolua_constant(L,"MSG_S2C_PUTDOWNGOOD",MSG_S2C_PUTDOWNGOOD);	//N								//gameserver返回放下一个物品的结果
			tolua_constant(L,"MSG_S2C_CHANGEGOOD",MSG_S2C_CHANGEGOOD);	//N									//gameserver返回物品调换结果
			tolua_constant(L,"MSG_S2C_DROPGOOD",MSG_S2C_DROPGOOD);	//N									//gameserver返回扔物品结果
			tolua_constant(L,"MSG_S2C_PICKGOOD",MSG_S2C_PICKGOOD);			//N							//gameserver返回拣物品结果
			tolua_constant(L,"MSG_S2C_DROPMONEY",MSG_S2C_DROPMONEY);		//Y					
			tolua_constant(L,"MSG_S2C_PICKMONEY",MSG_S2C_PICKMONEY);		//Y					
			tolua_constant(L,"MSG_S2C_SETHOTKEY",MSG_S2C_SETHOTKEY);//		N								//gameserver返回设置HOTKEY结果
			tolua_constant(L,"MSG_S2C_CLEARHOTKEY",MSG_S2C_CLEARHOTKEY);//	N					
			tolua_constant(L,"MSG_S2C_EXCHANGEHOTKEY",MSG_S2C_EXCHANGEHOTKEY);//N						
			tolua_constant(L,"MSG_S2C_EQUIPDAMAGE",MSG_S2C_EQUIPDAMAGE); //      Y                             //装备坏掉
			tolua_constant(L,"MSG_S2C_EQUIPDURUPDATE",MSG_S2C_EQUIPDURUPDATE);	//Y								//更新装备的耐久
			tolua_constant(L,"MSG_S2C_DELETEEQUIP",MSG_S2C_DELETEEQUIP);//Y									//删除装备
			tolua_constant(L,"MSG_S2C_DELETEBAGGOODS",MSG_S2C_DELETEBAGGOODS);//N									//删除背包物品
			tolua_constant(L,"MSG_S2C_DELETEGOODS",MSG_S2C_DELETEGOODS);//N									
			tolua_constant(L,"MSG_S2C_DELETEALLGOODS",MSG_S2C_DELETEALLGOODS);		//N				
			tolua_constant(L,"MSG_S2C_VIEWEQUIP",MSG_S2C_VIEWEQUIP);        //Ns                              
			tolua_constant(L,"MSG_S2C_OPENGOODSUPGRADE",MSG_S2C_OPENGOODSUPGRADE);	//Y							//gameserver通知玩家打开物品升级界面
			tolua_constant(L,"MSG_S2C_OPENGOODSENCHASE",MSG_S2C_OPENGOODSENCHASE);//N
			tolua_constant(L,"MSG_S2C_ENCHASE_OPEN_HOLE",MSG_S2C_ENCHASE_OPEN_HOLE);//N
			tolua_constant(L,"MSG_S2C_ENCHASE_CLOSE_HOLE",MSG_S2C_ENCHASE_CLOSE_HOLE);	//N
			tolua_constant(L,"MSG_S2C_ENCHASE_ENCHASECARD",MSG_S2C_ENCHASE_ENCHASECARD);
			tolua_constant(L,"MSG_S2C_ENCHASE_REMOVECARD",MSG_S2C_ENCHASE_REMOVECARD);
			//			tolua_constant(L,"MSG_S2C_ENCHASE_OPENPAGE",MSG_S2C_ENCHASE_OPENPAGE);
			tolua_constant(L,"MSG_S2C_CLOSEGOODSUPGRADE",MSG_S2C_CLOSEGOODSUPGRADE);//N
			tolua_constant(L,"MSG_S2C_GOODSUPGRADE_PUT",MSG_S2C_GOODSUPGRADE_PUT);	//Y				
			tolua_constant(L,"MSG_S2C_GOODSUPGRADE_FETCH",MSG_S2C_GOODSUPGRADE_FETCH);//N					
			tolua_constant(L,"MSG_S2C_GOODSUPGRADE_CHANGE",MSG_S2C_GOODSUPGRADE_CHANGE);//N				
			tolua_constant(L,"MSG_S2C_GOODSUPGRADE_UPGRADE",MSG_S2C_GOODSUPGRADE_UPGRADE);	//Y						//gameserver返回升级一个物品的结果
			tolua_constant(L,"MSG_S2C_GOODS_UPDATEPROPERTY",MSG_S2C_GOODS_UPDATEPROPERTY);	//Y						//更新物品属性
			tolua_constant(L,"MSG_S2C_GOODS_OPEN_CONTAINER",MSG_S2C_GOODS_OPEN_CONTAINER);	//Y						// 打开物品容器
			tolua_constant(L,"MSG_S2C_SUIT_ACTIVE",MSG_S2C_SUIT_ACTIVE);		//N	//套装激活

			//! GoodsExManage应用
			tolua_constant(L,"MSG_S2C_GOODS_OPEN_WINDOWS",MSG_S2C_GOODS_OPEN_WINDOWS);
			tolua_constant(L,"MSG_S2C_GOODS_CLOSE_WINDOWS",MSG_S2C_GOODS_CLOSE_WINDOWS);
			tolua_constant(L,"MSG_S2C_GOODS_RE_DISASSEMBLE_PREVIEW",MSG_S2C_GOODS_RE_DISASSEMBLE_PREVIEW);
			tolua_constant(L,"MSG_S2C_GOODS_RE_DISASSEMBLE_DO",MSG_S2C_GOODS_RE_DISASSEMBLE_DO);
			tolua_constant(L,"MSG_S2C_GOODS_RE_SYNTHESIZE_PREVIEW",MSG_S2C_GOODS_RE_SYNTHESIZE_PREVIEW);
			tolua_constant(L,"MSG_S2C_GOODS_RE_SYNTHESIZE_DO",MSG_S2C_GOODS_RE_SYNTHESIZE_DO);
			tolua_constant(L,"MSG_S2C_GOODS_RE_REINFORCE_POWER",MSG_S2C_GOODS_RE_REINFORCE_POWER);
			tolua_constant(L,"MSG_S2C_GOODS_BIND",MSG_S2C_GOODS_BIND);
			tolua_constant(L,"MSG_S2C_GOODS_FROST",MSG_S2C_GOODS_FROST);
			tolua_constant(L,"MSG_S2C_GOODS_RE_UPGRADE_VIEW",MSG_S2C_GOODS_RE_UPGRADE_VIEW);
			tolua_constant(L,"MSG_S2C_GOODS_RE_UPGRADE_DO",MSG_S2C_GOODS_RE_UPGRADE_DO);
			//			tolua_constant(L,"MSG_S2C_GOODS_RE_STUFF_SYNTHESIZE",MSG_S2C_GOODS_RE_STUFF_SYNTHESIZE);
			//			tolua_constant(L,"MSG_S2C_GOODS_RE_PLUG_EVIL_VIEW",MSG_S2C_GOODS_RE_PLUG_EVIL_VIEW);
			tolua_constant(L,"MSG_S2C_GOODS_RE_PLUG_EVIL_DO",MSG_S2C_GOODS_RE_PLUG_EVIL_DO);
			tolua_constant(L,"MSG_C2S_BOUNS_INFO",MSG_C2S_BOUNS_INFO);
			tolua_constant(L,"MSG_S2C_DEPOT_OPEN",MSG_S2C_DEPOT_OPEN);
			tolua_constant(L,"MSG_S2C_DEPOT_RE_INPUT_PWD",MSG_S2C_DEPOT_RE_INPUT_PWD);
			tolua_constant(L,"MSG_S2C_DEPOT_RE_CLOSE",MSG_S2C_DEPOT_RE_CLOSE);
			tolua_constant(L,"MSG_S2C_DEPOT_RE_CHANGE_PWD",MSG_S2C_DEPOT_RE_CHANGE_PWD);
			tolua_constant(L,"MSG_S2C_DEPOT_RE_BUY_SUBPACK",MSG_S2C_DEPOT_RE_BUY_SUBPACK);
			//			tolua_constant(L,"MSG_C2S_GOODS_EVIL_BACKOUT_VIEW",	MSG_C2S_GOODS_EVIL_BACKOUT_VIEW);


			///MSG_S2C_GOODS end
			//MSG_S2C_SHOP start
			tolua_constant(L,"MSG_S2C_SHOP",MSG_S2C_SHOP);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_SHOP_OPEN",MSG_S2C_SHOP_OPEN);	//N									//打开商店			
			tolua_constant(L,"MSG_S2C_SHOP_BUY",MSG_S2C_SHOP_BUY);	//N
			tolua_constant(L,"MSG_S2C_SHOP_BUY_BAT",MSG_S2C_SHOP_BUY_BAT);//Y
			tolua_constant(L,"MSG_S2C_SHOP_SELL",MSG_S2C_SHOP_SELL);//N	
			tolua_constant(L,"MSG_S2C_SHOP_SELL_BAT",MSG_S2C_SHOP_SELL_BAT);//Y
			tolua_constant(L,"MSG_S2C_SHOP_REPAIR",MSG_S2C_SHOP_REPAIR);	//N					
			tolua_constant(L,"MSG_S2C_SHOP_REPAIRALL",MSG_S2C_SHOP_REPAIRALL);	//N					
			tolua_constant(L,"MSG_S2C_SHOP_DURUPDATE",MSG_S2C_SHOP_DURUPDATE);		//N				
			tolua_constant(L,"MSG_S2C_SHOP_CLOSE",MSG_S2C_SHOP_CLOSE);		//##关闭个人商店	 //N
			tolua_constant(L,"MSG_S2C_BUSINESS_OPEN",MSG_S2C_BUSINESS_OPEN);
			tolua_constant(L,"MSG_S2C_BUSINESS_BUY",MSG_S2C_BUSINESS_BUY);
			tolua_constant(L,"MSG_S2C_BUSINESS_SELL",MSG_S2C_BUSINESS_SELL);
			tolua_constant(L,"MSG_S2C_BUSINESS_INVEST",MSG_S2C_BUSINESS_INVEST);

			//end
			tolua_constant(L,"MSG_C2S_SPRITE_AutoSupply_ChangeSetup_Request",MSG_C2S_SPRITE_AutoSupply_ChangeSetup_Request);
			//MSG_S2C_DEPOT start
			tolua_constant(L,"MSG_S2C_DEPOT",MSG_S2C_DEPOT);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_DEPOT_OPEN",MSG_S2C_DEPOT_OPEN);	//Y						
			//MSG_S2C_TEAM start
			tolua_constant(L,"MSG_S2C_TEAM",MSG_S2C_TEAM);		
			// ----------------------------------------------------
			//! [2007-7-19 add]
			tolua_constant(L,"MSG_S2C_TEAM_TeamData",MSG_S2C_TEAM_TeamData);//	Y								//! 所有队伍数据到成员
			tolua_constant(L,"MSG_S2C_TEAM_MemberData",MSG_S2C_TEAM_MemberData);//Y								//! 一个成员的队伍数据
			tolua_constant(L,"MSG_S2C_TEAM_TeamDataChange",MSG_S2C_TEAM_TeamDataChange);//N							//! 队伍数据改变
			tolua_constant(L,"MSG_S2C_TEAM_MemberAllIdioinfo",MSG_S2C_TEAM_MemberAllIdioinfo);	//Y 						//! 一个成员所有非队伍成员数据
			tolua_constant(L,"MSG_S2C_TEAM_UpdateIdioinfo",MSG_S2C_TEAM_UpdateIdioinfo);//	Y						//! 更新某个队伍成员数据
			tolua_constant(L,"MSG_S2C_TEAM_MembereExStateChange",MSG_S2C_TEAM_MembereExStateChange);//N						//! 成员异常状态改变
			tolua_constant(L,"MSG_S2C_TEAM_MemberStateChange",MSG_S2C_TEAM_MemberStateChange);	//N						//! 成员在线状态改变
			tolua_constant(L,"MSG_S2C_TEAM_MemberLeave",MSG_S2C_TEAM_MemberLeave);		//N						//! 成员离队
			tolua_constant(L,"MSG_S2C_TEAM_MemberLeaveHere",MSG_S2C_TEAM_MemberLeaveHere);							//! 成员离开本服(GS)
			tolua_constant(L,"MSG_S2C_TEAM_MastterChange",MSG_S2C_TEAM_MastterChange);								//! 队长改变
			tolua_constant(L,"MSG_S2C_TEAM_Disband",MSG_S2C_TEAM_Disband);									//! 队伍解散
			tolua_constant(L,"MSG_S2C_TEAM_GoodsSetChange",MSG_S2C_TEAM_GoodsSetChange);							//! 修改物品分配设置
			tolua_constant(L,"MSG_S2C_TEAM_MemberPos",MSG_S2C_TEAM_MemberPos);			//n function						//! 更新成员位置
			tolua_constant(L,"MSG_S2C_TEAM_RECRUIT",MSG_S2C_TEAM_RECRUIT);									//! 更新招募状态
			tolua_constant(L,"MSG_S2C_TEAM_RECRUITED_NUM",MSG_S2C_TEAM_RECRUITED_NUM);								//! 更新已招募的人数
			tolua_constant(L,"MSG_S2C_TEAM_QUERY_JOIN",MSG_S2C_TEAM_QUERY_JOIN);		//Y						//邀请加入
			//! old unused
			//			tolua_constant(L,"MSG_S2C_TEAM_ANSWER_JOIN",MSG_S2C_TEAM_ANSWER_JOIN);								//回复邀请加入
			//			tolua_constant(L,"MSG_S2C_TEAM_SYNCHRONIZE",MSG_S2C_TEAM_SYNCHRONIZE);								//同步队伍的信息
			//			tolua_constant(L,"MSG_S2C_TEAM_TEAMATE_JOIN",MSG_S2C_TEAM_TEAMATE_JOIN);
			//			tolua_constant(L,"MSG_S2C_TEAM_TEAMATE_LEAVE",MSG_S2C_TEAM_TEAMATE_LEAVE);
			//			tolua_constant(L,"MSG_S2C_TEAM_TEAMATE_CHANGE_REGION",MSG_S2C_TEAM_TEAMATE_CHANGE_REGION);
			//			tolua_constant(L,"MSG_S2C_TEAM_CHANGE_LEAER",MSG_S2C_TEAM_CHANGE_LEAER);									//改变队长
			//			tolua_constant(L,"MSG_S2C_TEAM_CHANGE_ALLOCATION_SCHEME",MSG_S2C_TEAM_CHANGE_ALLOCATION_SCHEME);
			tolua_constant(L,"MSG_S2C_TEAM_CHAT",MSG_S2C_TEAM_CHAT);
			//			tolua_constant(L,"MSG_S2C_TEAM_CHANGE_STATE",MSG_S2C_TEAM_CHANGE_STATE);
			//end
			//MSG_S2C_LINKMAN
			// tanhaowen
			//  2008.10.23
			tolua_constant(L,"MSG_S2C_LINKMAN",MSG_S2C_LINKMAN);
			//------------------------------------------------------
			tolua_constant(L,"MSG_S2C_LINKMAN_INIT",MSG_S2C_LINKMAN_INIT);
			tolua_constant(L,"MSG_S2C_LINKMAN_ADD",MSG_S2C_LINKMAN_ADD);
			tolua_constant(L,"MSG_S2C_LINKMAN_DEL",MSG_S2C_LINKMAN_DEL);
			tolua_constant(L,"MSG_S2C_LINKMAN_MOVE",MSG_S2C_LINKMAN_MOVE);
			tolua_constant(L,"MSG_S2C_LINKMAN_OTHER_HANDLE",MSG_S2C_LINKMAN_OTHER_HANDLE);
			tolua_constant(L,"MSG_S2C_LINKMAN_REQUEST_ADD",MSG_S2C_LINKMAN_REQUEST_ADD);
			tolua_constant(L,"MSG_S2C_LINKMAN_LOGIN",MSG_S2C_LINKMAN_LOGIN);
			tolua_constant(L,"MSG_S2C_LINKMAN_LOGOUT",MSG_S2C_LINKMAN_LOGOUT);
			tolua_constant(L,"MSG_S2C_LINKMAN_INFO_CHANGE",MSG_S2C_LINKMAN_INFO_CHANGE);
			tolua_constant(L,"MSG_S2C_LINKMAN_DELSTATE_CHANGE",MSG_S2C_LINKMAN_DELSTATE_CHANGE);
			//end
			//MSG_S2C_SKILL start
			tolua_constant(L,"MSG_S2C_SKILL",MSG_S2C_SKILL);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_SKILL_USE",MSG_S2C_SKILL_USE);	//N									// 使用技能
			tolua_constant(L,"MSG_S2C_SKILL_STATE_CHANGE",MSG_S2C_SKILL_STATE_CHANGE);		//N						// 使用技能后导致HP/MP/RP/YP状态改变
			tolua_constant(L,"MSG_S2C_SKILL_ADD_EX_STATE",MSG_S2C_SKILL_ADD_EX_STATE);	//Y							// 使用技能后添加异常状态
			tolua_constant(L,"MSG_S2C_SKILL_END_EX_STATE",MSG_S2C_SKILL_END_EX_STATE);	//Y							// 异常状态的消失,服务器通知客户端
			tolua_constant(L,"MSG_S2C_SKILL_EX_STATE_ADDITIONAL_DATA",MSG_S2C_SKILL_EX_STATE_ADDITIONAL_DATA);	//N		// 异常状态的附加数据
			tolua_constant(L,"MSG_S2C_SKILL_AFFECT_FIELD",MSG_S2C_SKILL_AFFECT_FIELD);		//N						// 发送受影响的区域给客户端
			tolua_constant(L,"MSG_S2C_SKILL_STUDY_FAILED",MSG_S2C_SKILL_STUDY_FAILED);
			tolua_constant(L,"MSG_S2C_SKILL_STUDY_BEGIN",MSG_S2C_SKILL_STUDY_BEGIN);
			tolua_constant(L,"MSG_S2C_OCCUSKILL_STUDY_BEGIN",MSG_S2C_OCCUSKILL_STUDY_BEGIN);							// 收到打开副职业技能学习页面消息
			tolua_constant(L,"MSG_S2C_OCCUSKILL_STUDY",MSG_S2C_OCCUSKILL_STUDY);								// 收到学习副职业技能消息
			//end 
			//MSG_C2S_ORGSYS
			tolua_constant(L,"MSG_C2S_ORGSYS",MSG_C2S_ORGSYS);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_C2S_ORGSYS_CreateFaction_Reques",MSG_C2S_ORGSYS_CreateFaction_Reques);	
			tolua_constant(L,"MSG_C2S_ORGSYS_DisbandFaction_Request",MSG_C2S_ORGSYS_DisbandFaction_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_SetPlacard_Request",MSG_C2S_ORGSYS_SetPlacard_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_SetLeaveword_Request",MSG_C2S_ORGSYS_SetLeaveword_Request);
			tolua_constant(L,"MSG_C2S_ORGSYS_SetJobName_Request",MSG_C2S_ORGSYS_SetJobName_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_SetJobPurview_Request",MSG_C2S_ORGSYS_SetJobPurview_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_SetMemberJob_Request",MSG_C2S_ORGSYS_SetMemberJob_Request);	
			tolua_constant(L,"MSG_C2S_Faction_SetMemberTitle_Request",MSG_C2S_Faction_SetMemberTitle_Request);	
			tolua_constant(L,"MSG_C2S_Faction_SetChairman_Request",MSG_C2S_Faction_SetChairman_Request);	

			tolua_constant(L,"MSG_C2S_ORGSYS_Quit_Request",MSG_C2S_ORGSYS_Quit_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_Kick_Request",MSG_C2S_ORGSYS_Kick_Request);	
			tolua_constant(L,"MSG_C2S_Faction_InvitePlayer_Request",MSG_C2S_Faction_InvitePlayer_Request);	
			tolua_constant(L,"MSG_C2S_Faction_InvitePlayer_Answer",MSG_C2S_Faction_InvitePlayer_Answer);
			tolua_constant(L,"MSG_C2S_ORGSYS_CheckRequisition_Request",MSG_C2S_ORGSYS_CheckRequisition_Request);	
			tolua_constant(L,"MSG_C2S_Faction_UpLoadIcon_Request",MSG_C2S_Faction_UpLoadIcon_Request);	

			tolua_constant(L,"MSG_C2S_ORGSYS_JoinFac_Request",MSG_C2S_ORGSYS_JoinFac_Request);	

			tolua_constant(L,"MSG_C2S_FacData_Request",MSG_C2S_FacData_Request);	
			tolua_constant(L,"MSG_C2S_Faction_AllIconData_Request",MSG_C2S_Faction_AllIconData_Request);	
			tolua_constant(L,"MSG_C2S_ORGSYS_CloseFacBoard_Notifye",MSG_C2S_ORGSYS_CloseFacBoard_Notifye);
			tolua_constant(L,"MSG_C2S_FACTION_Upgrade_Request",MSG_C2S_FACTION_Upgrade_Request);	
			//end
			//MSG_S2C_PALYERSHOP
			tolua_constant(L,"MSG_S2C_PLAYERSHOP",MSG_S2C_PLAYERSHOP);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_PALYERSHOP_CREATE",MSG_S2C_PALYERSHOP_CREATE);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_BUYITEM",MSG_S2C_PLAYERSHOP_BUYITEM);//N
			//tolua_constant(L,"MSG_S2C_PLAYERSHOP_ADDSUCCEED",MSG_S2C_PLAYERSHOP_ADDSUCCEED);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_OPENFORBUSINESS",MSG_S2C_PLAYERSHOP_OPENFORBUSINESS);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_CLOSEDOWN",MSG_S2C_PLAYERSHOP_CLOSEDOWN);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_LOOKGOODS",MSG_S2C_PLAYERSHOP_LOOKGOODS);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_CLOSE",MSG_S2C_PLAYERSHOP_CLOSE);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_QUIT",MSG_S2C_PLAYERSHOP_QUIT);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_TOBUYER",MSG_S2C_PLAYERSHOP_TOBUYER);//Y
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_TOSELLER",MSG_S2C_PLAYERSHOP_TOSELLER);//N
			tolua_constant(L,"MSG_S2C_PLAYERSHOP_UPDATE",MSG_S2C_PLAYERSHOP_UPDATE);//N
			//tolua_constant(L,"MSG_S2C_PLAYERSHOP_REMOVE",MSG_S2C_PLAYERSHOP_REMOVE);//N
			//end

			// 容器
			tolua_constant(L,"MSG_S2C_CONTAINER",MSG_S2C_CONTAINER);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_CONTAINER_OBJECT_MOVE",MSG_S2C_CONTAINER_OBJECT_MOVE);					//Y
			tolua_constant(L,"MSG_S2C_CONTAINER_OBJECT_UNLOCK",MSG_S2C_CONTAINER_OBJECT_UNLOCK);
			tolua_constant(L,"MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE",MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE);  //Y
			tolua_constant(L,"MSG_S2C_CONTAINER_CLEAR",MSG_S2C_CONTAINER_CLEAR);								//Y

			// 宠物
			tolua_constant(L,"MSG_S2C_PET",MSG_S2C_PET);
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_PET_CHANGE_MASTER",MSG_S2C_PET_CHANGE_MASTER);	//Y							//改变主人
			tolua_constant(L,"MSG_S2C_PET_CHANGE_MODE",MSG_S2C_PET_CHANGE_MODE);	//N							//改变攻击模式
			//			tolua_constant(L,"MSG_S2C_PET_CHANGE_EXPERIENCE",MSG_S2C_PET_CHANGE_EXPERIENCE);	//Y						//改变等级
			tolua_constant(L,"MSG_S2C_PET_CHANGE_ACTION",MSG_S2C_PET_CHANGE_ACTION);//N								//改变宠物当前动作
			//			tolua_constant(L,"MSG_S2C_PET_CARRIAGE",MSG_S2C_PET_CARRIAGE);//Y

			//计费
			tolua_constant(L,"MSG_S2C_COST",MSG_S2C_COST); 
			// -----------------------------------------------------
			tolua_constant(L,"MSG_S2C_COST_NOTENOUGHCOST",MSG_S2C_COST_NOTENOUGHCOST);

			//寄售点卡 s->c
			tolua_constant(L,"MSG_S2C_CARD",MSG_S2C_CARD);
			// -----------------------------------------------------
			tolua_constant(L,"MSG_S2C_CARD_SHOWWINDOW",MSG_S2C_CARD_SHOWWINDOW);								//　通知打开点卡出售窗口
			tolua_constant(L,"MSG_S2C_CARD_SELL_RESPONSE",MSG_S2C_CARD_SELL_RESPONSE);								//　gs回应寄售请求结果
			tolua_constant(L,"MSG_S2C_CARD_BUY_RESPONSE",MSG_S2C_CARD_BUY_RESPONSE);
			tolua_constant(L,"MSG_S2C_CARD_CANCEL_RESPONSE",MSG_S2C_CARD_CANCEL_RESPONSE);
			tolua_constant(L,"MSG_S2C_CARD_LIST_RESPONSE",MSG_S2C_CARD_LIST_RESPONSE);								//	返回当前出售的点卡列表
			tolua_constant(L,"MSG_S2C_CARD_SALECOSTS_RESPONSE",MSG_S2C_CARD_SALECOSTS_RESPONSE);						//	返回当前出售的点卡列表

			tolua_constant(L,"MSG_S2C_CARD_CHARGE_RESPONSE",MSG_S2C_CARD_CHARGE_RESPONSE);

			//国家
			tolua_constant(L,"MSG_S2C_COUNTRY",MSG_S2C_COUNTRY);
			// -----------------------------------------------------
			tolua_constant(L,"MSG_S2C_COUNTRY_CHANGE",MSG_S2C_COUNTRY_CHANGE);				//Y					// 更改国籍


			// 增值商店
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP",MSG_S2C_INCREMENTSHOP);			//N
			// ----------------------------------------------------
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP_OPEN",MSG_S2C_INCREMENTSHOP_OPEN);										//打开商店			
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP_BUY",MSG_S2C_INCREMENTSHOP_BUY);										//gameserver返回从商店中购买一个物品的结果					
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP_CLOSE",MSG_S2C_INCREMENTSHOP_CLOSE);							//##关闭个人商店
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP_AFFICHE_RESPONSE",MSG_S2C_INCREMENTSHOP_AFFICHE_RESPONSE);					// 公告窗口开
			tolua_constant(L,"MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE",MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE);					// 返回增值日志
			//  MSG_S2C_ORGSYS------->begin
			tolua_constant(L,"MSG_S2C_ORGSYS_InitOrgSys_Notify",MSG_S2C_ORGSYS_InitOrgSys_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_UpdatePlayerFacInfo",MSG_S2C_ORGSYS_UpdatePlayerFacInfo);
			tolua_constant(L,"MSG_S2C_ORGSYS_CreateFaction_Notify",MSG_S2C_ORGSYS_CreateFaction_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_CreateFaction_Re",MSG_S2C_ORGSYS_CreateFaction_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_UpgradeFaction_Notify",MSG_S2C_ORGSYS_UpgradeFaction_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_UpgradeFaction_Re",MSG_S2C_ORGSYS_UpgradeFaction_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_DisbandFaction_Notify",MSG_S2C_ORGSYS_DisbandFaction_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_DisbandFaction_Re",MSG_S2C_ORGSYS_DisbandFaction_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetPlacard_Re",MSG_S2C_ORGSYS_SetPlacard_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetLeaveword_Re",MSG_S2C_ORGSYS_SetLeaveword_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetJobName_Re",MSG_S2C_ORGSYS_SetJobName_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetJobPurview_Re",MSG_S2C_ORGSYS_SetJobPurview_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetJobPurview_Notify",MSG_S2C_ORGSYS_SetJobPurview_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetMemberJob_Re",MSG_S2C_ORGSYS_SetMemberJob_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_SetMemberJob_Notify",MSG_S2C_ORGSYS_SetMemberJob_Notify);
			tolua_constant(L,"MSG_S2C_Faction_SetMemberTitle_Re",MSG_S2C_Faction_SetMemberTitle_Re);
			tolua_constant(L,"MSG_S2C_Faction_SetMemberTitle_Notify",MSG_S2C_Faction_SetMemberTitle_Notify);
			tolua_constant(L,"MSG_S2C_Faction_SetChairman_Re",MSG_S2C_Faction_SetChairman_Re);
			tolua_constant(L,"MSG_S2C_Faction_SetChairman_Notify",MSG_S2C_Faction_SetChairman_Notify);
			tolua_constant(L,"MSG_S2C_Faction_ChangeRecruitState_Notify",MSG_S2C_Faction_ChangeRecruitState_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_Quit_Re",MSG_S2C_ORGSYS_Quit_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_Kick_Re",MSG_S2C_ORGSYS_Kick_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_Kick_Notify",MSG_S2C_ORGSYS_Kick_Notify);
			tolua_constant(L,"MSG_S2C_ORGSYS_CheckRequisition_Re",MSG_S2C_ORGSYS_CheckRequisition_Re);
			tolua_constant(L,"MSG_S2C_ORGSYS_CheckRequisition_Notify",MSG_S2C_ORGSYS_CheckRequisition_Notify);
			tolua_constant(L,"MSG_S2C_Faction_InvitePlayer_Re",MSG_S2C_Faction_InvitePlayer_Re);	
			tolua_constant(L,"MSG_S2C_Faction_InvitePlayer_Notify",MSG_S2C_Faction_InvitePlayer_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_AddMember_Notify",MSG_S2C_Faction_AddMember_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_ApplyBoardOpen_Notify",MSG_S2C_Faction_ApplyBoardOpen_Notify);	
			tolua_constant(L,"MSG_S2C_ORGSYS_JoinFac_Re",MSG_S2C_ORGSYS_JoinFac_Re);	
			tolua_constant(L,"MSG_S2C_Faction_PublicData_Notify",MSG_S2C_Faction_PublicData_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_BaseData_Notify",MSG_S2C_Faction_BaseData_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_Member_Notify",MSG_S2C_Faction_Member_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_Apply_Notify",MSG_S2C_Faction_Apply_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_Placard_Notify",MSG_S2C_Faction_Placard_Notify);
			tolua_constant(L,"MSG_S2C_Faction_Leaveword_Notify",MSG_S2C_Faction_Leaveword_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_IconBoardOpen_Notify",MSG_S2C_Faction_IconBoardOpen_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_IconData_Re",MSG_S2C_Faction_IconData_Re);	
			tolua_constant(L,"MSG_S2C_Faction_IconData_Notify",MSG_S2C_Faction_IconData_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_AllIconData_Re",MSG_S2C_Faction_AllIconData_Re);	
			tolua_constant(L,"MSG_S2C_Faction_MemberMap_Notifye",MSG_S2C_Faction_MemberMap_Notifye);	
			tolua_constant(L,"MSG_S2C_Faction_MemberLevel_Notifye",MSG_S2C_Faction_MemberLevel_Notifye);	
			tolua_constant(L,"MSG_S2C_Faction_MemberOccu_Notifye",MSG_S2C_Faction_MemberOccu_Notifye);	
			tolua_constant(L,"MSG_S2C_Faction_MemberOnline_Notifye",MSG_S2C_Faction_MemberOnline_Notifye);	
			tolua_constant(L,"MSG_S2C_Faction_ChangeRes_Notify",MSG_S2C_Faction_ChangeRes_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_ChangeExp_Notify",MSG_S2C_Faction_ChangeExp_Notify);	
			tolua_constant(L,"MSG_S2C_Faction_ChangeMemberContribute_Notify",MSG_S2C_Faction_ChangeMemberContribute_Notify);	
			////MSG_S2C_ORGSYS  ->end

			tolua_constant(L,"MSG_S2C_Quest_Add",MSG_S2C_Quest_Add);										//添加一个任务
			tolua_constant(L,"MSG_S2C_Quest_Complete",MSG_S2C_Quest_Complete);									//完成一个任务
			tolua_constant(L,"MSG_S2C_Quest_Remove",MSG_S2C_Quest_Remove);	
			//			tolua_constant(L,"MSG_S2C_ORGSYS_OneFacPublicData_Notify",MSG_S2C_ORGSYS_OneFacPublicData_Notify);
			//移出一个任务
			//MSG_S2C_Quest_Update,									
			tolua_constant(L,"MSG_S2C_Quest_UpdateStep",MSG_S2C_Quest_UpdateStep);								//更新一个步骤
			tolua_constant(L,"MSG_S2C_Quest_UpdateData",MSG_S2C_Quest_UpdateData);								//更新一个任务数据
			tolua_constant(L,"MSG_S2C_Quest_Talk",MSG_S2C_Quest_Talk);

			tolua_constant(L,"MSG_S2C_ANSWER_PlayerRanks",MSG_S2C_ANSWER_PLAYERRANKS);								//向客户端发送玩家排行榜


			//MSG_S2C_MAIL
			// 新添加 邮件 tanhaowen
			tolua_constant(L,"MSG_S2C_MAIL", MSG_S2C_MAIL);
			//------------------------------------------------------

			tolua_constant(L,"MSG_S2C_MAIL_RECEIVER",			 MSG_S2C_MAIL_RECEIVER);			//!获得邮件数据	
			tolua_constant(L,"MSG_S2C_MAIL_SEND_FAILED",		 MSG_S2C_MAIL_SEND_FAILED);			//!发信失败		
			tolua_constant(L,"MSG_S2C_MAIL_SEND",				 MSG_S2C_MAIL_SEND);				//!新邮件发送					
			tolua_constant(L,"MSG_S2C_MAIL_READ",				 MSG_S2C_MAIL_READ);				//!读邮件			
			tolua_constant(L,"MSG_S2C_MAIL_FAILED_MOVE_GOODS",	 MSG_S2C_MAIL_FAILED_MOVE_GOODS);	//!邮件取物品失败
			tolua_constant(L,"MSG_S2C_MAIL_MOVE_GOODS_RESULT",   MSG_S2C_MAIL_MOVE_GOODS_RESULT);	//!邮件取物品结果
			//tolua_constant(L,"MSG_S2C_MAIL_DELETE_RESULT",		 MSG_S2C_MAIL_DELETE_RESULT);		//!删除有附件邮件的确认消息
			tolua_constant(L,"MSG_S2C_MAIL_DELETE_OK",			 MSG_S2C_MAIL_DELETE_OK);			//!删除邮件
			tolua_constant(L,"MSG_S2C_MAIL_AGREED_PAY",			 MSG_S2C_MAIL_AGREED_PAY);			//!付款收信接收消息
			//tolua_constant(L,"MSG_S2C_MAIL_AGREED_PRESENT",		 MSG_S2C_MAIL_AGREED_PRESENT);		//!同意赠送
			tolua_constant(L,"MSG_S2C_MAIL_GET_GOLD_RESULT",	 MSG_S2C_MAIL_GET_GOLD_RESULT);		//!邮箱取金钱结果
			//tolua_constant(L,"MSG_S2C_MAIL_PRESENT_FAILED",		 MSG_S2C_MAIL_PRESENT_FAILED);		//赠送失败
			//tolua_constant(L,"MSG_S2C_MAIL_PRESENT_OK",			 MSG_S2C_MAIL_PRESENT_OK);			//附合赠送条件

			//MSG_S2C_AC_QUESTION
			// 答题
			tolua_constant(L,"MSG_S2C_ANTICHEAT", MSG_S2C_ANTICHEAT);
			//-------------------------------------------------------------
			tolua_constant(L,"MSG_S2C_AC_QUESTION", MSG_S2C_AC_QUESTION);
			//end
			//--------------------------------------------------------------------------------------------------------
			//	Mapping File On DriveClient, Test WirteMappingFile to Client
			//   by MartySa 2009.2.2
			//--------------------------------------------------------------------------------------------------------

			// L2T
			tolua_constant(L,"MSG_L2T",							 MSG_L2T);						 //Log -> Test
			//------------------------------------------------------------
			//------------------------------------------------------------

			//	MSG_L2T_LOG
			tolua_constant(L,"MSG_L2T_LOG",						 MSG_L2T_LOG);					 //登陆类型(对应MSG_L2C_LOG = 718080)
			//------------------------------------------------------------
			tolua_constant(L,"MSG_L2T_LOG_ANSWER_PLAYERDATA",	 MSG_L2T_LOG_ANSWER_PLAYERDATA); //player的详细数据(对应MSG_L2C_LOG_ANSWER_PLAYERDATA = 718083)				
			//end

			// T2C
			tolua_constant(L,"MSG_T2C",							 MSG_T2C);	
			//------------------------------------------------------------
			//------------------------------------------------------------

			// MSG_T2C_MOUSE
			tolua_constant(L,"MSG_T2C_MOUSE",					 MSG_T2C_MOUSE);
			//------------------------------------------------------------
			tolua_constant(L,"MSG_T2C_MOUSE_MOVE",				 MSG_T2C_MOUSE_MOVE);
			tolua_constant(L,"MSG_T2C_MOUSE_LBUTTONDOWN",		 MSG_T2C_MOUSE_LBUTTONDOWN);
			tolua_constant(L,"MSG_T2C_MOUSE_LBUTTONUP",			 MSG_T2C_MOUSE_LBUTTONUP);			
			tolua_constant(L,"MSG_T2C_MOUSE_LBUTTONDBLCLK",		 MSG_T2C_MOUSE_LBUTTONDBLCLK);	//双击
			tolua_constant(L,"MSG_T2C_MOUSE_RBUTTONDOWN",		 MSG_T2C_MOUSE_RBUTTONDOWN);	
			tolua_constant(L,"MSG_T2C_MOUSE_RBUTTONUP",			 MSG_T2C_MOUSE_RBUTTONUP);	
			tolua_constant(L,"MSG_T2C_MOUSE_RBUTTONDBLCLK",		 MSG_T2C_MOUSE_RBUTTONDBLCLK);	
			tolua_constant(L,"MSG_T2C_MOUSE_MOUSEWHEEL",		 MSG_T2C_MOUSE_MOUSEWHEEL);	    //中键滚动
			//end

			// MSG_T2C_KEY
			tolua_constant(L,"MSG_T2C_KEY",						 MSG_T2C_KEY);
			//------------------------------------------------------------
			tolua_constant(L,"MSG_T2C_KEY_DOWN",				 MSG_T2C_KEY_DOWN);
			tolua_constant(L,"MSG_T2C_KEY_UP",					 MSG_T2C_KEY_UP);
			tolua_constant(L,"MSG_T2C_KEY_CHAR",				 MSG_T2C_KEY_CHAR);
			//end

			// MSG_T2C_LOGIC
			tolua_constant(L,"MSG_T2C_LOGIC_SELECTEDSHAPE",		 MSG_T2C_LOGIC_SELECTEDSHAPE);	//选中某个对象
			tolua_constant(L,"MSG_T2C_LOGIC_MOVE",				 MSG_T2C_LOGIC_MOVE);			//移动Client Player
			tolua_constant(L,"MSG_T2C_LOGIC_UIISOPENED",		 MSG_T2C_LOGIC_UIISOPENED);		//发送请求界面是打开	
			//MSG_T2C_LOGIC_PICKGOODS
			tolua_constant(L,"MSG_T2C_LOGIC_PICKGOODS",			 MSG_T2C_LOGIC_PICKGOODS);		//请求拾取物品								
			tolua_constant(L,"MSG_T2C_LOGIC_CALLSCREAPT",		 MSG_T2C_LOGIC_CALLSCREAPT);// 请求客户端向服务器发送脚本调用功能	
			tolua_constant(L,"MSG_T2C_LOGIC_SPLITGOODS",		 MSG_T2C_LOGIC_SPLITGOODS);	// 请求客户端向服务器发送拆分物品消息
			//end
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}