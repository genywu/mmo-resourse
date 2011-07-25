#include "Precompiled.h"
#include "EmptyWinTest.h"

//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = ::GetDC(hWnd);

	switch (message)
	{
    case WM_CREATE:
        {
            ;
        }break;
    case WM_XBUTTONDOWN:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            if (XBUTTON1 == GET_XBUTTON_WPARAM(wParam))
            {
                ::TextOut(hdc, 0, 0, _T("按下了鼠标扩展键(一)"), (INT)_tcslen(_T("按下了鼠标扩展键(一)")));
            }
            else if (XBUTTON2 == GET_XBUTTON_WPARAM(wParam))
            {
                ::TextOut(hdc, 0, 0, _T("按下了鼠标扩展键(二)"), (INT)_tcslen(_T("按下了鼠标扩展键(二)")));
            }
            else
            {
                ::TextOut(hdc, 0, 0, _T("按下了鼠标扩展键(未知)"), (INT)_tcslen(_T("按下了鼠标扩展键(未知)")));
            }
        }break;
    case WM_XBUTTONUP:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            if (XBUTTON1 == GET_XBUTTON_WPARAM(wParam))
            {
                ::TextOut(hdc, 0, 0, _T("松开了鼠标扩展键(一)"), (INT)_tcslen(_T("按下了鼠标扩展键(一)")));
            }
            else if (XBUTTON2 == GET_XBUTTON_WPARAM(wParam))
            {
                ::TextOut(hdc, 0, 0, _T("松开了鼠标扩展键(二)"), (INT)_tcslen(_T("按下了鼠标扩展键(二)")));
            }
            else
            {
                ::TextOut(hdc, 0, 0, _T("松开了鼠标扩展键(未知)"), (INT)_tcslen(_T("按下了鼠标扩展键(未知)")));
            }
        }break;
    case WM_LBUTTONDOWN:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("按下了鼠标左键"), (INT)_tcslen(_T("按下了鼠标左键")));
        }break;
    case WM_LBUTTONUP:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("松开了鼠标左键"), (INT)_tcslen(_T("松开了鼠标左键")));
        }break;
    case WM_RBUTTONDOWN:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("按下了鼠标右键"), (INT)_tcslen(_T("按下了鼠标右键")));
        }break;
    case WM_RBUTTONUP:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("松开了鼠标右键"), (INT)_tcslen(_T("松开了鼠标右键")));
        }break;
    case WM_MBUTTONDOWN:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("按下了鼠标中键"), (INT)_tcslen(_T("按下了鼠标中键")));
        }break;
    case WM_MBUTTONUP:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            ::TextOut(hdc, 0, 0, _T("松开了鼠标中键"), (INT)_tcslen(_T("松开了鼠标中键")));
        }break;
    case WM_MOUSEWHEEL:
        {
            ::TextOut(hdc, 0, 0, _T("                                                            "), (INT)_tcslen(_T("                                                            ")));
            short nWheel = ((signed short)(HIWORD(wParam))) / WHEEL_DELTA;
            if (nWheel > 0)
            {
                ::TextOut(hdc, 0, 0, _T("鼠标滚轮上滚"), (INT)_tcslen(_T("鼠标滚轮上滚")));
            }
            else if (nWheel < 0)
            {
                ::TextOut(hdc, 0, 0, _T("鼠标滚轮下滚"), (INT)_tcslen(_T("鼠标滚轮下滚")));
            }
            else
            {
                ::TextOut(hdc, 0, 0, _T("鼠标滚轮咋啦?"), (INT)_tcslen(_T("鼠标滚轮咋啦?")));
            }
        }break;
    case WM_PAINT:
        {
            hdc = ::BeginPaint(hWnd, &ps);
            ::EndPaint(hWnd, &ps);
        }break;
	case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }break;
	default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
