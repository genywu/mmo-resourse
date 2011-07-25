// Check.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Check.h"
#include <Tlhelp32.h>

// 宏定义无效ID
#define INVALID_ID -1

// 断点地址
#define INTERMIT_ADDR   (LPVOID)0x006A9E81

// 检查地址
#define CHECK_ADDR      (LPVOID)0x0093E977
//0x003F6EC5
// 跳转地址
#define JMP_ADDR1       (LPVOID)0x006A4AB1
#define JMP_ADDR2       (LPVOID)0x006A4B4E
#define JMP_ADDR3       (LPVOID)0x006A4C58
//#define JMP_ADDR4       (LPVOID)0x0094BB74

// 查找窗口最大次数
#define FIND_WND_MAX_TIME 1000;

PROCESS_INFORMATION g_ProcessInfo;  // 进程信息
DWORD g_dwflOldIntermitProtect;     // 断点权限备份
DWORD g_dwflOldCheckProtect;        // 检查点权限备份
DWORD g_dwflOldJmpProtect;          // 跳转点权限备份

BOOL g_IsBreakPoint = FALSE;        // 断点是否已经触发
BOOL g_IsContinueBreakPoint = FALSE;// 是否继续断点执行
BOOL g_AllOK = FALSE;               // 操作全部完毕,退出

BOOL ChangeOfficeName(VOID);
BOOL InitProcess(VOID);
BOOL AmendAddrProtect(VOID);
BOOL ResumeAddrProtect(VOID);

DWORD WINAPI WatchBreakPoint(LPVOID lpThreadParameter);

INT WINAPI _tWinMain(
              HINSTANCE hInstance,
              HINSTANCE hPrevInstance,
              LPTSTR lpCmdLine,
              INT nShowCmd)
{
    BOOL bRet = TRUE;

    // 修改单位名称
    bRet = ChangeOfficeName();
    if (!bRet)
    {
        return INVALID_ID;
    }
    
    // 打开进程
    bRet = InitProcess();
    if (!bRet)
    {
        return INVALID_ID;
    }

    // 获取修改权限
    bRet = AmendAddrProtect();
    if (!bRet)
    {
        return INVALID_ID;
    }

    // 下断点
    BYTE bCC = 0xCC;
    SIZE_T NumberOfBytesWritten;
    bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, INTERMIT_ADDR, &bCC, 0x01, &NumberOfBytesWritten);
    if (!bRet || (0x01 != NumberOfBytesWritten))
    {
        ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
        return INVALID_ID;
    }

    DEBUG_EVENT DebugEvent;
    ::CreateThread(NULL, 0, WatchBreakPoint, NULL, 0, NULL);
    do
    {
        g_IsBreakPoint = TRUE;
        ::WaitForDebugEvent(&DebugEvent, INFINITE);

        if ((EXCEPTION_DEBUG_EVENT == DebugEvent.dwDebugEventCode) && (EXCEPTION_BREAKPOINT == DebugEvent.u.Exception.ExceptionRecord.ExceptionCode))
        {
            // 循环检查进程是否已经脱壳
            BYTE bProcessCode;
            SIZE_T NumberOfBytesRead;
            bRet = ::ReadProcessMemory(g_ProcessInfo.hProcess, CHECK_ADDR, &bProcessCode, 0x01, &NumberOfBytesRead);
            if (!bRet || (0x01 != NumberOfBytesRead))
            {
                ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }
            if (!bProcessCode)
            {
                ::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DBG_CONTINUE);
                continue;
            }

            SIZE_T NumberOfBytesWritten;
            // 修改跳转点
            BYTE bCode1[] = {0xE9, 0x8A, 0x00, 0x00, 0x00, 0x90};
            bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, JMP_ADDR1, bCode1, sizeof(bCode1), &NumberOfBytesWritten);
            if (!bRet || (sizeof(bCode1) != NumberOfBytesWritten))
            {
                ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }

            // 修改跳转点2
            BYTE bCode2[] = {0xE9, 0xB7, 0x01, 0x00, 0x00, 0x90};
            bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, JMP_ADDR2, bCode2, sizeof(bCode2), &NumberOfBytesWritten);
            if (!bRet || (sizeof(bCode2) != NumberOfBytesWritten))
            {
                ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }

            // 修改跳转点3
            BYTE bCode3[] = {0xE9, 0x85, 0x00, 0x00, 0x00, 0x90};
            bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, JMP_ADDR3, bCode3, sizeof(bCode3), &NumberOfBytesWritten);
            if (!bRet || (sizeof(bCode3) != NumberOfBytesWritten))
            {
                ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }

            // 去掉断点
            BYTE b50 = 0x50;
            bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, INTERMIT_ADDR, &b50, 0x01, &NumberOfBytesWritten);
            if (!bRet || (0x01 != NumberOfBytesWritten))
            {
                ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }

            CONTEXT context;
            context.ContextFlags = CONTEXT_ALL;
            if (!::GetThreadContext(g_ProcessInfo.hThread, &context))
            {
                ::MessageBox(::GetActiveWindow(), _T("清除断点失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }
            context.Eip = (DWORD)INTERMIT_ADDR;
            if (!::SetThreadContext(g_ProcessInfo.hThread, &context))
            {
                ::MessageBox(::GetActiveWindow(), _T("清除断点失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }
            ::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DBG_CONTINUE);

            // 恢复地址权限
            bRet = ResumeAddrProtect();
            if (!bRet)
            {
                return INVALID_ID;
            }
            break;
        }
        else
        {
            if (EXCEPTION_DEBUG_EVENT == DebugEvent.dwDebugEventCode)
            {
                ::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
            }
            else
            {
                ::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DBG_CONTINUE);
            }
        }
    } while (TRUE);

    bRet = ::DebugActiveProcessStop(g_ProcessInfo.dwProcessId);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("脱离子程序失败！"), _T("提示"), MB_OK);
        return INVALID_ID;
    }
	return 0;
}

BOOL ChangeOfficeName(VOID)
{
    HANDLE hFileExe = ::CreateFile(_T("宾馆管理系统.exe"), FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFileExe == INVALID_HANDLE_VALUE)
    {
        return TRUE;
    }
    else
    {
        DWORD dwWriteLen = sizeof(TCHAR) * 6;
        ::SetFilePointer(hFileExe, 0x000a8064, 0, FILE_BEGIN);// 0x006A43FF  0x004a8064
        if (!::WriteFile(hFileExe, _T("宾馆试营业"), dwWriteLen, &dwWriteLen, NULL))
        {
            return FALSE;
        }
        ::CloseHandle(hFileExe);
    }
    return TRUE;
}

BOOL InitProcess(VOID)
{
    BOOL bRet = TRUE;
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    TCHAR* strProcessName = _T("宾馆管理系统.exe");

    // 创建进程
    bRet = ::CreateProcess(
        strProcessName,
        strProcessName,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS | CREATE_SUSPENDED,
        NULL,
        NULL,
        &StartInfo,
        &g_ProcessInfo);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("启动项目失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    return TRUE;
}

BOOL AmendAddrProtect(VOID)
{
    BOOL bRet = TRUE;
    // 修改断点地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, INTERMIT_ADDR, 0x01, PAGE_EXECUTE_READWRITE, &g_dwflOldIntermitProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 修改检查点地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, CHECK_ADDR, 0x01, PAGE_EXECUTE_READWRITE, &g_dwflOldCheckProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 修改跳转点地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR1, 0x06, PAGE_EXECUTE_READWRITE, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 修改跳转点地址权限2
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR2, 0x06, PAGE_EXECUTE_READWRITE, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 修改跳转点地址权限3
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR3, 0x06, PAGE_EXECUTE_READWRITE, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 修改跳转点地址权限4
    //bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR4, 0x06, PAGE_EXECUTE_READWRITE, &g_dwflOldJmpProtect);
    //if (!bRet)
    //{
    //    ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
    //    return FALSE;
    //}
    return TRUE;
}

BOOL ResumeAddrProtect(VOID)
{
    BOOL bRet = TRUE;
    // 恢复断点地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, INTERMIT_ADDR, 0x01, g_dwflOldIntermitProtect, &g_dwflOldIntermitProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("恢复断点地址失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复检查地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, CHECK_ADDR, 0x01, g_dwflOldCheckProtect, &g_dwflOldCheckProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("恢复检查地址失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复跳转地址权限
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR1, 0x06, g_dwflOldJmpProtect, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复跳转地址权限2
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR2, 0x06, g_dwflOldJmpProtect, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复跳转地址权限3
    bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR3, 0x06, g_dwflOldJmpProtect, &g_dwflOldJmpProtect);
    if (!bRet)
    {
        ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
        return FALSE;
    }

    // 恢复跳转地址权限4
    //bRet = ::VirtualProtectEx(g_ProcessInfo.hProcess, JMP_ADDR4, 0x06, g_dwflOldJmpProtect, &g_dwflOldJmpProtect);
    //if (!bRet)
    //{
    //    ::MessageBox(::GetActiveWindow(), _T("打开进程空间失败！"), _T("提示"), MB_OK);
    //    return FALSE;
    //}
    return TRUE;
}

DWORD WINAPI WatchBreakPoint(LPVOID lpThreadParameter)
{
    BOOL bRet = TRUE;
    do 
    {
        ::SleepEx(100, TRUE);
    } while (!g_IsBreakPoint);
    // 运行进程
    if (INVALID_ID == ::ResumeThread(g_ProcessInfo.hThread))
    {
        ::MessageBox(::GetActiveWindow(), _T("启动项目失败！"), _T("提示"), MB_OK);
        bRet = ::TerminateProcess(g_ProcessInfo.hProcess, INVALID_ID);
        if (!bRet)
        {
            ::MessageBox(::GetActiveWindow(), _T("终止进程失败\n系统中残留了进程碎片,无法清理！！"), _T("提示"), MB_OK);
        }
    }
    return 0;
}
