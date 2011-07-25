#pragma once            //┃//┃
#ifndef __MSWINMAIN_H__ //┃//┃
#define __MSWINMAIN_H__ //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsWinMain.h     //┃
//功能描述：WinMain入口     //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

// HookApi记录结构体
struct HOOKAPI32
{
    TCHAR*      m_strFunc;
    DWORD       m_pOldProc;
    DWORD       m_pNewProc;
};

struct HOOKAPI64
{
    TCHAR*      m_strFunc;
    ULONG_PTR   m_pOldProc;
    ULONG_PTR   m_pNewProc;
};

#ifdef _WIN64
#define HOOKAPI HOOKAPI64
#else
#define HOOKAPI HOOKAPI32
#endif

struct S_2DEngineWndInfo
{
    BOOL    m_bTopMost;
    INT     m_nX;
    INT     m_nY;
    INT     m_nWidth;
    INT     m_nHeight;
    DWORD   m_dwStyle;
};

// WinMain执行流程
enum E_WIN_MAIN_FLOW
{
    E_WIN_MAIN_FLOW_NORMAL = 0,     // 标准流程
    E_WIN_MAIN_FLOW_2DENGINE,       // 2D引擎流程


};
class CMsWinMain
    : public CMsThread
{
public:
    enum E_ERROR_CODE
    {
        ERROR_UNKNOW = -1,          // 未知错误
        ERROR_NO_ERROR = 0,         // 无错误
        ERROR_UNSPECIFIED_DIALOGID, // 未指定对话框ID
        ERROR_OPERATE_EXCEPTION,    // 运行异常
    };

public:

    // 构造函数
    CMsWinMain(
        LPTSTR szClassName,
        LPTSTR szWndName,
        HOOKWNDPROC HookWndProc,
        WNDPROC WndProc,
        PTHREAD_START_ROUTINE ThreadProc,
        LPVOID lpCreateParameter
        );

    static LRESULT CALLBACK CMsWinMainWndProc(HWND,UINT,WPARAM,LPARAM);

    virtual BOOL ForeStop(VOID);        // 停止之前
    virtual BOOL ThreadCallBack(VOID);  // 线程回调

    // 初始化
    VOID Init(
        __in HINSTANCE hInstance,           // 应用程序实例句柄
        __in_opt HINSTANCE hPrevInstance,   // 前一个次程序的实例句柄
        __in LPTSTR lpCmdLine,              // 命令行参数
        __in INT nShowCmd,                  // 窗口显示模式
        S_2DEngineWndInfo* s_pewi
        );

    // 设置窗口快捷键表ID
    HACCEL SetAcceleratorID(DWORD dwAcceleratoID);

    DWORD GetWndThreadId(VOID);                 // 获取窗口线程Id
    VOID WaitQuit(VOID);                        // 等待结束
    LPTCHAR GetWinCommandLine(VOID);            // 获取命令行参数
    HINSTANCE GetHInstance(VOID);               // 获取实例句柄
    HINSTANCE GetPrevHInstance(VOID);           // 获取上一个实例句柄
    HWND GethWnd(VOID);                         // 获取窗口句柄

    // 直接投递消息
    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

    BOOL SetWndName(LPTSTR szWndName);          // 设置窗口名称
    BOOL SetWinShow(INT nShowCmd);              // 设置显示方式
    BOOL SetWinMenu(HMENU hMenu);               // 设置菜单
    BOOL CloseWindow(VOID);                     // 关闭窗口
    VOID SetMaximize(BOOL bEnabled);            // 设置最大化按钮
    VOID SetMinimize(BOOL bEnabled);            // 设置最小化按钮
    VOID SetSysMenu(BOOL bEnabled);             // 设置系统菜单
    VOID SetIcon(INT nIconId, BOOL bBigIcon);   // 设置图标
    VOID SetIcon(HICON hIcon, BOOL bBigIcon);   // 设置图标
    VOID SetEnabled(BOOL bEnabled);             // 是否激活
    VOID SetMinSize(POINT& Pos);                // 窗口最小尺寸
    VOID SetMaxSize(POINT& Pos);                // 窗口最大尺寸
    VOID SetLockSize(POINT& Pos);               // 窗口锁定尺寸
    VOID SetLockSize(DWORD dwWidth, DWORD dwHeight);
    VOID SetClientSize(POINT& Pos);             // 设置窗口客户区尺寸

    // 设置系统托盘图标
    VOID SetSysIcon(UINT IconID);

    // 添加系统托盘
    BOOL AddSysIcon(UINT IconID, TCHAR* szToolTip);

    // 删除系统托盘
    BOOL DelSysIcon(UINT IconID);

    // 弹出系统托盘消息
    BOOL PopSysIconMsg(UINT IconID, TCHAR* szTitle, TCHAR* szInfo);

    VOID SetIconPreCreate(DWORD dwIconID);      // 设置图标(必须在运行前调用)
    VOID SetIconPreCreate(HICON m_hIcon);       // 设置图标(必须在运行前调用)

    // 获取IAT信息
    static VOID LocationIAT(
        HMODULE hModule,
        LPTCHAR szImportMod,
        PIMAGE_IMPORT_DESCRIPTOR& lpImportDesc
        );

    // 替换IAT记录
    static BOOL HookAPIByName(
        HMODULE hModule,
        LPTCHAR szImportMod,
        HOOKAPI* pHookApi
        );

private:
    BOOL MsCreateMainWindow(VOID);      // 创建主窗口
    BOOL MsLeachMsg(VOID);              // 内部过滤消息
    HINSTANCE       m_hInstance;
    HINSTANCE       m_hPrevInstance;
    INT             m_nShowCmd;
    HWND            m_hMainWnd;
    WNDPROC         m_AWndProc;
    HOOKWNDPROC     m_HookWndProc;
    PTHREAD_START_ROUTINE m_ThreadProc;
    HICON           m_hAIcon;
    HICON           m_hSysIcon;
    TCHAR           m_szAstrWndClassName[MAX_PATH];
    TCHAR           m_szAstrWndName[MAX_PATH];
    TCHAR           m_szCommandLine[MAX_PATH];
    HACCEL          m_MsRun_hAccelTable;
    E_ERROR_CODE    m_eWinMainErrorCode;
    DWORD           m_dwMsgThreadId;
    LPVOID          m_lpCreateParameter;    // 创建窗口所传递的参数
    BOOL            m_bTopMost;
    INT             m_nX;
    INT             m_nY;
    INT             m_nWidth;
    INT             m_nHeight;
    DWORD           m_dwStyle;
    BOOL            m_bMinLockSize;
    BOOL            m_bMaxLockSize;
    POINT           m_MinSize;
    POINT           m_MaxSize;
    NOTIFYICONDATA  m_nId;
    MSG             m_Msg;
};

extern CMsWinMain* g_pCMsWinMain;


//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
