//=============================================================================
/**
 *  file: ScriptSys.cpp
 *
 *  Brief:实现对<lua_State* >状态的管理
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "StdAfx.h"
#include ".\scriptsys.h"
//#include "../../nets/Servers.h"
#include "../GameTest/resource.h"		// 主符号
#include "../GameTest/GameTestDlg.h"
#include "../GameTest/TestScript.h"
#include "../GameTest/WndMsgDef.h"
#include "../App/ClientRegion.h"
#include "../App/other/GoodsList.h"
#include "..\App\Goods.h"
//#include "../App/CCDKey.h"
//#include "../App/other/questsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int  baseobject_open(lua_State* L);
int  shape_open(lua_State* L);
int  moveshape_open(lua_State* L);
int  player_open(lua_State* L);
int  message_open(lua_State* L);
int  tools_open(lua_State* L);
int  game_client_open(lua_State* L);
int  questcontent_open(lua_State* L);
int  cdkey_open(lua_State* L);
int  client_region_open(lua_State* L);
int  script_sys_open(lua_State* L);
int  dialog_open(lua_State* L);
int  display_info_open(lua_State* L);
int  game_client_tree_open(lua_State* L);
int  test_script_dlg_open(lua_State* L);
int  guID_open(lua_State* L);
int  goods_open(lua_State* L);
int  goodslist_open(lua_State* L);
int  monster_open(lua_State* L);


CScriptSys::CScriptSys(void)
{
	m_luaState = NULL;
	m_pTestDialog = NULL;
	m_pDB = NULL;
	m_nDBSize = 0;
	m_strIp.clear();

	m_hStateMutex = NULL;
}

CScriptSys::~CScriptSys(void)
{
}

CScriptSys* CScriptSys::getInstance()
{
	static CScriptSys*	pInstance = NULL;
	if(NULL == pInstance)
	{
		pInstance= new CScriptSys();
	}
	return pInstance;
}

bool CScriptSys::Release()
{
	CScriptSys* pInstance = getInstance();
	pInstance->PerformString("ReleaseLuaObject()");
	getInstance()->UnInitial();  //ReleaseLuaObject()
	SAFE_DELETE(pInstance);
	return true;
}

bool CScriptSys::Init(CGameTestDlg* pTestDialog)
{
	m_pTestDialog = pTestDialog;
	m_hStateMutex = CreateMutex(NULL,FALSE,NULL);
	
	m_luaState = lua_open();

	if(NULL == m_luaState)
	{
		PutDebugString("在函数CScriptSys::Init()中,lua_open()操作失败！");
		return false;
	}
	//设置垃圾搜集参数
	int nRet = lua_gc(m_luaState,LUA_GCSETPAUSE,100);
	nRet = lua_gc(m_luaState,LUA_GCSETSTEPMUL,200);

	luaL_openlibs(m_luaState);

	baseobject_open(m_luaState);
	shape_open(m_luaState);
	moveshape_open(m_luaState);
	player_open(m_luaState);
	message_open(m_luaState);
	tools_open(m_luaState);
	game_client_open(m_luaState);
	client_region_open(m_luaState);
	script_sys_open(m_luaState);
	guID_open(m_luaState);
	goods_open(m_luaState);
	goodslist_open(m_luaState);
	monster_open(m_luaState);
	cdkey_open(m_luaState);
	questcontent_open(m_luaState);
	//控件库
	dialog_open(m_luaState);
	display_info_open(m_luaState);
	game_client_tree_open(m_luaState);
	test_script_dlg_open(m_luaState);

	return true;
}

//加载一个指定的lua文件
BOOL CScriptSys::LoadOneFileOrString(const char* strFileName)
{
	WaitForSingleObject(m_hStateMutex,INFINITE);

	int nRet = luaL_dofile(m_luaState,strFileName);
	if( nRet != 0 )
	{
		char strInfo[102400]="";
		sprintf(strInfo,"error in function <CScriptSys::LoadOneFileOrString>(ErroNum:%d,info:%s)",
			nRet,tolua_tostring(m_luaState,-1,0));
		PutDebugString(strInfo);
		m_pTestDialog->OutputScriptInfo(strInfo);
		return FALSE;
	}
	ReleaseMutex(m_hStateMutex);
	return TRUE;
}
//
bool CScriptSys::UnInitial()
{
	lua_close(m_luaState);
	if(m_nDBSize>0)
	{
		SAFE_DELETE_ARRAY(m_pDB);
		m_nDBSize = 0;
	}
	return true;
}

BYTE* CScriptSys::GetDB(int minsize)
{
	if(minsize>m_nDBSize)
	{
		SAFE_DELETE_ARRAY(m_pDB);
		m_nDBSize=minsize;
		m_pDB = new BYTE[m_nDBSize];
	}
	return m_pDB;
}

int OnScriptMessage(lua_State* L,CMessage* pMsg);

void CScriptSys::RunMessage(CMessage *pMsg)
{
	WaitForSingleObject(m_hStateMutex,INFINITE);

	long lType = pMsg->GetType();

	lType = lType&0xfffff00;
	if( lType == MSG_L2C_LOG || lType == MSG_S2C_LOG || lType == 0 )
	{
		OnScriptMessage(m_luaState,pMsg);
	}
	else
	{
		long lSocketID = pMsg->GetSocketID();
		CGameClient* pClient = m_pTestDialog->GetGameClient(lSocketID);
		if(pClient )
		{
			pClient->OnMessage(pMsg);
		}
	}

	ReleaseMutex(m_hStateMutex);
}

//执行某个字符串 -->这里指Lua脚本文件中的某个具体函数名 需加括号
void CScriptSys::PerformString(const char* strContent)
{
	WaitForSingleObject(m_hStateMutex,INFINITE);
	if(NULL == strContent) 
	{
		ReleaseMutex(m_hStateMutex);
		return;
	}

	int nRet = luaL_dostring(m_luaState,strContent);
	if( nRet != 0 )
	{
		char strInfo[102400]="";
		sprintf(strInfo,"error in function <CScriptSys::PerformString>(ErroNum:%d,info:%s)",
						nRet,tolua_tostring(m_luaState,-1,0));
		PutDebugString(strInfo);
		m_pTestDialog->OutputScriptInfo(strInfo);
	}
	ReleaseMutex(m_hStateMutex);
}

//定时响应
long CScriptSys::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	WaitForSingleObject(m_hStateMutex,INFINITE);

	tagScriptTimer* pScrTimer = (tagScriptTimer*)var;
	lua_pushvalue(m_luaState,LUA_GLOBALSINDEX);
	lua_pushstring(m_luaState, "OnTimeOut");
	lua_rawget(m_luaState,-2);
	lua_pushnumber(m_luaState,timerid);
	lua_pushnumber(m_luaState,pScrTimer->arg);
	int nRet = lua_pcall(m_luaState,2,1,0);
	if(nRet != 0)
	{
		char strInfo[512]="";
		sprintf(strInfo,"error in function <CScriptSys::OnTimeOut>(ErroNum:%d,info:%s)",
			nRet,tolua_tostring(m_luaState,-1,0));
		PutDebugString(strInfo);
		m_pTestDialog->OutputScriptInfo(strInfo);
		lua_pop(m_luaState,1);
		return 0;
	}
	nRet = (int)tolua_tonumber(m_luaState,-1,0);
	lua_pop(m_luaState,1);

	ReleaseMutex(m_hStateMutex);
	return nRet;
}
//当定时器被取消的调用
void CScriptSys::OnTimerCancel(DWORD timerid,const void* var)
{
	tagScriptTimer* pScrTimer = (tagScriptTimer*)var;
	SAFE_DELETE(pScrTimer);
}

//定制一个定时器
long CScriptSys::Schedule(	unsigned long arg,
			  unsigned long futuretime,
			  unsigned long interval)
{
	futuretime += timeGetTime();         //得到当前时间+futuretime        
	tagScriptTimer* pScrTimer = new tagScriptTimer;
	pScrTimer->arg = arg;
	long lTiemrID = m_pTestDialog->GetTimer()->Schedule(this,(void*)pScrTimer,futuretime,interval);
	//如果有错误
	if(-1 == lTiemrID)
		delete pScrTimer;
	return lTiemrID;
}

//取消一个定时器
int CScriptSys::Cancel(long timerid)
{
	int nRet = m_pTestDialog->GetTimer()->Cancel(timerid);	
	return nRet;
}

//	连接到登录服务器
int CScriptSys::ConnectLoginServer()
{
	const CTestScript::tagServerList* pServer = m_pTestDialog->GetTestScript()->GetServerIpAddrAndPort();
	return m_pTestDialog->GetNetServer()->Connect(pServer->strIp.c_str(),pServer->dwPort,100);
}
//连接到服务器
int CScriptSys::ConnectServer(const char* strIP,int nPort,long lFlag)
{
	return m_pTestDialog->GetNetServer()->Connect(strIP,nPort,lFlag);
}

int CScriptSys::ShutDownSocket(long lSocket)
{
	return m_pTestDialog->GetNetServer()->DisConn(lSocket);
}

//脚本调度
int ScheduleTimer(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnumber(L,1,0,&err) ||				//socketid
		!tolua_isnumber(L,2,0,&err) ||				//
		!tolua_isnumber(L,3,0,&err) ||				//时间间隔
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'ScheduleTimer'.",&err);
		return 0;
	}
#endif
    DWORD arg = tolua_tonumber(L,1,0);
	DWORD furturetime = tolua_tonumber(L,2,0);
	DWORD interval =  tolua_tonumber(L,3,0);
	DWORD lTimerID = CScriptSys::getInstance()->Schedule(arg,furturetime,interval);
	tolua_pushnumber(L,(lua_Number)lTimerID);
	return 1;
}

int CancelTimer(lua_State* L)
{	
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isnumber(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'CancelTimer'.",&err);
		return 0;
	}
#endif
	long lTimerID = tolua_tonumber(L,1,0);
	long nRet = CScriptSys::getInstance()->Cancel(lTimerID);
	tolua_pushnumber(L,(lua_Number)nRet);
	return 1;
}

int ConnectLoginServer(lua_State*L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'ConnectServer'.",&err);
		return 0;
	}
#endif
	long lSokcetID =  CScriptSys::getInstance()->ConnectLoginServer();
	tolua_pushnumber(L,lSokcetID);
	return 1;
}

int ConnectServer(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(!tolua_isstring(L,1,0,&err) ||
		!tolua_isnumber(L,2,0,&err) ||
		!tolua_isnumber(L,3,0,&err) ||
		!tolua_isnoobj(L,4,&err) )
	{
		tolua_error(L,"#ferror in function 'ConnectServer'.",&err);
		return 0;
	}
#endif
	const char* strip = tolua_tostring(L,1,0);
	long port = tolua_tonumber(L,2,0);
	long flag = tolua_tonumber(L,3,0);
	long lSokcetID =  CScriptSys::getInstance()->ConnectServer(strip,port,flag);
	tolua_pushnumber(L,lSokcetID);
	return 1;
}


int ShutDownSocket(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'ShutDownSocket'.",&err);
		return 0;
	}
#endif
	long lSocketID = (long)tolua_tonumber(L,1,0);
	int nRet = CScriptSys::getInstance()->ShutDownSocket(lSocketID);
	tolua_pushnumber(L,nRet);
	return 1;
}

int GetDB(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) ||
		!tolua_isnoobj(L,2,&err) )
	{
		tolua_error(L,"#ferror in function 'GetDB'.",&err);
		return 0;
	}
#endif
	long num = (long)tolua_tonumber(L,1,0);
	BYTE* pDB = CScriptSys::getInstance()->GetDB(num);
	tolua_pushuserdata(L,(void*)pDB);
	return 1;
}

int Get_Read_DB(lua_State*L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'Get_Read_DB'.",&err);
		return 0;
	}
#endif
	DBReadSet readDB;
	return 0;
}
int GetDialog(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'GetDialog'.",&err);
		return 0;
	}
#endif
	void* dialog = (void*)CScriptSys::getInstance()->GetTestDialog();
	tolua_pushusertype(L,dialog,"CGameTestDlg");
	return 1;
}

int SendMessage(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) )
	{
		tolua_error(L,"#ferror in function 'GetDialog'.",&err);
		return 0;
	}
#endif
	UINT msg = (UINT)tolua_tonumber(L,1,0);
	DWORD wParam = (DWORD)tolua_touserdata(L,2,0);
	DWORD lParam = (DWORD)tolua_touserdata(L,3,0);
	::SendMessage(CScriptSys::getInstance()->GetTestDialog()->m_hWnd,msg,
					wParam,lParam);
	return 0;
}

int PostMessage(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) )
	{
		tolua_error(L,"#ferror in function 'GetDialog'.",&err);
		return 0;
	}
#endif
	UINT msg = (UINT)tolua_tonumber(L,1,0);
	DWORD wParam = (DWORD)tolua_tonumber(L,2,0);
	DWORD lParam = (DWORD)tolua_tonumber(L,3,0);
	::PostMessage(CScriptSys::getInstance()->GetTestDialog()->m_hWnd,msg,
					wParam,lParam);
	return 0;
}

int AllocToken(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err) )
	{
		tolua_error(L,"#ferror in function 'AllocToken'.",&err);
		return 0;
	}
#endif
	CGameTestDlg::tagTokenCDKey* pToken = CScriptSys::getInstance()->GetTestDialog()->AllocTokenCDKey();
	long l = 0;
	if( pToken )
	{
		tolua_pushstring(L,pToken->strNumCDKey.c_str());
		tolua_pushstring(L,pToken->strToken.c_str());
		l = 2;
	}
	return l;
}

int FreeToken(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isstring(L,1,0,&err) )
	{
		tolua_error(L,"#ferror in function 'FreeToken'.",&err);
		return 0;
	}
#endif
	char* strToken = (char*)tolua_tostring(L,1,0);
	CScriptSys::getInstance()->GetTestDialog()->FreeTokenCDKey(strToken);
	return 0;
}

//  [8/11/2009 Square]
//	参数：	resourceid,giud
//	返回值：返回一个CClientRegion
int FindRegion(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) ||
		!tolua_isusertype(L,2,"CGUID",0,&err) ||
		!tolua_isnoobj(L,3,&err) )
	{
		tolua_error(L,"#ferror in function 'FindRegion'.",&err);
		return 0;
	}
#endif
	long lResourceID = (long)tolua_tonumber(L,1,0);
	CGUID & guid = *(CGUID*)tolua_tousertype(L,2,0);
	
	CClientRegion* pRegion = CScriptSys::getInstance()->GetTestDialog()->FindClientRegion(lResourceID,guid);
	tolua_pushusertype(L,pRegion,"CClientRegion");
	return 1;
}

int GetTime(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err))
	{
		tolua_error(L,"#ferror in function 'GetTime'.",&err);
		return 0;
	}
#endif
	DWORD dwTime = timeGetTime();
	tolua_pushnumber(L,dwTime);
	return 1;
}

//新加界面MessageBox支持 by:LGR
int MessageBox(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isstring(L,1,0,&err) &&
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'MessageBox'.",&err);
		return 0;
	}
#endif
	const char* pchar = (char*)tolua_tostring(L,1,0);
	CScriptSys::getInstance()->GetTestDialog()->OperMessageBox(pchar);
	return 0;
}
//新加界面FlashWindows支持 by:LGR
int FlashWindows(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err))
	{
		tolua_error(L,"#ferror in function 'FlashWindows'.",&err);
		return 0;
	}
#endif
	CScriptSys::getInstance()->GetTestDialog()->SetFlashWindows();
	return 0;
}

//新加托盘信息显示支持  by:LGR
int TaskbarNotifier(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isstring(L,1,0,&err) &&
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'TaskbarNotifier'.",&err);
		return 0;
	}
#endif
	const char* cBarInfo = (char*)lua_tostring(L,1,0);
	CScriptSys::getInstance()->GetTestDialog()->SetTaskbarNotifierInfo(cBarInfo);
	return 0;
}

//新加Client连接量支持  by:LGR
int GetClientConnnectCount(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnoobj(L,1,&err))
	{
		tolua_error(L,"#ferror in function 'GetClientConnnectCount'.",&err);
		return 0;
	}
#endif
	long lCilent = CScriptSys::getInstance()->GetTestDialog()->GetNetServer()->GetClientCount();
	tolua_pushnumber(L,lCilent);
	return 1;
}

//新加tbWaitLogin量支持  by:LGR
int SettbwaitLoginCount(lua_State* L)
{
#ifdef _DEBUG
	tolua_Error err;
	if(	!tolua_isnumber(L,1,0,&err) &&
		!tolua_isnoobj(L,2,&err))
	{
		tolua_error(L,"#ferror in function 'SettbwaitLoginCount'.",&err);
		return 0;
	}
#endif
	long lCount = (long)tolua_tonumber(L,1,0);
	CScriptSys::getInstance()->GetTestDialog()->SettbWaitLoginCount(lCount);
	return 0;
}



int  script_sys_open(lua_State* L)
{
	tolua_open(L);
	tolua_module(L,NULL,0);
	tolua_beginmodule(L,NULL);
	{
		tolua_module(L,"Game",0);
		tolua_beginmodule(L,"Game");
		{
			//注册函数
			//					Lua函数名		C++函数名
			tolua_function(L,"schedule_timer",	ScheduleTimer);
			tolua_function(L,"cancel_timer",	CancelTimer);
			tolua_function(L,"connect_login",	ConnectLoginServer);
			tolua_function(L,"connect_server",	ConnectServer);
			tolua_function(L,"shutdown_socket",	ShutDownSocket);
			tolua_function(L,"get_db",			GetDB);
			tolua_function(L,"get_db_read",		Get_Read_DB);		//get_db_read
			tolua_function(L,"get_dialog",		GetDialog);
			tolua_function(L,"send_message",	SendMessage);
			tolua_function(L,"post_message",	PostMessage);
			tolua_function(L,"get_time",		GetTime);

			//2010.5.4 by:LGR
			tolua_function(L,"message_box",		MessageBox);
			tolua_function(L,"flash_windows",	FlashWindows);
			tolua_function(L,"taskbar_notifier",TaskbarNotifier);
			tolua_function(L,"get_client_count",GetClientConnnectCount);
			tolua_function(L,"set_tbwaitlogin_count",SettbwaitLoginCount);

			//  [8/11/2009 Square]
			tolua_function(L,"find_region",		FindRegion);

			tolua_function(L,"alloc_token",		AllocToken);
			tolua_function(L,"free_token",		FreeToken);

			tolua_constant(L,"TYPE_PET",	TYPE_PET);
			tolua_constant(L,"TYPE_PLAYER",	TYPE_PLAYER);
			tolua_constant(L,"TYPE_GOODS",	TYPE_GOODS);
			tolua_constant(L,"TYPE_REGION",	TYPE_REGION);
			tolua_constant(L,"TYPE_MONSTER",	TYPE_MONSTER);
			tolua_constant(L,"TYPE_SESSION",TYPE_SESSION);
			tolua_constant(L,"TYPE_PLUG",TYPE_PLUG);

			tolua_constant(L,"PEI_PACKET",	PEI_PACKET);
			tolua_constant(L,"PEI_EQUIPMENT",PEI_EQUIPMENT);
			tolua_constant(L,"PEI_WALLET",	PEI_WALLET	);							// 金币钱包
			tolua_constant(L,"PEI_SILVERWALLET",PEI_SILVERWALLET);					// 银币钱包
			tolua_constant(L,"PEI_GEM",PEI_GEM);							// 宝石
			tolua_constant(L,"PEI_YUANBAO",PEI_YUANBAO);						// 元宝
			tolua_constant(L,"PEI_JIFEN",PEI_JIFEN);							// 积分
			tolua_constant(L,"PEI_MEDAL",PEI_MEDAL);
			tolua_constant(L,"PEI_SUBPACK",PEI_SUBPACK);//=10000,
			tolua_constant(L,"PEI_PACK1",PEI_PACK1);//=10001,
			tolua_constant(L,"PEI_PACK2",PEI_PACK2);//=10002,
			tolua_constant(L,"PEI_PACK3",PEI_PACK3);//=10003,
			tolua_constant(L,"PEI_PACK4",PEI_PACK4);//=10004,
			tolua_constant(L,"PEI_PACK5",PEI_PACK5);//=10005

			tolua_constant(L,"GT_MONEY",	GBT_MONEY);
			tolua_constant(L,"GT_SILVERMONEY",	GBT_SILVERMONEY);

			tolua_constant(L,"eDCT_Primary",	eDCT_Primary);
			tolua_constant(L,"eDCT_Secondary",	eDCT_Secondary);
			tolua_constant(L,"eDCT_Gold",	eDCT_Gold);
			//tolua_constant(L,"eDCT_Gem",	eDCT_Gem);

//			tolua_variable(L,"INVALID_GUID",CGUID::GUID_INVALID);
			tolua_constant(L,"GoodsDecomp",			PT_DisassembleWindow);//! 分解窗口
			tolua_constant(L,"GoodsComp",			PT_PrescriptionWindow);//! 合成窗口
			tolua_constant(L,"GoodsBind",			PT_BindWindow);//! 绑定窗口
			tolua_constant(L,"GoodsFreeze",			PT_FrostWindow);//! 冻结窗口
			tolua_constant(L,"GoodsUpgrade",		PT_UpgradeWindow);//! 升级窗口
			tolua_constant(L,"GoodsMagic",			PT_PlusEvilWindow);//! 魔化窗口
			tolua_constant(L,"GoodsBackoutMagic",	PT_EvilBackoutWindow);//! 魔化拆除窗口
			tolua_constant(L,"GoodsMgcStuffComp",	PT_EvilSynthesizeWindow);//! 魔化材料合成窗口
			tolua_constant(L,"GoodsDepot",			PT_DepotWindow);		//! 仓库窗口
		}
		tolua_endmodule(L);
	}
	tolua_endmodule(L);
	return 1;
}



