#pragma once

// 最大进程个数
#define MAX_PROCESS_COUNT 1024
using namespace std;

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CGlobalValue
{
public:
    static HWND     g_hWinMine;                     // 扫雷窗口句柄
    static DWORD    g_dwWinMineProcessId;           // 扫雷程序Id
    static HANDLE   g_hWinMineProcessHandle;        // 扫雷程序句柄


    static BOOL     g_bMineIsBegin;     // 地雷启动标记
    static DWORD    g_dwMineCount;      // 地雷个数
    static INT      g_nMineWidth;       // 雷区宽度
    static INT      g_nMineHeight;      // 雷区高度
    static DWORD    g_dwMineTimerCount; // 扫雷计时
    static DWORD    g_dwWinMineCheck;   // 扫雷检查标记
    static LPBYTE   g_lpMineArea;       // 扫雷游戏区
    static DWORD    g_dwMineCurrSpace;  // 扫雷剩余空地
    static DWORD    g_dwMineMaxSpace;   // 扫雷最大空地



};

typedef CGlobalValue Global, G;