#include "Precompiled.h"

LOG_FILE_INFO g_LogFileInfo[LOG_FILE_MAX] = {
    NULL, _T("./Log/EmptyWinTest.log"),
};
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
CMsBaseRun g_CMsBaseRun(_T("./Log/AssertxWin32MouseButtonTest.log"), g_LogFileInfo, (sizeof(g_LogFileInfo) / sizeof(LOG_FILE_INFO)), _T("EmptyWinTest"), _T(" Û±Í∞¥º¸≤‚ ‘"), WndProc, NULL);
