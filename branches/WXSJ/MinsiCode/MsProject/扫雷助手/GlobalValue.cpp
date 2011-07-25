#include "StdAfx.h"

HWND    G::g_hWinMine                   = NULL;
DWORD   G::g_dwWinMineProcessId         = 0;
HANDLE  G::g_hWinMineProcessHandle      = NULL;

BOOL    G::g_bMineIsBegin       = 0;
DWORD   G::g_dwMineCount        = 0;
INT     G::g_nMineWidth         = 0;
INT     G::g_nMineHeight        = 0;
DWORD   G::g_dwMineTimerCount   = 0;
DWORD   G::g_dwWinMineCheck     = 0;
LPBYTE  G::g_lpMineArea         = NULL;
DWORD   G::g_dwMineCurrSpace    = 0;
DWORD   G::g_dwMineMaxSpace     = 0;
