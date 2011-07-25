#include "Precompiled.h"
#include "Test_KeyBoardHook_Dll.h"

LRESULT CALLBACK Test_KeyBoardHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (g_hHook)
    {
        g_hHook = g_hHook;
    }
    
    switch (uMsg)
    {
    case WM_CREATE:
        {
            ;
        }break;

    case WM_HOTKEY:
        {
            ::DestroyWindow(hWnd);
        }break;

    case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }break;

    default:
        {
            ;
        }break;
    }
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
