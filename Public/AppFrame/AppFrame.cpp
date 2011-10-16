#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE g_hInst;								        // 当前实例
TCHAR g_szTitle[MAX_LOADSTRING] = "AppFrame";         	// 标题栏文本
TCHAR g_szWindowClass[MAX_LOADSTRING] = "AppFrame";		// 主窗口类名
unsigned (__stdcall *mainThreadFunc)(void*);


#include "AppFrameDef.h"
#include "../Package/ConfigParse.h"

namespace AppFrame
{
    ATOM				MyRegisterClass(HINSTANCE hInstance);
    BOOL				InitInstance(HINSTANCE, int);
    LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

    BOOL InitApp(HINSTANCE hInstance, int nCmdShow, unsigned (__stdcall* func)(void*))
    {
        EnableDebugNew();
        mainThreadFunc = func;

        MSG msg;
        memset(&msg, 0, sizeof(MSG));
        MyRegisterClass(hInstance);

        // 执行应用程序初始化:
        if (!InitInstance (hInstance, nCmdShow))
        {
            return FALSE;
        }

        // 主消息循环:
        while(1)
        {
            if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
            {
                if( !GetMessage( &msg, NULL, 0, 0 ) )
                {
                    FiniConfig();
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
        wcex.hIcon			= LoadIcon(hInstance, NULL);
        wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName	= NULL;
        wcex.lpszClassName	= g_szWindowClass;
        wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);

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
        g_hInst = hInstance; // 将实例句柄存储在全局变量中
        HWND hWnd = CreateWindow(g_szWindowClass, g_szTitle,
            /*WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX*/WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
            NULL, NULL, hInstance, NULL);

        if (!hWnd)
        {
            return FALSE;
        }

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
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

            // 自定义控件消息处理
            if (MsgProc((HWND)lParam, wmId, wmEvent))
                return 0;
            // 分析菜单选择:
            switch (wmId)
            {
                // TODO:
                // 添加若干通用菜单处理, 如 ABOUT
            case 0:
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

        case WM_CREATE:
            if (!InitConfig(hWnd))
                PostQuitMessage(0);
            break;

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此添加任意绘图代码...
            EndPaint(hWnd, &ps);
            break;

        case WM_WINDOWPOSCHANGED:
            {
                int cx = ((LPWINDOWPOS)lParam)->cx;
                int cy = ((LPWINDOWPOS)lParam)->cy;
                RECT rect;
                GetWindowRect(hWnd,&rect);
                int frameHeight = GetFrameHeight();
                int frameWidth = GetFrameWidth();

                HWND subWnd = GetWnd(INFO);
                if (!subWnd) break;
                Layouts::Window* wnd = GetWnd(subWnd);
                if (wnd)
                {
                    int x = wnd->x;
                    int y = wnd->y;
                    int width = wnd->width;
                    int height = wnd->height;
                    ::SetWindowPos(subWnd,
                        0,
                        x,
                        y,
                        (rect.right-rect.left)-frameWidth+width,
                        height,
                        SWP_SHOWWINDOW | SWP_NOZORDER);
                }

                subWnd = GetWnd(LOG);
                if (!subWnd) break;
                wnd = GetWnd(subWnd);
                if (wnd)
                {
                    int x = wnd->x;
                    int y = wnd->y;
                    int width = wnd->width;
                    int height = wnd->height;
                    ::SetWindowPos(subWnd,
                        0,
                        x,
                        y,
                        (rect.right-rect.left)-frameWidth+width,
                        (rect.bottom-rect.top)-frameHeight+height,
                        SWP_SHOWWINDOW | SWP_NOZORDER);
                }

                subWnd = GetWnd(LIST);
                if (!subWnd) break;
                wnd = GetWnd(subWnd);
                if (wnd)
                {
                    int x = wnd->x;
                    int y = wnd->y;
                    int width = wnd->width;
                    int height = wnd->height;
                    ::SetWindowPos(subWnd,
                        0,
                        (rect.right-rect.left)-frameWidth + x,
                        y,
                        width,
                        (rect.bottom-rect.top)-frameHeight+height,
                        SWP_SHOWWINDOW | SWP_NOZORDER);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CLOSE:
            if (NotifyMessageBox("是否退出?", "msg", MB_YESNO)==IDYES)
                PostQuitMessage(0);
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
}
