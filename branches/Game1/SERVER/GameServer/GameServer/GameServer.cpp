// GameServer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GameServer.h"
#include "Game.h"
#include "../AppServer/ServerRegion.h"
#include "../AppServer/script/Script.h"
#include "../nets/Servers.h"
#include "../AppServer/RgnManager.h"
#include "GameManager.h"
#include "..\LogClient\gs\LogicLogInterface.h"
#include "../AppServer/AntiCheat/AntiCheatCommon.h"

#define MAX_LOADSTRING 100
	


// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HWND g_hWnd;
HWND g_hInfoText;
HWND g_hLogText;
HWND g_hRegionListBox;
//HWND g_hPlayerListBox;

long g_lInfoWindowHeight = 140;			// 信息显示窗口的大小

extern unsigned volatile RELOAD_SETUP_FLAGS;	// 重度配置文件标示
extern const unsigned RELOAD_SETUP;
extern const unsigned RELOAD_SETUPEX;
extern const unsigned RELOAD_SETUPMAPFILE;
extern const unsigned OUTPUT_SCRIPT_INFO;
extern const unsigned OUTPUT_MEMPOOL_INFO;


// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	EnableDebugNew();
	//_CrtSetBreakAlloc(333);

 	// TODO: 在此放置代码。
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GAMESERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_GAMESERVER);

	unsigned threadID;
	HANDLE  hGameThread = (HANDLE)_beginthreadex(NULL,0,GameThreadFunc,NULL,0,&threadID);

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

	// 保存日志记录
	SaveLogText(true);

	PutLogInfo("\r\n//////////////////////////////////");
	PutLogInfo("\r\n//       GameServer Exit        //");
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_GAMESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_GAMESERVER;
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
	g_hRegionListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-160,0,160,rect.bottom,g_hWnd,0,hInst,NULL);
//	g_hPlayerListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-160,g_lInfoWindowHeight,160,h-g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
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

		case ID_CONNECT_WORLDSERVER:
			//GetGame()->ReConnectWorldServer();
			GetGame()->CreateConnectWorldThread();
			break;
		case ID_CONNECT_DBSERVER:
			//GetGame()->ReConnectWorldServer();
			GetGame()->CreateConnectDBThread();
			break;

		case ID_CONNECT_BOUNSSERVER:
			if(GetGame()->GetSetup()->lUseBounsServer)
			{
				GetGame()->CreateConnectBounsServerThread();
			}			
			break;

		case ID_RELOADSETUP:
			RELOAD_SETUP_FLAGS |= RELOAD_SETUP;
			break;
		case ID_RELOADSETUPEX:
			RELOAD_SETUP_FLAGS |= RELOAD_SETUPEX;
			break;
		case ID_RELOAD_MAPFILESETUP:
			RELOAD_SETUP_FLAGS |= RELOAD_SETUPMAPFILE;
			break;
		case ID_OUTPUTSCRIPTINFO:
			RELOAD_SETUP_FLAGS |= OUTPUT_SCRIPT_INFO;
			break;
		case ID_MEMPOOLINFO:
			RELOAD_SETUP_FLAGS |= OUTPUT_MEMPOOL_INFO;
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
			MoveWindow(g_hInfoText, 0, 0, lW-160, g_lInfoWindowHeight-4, true);
			MoveWindow(g_hLogText, 0, g_lInfoWindowHeight, lW-160, lH-g_lInfoWindowHeight, true);
			MoveWindow(g_hRegionListBox, lW-160,0,160,lH, true);
		//	MoveWindow(g_hPlayerListBox, lW-160,g_lInfoWindowHeight,160,lH-g_lInfoWindowHeight, true);
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
		timeGetTime() - dwLastSaveTime > GetGame()->GetSetup()->dwSaveInfoTime ||		// 大于时间
		GetWindowTextLength(g_hLogText) >= lMaxLength		// 内容超过64K
		)
	{
		dwLastSaveTime = timeGetTime();

		PutLogInfo("\r\n=============================== Start Save Log ===============================\r\n");

		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		_snprintf(str, lMaxLength, "%04d-%02d-%02d %02d:%02d:%02d\r\n\r\n", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		PutLogInfo(str);

		GetWindowText(g_hInfoText, str, lMaxLength);
		PutLogInfo(str);

		PutLogInfo("\r\n");

	//	GetWindowText(g_hLogText, str, lMaxLength);
	//	PutLogInfo(str);
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
	_snprintf(str, lMaxLength, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va,msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat_s(str, lMaxLength, "\r\n");

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
	_snprintf(str, lMaxLength, "[%02d-%02d %02d:%02d:%02d] <error> ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va,msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat_s(str, 64000, "\r\n");

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

	static long lMaxConnects = 0,lMaxPlayers = 0, lMaxTeams = 0, lMaxScripts = 0;
	static long lSendSizePerSecondToClient = 0, lRecvSizePerSecondFromClient = 0;
	static long lSendSizePerSecondToWorld = 0, lRecvSizePerSecondFromWorld = 0;

	static long lMaxTeamRgnNum = 0;
	static long lMaxPRgnNum = 0;
	static DWORD dwMaxAvaiVirMem = 0;
	static DWORD dwHandleCount = 0;

	static long lMaxDupRgnDataNum = 0;

	long lConnects = GetGame()->GetNetServer()->GetClientCount();
	long lPlayers = GetGame()->GetPlayerCount();
	long lTeams = GetGame()->GetNumTeams();
	long lScripts = g_Scripts.size();
	MEMORYSTATUS msC;GlobalMemoryStatus(&msC);
	DWORD dwCurAvaiVirMem = msC.dwAvailVirtual/1048576;
	DWORD dwCurNandleCount = 0;
	GetProcessHandleCount(GetCurrentProcess(),&dwCurNandleCount);

	DWORD dwAcceptThreadTick = GetGame()->GetNetServer()->GetAcceptThreadTick();
	DWORD dwNetThreadTick = GetGame()->GetNetServer()->GetNetThreadTick();
	DWORD dwWorkThreadTick = GetGame()->GetNetServer()->GetWorkThreadTick();
	DWORD dwMainThreadTick = GetGame()->GetAITick();

	long lCurTeamRgnNum = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().size();
	long lCurPRgnNum = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().size();

	long lSendSizePerSecondToClientTempt = GetGame()->GetNetServer()->GetSendSizePerSecond(NF_Server_Client);
	long lRecvSizePerSecondFromClientTempt = GetGame()->GetNetServer()->GetRecvSizePerSecond(NF_Server_Client);	
	long lSendSizePerSecondToWorldTempt = GetGame()->GetNetServer()->GetSendSizePerSecond(NF_WServer_Client);
	long lRecvSizePerSecondFromWorldTempt = GetGame()->GetNetServer()->GetRecvSizePerSecond(NF_WServer_Client);

	long lCurDupRgnDataNum = GameManager::GetInstance()->GetRgnManager()->GetDupRgnDataNum();

	if (lConnects>lMaxConnects)
		lMaxConnects = lConnects;
	if (lPlayers>lMaxPlayers)
		lMaxPlayers = lPlayers;
	if (lTeams>lMaxTeams)
		lMaxTeams = lTeams;
	if (lScripts>lMaxScripts)
		lMaxScripts = lScripts;

	if(lCurTeamRgnNum>lMaxTeamRgnNum)
		lMaxTeamRgnNum = lCurTeamRgnNum;

	if(lCurPRgnNum > lMaxPRgnNum)
		lMaxPRgnNum = lCurPRgnNum;

	if(dwCurAvaiVirMem > dwMaxAvaiVirMem)
		dwMaxAvaiVirMem = dwCurAvaiVirMem;
	if(dwCurNandleCount > dwHandleCount)
		dwHandleCount = dwCurNandleCount;

	if(lCurDupRgnDataNum > lMaxDupRgnDataNum)
		lMaxDupRgnDataNum = lCurDupRgnDataNum;

	
	if(lSendSizePerSecondToClientTempt > lSendSizePerSecondToClient)
		lSendSizePerSecondToClient = lSendSizePerSecondToClientTempt;
	if(lRecvSizePerSecondFromClientTempt > lRecvSizePerSecondFromClient)
		lRecvSizePerSecondFromClient = lRecvSizePerSecondFromClientTempt;
	if(lSendSizePerSecondToWorldTempt > lSendSizePerSecondToWorld)
		lSendSizePerSecondToWorld = lSendSizePerSecondToWorldTempt;
	if(lRecvSizePerSecondFromWorldTempt > lRecvSizePerSecondFromWorld)
		lRecvSizePerSecondFromWorld = lRecvSizePerSecondFromWorldTempt;

	// 反外挂信息监控 :Ques\AccWrong\AccTimeOut\AccRight
	const AntiCheat::LogInfo *ac_log = AntiCheat::GetLogInfo();

	_snprintf(str, 102400, "Connects = %d/%d  Players = %d/%d\r\n\
HandleCount=%d/%d	AvaiVirMem =%d/%d\r\n\
AccTTick=%u NetTTick=%u WorkTTick=%u MainTTick=%u\r\n\
Teams=%d/%d  Scripts=%d/%d  ServerID = %d\r\nNT-S2C=%d/%d(B/S)\t\tNT-C2S=%d/%d(B/S)\r\nNT-S2W=%d/%d(B/S)\t\tNT-W2S=%d/%d(B/S)\r\n队伍副本个数=%d/%d 个人副本个数=%d/%d 副本数据块个数=%d/%d\r\n\
AntiCheat=%u/%u/%u/%u/%u\r\n\r\n",
					lConnects, lMaxConnects,lPlayers, lMaxPlayers,
					dwCurNandleCount,dwHandleCount,dwCurAvaiVirMem,dwMaxAvaiVirMem,
					dwAcceptThreadTick,dwNetThreadTick,dwWorkThreadTick,dwMainThreadTick,
					lTeams, lMaxTeams,lScripts, lMaxScripts, GetGame()->GetIDIndex(),
					lSendSizePerSecondToClientTempt,lSendSizePerSecondToClient,lRecvSizePerSecondFromClientTempt,lRecvSizePerSecondFromClient,
					lSendSizePerSecondToWorldTempt,lSendSizePerSecondToWorld,lRecvSizePerSecondFromWorldTempt,lRecvSizePerSecondFromWorld, 
					lCurTeamRgnNum, lMaxTeamRgnNum, lCurPRgnNum, lMaxPRgnNum, lCurDupRgnDataNum, lMaxDupRgnDataNum,
					ac_log->AccQues, ac_log->AccWrong, ac_log->AccTimeOut, ac_log->AccRight, ac_log->AccSysErr );


	// 显示每个场景的人数
	static char strTemp[256];
	

	
	// Normal Region
	//for(map<long, CServerRegion*>::iterator it=GetRegionMap()->begin(); it!=GetRegionMap()->end(); it++)
	for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end(); it++)
	{
		CServerRegion* pRegion = it->second;
		if (pRegion)
		{
			_snprintf(strTemp, 256, "%s=%d ", pRegion->GetName(), pRegion->GetPlayerAmout() );
			strcat(str, strTemp);
		}
	}

	SetWindowText(g_hInfoText, str);
}

void AddPlayerList(const char *str)
{
	//SendMessage(g_hPlayerListBox,LB_INSERTSTRING,-1,(LPARAM)(str));
}

void DelPlayerList(const char *str)
{
	//LRESULT ret = SendMessage(g_hPlayerListBox,LB_SELECTSTRING, 0,(LPARAM)(str));
	//if (ret!=LB_ERR)
	//{
	//	SendMessage(g_hPlayerListBox, LB_DELETESTRING, ret, 0);
	//}
}

void AddRegionList(const char *str)
{
	SendMessage(g_hRegionListBox,LB_INSERTSTRING,-1,(LPARAM)(str));
}

void DelRegionList(const char *str)
{
	LRESULT ret = SendMessage(g_hRegionListBox,LB_SELECTSTRING, 0,(LPARAM)(str));
	if (ret!=LB_ERR)
	{
		SendMessage(g_hRegionListBox, LB_DELETESTRING, ret, 0);
	}
}
