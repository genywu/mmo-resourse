#include "Precompiled.h"
#include "Test_ImageTest.h"

CMsGdiPlusImage g_CMsImageBmp;
CMsGdiPlusImage g_CMsImageJpg;
CMsGdiPlusImage g_CMsImagePng;
CMsGdiPlusImage g_CMsImageTif;
CMsGdiPlusImage g_CMsImageGif;

LRESULT CALLBACK Test_ImageTest_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = ::GetDC(hWnd);

	switch (message)
	{
    case WM_CREATE:
        {
            g_CMsImageBmp.Init(hWnd);
            g_CMsImageJpg.Init(hWnd);
            g_CMsImagePng.Init(hWnd);
            g_CMsImageTif.Init(hWnd);
            g_CMsImageGif.Init(hWnd);

            g_CMsImageBmp.Open(_T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/00.bmp"));
            g_CMsImageJpg.Open(_T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/01.jpg"));
            g_CMsImagePng.Open(_T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/00.png"));
            g_CMsImageTif.Open(_T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/03.tif"));
            g_CMsImageGif.Open(_T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/105.gif"));
        }break;

    case WM_CLOSE:
        {
            ::DestroyWindow(hWnd);
        }break;

    case WM_PAINT:
        { 
            hdc = ::BeginPaint(hWnd, &ps);
            g_CMsImageBmp.Draw(0, 0, 50, 50);
            g_CMsImageJpg.Draw(50, 0, 50, 50);
            g_CMsImagePng.Draw(100, 0, 50, 50);
            g_CMsImageTif.Draw(150, 0, 50, 50);
            g_CMsImageGif.Draw(200, 0, 50, 50);
            ::EndPaint(hWnd, &ps);
        }break;

	case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }break;

	default:
        {
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }break;
	}
	return 0;
}
