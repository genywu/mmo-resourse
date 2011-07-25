// Check.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Check.h"

// 宏定义无效ID
#define INVALID_ID -1

// 断点地址
#define INTERMIT_ADDR   (LPVOID)0x007C7C8E

// 修正地址
//#define CHECK_ADDR1     (LPVOID)0x0040D2BD
//#define CHECK_ADDR2     (LPVOID)0x0040D2C2

PROCESS_INFORMATION g_ProcessInfo;  // 进程信息
DWORD g_dwflOldIntermitProtect;     // 断点权限备份
DWORD g_dwflOldCheckProtect;        // 检查点权限备份

BOOL g_IsBreakPoint = FALSE;        // 断点是否已经触发
//BOOL g_IsContinueBreakPoint = TRUE; // 是否继续断点执行
//BOOL g_IsFirst = TRUE;              // 操作全部完毕,退出

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

    // 打开进程
    bRet = InitProcess();
    if (!bRet)
    {
        return INVALID_ID;
    }

    //// 获取修改权限
    //bRet = AmendAddrProtect();
    //if (!bRet)
    //{
    //    return INVALID_ID;
    //}

    // 下断点
    BYTE bCC = 0xCC;
    //BYTE b66 = 0x66;
    BYTE b8B = 0x8B;
    SIZE_T NumberOfBytesWritten;
    CONTEXT con;
    con.ContextFlags = CONTEXT_ALL;

    ::VirtualProtectEx(g_ProcessInfo.hProcess, INTERMIT_ADDR, 0x01, PAGE_EXECUTE_READWRITE, &g_dwflOldIntermitProtect);
    ::WriteProcessMemory(g_ProcessInfo.hProcess, INTERMIT_ADDR, &bCC, 0x01, &NumberOfBytesWritten);

    DEBUG_EVENT DebugEvent;
    ::CreateThread(NULL, 0, WatchBreakPoint, NULL, 0, NULL);
    static INT i = 0;
    g_IsBreakPoint = TRUE;
    do
    {
        ::WaitForDebugEvent(&DebugEvent, INFINITE);
        ::GetThreadContext(g_ProcessInfo.hThread, &con);

        if ((EXCEPTION_DEBUG_EVENT == DebugEvent.dwDebugEventCode) && (EXCEPTION_BREAKPOINT == DebugEvent.u.Exception.ExceptionRecord.ExceptionCode) && (con.Eip < 0x70000000))
        {
            CHAR szOfficeName[] = "镜湖洗浴";
            NumberOfBytesWritten = sizeof(szOfficeName);

            ::VirtualProtectEx(g_ProcessInfo.hProcess, (LPVOID)con.Eax, NumberOfBytesWritten, PAGE_EXECUTE_READWRITE, &g_dwflOldCheckProtect);
            ::WriteProcessMemory(g_ProcessInfo.hProcess, (LPVOID)con.Eax, szOfficeName, NumberOfBytesWritten, &NumberOfBytesWritten);
            ::VirtualProtectEx(g_ProcessInfo.hProcess, (LPVOID)con.Eax, NumberOfBytesWritten, g_dwflOldCheckProtect, &g_dwflOldCheckProtect);

            struct  
            {
                BYTE    bCode;
                DWORD   dwCode;
                WORD    wCode;
            } s_Code;
            s_Code.bCode   = 0xB8;
            s_Code.dwCode  = 0x00000003;
            s_Code.wCode   = 0x9090;
            NumberOfBytesWritten = sizeof(s_Code);

            ::VirtualProtectEx(g_ProcessInfo.hProcess, (LPVOID)0x009E77DC, NumberOfBytesWritten, PAGE_EXECUTE_READWRITE, &g_dwflOldCheckProtect);
            ::WriteProcessMemory(g_ProcessInfo.hProcess, (LPVOID)0x009E77DC, &s_Code, NumberOfBytesWritten, &NumberOfBytesWritten);
            ::VirtualProtectEx(g_ProcessInfo.hProcess, (LPVOID)0x009E77DC, NumberOfBytesWritten, g_dwflOldCheckProtect, &g_dwflOldCheckProtect);

            // 去掉前断点
            bRet = ::WriteProcessMemory(g_ProcessInfo.hProcess, INTERMIT_ADDR, &b8B, 0x01, &NumberOfBytesWritten);
            if (!bRet || (0x01 != NumberOfBytesWritten))
            {
                ::MessageBox(::GetActiveWindow(), _T("注入进程空间失败！"), _T("提示"), MB_OK);
                return INVALID_ID;
            }

            // 获取寄存器
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
            break;
        }
        else
        {
            ::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DBG_CONTINUE);
        }
    } while (TRUE);

    ::VirtualProtectEx(g_ProcessInfo.hProcess, INTERMIT_ADDR, 0x01, g_dwflOldIntermitProtect, &g_dwflOldIntermitProtect);
    ::DebugActiveProcessStop(g_ProcessInfo.dwProcessId);
    return 0;
}

BOOL InitProcess(VOID)
{
    BOOL bRet = TRUE;
    STARTUPINFOW StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    TCHAR* strProcessName = _T("HTClient.exe");

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
        ::TerminateProcess(g_ProcessInfo.hProcess, INVALID_ID);
    }
    return 0;
}
