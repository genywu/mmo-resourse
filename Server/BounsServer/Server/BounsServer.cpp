// BounsServer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "BounsServer.h"

#include "Game.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

const long	CW_WIDTH		=	700;			// 主窗口宽
const long	CW_HEIGHT		=	500;			// 主窗口高
const long	INFOTEXT_WIDTH	=	200;			// 信息显示窗口宽
const long	INFOTEXT_HEIGHT	=	140;			// 信息显示窗口高

HWND g_hWnd;
HWND g_hInfoText;
HWND g_hLogText;
HWND g_hClientListBox;

bool g_bDealMsgState	= true;	// 当前奖励消息处理开关
long g_lLoadMsgNum		= 0;		// 请求查询的消息数量
long g_lDealLoadMsgNum	= 0;		// 处理完的查询消息数量
long g_lFreezeMsgNum	= 0;		// 请求冻结的消息数量
long g_lFreezeBakNum	= 0;		// 处理完的冻结消息数量
long g_lObtainMsgNum	= 0;		// 获取奖励的消息数量
long g_lObtainBakNum	= 0;		// 处理完的获取消息数量
long g_lConnectedGSNum	= 0;		// 连接上的GS数量


// 保存日志
void SaveLogText(bool bForce);

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BOUNSDB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOUNSDB));

	unsigned int threadID;
	HANDLE hGameThread = (HANDLE)_beginthreadex(NULL, 0, GameThreadFunc, NULL, 0, &threadID);

	g_hGameThreadExitEvent = CreateEvent(NULL, 0, 0, 0);

	// 主消息循环:
	while(1)
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( !GetMessage(&msg, NULL, 0, 0) )
			{
				g_bGameThreadExit = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(1);
		}
	}

	// 保存日志信息
	SaveLogText(true);
	PutLogInfo("\r\n//////////////////////////////////");
	PutLogInfo("\r\n//       BounsServer Server Exit	//");
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
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOUNSDB));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BOUNSDB);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
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
      CW_USEDEFAULT, 0, CW_WIDTH, CW_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   RECT rect;
   GetClientRect(g_hWnd, &rect);
   long lWeiht  = rect.right - rect.left;
   long lHeight = rect.bottom - rect.top;

   g_hInfoText	= CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,0,rect.right-INFOTEXT_WIDTH, INFOTEXT_HEIGHT-4,g_hWnd,0,hInst,NULL);
   g_hLogText	= CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_READONLY |LBS_STANDARD ,0,INFOTEXT_HEIGHT,rect.right-INFOTEXT_WIDTH, rect.bottom-INFOTEXT_HEIGHT,g_hWnd,0,hInst,NULL);
   g_hClientListBox	= CreateWindow("LISTBOX","",WS_CHILD|WS_VISIBLE|LBS_STANDARD,rect.right-INFOTEXT_WIDTH,0,INFOTEXT_WIDTH,rect.bottom,g_hWnd,0,hInst,NULL);
   SendMessage(g_hLogText, EM_SETLIMITTEXT, -1, 0);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			{
				if(MessageBox(g_hWnd, "是否退出?", "提示", MB_YESNO) == IDYES)
				{
					DestroyWindow(hWnd);
				}
			}
			break;

			// 重读GSSetup.ini
		case ID_OPR_RELAOD_GSSETUP:
			{
				GetGame()->ReLoadGSSetup();
			}
			break;
			// 奖励消息处理开关
		case ID_OPR_MSG:
			{
				HMENU hMenu = GetMenu(g_hWnd);
				UINT uState = GetMenuState(hMenu, ID_OPR_MSG,MF_BYCOMMAND);
				if(uState == MF_CHECKED)
				{
				//	ModifyMenu(hMenu, ID_OPR_MSG, 0, ID_OPR_MSG,"DealMsg(Close)" );
					CheckMenuItem(hMenu,ID_OPR_MSG, MF_BYCOMMAND | MF_UNCHECKED); 
					g_bDealMsgState = true;
				}
				else
				{
				//	ModifyMenu(hMenu, ID_OPR_MSG, 0, ID_OPR_MSG,"DealMsg(Open)" );
					CheckMenuItem(hMenu,ID_OPR_MSG, MF_BYCOMMAND | MF_CHECKED); 
					g_bDealMsgState = false;
				}

				RefeashInfoText();
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
		{
			if(MessageBox(g_hWnd, "是否退出?", "提示", MB_YESNO) == IDYES)
			{
				PostQuitMessage(0);
			}
		}
		break;
	case WM_SIZE:
		{
			long lW = LOWORD(lParam);
			long lH = HIWORD(lParam);
			MoveWindow(g_hInfoText, 0,0, lW-INFOTEXT_WIDTH, INFOTEXT_HEIGHT-4, true);
			MoveWindow(g_hLogText, 0, INFOTEXT_HEIGHT, lW-INFOTEXT_WIDTH, lH-INFOTEXT_HEIGHT, true);
			MoveWindow(g_hClientListBox, lW-INFOTEXT_WIDTH,0,INFOTEXT_WIDTH,lH, true);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// 保存日志
void SaveLogText(bool bForce)
{
	// log信息最大长度
	const int lMaxLength = 64000;
	static char str[lMaxLength];

	// 写日志文件
	static DWORD dwLastSaveTime = timeGetTime();
	if( bForce ||
		timeGetTime() - dwLastSaveTime > GetGame()->GetSetup()->dwSaveInfoTime ||
		GetWindowTextLength(g_hLogText) >= lMaxLength )
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
	// log信息最大长度
	const int lMaxLength = 64000;
	static char str[lMaxLength];

	// 写日志文件
	SaveLogText(false);

	// 写服务器界面
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(str, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat(str, "\r\n");

	PutLogInfo(str);

	SendMessage(g_hLogText, EM_SETSEL, 0, -1);
	SendMessage(g_hLogText, EM_SETSEL, -1, 0);
	SendMessage(g_hLogText, EM_REPLACESEL, FALSE, (LPARAM)str);
}

// 加入一条错误的记录
void AddErrorLogText(char* msg, ...)
{
	// log信息最大长度
	const int lMaxLength = 64000;
	static char str[lMaxLength];

	// 写日志文件
	SaveLogText(false);

	// 写服务器界面
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(str, "[%02d-%02d %02d:%02d:%02d] <error>", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat(str, "\r\n");

	PutLogInfo(str);

	SendMessage(g_hLogText, EM_SETSEL, 0, -1);
	SendMessage(g_hLogText, EM_SETSEL, -1, 0);
	SendMessage(g_hLogText, EM_REPLACESEL, FALSE, (LPARAM)str);
}

void RefeashInfoText()
{
	static char str[1024];
	sprintf(str, "GSConnects: %d/%d\r\n\
当前消息处理状态:%s\r\n\
奖励(查询/返回):%d/%d\r\n\
奖励(冻结/返回): %d/%d\r\n\
奖励(获取/返回): %d/%d\r\n",
		GetGame()->GetGSMapSize(), g_lConnectedGSNum,
		(g_bDealMsgState==true)?("打开"):("关闭"), 
		g_lLoadMsgNum, g_lDealLoadMsgNum, 
		g_lFreezeMsgNum,g_lFreezeBakNum, 
		g_lObtainMsgNum, g_lObtainBakNum);
	SetWindowText(g_hInfoText, str);
}
