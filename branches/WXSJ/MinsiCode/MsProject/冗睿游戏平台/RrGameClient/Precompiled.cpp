#include "Precompiled.h"
CMsIni g_RrGameStationIni(_T("%s/RrGameStation.ini"));
CMsIni g_RrResConfigIni(_T("%s/RrResConfig.ini"));
BOOL MSCALL RrGameClientHookProc(MSG*);
LRESULT CALLBACK RrGameClientWndProc(HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
CMsBaseRun s_CMsBaseRun(
    _T("%s/Log/Exe_RrGameStation_Assertx.log"),
    _T("Exe_RrGameStation"),
    _T("»ﬂÓ£”Œœ∑∆ΩÃ®"),
    RrGameClientHookProc,
    RrGameClientWndProc,
    NULL
    );
