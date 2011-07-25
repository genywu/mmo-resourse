#include "Precompiled.h"
#pragma comment(lib, "Kernel32.Lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.Lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "GdiPlus")

//━━━━━━━━━━━〖MsBaseDef 相关〗━━━━━━┓
// 全局防重码随机种子数组                           //┃
DOUBLE      g_dRandRadix[RFREQUENCY] = {0.0f};      //┃
                                                    //┃
// 全局防重码随机种子索引                           //┃
UINT        g_uRandRadixIndex               = 0;    //┃
                                                    //┃
// 全局基本类型初始化线程ID                         //┃
DWORD       g_BaseInitthreadID              = NULL; //┃
                                                    //┃
ULONG_PTR   g_lToken                        = 0;    //┃
                                                    //┃
BOOL        g_IsMsBaseInit                  = FALSE;//┃
                                                    //┃
// 当前时间的全局变量                               //┃
SYSTEMTIME  g_LocalTime                     = {0};  //┃
                                                    //┃
// 数组对象申请助手使用的对象参数                   //┃
DWORD       g_dwArrayHelperClassParameter   = 0;    //┃
MsLock      g_ArrayHelperlock;                      //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━〖MsAssertx 相关〗━━━━━━━━━━┓
#ifdef  NO_LOG          // 定义此宏去掉断言日志             //┃
INT     g_Command_Assert_Log = COMMAND_IGNORE_LOG;          //┃
#else                                                       //┃
INT     g_Command_Assert_Log = COMMAND_NORMAL_LOG;          //┃
#endif                                                      //┃
                                                            //┃
#ifdef  NO_MSGBOX       // 定义此宏去掉断言对话框           //┃
INT     g_Command_MessageBox = COMMAND_IGNORE_MSGBOX;       //┃
#else                                                       //┃
INT     g_Command_MessageBox = COMMAND_NORMAL_MSGBOX;       //┃
#endif                                                      //┃
                                                            //┃
HANDLE  g_hAssertLogFile = NULL;// 全局断言日志文件句柄     //┃
BOOL    g_bAssertInit = FALSE;  // 全局断言是否初始化标记   //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━〖Rand相关〗━━━━━━━━━━━━━┓
BOOL InitRand(VOID* /*Parameter*/)                          //┃
{                                                           //┃
    g_dRandRadix[0] = R_DOUBLE_A(0.0f,1.0f);                //┃
    for (DWORD i = 1; i < RFREQUENCY; i++) {                //┃
        g_dRandRadix[i] = R_DOUBLE_A(0.0f,1.0f);            //┃
        if (g_dRandRadix[i] == g_dRandRadix[i-1]) {         //┃
            g_dRandRadix[i] += R_DOUBLE_A(0.0f,1.0f);       //┃
        }                                                   //┃
    }                                                       //┃
    return TRUE;                                            //┃
}                                                           //┃
//━━━━━━━━━〖Rand相关〗━━━━━━━━━━━━━━━┛

//━━━━━━━━━〖初始化断言函数〗━━━━━━━━━━━━┓
BOOL InitAssertx(TCHAR* szAssertFileName)                   //┃
{                                                           //┃
    if (!szAssertFileName)                                  //┃
    {                                                       //┃
        return FALSE;                                       //┃
    }                                                       //┃
                                                            //┃
    TCHAR szAssertPathBuff[MAX_PATH] = {0};                 //┃
    TCHAR szCurrentDirectory[MAX_PATH] = {0};               //┃
    ::GetCurrentDirectory(MAX_PATH, szCurrentDirectory);    //┃
    _stprintf_s(                                            //┃
        szAssertPathBuff,                                   //┃
        MAX_PATH,                                           //┃
        szAssertFileName,                                   //┃
        szCurrentDirectory                                  //┃
        );                                                  //┃
                                                            //┃
    // 日志缓冲区                                           //┃
    TCHAR szLog[ASSERTX_STR_MAX_LEN] = {0};                 //┃
                                                            //┃
    ::CreateDirectory(_T("./Log/"), NULL);                  //┃
    if (!g_hAssertLogFile)                                  //┃
    {                                                       //┃
        g_hAssertLogFile = ::CreateFile(                    //┃
            szAssertPathBuff,                               //┃
            FILE_APPEND_DATA,                               //┃
            FILE_SHARE_READ,                                //┃
            NULL,                                           //┃
            OPEN_EXISTING,                                  //┃
            0,                                              //┃
            NULL                                            //┃
            );                                              //┃
    }                                                       //┃
    if (INVALID_HANDLE_VALUE == g_hAssertLogFile)           //┃
    {                                                       //┃
        if (ERROR_FILE_NOT_FOUND == ::GetLastError())       //┃
        {                                                   //┃
            g_hAssertLogFile = ::CreateFile(                //┃
                szAssertPathBuff,                           //┃
                FILE_APPEND_DATA,                           //┃
                FILE_SHARE_READ,                            //┃
                NULL,                                       //┃
                CREATE_NEW,                                 //┃
                NULL,                                       //┃
                NULL                                        //┃
                );                                          //┃
#ifdef UNICODE                                              //┃
            // 成功写入日志的字节数                         //┃
            DWORD uLogSize = 0;                             //┃
            WORD UnicodeHead = 0xFEFF;                      //┃
            uLogSize = sizeof(UnicodeHead);                 //┃
            if (!::WriteFile(                               //┃
                g_hAssertLogFile,                           //┃
                &UnicodeHead,                               //┃
                uLogSize,                                   //┃
                &uLogSize,                                  //┃
                NULL                                        //┃
                ))                                          //┃
            {                                               //┃
                _stprintf_s(                                //┃
                    szLog,                                  //┃
                    ASSERTX_STR_MAX_LEN,                    //┃
                    _T("断言日志文件打开失败！！！\n%d"),   //┃
                    ::GetLastError()                        //┃
                    );                                      //┃
                MESSAGEBOX(AL_SHOW_ONLY, szLog);            //┃
            }                                               //┃
#endif                                                      //┃
        }                                                   //┃
        else                                                //┃
        {                                                   //┃
            _stprintf_s(                                    //┃
                szLog,                                      //┃
                ASSERTX_STR_MAX_LEN,                        //┃
                _T("断言日志文件打开失败！！！\n%d"),       //┃
                ::GetLastError()                            //┃
                );                                          //┃
            MESSAGEBOX(AL_SHOW_ONLY, szLog);				//┃
        }                                                   //┃
    }                                                       //┃
                                                            //┃
    g_bAssertInit = TRUE;                                   //┃
    return TRUE;                                            //┃
}                                                           //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━〖释放断言函数〗━━━━━━━━━━━━┓
BOOL ReleaseAssertx(VOID* /*Parameter*/)                    //┃
{                                                           //┃
    // 布尔类型返回值                                       //┃
    BOOL bRet = TRUE;                                       //┃
    // 如果断言日志文件已经打开                             //┃
    if (g_hAssertLogFile && g_bAssertInit)                  //┃
    {                                                       //┃
        bRet = ::CloseHandle(g_hAssertLogFile);             //┃
        if (bRet)                                           //┃
        {                                                   //┃
            g_hAssertLogFile = NULL;                        //┃
        }                                                   //┃
        g_bAssertInit = FALSE;                              //┃
        return bRet;                                        //┃
    }                                                       //┃
    else                                                    //┃
    {                                                       //┃
        g_bAssertInit = FALSE;                              //┃
        return TRUE;                                        //┃
    }                                                       //┃
}                                                           //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//共享锁                                                    //┃
MsLock::MsLock()                                            //┃
{                                                           //┃
    ::InitializeCriticalSection(&m_Lock);                   //┃
}                                                           //┃
                                                            //┃
//析构中删除共享标记                                        //┃
MsLock::~MsLock()                                           //┃
{                                                           //┃
    ::DeleteCriticalSection(&m_Lock);                       //┃
}                                                           //┃
                                                            //┃
//重置临界区                                                //┃
inline VOID MsLock::ReSet()                                 //┃
{                                                           //┃
    ::InitializeCriticalSection(&m_Lock);                   //┃
}                                                           //┃
                                                            //┃
//进入临界区                                                //┃
inline VOID MsLock::Lock()                                  //┃
{                                                           //┃
    ::EnterCriticalSection(&m_Lock);                        //┃
}                                                           //┃
                                                            //┃
//离开临界区                                                //┃
inline VOID MsLock::UnLock()                                //┃
{                                                           //┃
    ::LeaveCriticalSection(&m_Lock);                        //┃
}                                                           //┃
                                                            //┃
inline AutoLock::AutoLock(MsLock& rLock, BOOL bUse)         //┃
{                                                           //┃
    m_bUse = bUse;                                          //┃
    if (m_bUse)                                             //┃
    {                                                       //┃
        m_pLock = &rLock;                                   //┃
        m_pLock->Lock();                                    //┃
    }                                                       //┃
}                                                           //┃
                                                            //┃
//析构中解锁                                                //┃
inline AutoLock::~AutoLock()                                //┃
{                                                           //┃
    if (m_bUse)                                             //┃
    {                                                       //┃
        m_pLock->UnLock();                                  //┃
    }                                                       //┃
}                                                           //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

VOID InitMsBase(
    TCHAR* szAssertFileName,
    BOOL bConsole,
    RUNCALLBACK pRunCallBack,
    LPTSTR szClassName,
    LPTSTR szWndName,
    HOOKWNDPROC HookWndProc,
    WNDPROC WndProc,
    PTHREAD_START_ROUTINE ThcreadProc
    )
{
    USE_PARAM(ThcreadProc, WndProc, HookWndProc, szWndName, szClassName, pRunCallBack, bConsole);
    g_IsMsBaseInit = FALSE;
    if (g_IsMsBaseInit)
    {
        return;
    }

    // 设置调试标记
    _DEBUG_CODE(_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF));
    setlocale(LC_ALL, "chs");

    InitRand(NULL);
    if (szAssertFileName)
    {
        InitAssertx(szAssertFileName);
    }

    WSADATA WsaData; // Winsock详细信息
    // 初始化 Winsock
    INT nRet = ::WSAStartup(MAKEWORD(2,2), &WsaData);
    AssertEx(AL_CATAST, !nRet, _T("WSAStartup失败,函数失败"));

#ifndef USE_MFC
    if (bConsole)
    {
        if (pRunCallBack)
        {
            g_pCMsConsole = NEW CMsConsole(pRunCallBack);
        }
    }
    else if (WndProc || ThcreadProc)
    {
        g_pCMsWinMain = NEW CMsWinMain(
            szClassName,
            szWndName,
            HookWndProc,
            WndProc,
            ThcreadProc,
            NULL
            );

        Gdiplus::GdiplusStartupInput Input;
        Gdiplus::GdiplusStartup(&g_lToken, &Input, NULL);
    }
    else
    {
        AssertEx(AL_CATAST, FALSE, _T("【CMsBaseRun】对象执行函数不能全部为空"));
    }
#endif
    g_IsMsBaseInit = TRUE;
}


VOID ReleaseMsBase(LPVOID)
{
    if (!g_IsMsBaseInit)
    {
        return;
    }
    if (g_lToken)
    {
        Gdiplus::GdiplusShutdown(g_lToken);
    }

#ifndef USE_MFC
    HANDLE hProcess32Snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcess32Snap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        ZeroMemoryStruct(pe32);
        pe32.dwSize = sizeof(PROCESSENTRY32);
        do 
        {
            ::SleepEx(10, TRUE);
            if (::Process32First(hProcess32Snap, &pe32))
            {
                if (pe32.cntThreads > 1)
                {
                    break;
                }
            }
            else
            {
                AssertEx(AL_CATAST, FALSE, _T("无法查看线程信息！"));
                break;
            }
        } while (pe32.cntThreads > 1);
    }
    ::CloseHandle(hProcess32Snap);
    SAFE_DELETE(g_pCMsWinMain);
    SAFE_DELETE(g_pCMsConsole);
    ::WSACleanup();
#endif
    ReleaseAssertx(NULL);
    g_IsMsBaseInit = FALSE;
}