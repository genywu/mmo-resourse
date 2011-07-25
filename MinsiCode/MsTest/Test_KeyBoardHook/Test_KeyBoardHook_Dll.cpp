#include "Test_KeyBoardHook_Dll.h"

HHOOK g_hHook = NULL;

LRESULT CALLBACK Test_KeyBoardHook_Dll_Proc(INT nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT*)lParam;

    if (p->vkCode == VK_LCONTROL || p->vkCode == VK_RCONTROL ||
        p->vkCode == VK_LMENU || p->vkCode == VK_RMENU ||
        p->vkCode == VK_DELETE)
    {
        ZeroMemory(p, sizeof(KBDLLHOOKSTRUCT));
        return TRUE;
    }
    else
    {
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }
}

BOOL WINAPI DllMain(
    IN HINSTANCE /*hDllHandle*/, 
    IN DWORD     nReason,    
    IN LPVOID    /*Reserved*/    
    )
{
    switch (nReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            HMODULE hModule = ::GetModuleHandle(NULL);
            g_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, Test_KeyBoardHook_Dll_Proc, hModule, 0);
        }break;

    case DLL_PROCESS_DETACH:
        {
            ::UnhookWindowsHookEx(g_hHook);
        }break;
    }
    return TRUE;
}
