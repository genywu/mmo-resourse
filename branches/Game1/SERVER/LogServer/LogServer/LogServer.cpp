// LogServer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "LogServer.h"
#include "game.h"
#include "..\nets\netlog\message.h"
//#include "..\nets\netlog/MyNetServer.h"
#include "..\../setup/GlobeSetup.h"


#define MAX_LOADSTRING 100



// end define

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
//HWND g_hGameServerListBox;
HWND g_hPlayerListBox;

long g_lInfoWindowHeight = 200;			// 信息显示窗口的大小
SYSTEMTIME g_stLastSaveTime;			// 上次成功存盘的时间
DWORD g_dwLastSaveTick=timeGetTime();	// 上次存盘成功的计数
DWORD g_dwThisSaveStartTime = 0;		// 本次存盘开始时间

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
	LoadString(hInstance, IDC_LOGSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	GetCurrentDirectory(MAX_PATH, g_strCurrentPath);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LOGSERVER);

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
	PutLogInfo("\r\n//       LogServer Exit       //");
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LOGSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_LOGSERVER;
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
		CW_USEDEFAULT, 0, 900, 400, NULL, NULL, hInstance, NULL);

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

	g_hInfoText=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,0,rect.right-320, g_lInfoWindowHeight-4,g_hWnd,0,hInst,NULL);
	g_hLogText=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,g_lInfoWindowHeight,rect.right-320, rect.bottom-g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
	//g_hGameServerListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-320,0,320,g_lInfoWindowHeight,g_hWnd,0,hInst,NULL);
	g_hPlayerListBox=CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-320,0,320,h,g_hWnd,0,hInst,NULL);
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
			if (MessageBox(g_hWnd, "End Server?", "msg", MB_YESNO)==IDYES)
				DestroyWindow(hWnd);
			break;
		
			// 踢世界所有人
		case ID_MENU_KICKALL:
			{
				map<DWORD, CGame::tagGameServer>* pServerList = GetGame() -> GetGameServerList();

				if( pServerList ) {
					CMessage msg( MSG_W2S_OTHER_KICKALL );
					//msg.Add( lPlayerID_GM );
					for( map<DWORD, CGame::tagGameServer>::iterator itBegin = pServerList -> begin();
						itBegin != pServerList -> end(); itBegin ++ ) {
						if( itBegin -> second.bConnected ) {				
							msg.SendGSID( itBegin->first );
						}
					}
				}
			}
			break;




		///* 重读配置
		case ID_RELOADSETUP:
			{
				//bool bOldUseLogSys = GetGame()->GetSetup()->bUseLogSys;
				GetGame()->LoadSetup();

			}
			break;
		case ID_32870:
			{
				//! 重读日志的读写控制配置，并发送到所有连接的客户端
				GetGame()->ReLoadConfig();
			}
			break;

		// 显示存盘消息开关
		case ID_SHOWSAVEINFO:
			g_bShowSaveInfo ? g_bShowSaveInfo=false : g_bShowSaveInfo=true;
			break;


		case ID_CREATE_DB_TABLE://按配置在DB中创建数据表
			{
				GetGame()->CreateTableInDB();
			}
			break;
		case ID_CREATE_SQL://生成数据表创建脚本
			{
				GetGame()->CreateTableInSqlFile();
				AddLogText("create_table.sql文件已保存在服务目录下！");
			}
			break;
		case ID_32854:
			{
				GetGame()->OutTableInfoToFile();
			}
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
		if (wParam!=0 || MessageBox(g_hWnd, "End Server?", "msg", MB_YESNO)==IDYES)
			PostQuitMessage(0);
		break;
	case WM_SIZE:
		{
			long lW = LOWORD(lParam);
			long lH = HIWORD(lParam);
			MoveWindow(g_hInfoText, 0,0, lW-320, g_lInfoWindowHeight-4, true);
			MoveWindow(g_hLogText, 0,g_lInfoWindowHeight, lW-320, lH-g_lInfoWindowHeight, true);
			//MoveWindow(g_hGameServerListBox, lW-320,0,320,g_lInfoWindowHeight, true);
			MoveWindow(g_hPlayerListBox, lW-320,0,320,lH, true);
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
void RefeashInfoText(const char *pInfoText)
{
	if(NULL == pInfoText) 
		return;
	SetWindowText(g_hInfoText, pInfoText);
}

void AddClientList(const char *str)
{
	SendMessage(g_hPlayerListBox,LB_INSERTSTRING,-1,(LPARAM)(str));
}

void DelClientList(const char *str)
{
	LRESULT ret = SendMessage(g_hPlayerListBox,LB_SELECTSTRING, 0,(LPARAM)(str));
	if (ret!=LB_ERR)
	{
		SendMessage(g_hPlayerListBox, LB_DELETESTRING, ret, 0);
	}
}
