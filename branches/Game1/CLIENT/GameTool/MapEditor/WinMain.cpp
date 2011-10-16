// Test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinMain.h"
#include "Resource.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\input.h"
#include "setting.h"

#include "GameWorld.h"
#include <objbase.h>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInstMain = NULL;				
BOOL	  bActive   = FALSE;	
HWND	  hWndMain	= NULL;		
static TCHAR	  szTitle[MAX_LOADSTRING] = MAPEDITORNAME ;					
static TCHAR	  szWindowClass[MAX_LOADSTRING] = "FROSTENGINE";

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
		
		//_CrtSetBreakAlloc(5660);
	#endif

//	CoInitialize(NULL);//COINIT_APARTMENTTHREADED);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	BOOL bHasMsg;
	MSG  msg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	DWORD dwResult = 0;BOOL bDone = FALSE;
	while (WM_QUIT != msg.message) 
	{
		if( bActive )
            bHasMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        else
            bHasMsg = GetMessage( &msg, NULL, 0U, 0U );
		
		if (bHasMsg)
		{
			TranslateMessage( &msg );
            DispatchMessage( &msg );		
		}
		else
		{	
			//update input message 60fps
			Sleep(1);
			GameWorld::GetInstance()->Update();
			

			//render 
			GameWorld::GetInstance()->Render();

		}
	}

	ExitInstance();

	return (int)msg.wParam;
}

//--------------------------------------------------------------------
//  Nane: InitInstance(HANDLE, int)
//	Desc:
//--------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInstMain = hInstance; // Store instance handle in our global variable
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPEDITOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPEDITOR));
	
	RegisterClassEx(&wcex);
	
	
	hWndMain = CreateWindow(szWindowClass, szTitle, 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,0,1,1, 
		NULL, NULL, hInstance, NULL);

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

	OutputConsole("WinMain: GameWorld Created.\n");
   
	return TRUE;
}
//--------------------------------------------------------------------
// Name: ExitInstance()
// Desc: 
//--------------------------------------------------------------------
BOOL ExitInstance()
{
	OutputConsole("WinMain: GameWorld Destroyed.\n");

	GameWorld::GetInstance()->Destroy();
	GameWorld::ExitInstance(); 

	
	

	//销毁引擎
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
	//调用引擎的WindowProc函数
	input::Interface *pInput = input::Interface::GetInstance();
	if (pInput && pInput->ParseMsg(message,wParam,lParam))
	{
		return 0;
	}

	switch (message) 
	{
	case WM_ACTIVATE:
		bActive = LOWORD(wParam);  
		break;
	//case WM_SETCURSOR:
		//::SetCursor(NULL);
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
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU)
			break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
