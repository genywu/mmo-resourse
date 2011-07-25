#include "Precompiled.h"

BOOL MSCALL RrGameClientHookProc(MSG* pMsg)
{
    USE_PARAM(pMsg);
    return FALSE;
}

LRESULT CALLBACK RrGameClientWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _ENTER_FUN_C;

    USE_PARAM(hWnd, uMsg, wParam, lParam);

    //switch (uMsg)
    //{
    //    // Ä¬ÈÏ´¦Àí
    //default:
    //    {
    //        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    //    }break;
    //}
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}
