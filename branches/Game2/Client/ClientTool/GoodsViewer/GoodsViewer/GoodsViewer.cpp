// GoodsViewer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GoodsViewer.h"
#include <shellapi.h>
#include <CommDlg.h>
#include "GameSetting.h"
#include "GameWorld.h"
#include "GoodsModelManager.h"
#include "..\CEGUIEngine\CEGUIManager.h"

#include <vld.h>

#define MAX_LOADSTRING 100
// 全局变量:
WNDCLASSEX wcex;
HINSTANCE hInst = NULL;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

BOOL	  bActive   = FALSE;
HWND	  hWndMain	= NULL;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL                ExitInstance(HINSTANCE hInstance);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	//HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GOODSVIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	ZeroMemory(&msg,sizeof(msg));
	while(msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if(!IsIconic(hWndMain))//!< 窗口未最小化
			{
				//这里添加渲染的函数
				GameWorld::GetInstance()->Update();
				GameWorld::GetInstance()->Render();
			}
		}
	}

	//退出后的处理
	ExitInstance(hInstance);
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
	

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOODSVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GOODSVIEWER);
	wcex.lpszMenuName   = NULL;
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
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWndMain=hWnd = CreateWindow(szWindowClass, szTitle,WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   0,0,1,1, 
	   NULL, NULL, hInstance, NULL);
  
   DragAcceptFiles(hWndMain,TRUE);//支持文件拖拽

   if (!hWnd)
   {
      return FALSE;
   }

   Setting::InitInstance();
   GameWorld::InitInstance();

   if (!Setting::GetInstance()->LoadSetting())
   {
	   Setting::ExitInstance();
	   return FALSE;
   }

   GameWorld::GetInstance()->Create(hWndMain);

   ShowWindow(hWndMain, nCmdShow);
   UpdateWindow(hWndMain);
   OutputConsole("GameWorld Created.\n");

   return TRUE;
}

//--------------------------------------------------------------------
// Name: ExitInstance()
// Desc: 
//--------------------------------------------------------------------
BOOL ExitInstance(HINSTANCE hInstance)
{
	OutputConsole("GameWorld Class Destroyed.\n");

	Setting::GetInstance()->SaveSetting();
	Setting::ExitInstance();

	GameWorld::GetInstance()->Destroy();
	GameWorld::ExitInstance(); 

	UnregisterClass(wcex.lpszClassName,hInstance);

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

	//调用CEGUI 的 WndProc函数 ，有自己的一套Dinput响应
	CEGUIManager* pUIManager = CEGUIManager::GetInstance();
	if (pUIManager)
	{
		pUIManager->ParseMsg(message,wParam,lParam);
	}

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
			DestroyWindow(hWnd);
			break;
		case IDM_OPEN://打开模型
			{
				OPENFILENAME ofn;								// common dialog box structure
				static char szFile[_MAX_PATH] = "model";		// buffer for file name

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWndMain;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = "model";
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 
				static char szOldDirectory[_MAX_PATH];
				GetCurrentDirectory(_MAX_PATH ,szOldDirectory);
				if (GetOpenFileName(&ofn)==TRUE)
				{
					SetCurrentDirectory(szOldDirectory);
					GoodsModelManager::GetInstance()->OpenModel(szFile,true);
				}
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
	case WM_DROPFILES:
		{
			char szQueryFile[1024] = { 0 };
			if (DragQueryFile((HDROP)wParam, 0, szQueryFile, 1023) != 0xFFFFFFFF)
			{
				//检查是目录?
				if (GetFileAttributes(szQueryFile) & FILE_ATTRIBUTE_DIRECTORY)
					strcat(szQueryFile,"\\model");
				GoodsModelManager::GetInstance()->OpenModel(szQueryFile,true);
			}
		}
		break;
	case 0x020A:  //WM_MOUSEWHEEL: 这里用于测试模型摄像机处理，之后放到CEGUI的消息响应中
		//GoodsModelManager::GetInstance()->MoveCamera(0.0f,static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(1200),static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(1200));
		/*CGoodsModelManager::GetInstance()->ScaleCumulate(static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(1200),\
												 static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(1200),\
												 static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(1200));*/
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

