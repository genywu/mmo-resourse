#pragma once            //┃//┃
#ifndef __MSCONSOLE_H__ //┃//┃
#define __MSCONSOLE_H__ //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsConsole.h     //┃
//功能描述：哈希图相关      //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

enum E_FOREGOUND_TEXT_COLOR
{
    FC_BLACK        = 0,                        // 黑色前景

    FC_WHITE        =                           // 白色标准前景
    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED,

    FC_BLUE         = FOREGROUND_BLUE,          // 蓝色前景
    FC_GREEN        = FOREGROUND_GREEN,         // 绿色前景
    FC_RED          = FOREGROUND_RED,           // 红色前景

    FC_BLUE_GREEN   =                           // 蓝绿前景
    FOREGROUND_BLUE|FOREGROUND_GREEN,

    FC_BLUE_RED     =                           // 蓝红前景
    FOREGROUND_BLUE|FOREGROUND_RED,

    FC_GREEN_RED    =                           // 绿红前景
    FOREGROUND_GREEN|FOREGROUND_RED,

    CL_RV           = COMMON_LVB_REVERSE_VIDEO, // 反色前景
    E_FOREGOUND_TEXT_COLOR_TO_DWORD = 0X7FFF
};

enum E_BACKGOUND_TEXT_COLOR
{
    BC_BLACK        = 0,                // 黑色标准背景

    BC_WHITE        =                   // 白色背景
    BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED,  

    BC_BLUE         = BACKGROUND_BLUE,  // 蓝色背景
    BC_GREEN        = BACKGROUND_GREEN, // 绿色背景
    BC_RED          = BACKGROUND_RED,   // 红色背景

    BC_BLUE_GREEN   =                   // 蓝绿背景
    BACKGROUND_BLUE|BACKGROUND_GREEN,

    BC_BLUE_RED     =                   // 蓝红背景
    BACKGROUND_BLUE|BACKGROUND_RED,

    BC_GREEN_RED    =                   // 绿红背景
    BACKGROUND_GREEN|BACKGROUND_RED,

    E_BACKGOUND_TEXT_COLOR_TO_DWORD = 0X7FFF
};
class CMsConsole
{
public:
    CMsConsole(RUNCALLBACK pRunCallBack);
    virtual ~CMsConsole(VOID);
public:

    // 初始化
    BOOL Init(INT nArgc, LPTCHAR* strArgv);

    // 释放
    BOOL Release(VOID);

    // 运行
    BOOL Run(VOID);

    // 设置控制台尺寸
    BOOL SetConsoleSize(DWORD dwSize);

    // 获取控制台文本颜色
    DWORD GetSithColor(VOID);

    // 设置控制台标题
    BOOL SetConsoleTitle(LPTCHAR szTitle);

    // 设置下文文本颜色
    BOOL SetSithTextColor(
        E_FOREGOUND_TEXT_COLOR fColor,
        E_BACKGOUND_TEXT_COLOR bColor,
        BOOL bfIntensity = FALSE,
        BOOL bbIntensity = FALSE
        );

    // 设置控制台场景颜色
    BOOL SetSceneColor(
        E_BACKGOUND_TEXT_COLOR bColor,
        BOOL bbIntensity = FALSE
        );

    //COORD GetCursorPosition(VOID);                      // 获取控制台光标位置
    //BOOL  SetCursorPosition(COORD dwCursorPosition);    // 设置控制台光标位置

    //VOID OutText(LPTCHAR strText, DWORD dwLength, COORD dwCoord);
    //VOID OutTextW(LPWCHAR strText, DWORD dwLength, COORD dwCoord);
    //VOID OutTextA(LPCHAR strText, DWORD dwLength, COORD dwCoord);

private:
    RUNCALLBACK m_pRunCallBack;
    INT         m_nArgc;
    LPTCHAR*    m_strArgv;
    HANDLE      m_hConsoleOut;
    HANDLE      m_hConsoleErr;
    HANDLE      m_hConsoleIn;
    CONSOLE_SCREEN_BUFFER_INFO m_csbi;
    CONSOLE_FONT_INFOEX m_cfi;
    LPWORD      m_lpwAttributesBuff;
    size_t      m_sScreenBuffSize;
};

extern CMsConsole* g_pCMsConsole;

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
