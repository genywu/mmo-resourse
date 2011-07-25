#include "Precompiled.h"
#include "MsWinMain.h"
#include "Ms2DEngineDef.h"
#include "Ms2DInput.h"
#include "Ms2DEngine.h"

#ifdef UNICODE
INT CALLBACK WinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in INT nShowCmd
    )
{
    //_ENTER_FUN_C;

    if (lpCmdLine)
    {
        static WCHAR szCmdLine[KB_SIZE] = {0};
        ::MultiByteToWideChar(
            CP_ACP, 
            0, 
            lpCmdLine,
            (INT)strlen(lpCmdLine) + 1,
            szCmdLine,
            KB_SIZE
            );
        return wWinMain(hInstance, hPrevInstance, szCmdLine, nShowCmd);
    }
    else
    {
        AssertEx(AL_NORMAL, lpCmdLine, _T("lpCmdLine为空"));
    }

    //_DISPOSE_END_C
    //{
    //    MESSAGEBOX(_T("最后的异常处理,程序即将结束!"));
    //}
    //_LEAVE_FUN;
    return INVALID_NID;
}
#endif

// Win入口函数
INT CALLBACK _tWinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPTSTR lpCmdLine,
    __in INT nShowCmd
    )
{
    //_ENTER_FUN_C;


    AssertEx(
		AL_NORMAL,
        g_pCMsWinMain,
        _T("请在预编译头cpp文件中定义全局【CMsBaseRun】对象【使用控制台处理方式】\n")
        _T("CMsBaseRun s_CMsBaseRun(")
        _T("_T(\"%s/Log/Exe_RrGameStation_Assertx.log\"),")
        _T("_T(\"Exe_RrGameStation\"),")
        _T("_T(\"冗睿游戏平台\"),")
        _T("Fun钩子消息线程回调")
        _T("Fun窗口消息线程回调")
        _T("NULL")
        _T(");")
        );

    INT nRet = INVALID_NID;
    S_2DEngineWndInfo s_ewi;
    s_ewi.m_bTopMost= FALSE;
    s_ewi.m_nX      = CW_USEDEFAULT;
    s_ewi.m_nY      = 0;
    s_ewi.m_nWidth  = CW_USEDEFAULT;
    s_ewi.m_nHeight = 0;
    s_ewi.m_dwStyle = WS_OVERLAPPEDWINDOW;

    if (g_pCMsWinMain && lpCmdLine)
    {
        USE_PARAM(nShowCmd, hPrevInstance, hInstance);
        g_pCMsWinMain->Init(hInstance, hPrevInstance, lpCmdLine, nShowCmd, &s_ewi);
        nRet = g_pCMsWinMain->Run();
        g_pCMsWinMain->WaitQuit();
    }
    return nRet;

    //_DISPOSE_END_C
    //{
    //    MESSAGEBOX(_T("最后的异常处理,程序即将结束!"));
    //}
    //_LEAVE_FUN;
    //return INVALID_NID;
}

LRESULT CALLBACK CMsWinMain::CMsWinMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (g_pCMsWinMain->m_HookWndProc)
    {
        // 调用消息钩子
        MSG Msg = {hWnd, message, wParam, lParam, 0, 0, 0};
        BOOL bHalt = g_pCMsWinMain->m_HookWndProc(&Msg);
        if (bHalt)
        {
            return bHalt;
        }
    }

    switch(message)
    {
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpmmInfo = (LPMINMAXINFO)lParam;
            if (g_pCMsWinMain->m_bMinLockSize)
            {
                lpmmInfo->ptMinTrackSize = g_pCMsWinMain->m_MinSize;
            }
            if (g_pCMsWinMain->m_bMaxLockSize)
            {
                lpmmInfo->ptMaxTrackSize = g_pCMsWinMain->m_MaxSize;
            }
        }break;
    //case WM_SIZING:
    //case WM_SIZE:
    //    {
    //        if (!g_pCMsWinMain->m_bCanSize)
    //        {
    //            return 0;
    //        }
    //    }break;

    default:
        {
            if (g_pCMsWinMain->m_AWndProc)
            {
                return g_pCMsWinMain->m_AWndProc(hWnd, message, wParam, lParam);
            }
            else
            {
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }
        }break;
    }
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

extern CMsWinMain* g_pCMsWinMain = NULL;

CMsWinMain::CMsWinMain(
    LPTSTR szClassName,
    LPTSTR szWndName,
    HOOKWNDPROC HookWndProc,
    WNDPROC WndProc,
    PTHREAD_START_ROUTINE ThreadProc,
    LPVOID lpCreateParameter
    )
: m_hInstance           (NULL)
, m_hPrevInstance       (NULL)
, m_nShowCmd            (0)
, m_hMainWnd            (NULL)
, m_MsRun_hAccelTable   (NULL)
, m_eWinMainErrorCode   (ERROR_NO_ERROR)
, m_ThreadProc          (ThreadProc)
, m_HookWndProc         (HookWndProc)
, m_AWndProc            (WndProc)
, m_dwMsgThreadId       (INVALID_UID)
, m_lpCreateParameter   (lpCreateParameter)
, m_bTopMost            (FALSE)
, m_hAIcon              (NULL)
, m_hSysIcon            (NULL)
, m_nX                  (CW_USEDEFAULT)
, m_nY                  (0)
, m_nWidth              (CW_USEDEFAULT)
, m_nHeight             (0)
, m_dwStyle             (WS_OVERLAPPEDWINDOW)
, m_bMinLockSize        (FALSE)
, m_bMaxLockSize        (FALSE)
{
    AssertEx(
		AL_NORMAL,
        (szWndName || ThreadProc),
        _T("调用错误szWndName 与 ThreadProc不能同时为空")
        );

    ZeroMemoryArray(m_szAstrWndClassName);
    ZeroMemoryArray(m_szAstrWndName);
    if (szClassName)
    {
        _tcscpy_s(m_szAstrWndClassName, MAX_PATH, szClassName);
    }
    if (szWndName)
    {
        _tcscpy_s(m_szAstrWndName, MAX_PATH, szWndName);
    }
    ZeroMemoryStruct(m_MinSize);
    ZeroMemoryStruct(m_MaxSize);
    ZeroMemoryStruct(m_nId);
    ZeroMemory(&m_nId, sizeof(m_nId));
    m_nId.cbSize            = sizeof(m_nId);
    m_nId.uCallbackMessage  = MSM_SYS_ICON_MESSAGE;
    m_nId.uVersion          = NOTIFYICON_VERSION; 
}

// 停止之前
BOOL CMsWinMain::ForeStop(VOID)
{
    return TRUE;
}

INT CMsWinMain::ThreadCallBack(VOID)
{
    MSG Msg;
    BOOL bRet = FALSE;
    BOOL bHalt = FALSE;

    if (0 == _tcslen(m_szAstrWndClassName))
    {
        return 0;
    }

    // 创建窗口
    this->MsCreateMainWindow();

    do
    {
        bRet = ::GetMessage(&Msg, NULL, 0, 0);
        if (bRet)
        {
            // 检查过滤消息
            if (this->MsLeachMsg())
            {
                continue;
            }

            if (!Msg.hwnd && g_pCMsWinMain->m_HookWndProc)
            {
                // 调用消息钩子
                bHalt = g_pCMsWinMain->m_HookWndProc(&Msg);
                if (bHalt)
                {
                    continue;
                }
            }

            if (Msg.hwnd && m_MsRun_hAccelTable)
            {
                // 解释热键表
                bHalt = ::TranslateAccelerator(Msg.hwnd, m_MsRun_hAccelTable, &Msg);
                if (bHalt)
                {
                    continue;
                }
            }

            // 分发消息
            ::TranslateMessage(&Msg);   // 解释消息
            ::DispatchMessage(&Msg);    // 发送消息
        }
    } while (bRet);
    return 0;
}

// 设置窗口快捷键表ID(必须在调用Init之前使用, )
HACCEL CMsWinMain::SetAcceleratorID(DWORD dwAcceleratoID)
{
    HACCEL hAccelTable = m_MsRun_hAccelTable;
    m_MsRun_hAccelTable = 
        ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(dwAcceleratoID));
    return hAccelTable;
}

VOID CMsWinMain::Init(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPTSTR lpCmdLine,
    __in INT nShowCmd,
    S_2DEngineWndInfo* s_pewi
    )
{
    _tcscpy_s(m_szCommandLine, lpCmdLine);

    m_hInstance     = hInstance;
    m_hPrevInstance = hPrevInstance;
    m_nShowCmd      = nShowCmd;
    m_bTopMost      = s_pewi->m_bTopMost;
    m_nX            = s_pewi->m_nX;
    m_nY            = s_pewi->m_nY;
    m_nWidth        = s_pewi->m_nWidth;
    m_nHeight       = s_pewi->m_nHeight;
    m_dwStyle       = s_pewi->m_dwStyle;
}

// 获取窗口线程Id
DWORD CMsWinMain::GetWndThreadId(VOID)
{
    return m_dwMsgThreadId;
}

// 等待结束
VOID CMsWinMain::WaitQuit(VOID)
{
    if (m_hThread)
    {
        ::WaitForSingleObject(m_hThread, INFINITE);
    }
    m_hThread = NULL;
}

// 直接投递消息
LRESULT CMsWinMain::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessage(m_hMainWnd, uMsg, wParam, lParam);
}

BOOL CMsWinMain::SetWndName(LPTSTR szWndName)
{
    if (szWndName)
    {
        return ::SetWindowText(m_hMainWnd, szWndName);
    }
    return FALSE;
}

BOOL CMsWinMain::MsCreateMainWindow(VOID)
{
    if (_tcslen(m_szAstrWndClassName))
    {
        WNDCLASSEX WndClassEx;
        ZeroMemoryStruct(WndClassEx);
        WndClassEx.style        = CS_HREDRAW | CS_VREDRAW;
        WndClassEx.cbSize       = sizeof(WNDCLASSEX);
        WndClassEx.hInstance    = m_hInstance;
        WndClassEx.lpfnWndProc  = CMsWinMainWndProc;
        WndClassEx.hIcon		= m_hAIcon;
        WndClassEx.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
        WndClassEx.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
        WndClassEx.lpszClassName= m_szAstrWndClassName;
        WndClassEx.hIconSm		= m_hAIcon;
        ::RegisterClassEx(&WndClassEx);

        if (m_bTopMost)
        {
            m_hMainWnd = CreateWindowEx(
                WS_EX_TOPMOST,
                m_szAstrWndClassName,
                m_szAstrWndName,
                m_dwStyle,
                m_nX, 
                m_nY,
                m_nWidth,
                m_nHeight,
                NULL,
                NULL,
                m_hInstance,
                m_lpCreateParameter
                );
        }
        else
        {
            m_hMainWnd = ::CreateWindow(
                m_szAstrWndClassName,
                m_szAstrWndName,
                m_dwStyle,
                m_nX,
                m_nY,
                m_nWidth,
                m_nHeight,
                NULL,
                NULL,
                m_hInstance,
                m_lpCreateParameter
                );
        }

        if (m_hMainWnd)
        {
            ::SendMessage(m_hMainWnd, MSM_INIT_MESSAGE, 0, 0);
            return this->SetWinShow(m_nShowCmd);
        }
        return FALSE;
    }
    if (m_ThreadProc)
    {
        ::CreateThread(NULL, 0, m_ThreadProc, this, 0, NULL);
    }
    return TRUE;
}

// 内部过滤消息
BOOL CMsWinMain::MsLeachMsg(VOID)
{
    //switch (m_Msg.message)
    //{
    //}
    return FALSE;
}

// 获取命令行参数
LPTCHAR CMsWinMain::GetWinCommandLine(VOID)
{
    return m_szCommandLine;
}

// 获取实例句柄
HINSTANCE CMsWinMain::GetHInstance(VOID)
{
    return m_hInstance;
}

HINSTANCE CMsWinMain::GetPrevHInstance(VOID)
{
    return m_hPrevInstance;
}

HWND CMsWinMain::GethWnd(VOID)
{
    return m_hMainWnd;
}

// 设置最大化按钮
VOID CMsWinMain::SetMaximize(BOOL bEnabled)
{
    if (bEnabled)
    {
        m_dwStyle |= WS_MAXIMIZEBOX;
    }
    else
    {
        m_dwStyle &= ~WS_MAXIMIZEBOX;
    }
    ::SetWindowLong(m_hMainWnd, GWL_STYLE, m_dwStyle);
}

// 设置最小化按钮
VOID CMsWinMain::SetMinimize(BOOL bEnabled)
{
    if (bEnabled)
    {
        m_dwStyle |= WS_MINIMIZEBOX;
    }
    else
    {
        m_dwStyle &= ~WS_MINIMIZEBOX;
    }
    ::SetWindowLong(m_hMainWnd, GWL_STYLE, m_dwStyle);
}

// 设置系统菜单
VOID CMsWinMain::SetSysMenu(BOOL bEnabled)
{
    if (bEnabled)
    {
        m_dwStyle |= WS_SYSMENU;
    }
    else
    {
        m_dwStyle &= ~WS_SYSMENU;
    }
    ::SetWindowLong(m_hMainWnd, GWL_STYLE, m_dwStyle);
}

// 设置显示方式
BOOL CMsWinMain::SetWinShow(INT nShowCmd)
{
    return ::ShowWindow(m_hMainWnd, nShowCmd);
}

// 设置菜单
BOOL CMsWinMain::SetWinMenu(HMENU hMenu)
{
    return ::SetMenu(m_hMainWnd, hMenu);
}

// 关闭窗口
BOOL CMsWinMain::CloseWindow(VOID)
{
    return ::CloseWindow(m_hMainWnd);
}

// 设置图标
VOID CMsWinMain::SetIcon(INT nIconId, BOOL bBigIcon)
{
    HICON hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(nIconId));
    return this->SetIcon(hIcon, bBigIcon);
}

// 设置图标
VOID CMsWinMain::SetIcon(HICON hIcon, BOOL bBigIcon)
{
    ::SendMessage(m_hMainWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon);
}

// 是否激活
VOID CMsWinMain::SetEnabled(BOOL bEnabled)
{
    ::EnableWindow(m_hMainWnd, bEnabled);
}

// 窗口最小尺寸
VOID CMsWinMain::SetMinSize(POINT& Pos)
{
    RECT rc;
    LONG lX, lY;
    ::GetWindowRect(m_hMainWnd, &rc);
    lX = rc.right - rc.left;
    lY = rc.bottom - rc.top;
    if (lX < Pos.x)
    {
        lX = rc.left + Pos.x;
    }
    if (lY < Pos.y)
    {
        lY = rc.top + Pos.y;
    }
    POINT Point = {lX, lY};
    this->SetClientSize(Point);
    m_MinSize = Pos;
    m_bMinLockSize = TRUE;
}

// 窗口最大尺寸
VOID CMsWinMain::SetMaxSize(POINT& Pos)
{
    RECT rc;
    LONG lX, lY;
    ::GetWindowRect(m_hMainWnd, &rc);
    lX = rc.right - rc.left;
    lY = rc.bottom - rc.top;
    if (lX > Pos.x)
    {
        lX = rc.left + Pos.x;
    }
    if (lY > Pos.y)
    {
        lY = rc.top + Pos.y;
    }
    POINT Point = {lX, lY};
    this->SetClientSize(Point);
    m_MaxSize = Pos;
    m_bMaxLockSize = TRUE;
}

// 窗口锁定尺寸
VOID CMsWinMain::SetLockSize(POINT& Pos)
{
    this->SetClientSize(Pos);
    m_MinSize = Pos;
    m_MaxSize = Pos;
    m_bMinLockSize = TRUE;
    m_bMaxLockSize = TRUE;
}

VOID CMsWinMain::SetLockSize(DWORD dwWidth, DWORD dwHeight)
{
    POINT p = {dwWidth, dwHeight};
    this->SetClientSize(p);
    m_MinSize = p;
    m_MaxSize = p;
    m_bMinLockSize = TRUE;
    m_bMaxLockSize = TRUE;
}

// 设置窗口客户区尺寸
VOID CMsWinMain::SetClientSize(POINT& Pos)
{
    RECT rc;
    ::GetClientRect(m_hMainWnd, &rc);
    Pos.x = Pos.x - (rc.right - rc.left);
    Pos.y = Pos.y - (rc.bottom - rc.top);
    ::GetWindowRect(m_hMainWnd, &rc);
    if (rc.left < 0 && rc.top < 0 && rc.right < 0 && rc.bottom < 0)
    {
        return;
    }
    Pos.x = (rc.right - rc.left) + Pos.x;
    Pos.y = (rc.bottom - rc.top) + Pos.y;
    ::SetWindowPos(m_hMainWnd, NULL, 0, 0, Pos.x, Pos.y, SWP_NOMOVE | SWP_NOZORDER);
}

// 设置系统托盘图标
VOID CMsWinMain::SetSysIcon(UINT IconID)
{
    m_hSysIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IconID));
}

// 添加系统托盘
BOOL CMsWinMain::AddSysIcon(UINT IconID, TCHAR* szToolTip)
{
    if (NULL == m_hSysIcon)
    {
        this->SetSysIcon(IconID);
    }

    // 增加
    m_nId.hWnd  = m_hMainWnd;
    m_nId.hIcon = m_hSysIcon;
    m_nId.uID   = IconID;
    m_nId.uFlags= NIF_ICON|NIF_MESSAGE|NIF_TIP;
    _tcscpy_s(m_nId.szTip, szToolTip);
    return ::Shell_NotifyIcon(NIM_ADD, &m_nId);
}

// 删除系统托盘
BOOL CMsWinMain::DelSysIcon(UINT IconID)
{
    if (m_hSysIcon)
    {
        m_hSysIcon = NULL;
    }
    // 删除
    m_nId.hWnd  = m_hMainWnd;
    m_nId.hIcon = m_hSysIcon;
    m_nId.uID   = IconID;
    m_nId.uFlags= NIF_ICON|NIF_MESSAGE|NIF_TIP;
    return ::Shell_NotifyIcon(NIM_DELETE, &m_nId);
}

// 弹出系统托盘消息
BOOL CMsWinMain::PopSysIconMsg(UINT IconID, TCHAR* szTitle, TCHAR* szInfo)
{
    // 提示
    m_nId.hWnd          = m_hMainWnd;
    m_nId.hIcon         = m_hSysIcon;
    m_nId.uFlags        = NIF_INFO;
    m_nId.uTimeout      = 1000;
    m_nId.uID           = IconID;
    m_nId.dwInfoFlags   = NIIF_INFO;
    _tcscpy_s(m_nId.szInfoTitle, szTitle);
    _tcscpy_s(m_nId.szInfo, szInfo);
    return ::Shell_NotifyIcon(NIM_MODIFY, &m_nId);
}

// 设置图标(必须在运行前调用)
VOID CMsWinMain::SetIconPreCreate(DWORD dwIconID)
{
    HICON hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(dwIconID));
    m_hAIcon = hIcon;
}

// 设置图标(必须在运行前调用)
VOID CMsWinMain::SetIconPreCreate(HICON m_hIcon)
{
    m_hAIcon = m_hIcon;
}

// 获取IAT信息
VOID CMsWinMain::LocationIAT(
    HMODULE hModule,
    LPTCHAR szImportMod,
    PIMAGE_IMPORT_DESCRIPTOR& lpImportDesc
    )
{
    if (!hModule)
    {
        lpImportDesc = NULL;
        return;
    }
    TCHAR szImportName[MAX_PATH] = {0};
    TCHAR szTempImportName[MAX_PATH] = {0};
    _tcscpy_s(szImportName, szImportMod);

    //检查是否为DOS程序，如是返回NULL，因DOS程序没有IAT。   
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
    if(pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        lpImportDesc = NULL;
        return;
    }

    //检查是否为NT标志，否则返回NULL。
    PIMAGE_NT_HEADERS pNTHeader = 
        (PIMAGE_NT_HEADERS)P_OPER_A(pDOSHeader, pDOSHeader->e_lfanew);

    if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        lpImportDesc = NULL;
        return;
    }

    //没有IAT表则返回NULL。
    DWORD dwVirtualAddress =
        pNTHeader->OptionalHeader.
        DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].
        VirtualAddress;

    if(0 == dwVirtualAddress)
    {
        lpImportDesc = NULL;
        return;
    }

    // 定位第一个IAT位置。
    lpImportDesc = 
        (PIMAGE_IMPORT_DESCRIPTOR)(
        (ULONG_PTR)pDOSHeader + 
        (ULONG_PTR)(pNTHeader->
        OptionalHeader.
        DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].
        VirtualAddress));

    // 根据输入库名称循环检查所有的IAT，
    // 如匹配则返回该IAT地址，
    // 否则检测下一个IAT。
    while (lpImportDesc->Name)
    {
        //获取该IAT描述的输入库名称。
        PSTR szCurrMod = (PSTR)P_OPER_A(pDOSHeader, lpImportDesc->Name);
#ifdef UNICODE
        ::MultiByteToWideChar(
            CP_ACP, 
            0, 
            szCurrMod,
            (DWORD)(strlen(szCurrMod) + 1),
            szTempImportName,
            MAX_PATH);
#else
        strcpy_s(szTempImportName, szCurrMod);
#endif
        _tcsupr_s(szImportName, MAX_PATH);
        _tcsupr_s(szTempImportName, MAX_PATH);
        if (_tcsicmp(szTempImportName, szImportName) == 0)
        {
            break;
        }
        else
        {
            lpImportDesc++;
        }
    }

    if(lpImportDesc->Name == NULL)
    {
        lpImportDesc = NULL;
    }
}

// 替换IAT记录
BOOL CMsWinMain::HookAPIByName(HMODULE hModule, LPTCHAR szImportMod, HOOKAPI* pHookApi)
{     
    // 定位szImportMod输入库在输入数据段中的IAT地址。
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    LocationIAT(hModule, szImportMod, pImportDesc);
    if (pImportDesc == NULL)
    {
        return FALSE;
    }

    // 第一个Thunk地址。
    PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)P_OPER_A(hModule, pImportDesc->OriginalFirstThunk);

    // 第一个IAT项的Thunk地址。
    PIMAGE_THUNK_DATA pRealThunk = (PIMAGE_THUNK_DATA)P_OPER_A(hModule ,pImportDesc->FirstThunk);

    // 循环查找被截API函数的IAT项，并使用替代函数地址修改其值。
    while(pOrigThunk->u1.Function)
    {     
        // 检测此Thunk是否为IAT项。
        if((pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) != 
            IMAGE_ORDINAL_FLAG)
        {   
            // 获取此IAT项所描述的函数名称。
            PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)P_OPER_A(hModule, pOrigThunk->u1.AddressOfData);
            if(0 == pByName->Name[0])
            {
                return FALSE;
            }

            ACHAR szFunName[MAX_PATH] = {0};
#ifdef UNICODE
            ::WideCharToMultiByte(
                CP_ACP, 
                0, 
                pHookApi->m_strFunc,
                (INT)_tcslen(pHookApi->m_strFunc) + 1,
                szFunName,
                MAX_PATH,
                NULL,
                NULL
                );
#else
            strcpy_s(szFunName, pHookApi->m_strFunc);
#endif
            if (_strcmpi(szFunName, (ACHAR*)pByName->Name) == 0)
            {     
                MEMORY_BASIC_INFORMATION mbi_thunk;

                // 查询修改页的信息。
                VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));

                // 改变修改页保护属性为PAGE_READWRITE。
                VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect);

                // 保存原来的API函数地址。
                if(pHookApi->m_pOldProc == NULL)
                {
                    pHookApi->m_pOldProc = pRealThunk->u1.Function;
                }

                // 修改API函数IAT项内容为替代函数地址。
                pRealThunk->u1.Function = pHookApi->m_pNewProc;

                // 恢复修改页保护属性。
                DWORD dwOldProtect;
                VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect);
            }
        }
        pOrigThunk++;
        pRealThunk++;
    }     
    return TRUE;
}