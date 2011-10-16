// Test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinMain.h"
#include "Resource.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\input.h"
#include "..\..\frostengine\render.h"

#include "GameWorld.h"
#include "gamesetting.h"
#include "wndviewer.h"

#define MAX_LOADSTRING 100
WNDCLASSEX wcex;

// Global Variables:
HINSTANCE hInstMain = NULL;
BOOL	  bActive   = FALSE;
HWND	  hWndMain	= NULL;
TCHAR	  szTitle[MAX_LOADSTRING] = "GameModel Viewer 4.2.1.0 LastUpdate:";
TCHAR	  szWindowClass[MAX_LOADSTRING] = "FROSTENGINE";

//-----------------------------------------------------------------
// Name: WinMain()
// Desc: 
//-----------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR  lpCmdLine,int nCmdShow)
{
#ifdef _DEBUG
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag( tmpFlag );
	//_CrtSetBreakAlloc(985);
#endif
	BOOL bHasMsg;
	MSG  msg;
	/*try
	{*/
		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow)) 
		{
			return FALSE;
		}
	
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		DWORD dwResult = 0;BOOL bDone = FALSE;
		while (WM_QUIT != msg.message) 
		{
			bHasMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );

			if (bHasMsg)
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );		
			}else
			{	
				Sleep(1);
				if (!IsIconic(hWndMain))
				{
					//update input message 60fps
					GameWorld::GetInstance()->Update();
					//render 
					GameWorld::GetInstance()->Render();
				}

			}
		}

		ExitInstance(hInstance);

	/*}*/
	/*catch(...)
	{
		
	}*/

	return (int)msg.wParam;
}

//--------------------------------------------------------------------
//  Nane: InitInstance(HANDLE, int)
//	Desc:
//--------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInstMain = hInstance; // Store instance handle in our global variable

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHARACTERVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHARACTERVIEWER));

	RegisterClassEx(&wcex);

	char szTileTemp[256];
	strcpy(szTileTemp,szTitle);
	strcat(szTileTemp,__DATE__);
	hWndMain = CreateWindow(szWindowClass, szTileTemp, 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,0,1,1, 
		NULL, NULL, hInstance, NULL);

	DragAcceptFiles(hWndMain,TRUE);

	if (!hWndMain) return FALSE;

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

	//ui::Manager * pUIMGR = ui::Manager::GetInstance();
	//WndViewer * pWnd = (WndViewer *)pUIMGR->GetMainWnd();
	//pWnd->OpenMainModelFile("E:\\project\\guangmang\\RUN\\Model\\effect\\1388086\\model");
	//E:\\chiyanMZ\\model
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

	UnregisterClass(wcex.lpszClassName,hInstance);

	GameWorld::ExitInstance(); 
	return TRUE;
}


//-------------------------------------------------------------------
//  Name: WndProc(HWND, unsigned, WORD, LONG)
//  Desc: 
//-------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	//调用引擎的WindowProc函数
	input::Interface * pInput = input::Interface::GetInstance();
	if (pInput && pInput->ParseMsg(message,wParam,lParam))
	{
		return 0;
	}

	switch (message) 
	{
	case WM_ACTIVATE:
		bActive = LOWORD(wParam);
		/*if (bActive)
		{
			input::Interface *pInput = input::Interface::GetInstance();
			if (pInput)
			{
				pInput->Destroy();
			}
		}*/
		break;
	case WM_SETCURSOR:
		::SetCursor(NULL);
		break;					  
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
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
				ui::Manager * pUIMGR = ui::Manager::GetInstance();
				if (pUIMGR)
				{
					POINT		ptQMDes;		//快捷菜单目的显示坐标
					WndViewer * pWnd;

					pInput->GetMousePosition(&ptQMDes, NULL);
					ptQMDes.x -= 5;
					ptQMDes.y -= 5;
					pWnd = (WndViewer *)pUIMGR->GetMainWnd();
					pWnd->SetQuickMenuVisible(true);
					pWnd->MoveQuickMenu(ptQMDes);

					//检查是目录?
					if (GetFileAttributes(szQueryFile) & FILE_ATTRIBUTE_DIRECTORY)
						strcat(szQueryFile,"\\model");

					pWnd->SetCurOperFileName(szQueryFile);
				}
			}
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU)
		{
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
