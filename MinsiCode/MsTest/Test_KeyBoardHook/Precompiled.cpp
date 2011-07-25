#include "Precompiled.h"
LRESULT CALLBACK Test_KeyBoardHook(HWND, UINT, WPARAM, LPARAM);
CMsBaseRun s_CMsBaseRun(_T("%s/Log/Test_KeyBoardHook_Assertx.log"), _T("Test_KeyBoardHook"), _T("¼üÅÌ¹³×Ó"), Test_KeyBoardHook, NULL);
