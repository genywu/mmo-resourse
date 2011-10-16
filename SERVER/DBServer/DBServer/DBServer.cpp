// dbserver.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DBServer.h"
#include "game.h"
#include "..\..\..\DBAccess\WorldDB\RsEntity.h"
#include "..\nets\Servers.h"

#include "../AppWorld/entity/entityManager.h"

#define MAX_LOADSTRING 100

extern unsigned volatile RELOAD_PROFILE_FLAGS;	// 重度配置文件标示
// define const
extern const unsigned RELOAD_LOGING;		//读日志配置
// end define

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool volatile g_bWriteLogThreadExit;
extern HANDLE g_hWriteLogThread;

extern bool volatile g_bPlayerLoadThreadsExit;
extern vector<HANDLE> g_vPlayerLoadThreads;

// 保存日志
void SaveLogText(bool bForce);

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HWND g_hWnd;
HWND g_hInfoText;
HWND g_hLogText;
HWND g_hGameServerListBox;
HWND g_hPlayerListBox;

long g_lInfoWindowHeight = 140;			// 信息显示窗口的大小
SYSTEMTIME g_stLastSaveTime;			// 上次成功存盘的时间
DWORD g_dwLastSaveTick=timeGetTime();	// 上次存盘成功的计数
DWORD g_dwThisSaveStartTime = 0;		// 本次存盘开始时间

// 定时刷新信息
long lConnects		= 0;
long lMapAccounts= 0;
long lMapScripts= 0;
long lMapProcs= 0;

long lMapPlayers	= 0;
long lPlayers		= 0;
long lFriendGroup	= 0;
long lOfflinePlayers= 0;
long lLoginPlayers	= 0;
long lCreatePlayers = 0;
long lDeletePlayers = 0;
long lResumePlayers = 0;
long lOperingLoadEntitys	= 0;
long lOperingSaveEntitys	= 0;
long lTeams			= 0;
long lLargess		= 0;
long lWriteLog		= 0;
long lLoadPlayer	= 0; 

long lMails = 0;
long lFactions = 0;

long lCurLoadThreadQueueSize = 0;
long lCurSaveThreadQueueSize = 0;

long lBeginLoadNum = 0;
long lBeginSaveNum = 0;
long lEndNum = 0;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

char g_strCurrentPath[MAX_PATH];

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	EnableDebugNew();

	//_CrtSetBreakAlloc(1214);

 	// TODO: 在此放置代码。
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_dbserver, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	GetCurrentDirectory(MAX_PATH, g_strCurrentPath);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_dbserver);

	unsigned threadID;
	HANDLE hGameThread  = (HANDLE)_beginthreadex(NULL,0,GameThreadFunc,NULL,0,&threadID);

	g_hGameThreadExitEvent = CreateEvent(NULL,0,0,0);

	// 主消息循环:
	while(1)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				g_bGameThreadExit = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //if(g_bActive)
		{
			Sleep(1);
		}
	}

	// 保存日志信息
	SaveLogText(true);

	PutLogInfo("\r\n//////////////////////////////////");
	PutLogInfo("\r\n//       dbserver Exit       //");
	PutLogInfo("\r\n//////////////////////////////////\r\n\r\n");

	// 等待主线程结束
	WaitForSingleObject(g_hGameThreadExitEvent, INFINITE);
	CloseHandle(hGameThread);

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释: 
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_dbserver);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_dbserver;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HANDLE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 400, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	RECT rect;
	GetClientRect(g_hWnd, &rect);
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

	g_hInfoText=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,0,rect.right-160, g_lInfoWindowHeight-4,g_hWnd,0,hInst,NULL);
	g_hLogText=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,g_lInfoWindowHeight,rect.right-160, rect.bottom-g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
	g_hGameServerListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-160,0,160,g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
	g_hPlayerListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-160,g_lInfoWindowHeight,160,h-g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
	SendMessage(g_hLogText, EM_SETLIMITTEXT, -1, 0);

	return TRUE;
}

//
//  函数: WndProc(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			if (MessageBox(g_hWnd, "是否退出?", "msg", MB_YESNO)==IDYES)
				DestroyWindow(hWnd);
			break;

		// 连接LoginServer
		case ID_CONNECT_LOGINSERVER:

			break;

		case ID_CONNECT_COSTSERVER:
			break;

		case ID_MENU_RELOAD_LOGSYSTEM:
			RELOAD_PROFILE_FLAGS |= RELOAD_LOGING;
			break;
	
		// 立刻存盘
		case ID_SAVEDATA:
			extern bool g_bSendSaveMsgNow;
			g_bSendSaveMsgNow = true;
			break;

		// 请求GameServer玩家数据
		case ID_MENU_COLLECTPLAYERDATA:
			extern bool g_bSendCollectPlayerDataMsgNow;
			g_bSendCollectPlayerDataMsgNow = true;
			break;

		case ID_DELPLAYER:
			extern bool g_bDelPlayers;
			g_bDelPlayers = true;
			break;
		// 立刻存当前数据
		case ID_SAVEALLDATA:
			extern bool g_bSaveNowData;
			g_bSaveNowData = true;
			break;
		case ID_MAKEBINDATA:
			GetGame()->Test3GoodsImageDataBuild();
			break;
		case ID_MAILGOODS:
			GetGame()->Test3MailGoodsDataBuild();
			break;

		case ID_Menu_GiftChargeItems:
			GetGame()->ReLoad("GiftItems", true, true);
			break;

	
		// 显示存盘消息开关
		case ID_SHOWSAVEINFO:
			g_bShowSaveInfo ? g_bShowSaveInfo=false : g_bShowSaveInfo=true;
			break;

		case ID_SWITCH_LOGSYS:
			
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		if (wParam !=0 || MessageBox(g_hWnd, "是否退出?", "msg", MB_YESNO)==IDYES)
			PostQuitMessage(0);
		break;
	case WM_SIZE:
		{
			long lW = LOWORD(lParam);
			long lH = HIWORD(lParam);
			MoveWindow(g_hInfoText, 0,0, lW-160, g_lInfoWindowHeight-4, true);
			MoveWindow(g_hLogText, 0,g_lInfoWindowHeight, lW-160, lH-g_lInfoWindowHeight, true);
			MoveWindow(g_hGameServerListBox, lW-160,0,160,g_lInfoWindowHeight, true);
			MoveWindow(g_hPlayerListBox, lW-160,g_lInfoWindowHeight,160,lH-g_lInfoWindowHeight, true);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// 保存日志
void SaveLogText(bool bForce)
{
	const int lMaxLength = 64000;	// log信息最大长度
	static char str[lMaxLength];

	// 写日志文件
	static DWORD dwLastSaveTime = timeGetTime();

	if (bForce ||											// 强制写
		timeGetTime() - dwLastSaveTime > GetGame()->GetSetup()->dwSaveInfoTime ||		// 大于
		GetWindowTextLength(g_hLogText) >= lMaxLength		// 内容超过64K
		)
	{
		dwLastSaveTime = timeGetTime();

		PutLogInfo("\r\n=============================== Start Save Log ===============================\r\n");

		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d\r\n\r\n", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		PutLogInfo(str);

		GetWindowText(g_hInfoText, str, lMaxLength);
		PutLogInfo(str);

		PutLogInfo("\r\n");

		SetWindowText(g_hLogText, "");

		PutLogInfo("================================ End Save Log ================================\r\n");
	}
}

// 加入一条记录
void AddLogText(char* msg, ...)
{
	const int lMaxLength = 64000;	// log信息最大长度
	static char str[lMaxLength];

	// 写日志文件
	SaveLogText(false);

	// 写服务器界面
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(str, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va,msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat(str, "\r\n");

	PutLogInfo(str);

	SendMessage(g_hLogText, EM_SETSEL, 0, -1);
	SendMessage(g_hLogText, EM_SETSEL, -1, 0);
	SendMessage(g_hLogText, EM_REPLACESEL, FALSE, (LPARAM)str);
}

// 加入一条出错记录
void AddErrorLogText(char* msg, ...)
{
	const int lMaxLength = 64000;	// log信息最大长度
	static char str[lMaxLength];

	// 写日志文件
	SaveLogText(false);

	// 写服务器界面
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(str, "[%02d-%02d %02d:%02d:%02d] <error> ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va,msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat(str, "\r\n");

	PutLogInfo(str);

	SendMessage(g_hLogText, EM_SETSEL, 0, -1);
	SendMessage(g_hLogText, EM_SETSEL, -1, 0);
	SendMessage(g_hLogText, EM_REPLACESEL, FALSE, (LPARAM)str);
}

// 刷新服务器信息
void RefeashInfoText()
{
	static char str[102400];

	if (!GetGame() || !GetGame()->GetNetServer())
	{
		return;
	}

	static long lMaxConnects = 0;

	static long lMaxMapAccounts= 0;
	static long lMaxMapScripts= 0;
	static long lMaxMapProcs= 0;

	static long lMaxMapPlayers = 0;
	static long lMaxDepots = 0;
	static long lMaxPlayers = 0;
	static long lMaxFriendGroup = 0;
	static long lMaxOfflinePlayers = 0;
	static long lMaxLoginPlayers = 0;
	static long lMaxCreatePlayers = 0;
	static long lMaxDeletePlayers = 0;
	static long lMaxResumePlayers = 0;
	static long lMaxLoadingEntitys = 0;
	static long lMaxSavingEntitys = 0;
	static long lMaxTeams = 0;
	static long lMaxLargess = 0;
	static long lMaxWriteLog = 0;
	static long lMaxLoadPlayer = 0;
	static long lMaxRebackMessage = 0;

	static long lMaxMails = 0;
	static long lMaxFactions = 0;
	static long lBeginLoadNumMax = 0;
	static long lBeginSaveNumMax = 0;
	static long lEndNumMax = 0;

	static DWORD dwMaxAvaiVirMem = 0;
	MEMORYSTATUS msC;GlobalMemoryStatus(&msC);
	DWORD dwCurAvaiVirMem = msC.dwAvailVirtual/1048576;

	extern long g_lRebackMessageCount;
	long lRebackMessage = g_lRebackMessageCount;

	if (lMapAccounts>lMaxMapAccounts)
		lMaxMapAccounts = lMapAccounts;
	if (lMapScripts>lMaxMapScripts)
		lMaxMapScripts = lMapScripts;
	if (lMapProcs>lMaxMapProcs)
		lMaxMapProcs = lMapProcs;

	if (lConnects>lMaxConnects)
		lMaxConnects = lConnects;
	if (lMapPlayers>lMaxMapPlayers)
		lMaxMapPlayers = lMapPlayers;
	if (lPlayers>lMaxPlayers)
		lMaxPlayers = lPlayers;
	if (lFriendGroup>lMaxFriendGroup)
		lMaxFriendGroup = lFriendGroup;
	if (lOfflinePlayers>lMaxOfflinePlayers)
		lMaxOfflinePlayers = lOfflinePlayers;
	if (lLoginPlayers>lMaxLoginPlayers)
		lMaxLoginPlayers = lLoginPlayers;
	if (lCreatePlayers>lMaxCreatePlayers)
		lMaxCreatePlayers = lCreatePlayers;
	if (lDeletePlayers>lMaxDeletePlayers)
		lMaxDeletePlayers = lDeletePlayers;
	if (lResumePlayers>lMaxResumePlayers)
		lMaxResumePlayers = lResumePlayers;
	if (lOperingLoadEntitys>lMaxLoadingEntitys)
		lMaxLoadingEntitys = lOperingLoadEntitys;
	if (lOperingSaveEntitys>lMaxSavingEntitys)
		lMaxSavingEntitys = lOperingSaveEntitys;
	if (lTeams>lMaxTeams)
		lMaxTeams=lTeams;
	if (lLargess>lMaxLargess)
		lMaxLargess=lLargess;
	if( lWriteLog > lMaxWriteLog )
		lMaxWriteLog = lWriteLog;
	if( lLoadPlayer > lMaxLoadPlayer )
		lMaxLoadPlayer = lLoadPlayer;
	if (lRebackMessage > lMaxRebackMessage)
		lMaxRebackMessage = lRebackMessage;

	if(lMails > lMaxMails)
		lMaxMails = lMails;
	if(lFactions > lMaxFactions)
		lMaxFactions = lFactions;

	if(lBeginLoadNum > lBeginLoadNumMax)
		lBeginLoadNumMax = lBeginLoadNum;

	if(lBeginSaveNum > lBeginSaveNumMax)
		lBeginSaveNumMax = lBeginSaveNum;

	if(lEndNum > lEndNumMax)
		lEndNumMax = lEndNum;

	if(dwCurAvaiVirMem > dwMaxAvaiVirMem)
		dwMaxAvaiVirMem = dwCurAvaiVirMem;

	int nSave = 0;

	static char *strInfo[] = {"(正常)", "(异常!!!!)"};

	sprintf(str, 
"Last Save Time : %d 毫秒	AvaiVirMem=%d/%d(MB)\r\n\
Connects=%d/%d    Mails=%d/%d	Factions=%d/%d\r\n\
Account=%d/%d   Player=%d/%d	ScriptVar=%d/%d	Proc=%d/%d\r\n\
LoadingEntitys=%d/%d SavingEntitys=%d/%d\r\n\
FriendGroup=%d/%d\r\n\
BeginLoadNum=%d/%d   BeginSaveNum=%d/%d\r\n\
WriteLog    = %d/%d    ReMsg = %d/%d\r\n\
Oper Load Thread Num[%d] Oper Save Thread Num[%d]\r\n",
		g_dwThisSaveStartTime,dwCurAvaiVirMem,dwMaxAvaiVirMem,
		lConnects, lMaxConnects, lMails, lMaxMails,lFactions, lMaxFactions,
		lMapAccounts, lMaxMapAccounts,lMapPlayers, lMaxMapPlayers, lMapScripts,lMaxMapScripts,lMapProcs,lMaxMapProcs,
		lOperingLoadEntitys, lMaxLoadingEntitys, lOperingSaveEntitys, lMaxSavingEntitys,
		lFriendGroup, lMaxFriendGroup,
		lBeginLoadNum,lBeginLoadNumMax,lBeginSaveNum,lBeginSaveNumMax,
		lWriteLog, lMaxWriteLog, lRebackMessage, lMaxRebackMessage,
		GetGame()->GetEntityManager()->GetLoadThreadMap().size(),
		GetGame()->GetEntityManager()->GetSaveThreadMap().size());

	// 复原存盘计时
	if(g_dwThisSaveStartTime != 0)
		g_dwThisSaveStartTime = 0;

	SetWindowText(g_hInfoText, str);
}

void AddPlayerList(const char *str)
{

}

void DelPlayerList(const char *str)
{

}
