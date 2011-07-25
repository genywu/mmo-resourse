#include "Precompiled.h"
BOOL MSCALL RrClientGameHookProc(MSG*);
LRESULT CALLBACK RrClientGameWndProc(HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
CMsBaseRun s_CMsBaseRun(
    _T("%s/Log/Exe_RrClientGame_Assertx.log"),
    _T("Exe_RrMMOClient"),
    _T("Èßî£MMOGAME"),
    RrClientGameHookProc,
    RrClientGameWndProc,
    NULL
    );
