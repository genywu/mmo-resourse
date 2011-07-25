#include "stdafx.h"
#include <Tlhelp32.h>

// 错误代码
#define ERROR_CODE      -1

// 扫雷程序窗口名称
#define MINE_PROCESS_WINDOW_NAME    "扫雷"

// 雷区最大高度
#define MAX_MINE_HEIGHT         26

// 雷区最大宽度
#define MAX_MINE_WIDTH          32

// 最大地雷个数
#define MAX_MINE_COUNT          667

// 扫雷计时
#define TIMER_COUNT     (LPVOID)0x0100579C
#define TIMER_COUNT_LEN 0x04

// 扫雷启动标记
#define MINE_BEGIN_FLAG_ADDRESS     (LPVOID)0x01005160
#define MINE_BEGIN_FLAG_ADDRESS_LEN 0x04

// 地雷个数
#define MINE_COUNT_ADDRESS      (LPVOID)0x01005330
#define MINE_COUNT_ADDRESS_LEN  0x04
#define MINE_COUNT_ADDRESS2     (LPVOID)0x01005194
#define MINE_COUNT_ADDRESS2_LEN 0x04

// 雷区宽度
#define MINE_WIDTH_ADDRESS      (LPVOID)(((ULONG_PTR)MINE_COUNT_ADDRESS) + MINE_COUNT_ADDRESS_LEN)
#define MINE_WIDTH_ADDRESS_LEN  0x04

// 雷区高度
#define MINE_HEIGHT_ADDRESS     (LPVOID)(((ULONG_PTR)MINE_WIDTH_ADDRESS) + MINE_WIDTH_ADDRESS_LEN)
#define MINE_HEIGHT_ADDRESS_LEN 0x04

// 扫雷检查地址
#define MINE_CHECK_ADDRESS      (LPVOID)(((ULONG_PTR)MINE_HEIGHT_ADDRESS) + MINE_HEIGHT_ADDRESS_LEN)
#define MINE_CHECK_ADDRESS_LEN  0x04

// 雷区地址
#define MINE_ADDRESS            (LPVOID)(((ULONG_PTR)MINE_CHECK_ADDRESS) + MINE_CHECK_ADDRESS_LEN)

// 雷区的最大值
#define MINE_ADDRESS_MAX_LEN    (MAX_MINE_HEIGHT) * (MAX_MINE_WIDTH)

// 扫雷剩余空地地址
#define MINE_MAX_SPACE_ADDRESS      (LPVOID)0x010057A0
#define MINE_MAX_SPACE_ADDRESS_LEN  0x04

// 扫雷剩余空地地址
#define MINE_CURR_SPACE_ADDRESS      (LPVOID)(((ULONG_PTR)MINE_MAX_SPACE_ADDRESS) + MINE_MAX_SPACE_ADDRESS_LEN)
#define MINE_CURR_SPACE_ADDRESS_LEN  0x04

enum E_MINE_FLAG
{
    E_NUMBER_ERROR                  = -2,   // 错误
    E_NUMBER_UNKNOW                 = 0x0F, // 未知
    E_NUMBER_EMPTY                  = 0x40, // 空
    E_NUMBER_1                      = 0x41, // 数字1
    E_NUMBER_2                      = 0x42, // 数字2
    E_NUMBER_3                      = 0x43, // 数字3
    E_NUMBER_4                      = 0x44, // 数字4
    E_NUMBER_5                      = 0x45, // 数字5
    E_NUMBER_6                      = 0x46, // 数字6
    E_NUMBER_7                      = 0x47, // 数字7
    E_NUMBER_8                      = 0x48, // 数字8
    E_NUMBER_NO_MINE_INCERTITUDE    = 0x0D, // 非雷标记为问
    E_NUMBER_IS_MINE_INCERTITUDE    = 0x8D, // 地雷标记为问
    E_NUMBER_NO_MINE_FLAG           = 0x0E, // 非雷标记为雷
    E_NUMBER_MINE                   = 0x8F, // 地雷
    E_NUMBER_IS_MINE_FLAG           = 0x8E, // 地雷标记为雷
    E_NUMBER_WALL                   = 0x10, // 墙
};


// 读取进程内存中的信息
BOOL MsReadProcessMemory(HANDLE hProcess, LPVOID lpAddr, LPVOID lpBuff, ULONG_PTR ulReadLen);

// 写入信息到进程内存中
BOOL MsWriteProcessMemory(HANDLE hProcess, LPVOID lpAddr, LPVOID lpBuff, ULONG_PTR ulReadLen);

// 读取扫雷内存中的信息
BOOL MsReadWinMineData(VOID);

// 写入信息到扫雷内存中
BOOL MsWriteWinMineData(VOID);

// 更新雷区信息
BOOL MsUpdateMineAreaData(BOOL bToMineProcess/*TRUE->更新到扫雷中, FALSE->更新到本地*/);

// 获取雷的标记
E_MINE_FLAG GetMineFlag(INT X, INT Y);

// 查找周围雷的个数
DWORD FindAroundMineCount(INT X, INT Y);

// 修改雷的标记
VOID ChangeMineFlag(INT X, INT Y, E_MINE_FLAG eMineFlag);

// 扫雷
BOOL DightMine(VOID);

// 清雷
BOOL ClearMine(VOID);

// 查雷
BOOL SeeMine(VOID);

INT_PTR CALLBACK MainDialogProc(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    );

INT WINAPI _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    INT nShowCmd)
{
    return ::DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, MainDialogProc);
}

INT_PTR CALLBACK MainDialogProc(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            while (TRUE)
            {
                G::g_hWinMine = ::FindWindow(_T("扫雷"), _T("扫雷"));
                if (!G::g_hWinMine)
                {
                    INT nRet = ::MessageBox(hwndDlg, _T("扫雷程序没有运行\n是否重新查找"), _T("提示"), MB_YESNO);
                    if (IDYES == nRet)
                    {
                        continue;
                    }
                    else
                    {
                        ::EndDialog(hwndDlg, ERROR_CODE);
                        return TRUE;
                    }
                }
                else
                {
                    ::GetWindowThreadProcessId(G::g_hWinMine, &G::g_dwWinMineProcessId);
                    G::g_hWinMineProcessHandle = ::OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, TRUE, G::g_dwWinMineProcessId);
                    if (!G::g_hWinMineProcessHandle)
                    {
                        ::MessageBox(hwndDlg, _T("扫雷程序打开失败"), _T("提示"), MB_OK);
                    }
                    break;
                }
            }

            G::g_lpMineArea = NEW BYTE[MINE_ADDRESS_MAX_LEN];
            if (!G::g_lpMineArea)
            {
                ::MessageBox(hwndDlg, _T("内存不足"), _T("提示"), MB_OK);
                ::EndDialog(hwndDlg, ERROR_CODE);
                return TRUE;
            }
            return TRUE;
        }break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                // 扫雷
            case IDC_DIGHT_MINE:
                {
                    // 获取雷区信息
                    BOOL bRet = MsUpdateMineAreaData(FALSE);
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("嵌入扫雷程序失败"), _T("提示"), MB_OK);
                        ::EndDialog(hwndDlg, ERROR_CODE);
                        return TRUE;
                    }

                    bRet = DightMine();
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("扫雷失败"), _T("提示"), MB_OK);
                    }
                    MsUpdateMineAreaData(TRUE);
                }break;

                // 清雷
            case IDC_CLEAR_MINE:
                {
                    // 获取雷区信息
                    BOOL bRet = MsUpdateMineAreaData(FALSE);
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("嵌入扫雷程序失败"), _T("提示"), MB_OK);
                        ::EndDialog(hwndDlg, ERROR_CODE);
                        return TRUE;
                    }

                    bRet = ClearMine();
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("清雷失败"), _T("提示"), MB_OK);
                    }
                    MsUpdateMineAreaData(TRUE);
                }break;

                // 查雷
            case IDC_SEE_MINE:
                {
                    // 获取雷区信息
                    BOOL bRet = MsUpdateMineAreaData(FALSE);
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("嵌入扫雷程序失败"), _T("提示"), MB_OK);
                        ::EndDialog(hwndDlg, ERROR_CODE);
                        return TRUE;
                    }

                    bRet = SeeMine();
                    if (!bRet)
                    {
                        ::MessageBox(hwndDlg, _T("查雷失败"), _T("提示"), MB_OK);
                    }
                    MsUpdateMineAreaData(TRUE);
                }break;
            }
        }break;

    case WM_CLOSE:
        {
            if (G::g_lpMineArea)
            {
                delete[] G::g_lpMineArea;
                G::g_lpMineArea = NULL;
            }
            ::EndDialog(hwndDlg, 0);
        }break;

    default:
        {
            ;
        }break;
    }
    return (INT_PTR)FALSE;
}

// 读取进程内存中的信息
BOOL MsReadProcessMemory(HANDLE hProcess, LPVOID lpAddr, LPVOID lpBuff, ULONG_PTR ulReadLen)
{
    // 内存地址以前的权限
    DWORD dwOldMemProtect;

    BOOL bRet = ::VirtualProtectEx(
        hProcess,
        lpAddr,
        ulReadLen,
        PAGE_READONLY,
        &dwOldMemProtect
        );
    if (!bRet)
    {
        ::MessageBox(NULL, _T("修改内存权限A失败"), _T("提示"), MB_OK);
        if (hProcess)
        {
            ::MessageBox(NULL, _T("有句柄"), _T("提示"), MB_OK);
        }
        if (lpAddr)
        {
            ::MessageBox(NULL, _T("有地址"), _T("提示"), MB_OK);
        }
        if (ulReadLen)
        {
            ::MessageBox(NULL, _T("有长度"), _T("提示"), MB_OK);
        }
        return FALSE;
    }

    // 读取内存中的内容
    DWORD dwNumberOfBytesRead = 0;
    bRet = ::ReadProcessMemory(
        hProcess,
        lpAddr,
        lpBuff,
        ulReadLen,
        &dwNumberOfBytesRead
        );
    if (!bRet || (ulReadLen != dwNumberOfBytesRead))
    {
        ::MessageBox(NULL, _T("读取内存内容失败"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复内存以前的访问权限
    bRet = ::VirtualProtectEx(
        hProcess,
        lpAddr,
        ulReadLen,
        dwOldMemProtect,
        &dwOldMemProtect
        );
    if (!bRet)
    {
        ::MessageBox(NULL, _T("修改内存权限B失败"), _T("提示"), MB_OK);
        return FALSE;
    }

    return TRUE;
}

// 写入信息到进程内存中
BOOL MsWriteProcessMemory(HANDLE hProcess, LPVOID lpAddr, LPVOID lpBuff, ULONG_PTR ulReadLen)
{
    // 内存地址以前的权限
    DWORD dwOldMemProtect;

    BOOL bRet = ::VirtualProtectEx(
        hProcess,
        lpAddr,
        ulReadLen,
        PAGE_WRITECOPY,
        &dwOldMemProtect
        );
    if (!bRet)
    {
        ::MessageBox(NULL, _T("修改内存权限AA失败"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 写入内存中的内容
    DWORD dwNumberOfBytesWritten = 0;
    bRet = ::WriteProcessMemory(
        hProcess,
        lpAddr,
        lpBuff,
        ulReadLen,
        &dwNumberOfBytesWritten
        );
    if (!bRet || (ulReadLen != dwNumberOfBytesWritten))
    {
        ::MessageBox(NULL, _T("写入内存内容失败"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复内存以前的访问权限
    bRet = ::VirtualProtectEx(
        hProcess,
        lpAddr,
        ulReadLen,
        dwOldMemProtect,
        &dwOldMemProtect
        );
    if (!bRet)
    {
        ::MessageBox(NULL, _T("修改内存权限BB失败"), _T("提示"), MB_OK);
        return FALSE;
    }

    return TRUE;
}

// 读取进程内存中的信息
BOOL MsReadWinMineData(VOID)
{
    // 读取扫雷启动标记
    BOOL bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_BEGIN_FLAG_ADDRESS,
        &G::g_bMineIsBegin, 
        MINE_BEGIN_FLAG_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE; 
    }

    // 读取地雷个数
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_COUNT_ADDRESS,
        &G::g_dwMineCount, 
        MINE_COUNT_ADDRESS_LEN
        );
    if (!bRet || (0 == G::g_dwMineCount))
    {
        return FALSE; 
    }

    // 读取雷区宽度
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_WIDTH_ADDRESS,
        &G::g_nMineWidth,
        MINE_WIDTH_ADDRESS_LEN
        );
    if (!bRet || (0 == G::g_nMineWidth))
    {
        return FALSE;
    }
    G::g_nMineWidth += 2;

    // 读取雷区高度
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_HEIGHT_ADDRESS,
        &G::g_nMineHeight,
        MINE_HEIGHT_ADDRESS_LEN
        );
    if (!bRet || (0 == G::g_nMineHeight))
    {
        return FALSE;
    }
    G::g_nMineHeight += 2;

    // 读取雷区检查标记
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_CHECK_ADDRESS,
        &G::g_dwWinMineCheck,
        MINE_CHECK_ADDRESS_LEN
        );
    if (!bRet || (0 != G::g_dwWinMineCheck))
    {
        return FALSE;
    }

    // 读取扫雷最大空地
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_MAX_SPACE_ADDRESS,
        &G::g_dwMineMaxSpace,
        MINE_MAX_SPACE_ADDRESS_LEN
        );
    if (!bRet || (0 == G::g_dwMineMaxSpace))
    {
        return FALSE;
    }

    // 读取扫雷当前空地
    bRet = MsReadProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_CURR_SPACE_ADDRESS,
        &G::g_dwMineCurrSpace,
        MINE_CURR_SPACE_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    if (((G::g_nMineWidth)*(G::g_nMineHeight)) > MINE_ADDRESS_MAX_LEN)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

// 写入信息到扫雷内存中
BOOL MsWriteWinMineData(VOID)
{
    // 写入扫雷启动标记
    BOOL bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_BEGIN_FLAG_ADDRESS,
        &G::g_bMineIsBegin, 
        MINE_BEGIN_FLAG_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE; 
    }

    // 写入地雷个数
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_COUNT_ADDRESS2,
        &G::g_dwMineCount, 
        MINE_COUNT_ADDRESS2_LEN
        );
    if (!bRet)
    {
        return FALSE; 
    }

    // 写入雷区宽度
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_WIDTH_ADDRESS,
        &G::g_nMineWidth,
        MINE_WIDTH_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    // 写入雷区高度
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_HEIGHT_ADDRESS,
        &G::g_nMineHeight,
        MINE_HEIGHT_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    // 写入雷区检查标记
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_CHECK_ADDRESS,
        &G::g_dwWinMineCheck,
        MINE_CHECK_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    // 写入扫雷计时
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        TIMER_COUNT,
        &G::g_dwMineTimerCount,
        TIMER_COUNT_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    // 读取雷区检查标记
    bRet = MsWriteProcessMemory(
        G::g_hWinMineProcessHandle,
        MINE_CURR_SPACE_ADDRESS,
        &G::g_dwMineCurrSpace,
        MINE_CURR_SPACE_ADDRESS_LEN
        );
    if (!bRet)
    {
        return FALSE;
    }

    return TRUE;
}

// 更新雷区信息
BOOL MsUpdateMineAreaData(BOOL bToMineProcess)
{
    BOOL bRet = TRUE;

    // 更新到扫雷
    if (bToMineProcess)
    {
        if (((G::g_nMineWidth)*(G::g_nMineHeight)) > MINE_ADDRESS_MAX_LEN)
        {
            return FALSE;
        }

        if (G::g_lpMineArea)
        {
            // 读取扫雷游戏区内容
            bRet = MsWriteProcessMemory(
                G::g_hWinMineProcessHandle,
                MINE_ADDRESS,
                G::g_lpMineArea,
                MINE_ADDRESS_MAX_LEN
                );
            if (!bRet)
            {
                return FALSE;
            }
            if (MsWriteWinMineData())
            {
                ::InvalidateRect(G::g_hWinMine, NULL, TRUE);
                return TRUE;
            }
            return FALSE;
        }
        else
        {
            return FALSE;
        }
    }

    // 更新到本地
    else
    {
        if (((G::g_nMineWidth)*(G::g_nMineHeight)) > MINE_ADDRESS_MAX_LEN)
        {
            return FALSE;
        }

        if (G::g_lpMineArea)
        {
            bRet = MsReadWinMineData();
            if (!bRet)
            {
                return FALSE;
            }

            // 读取扫雷游戏区内容
            bRet = MsReadProcessMemory(
                G::g_hWinMineProcessHandle,
                MINE_ADDRESS,
                G::g_lpMineArea,
                MINE_ADDRESS_MAX_LEN
                );
            if (!bRet)
            {
                return FALSE;
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}

// 获取雷的标记
E_MINE_FLAG GetMineFlag(INT X, INT Y)
{
    if (!G::g_lpMineArea || (X < 0) || (Y < 0))
    {
        return E_NUMBER_ERROR;
    }
    return (E_MINE_FLAG)G::g_lpMineArea[Y * MAX_MINE_WIDTH + X];
}

// 查找周围雷的个数
DWORD FindAroundMineCount(INT X, INT Y)
{
    if (!G::g_lpMineArea || (X < 0) || (Y < 0))
    {
        return E_NUMBER_ERROR;
    }
    DWORD dwMineCount = 0;
    E_MINE_FLAG eMineFlag = E_NUMBER_ERROR;

    eMineFlag = GetMineFlag(X-1, Y-1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X, Y-1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X+1, Y-1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X-1, Y);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X+1, Y);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X-1, Y+1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X, Y+1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    eMineFlag = GetMineFlag(X+1, Y+1);
    switch (eMineFlag)
    {
    case E_NUMBER_IS_MINE_INCERTITUDE:
    case E_NUMBER_IS_MINE_FLAG:
    case E_NUMBER_MINE:
        {
            dwMineCount++;
        }
    default:
        {
            ;
        }break;
    }

    return dwMineCount;
}

// 修改雷的标记
VOID ChangeMineFlag(INT X, INT Y, E_MINE_FLAG eMineFlag)
{
    if (!G::g_lpMineArea || (X < 0) || (Y < 0))
    {
        return;
    }
    G::g_lpMineArea[Y * MAX_MINE_WIDTH + X] = (BYTE)eMineFlag;
}

// 扫雷
BOOL DightMine(VOID)
{
    if (!G::g_lpMineArea)
    {
        return FALSE;
    }

    for (INT Y = 0; Y < G::g_nMineHeight; Y++)
    {
        for (INT X = 0; X < G::g_nMineWidth; X++)
        {
            E_MINE_FLAG eMineFlag = GetMineFlag(X, Y);
            switch (eMineFlag)
            {
            case E_NUMBER_UNKNOW:
            case E_NUMBER_NO_MINE_INCERTITUDE:
            case E_NUMBER_NO_MINE_FLAG:
                {
                    DWORD dwMineCount = FindAroundMineCount(X, Y);
                    ChangeMineFlag(X, Y, (E_MINE_FLAG)(dwMineCount + (DWORD)E_NUMBER_EMPTY));
                }break;

            case E_NUMBER_MINE:
            case E_NUMBER_IS_MINE_INCERTITUDE:
            case E_NUMBER_IS_MINE_FLAG:
                {
                    ChangeMineFlag(X, Y, E_NUMBER_IS_MINE_FLAG);
                }break;

            default:
            case E_NUMBER_WALL:
            case E_NUMBER_ERROR:
            case E_NUMBER_EMPTY:
            case E_NUMBER_1:
            case E_NUMBER_2:
            case E_NUMBER_3:
            case E_NUMBER_4:
            case E_NUMBER_5:
            case E_NUMBER_6:
            case E_NUMBER_7:
            case E_NUMBER_8:
                {
                    ;
                }break;
            }
        }
    }

    G::g_dwMineCount        = 0;
    G::g_bMineIsBegin       = TRUE;
    G::g_dwMineTimerCount   = 0;
    G::g_dwMineCurrSpace    = G::g_dwMineMaxSpace;
    return TRUE;
}

// 清雷
BOOL ClearMine(VOID)
{
    if (!G::g_lpMineArea)
    {
        return FALSE;
    }

    for (INT Y = 0; Y < G::g_nMineHeight; Y++)
    {
        for (INT X = 0; X < G::g_nMineWidth; X++)
        {
            E_MINE_FLAG eMineFlag = GetMineFlag(X, Y);
            switch (eMineFlag)
            {
            case E_NUMBER_MINE:
            case E_NUMBER_IS_MINE_INCERTITUDE:
            case E_NUMBER_IS_MINE_FLAG:
                {
                    ChangeMineFlag(X, Y, E_NUMBER_UNKNOW);
                }break;

            default:
            case E_NUMBER_UNKNOW:
            case E_NUMBER_NO_MINE_INCERTITUDE:
            case E_NUMBER_NO_MINE_FLAG:
            case E_NUMBER_WALL:
            case E_NUMBER_ERROR:
            case E_NUMBER_EMPTY:
            case E_NUMBER_1:
            case E_NUMBER_2:
            case E_NUMBER_3:
            case E_NUMBER_4:
            case E_NUMBER_5:
            case E_NUMBER_6:
            case E_NUMBER_7:
            case E_NUMBER_8:
                {
                    ;
                }break;
            }
        }
    }

    G::g_dwMineCount        = 0;
    G::g_dwMineCurrSpace    = G::g_dwMineMaxSpace;
    return TRUE;
}

// 查雷
BOOL SeeMine(VOID)
{
    if (!G::g_lpMineArea)
    {
        return FALSE;
    }

    for (INT Y = 0; Y < G::g_nMineHeight; Y++)
    {
        for (INT X = 0; X < G::g_nMineWidth; X++)
        {
            E_MINE_FLAG eMineFlag = GetMineFlag(X, Y);
            switch (eMineFlag)
            {
            case E_NUMBER_MINE:
            case E_NUMBER_IS_MINE_INCERTITUDE:
            case E_NUMBER_IS_MINE_FLAG:
                {
                    ChangeMineFlag(X, Y, E_NUMBER_IS_MINE_FLAG);
                }break;

            default:
            case E_NUMBER_UNKNOW:
            case E_NUMBER_NO_MINE_INCERTITUDE:
            case E_NUMBER_NO_MINE_FLAG:
            case E_NUMBER_WALL:
            case E_NUMBER_ERROR:
            case E_NUMBER_EMPTY:
            case E_NUMBER_1:
            case E_NUMBER_2:
            case E_NUMBER_3:
            case E_NUMBER_4:
            case E_NUMBER_5:
            case E_NUMBER_6:
            case E_NUMBER_7:
            case E_NUMBER_8:
                {
                    ;
                }break;
            }
        }
    }

    //G::g_dwMineCount        = 0;
    //G::g_bMineIsBegin       = TRUE;
    //G::g_dwMineTimerCount   = 0;
    return TRUE;
}