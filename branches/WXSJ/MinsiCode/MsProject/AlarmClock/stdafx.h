// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT



// 发生托盘消息ID
#define MS_WM_NOTIFYICON WM_USER + 0x0101

// 宏定义无效ID
#define INVALID_ID -1

// 定义单项闹钟记录个数
#define MAX_CLOCK_COUNT 100

// 闹钟类型定义
enum E_CLOCK_TYPE
{
    E_CLOCK_TYPE_UNKNOW = -1,   // 未知类型
    E_CLOCK_TYPE_ONCE,          // 仅一次
    E_CLOCK_TYPE_EVERYDAY,      // 在每天
    E_CLOCK_TYPE_ONLY_WEEK_0,   // 仅周日
    E_CLOCK_TYPE_ONLY_WEEK_1,   // 仅周一
    E_CLOCK_TYPE_ONLY_WEEK_2,   // 仅周二
    E_CLOCK_TYPE_ONLY_WEEK_3,   // 仅周三
    E_CLOCK_TYPE_ONLY_WEEK_4,   // 仅周四
    E_CLOCK_TYPE_ONLY_WEEK_5,   // 仅周五
    E_CLOCK_TYPE_ONLY_WEEK_6,   // 仅周六
    E_CLOCK_TYPE_MAX_OF_DWORD = 0x7FFFFFFF
};

// 提示类型定义
enum E_CLUE_TYPE
{
    E_CLUE_TYPE_NORMAL = 0, // 标准类型 ->  仅提示一次,收到与否忽略!
    E_CLUE_TYPE_ALONG,      // 一直类型 ->  一直提示,直到点击停闹!
    E_CLUE_TYPE_INTERVAL,   // 间隔类型 ->  间隔提示,每隔一段时间提示,直到点击停闹或达到一定次数!
    E_CLUE_TYPE_MAX_OF_DWORD = 0x7FFFFFFF
};

// 闹钟记录结构体
struct S_CLOCK_REGISTER
{
    S_CLOCK_REGISTER(VOID)
        : m_ClockType   (E_CLOCK_TYPE_ONCE)
        , m_ClueType    (E_CLUE_TYPE_NORMAL)
        , m_ClueLong    (10000)
        , m_IntervalLong(INVALID_ID)
        , m_MaxClueTime (INVALID_ID)
        , m_IsRuing     (FALSE)
    {
        ZeroMemory(&m_szClueOnText, sizeof(m_szClueOnText));
        ZeroMemory(&m_ClueOnTime, sizeof(m_ClueOnTime));
    }
    E_CLOCK_TYPE    m_ClockType;        // 闹钟类型
    TCHAR           m_szClueOnText[128];// 闹钟提示内容
    SYSTEMTIME      m_ClueOnTime;       // 提示时间
    E_CLUE_TYPE     m_ClueType;         // 提示类型
    UINT            m_ClueLong;         // 提示多久长度(毫秒)
    UINT            m_IntervalLong;     // 间隔多久长度(毫秒)
    UINT            m_MaxClueTime;      // 间隔提示次数
    BOOL            m_IsRuing;          // 正在闹铃
};






