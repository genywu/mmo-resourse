/**
* @file AppFrameDef.h
* 
* 使用该模块编写服务器或其它工具, 对于 Win32App 所作改动比较小. 主要集中在以下
* 几点 (可参看s/StringMan):
* 
* * 由于使用多线程, 需修改 Windows 消息循环;
* * 引擎提供了 debug 模块;
* * 主窗口样式使用 WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX;
* * 自定义控件消息处理 (需要在 WndProc 中添加):
*       if (AppFrame::MsgProc((HWND)lParam, wmId, wmEvent))
*           return 0;
* * 处理 WM_CREATE 消息 (需要在 WndProc 中添加) 时, 加载自定义配置资源 (含启动逻辑主线程):
*       InitConfig(hWnd);
* * 处理 WM_QUIT 消息 (需要主消息循环中添加) 时, 释放自定义配置资源 (含退出逻辑主线程):
*       FiniConfig();
* * 用于日志输出的窗口名必须配置为"log", 用于监控状况的输出窗口推荐配置为"info";
*
* 设计的初衷是将model, view 与 contrl 分开, 由于配置内容可直接应用于 Win32API,
* 该配置结构中的部分内容(如窗口布局)非为必需. 保留的考量主要是为集中说明作用.
* 
* 由于列表内容, 日志信息比较繁杂, 在重读窗口UI布局时上述信息不会重新加载. 故
* release 版本中重读窗口UI布局的菜单 '视图' 不可用.
* @author Fox (xiangyunfeng@svn.com)
* @date Mar.15, 2010
*/

#pragma once

//#include "Log.h"
#include "TextRes.h"
#include "../DefType.h"

namespace AppFrame
{
    const char* const LIST = "list";
    const char* const INFO = "info";
    const char* const LOG = "log";
    // 此代码模块中包含的函数的前向声明:
    BOOL                InitApp(HINSTANCE, int, unsigned (__stdcall *)(void*));
    /*ATOM				MyRegisterClass(HINSTANCE hInstance);
    BOOL				InitInstance(HINSTANCE, int);
    LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);*/

    /**
    * 菜单ID(16-bit)设计原则:
    * IDM = MY_MENUS | PARENT_INDEX << 5 | ITEM_INDEX
    * 所有自定义菜单起始ID为 MY_MENU, PARENT_INDEX (zero-based),
    * ITEM_INDEX (zero-based). 为实现跨线程处理, 以不同位标识菜单及
    * 菜单项, 因此最多允许 31 项下拉菜单 (id0), 每个下拉菜单最多拥
    * 有 32 个菜单项 (id1).
    *
    * 警告:
    * id0 == 1 的下拉菜单对应'视图'菜单, 必须保留.
    */

    /// 根据配置文件生成菜单 ID
    /// @param id0 下拉菜单 ID
    /// @param id1 菜单项 ID
#define GenMenuId(id0, id1) ((((id0) << 5)) | (id1))

    /// 标识菜单最小 ID
#define MY_MENU 0x0020

    /// 根据点选菜单项确认下拉菜单 ID
    /// 注意: 自定义下拉菜单 ID 从 2 开始, 此处获取 ID 从 0 开始,
    /// 以保证与 menuOps 下标对应
#define GetPopMenuId(id) (((id) >> 5) - 2)

    /// 根据点选菜单确认菜单项 ID
    /// 注意: 此 ID 对应自定义菜单枚举 (每个枚举值独占 1 位)
#define GetMenuItemId(id) (0x0001 << ((id) & 0x001F))

    /**
    * 确定UI样式的'视图'菜单仅当调试时可用
    */
    struct Menu
    {
        /// 菜单句柄
        HMENU handle;

        /// 菜单配置文件名称
        string file;
    };

    /**
    * 窗口布局没有top-left属性, 免去配置时的麻烦, 只根据窗口相对前一窗口位置布局.
    * 服务器窗口布局一般为: 左上, 左下, 右.
    */
    struct Layouts
    {
        struct Window
        {
            string name;
            HWND handle;
            bool callback;
            bool (*func)(HWND, int);
            int x;
            int y;
            int width;
            int height;
        };

        /// 窗口布局配置文件名称
        string file;

        /// 窗口集合, first: 窗口名字, second: 窗口句柄
        map<string, Window> windows;
    };

    typedef map<string, Layouts::Window>::iterator WndItr;

    /**
    * 用于线程控制, 目前只支持一个线程 (与应用程序关联的逻辑主线程)
    */
    struct Thread
    {
        HANDLE handle;                      ///< 线程句柄
        HANDLE event;                       ///< 线程事件
        unsigned id;                        ///< 逻辑线程ID
        bool exit;                          ///< 线程退出
    };

    struct Configure
    {
        Thread thread;                      ///< 与应用程序关联的逻辑线程
        HWND wnd;                           ///< 应用程序窗口句柄
        Menu menu;                          ///< 应用程序菜单
        HFONT font;                         ///< 子窗口字体(建议使用等宽字体)
        string log;                         ///< 应用程序日志文件
        int frameWidth;
        int frameHeight;
        Layouts layouts;                    ///< 应用程序窗口布局
    };

    /**
    * 初始化配置资源并注册逻辑主线程函数
    *  @param wnd 应用程序主窗口句柄
    *  @param func 原型: unsigned __stdcall MainThreadFunc(void* args);
    */
    bool InitConfig(HWND wnd);

    /**
    * 逻辑主线程退出, 释放配置资源
    */ 
    void FiniConfig();

    /**
    * 加载所有配置资源 (布局, 日志, 菜单等)
    * @param sync 是否同步加载
    */ 
    static bool LoadAll(bool sync = true);

    /**
    * 加载菜单 (由于菜单处理函数与菜单ID的关联, 暂不支持热加载)
    */
    static bool LoadMenus(bool sync = true);

    /// 禁用/启用菜单
    /// @params id 下拉菜单ID, 为 -1 则禁用所有菜单
    /// @return 对应菜单当前状态: true -- 启用; false -- 禁用
    bool ToggleMenus(int id = -1);

    /**
    * 加载窗口布局
    */
    static bool LoadLayout(bool sync = true);

    /**
    * 设置窗口标题
    * @param check 检查同名应用是否已存在, 若存在, 则退出
    */
    bool SetTitle(const string& title, bool check = true);
    const char* GetTitle(char* title);

    /**
    * 设置日志文件
    */
   // static bool SetLog(LogLevel level);

    /// 设置字体
    static bool SetFont(const string& name = "");

    /// 设置图标
    static bool SetIcon(const string& bi, const string& si);

    /**
    * WM_COMMAND消息处理, 含菜单响应及窗口控件响应.
    * 通过 RegisterMenuProc 注册菜单自定义处理函数 [event == 0];
    * 通过 RegisterWndProc 注册窗口自定义处理函数 [id == 0].
    */
    bool MsgProc(HWND handle, int id, int event);

    /// 获取给定名称的窗口句柄, 默认或name为空则返回主窗口句柄
    HWND GetWnd(const string& name = "");

    /**
    * 获取给定句柄的窗口对象
    */
    Layouts::Window* GetWnd(HWND handle);

    bool IsCallback(HWND handle);

    /**
    * 注册窗口自定义处理函数.
    * @param func 类似于MsgProc
    */
    void RegisterWndProc(const string& name, bool (*func)(HWND, int));

    //void ShowLogs(const string& log);
    void ResetList(const string& name);
    void AddToList(const string& name, const string& item);
    void DelFromList(const string& name, const string& item);
    void SetEdit(const string& name, const string& text);

    //Log& GetLog();

    void Exit();
    bool IsExit();

    /**
    * 获取主窗口的尺寸
    */
    const int GetFrameWidth();
    const int GetFrameHeight();

    /// 仅被线程函数调用
    void ExitThread();

    /**
    * 为了能够输出出错代码位置, 以宏的形式封装ErrorInfo.
    */
#define ERRFMT  "%s@%s(%d) : %s"
#define ERRARGS __FUNCTION__, __FILE__, __LINE__

// #define AddErrLog(ID_FMT, ...) do {                                     \
//     const int maxLength = 4096;                                         \
//     char info[maxLength];                                               \
//     sprintf(info, AppFrame::GetText(ID_FMT),##__VA_ARGS__);             \
//     AppFrame::AddLog(AppFrame::Error, ERRFMT, ERRARGS, info); } while (0)
// 
//     void AddWarnLog(const char* id, ...);
//     void AddStdLog(const char* id, ...);
//     void AddInfoLog(const char* id, ...);
//     void AddDebugLog(const char* id, ...);
//     void AddLog(LogLevel type, const char* fmt, ...);

    /**
    * 弹出MessageBox
    */
    int NotifyMessageBox(const char* msg,
        const char* title = "INFO",
        uint flag = MB_OK);

    /// 根据点选菜单项确认菜单操作
    void SetMenuOp(uint id);

    /// 判断菜单项是否需要处理 (被点击)
    bool MenuClicked(uint id);

    /// 获取当前所有菜单操作内容
    void GetMenuOps(uint* ops);
}; // namespace AppFrame