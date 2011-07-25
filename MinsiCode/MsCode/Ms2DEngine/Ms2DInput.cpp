#include "Precompiled.h"

// 英文按键名称
LPTCHAR CMs2DInput::s_pszKeyNames_En[MS2D_INPUT_MAX_KEY_COUNT] =
{
    _T("Enlarge Mouse Button"),
    _T("Left Mouse Button"), _T("Right Mouse Button"), 
    _T("?"), _T("Middle Mouse Button"),
    _T("?"), _T("?"), _T("?"), _T("Backspace"), _T("Tab"), 
    _T("?"), _T("?"), _T("?"), _T("Enter"), _T("?"), _T("?"),
    _T("Shift"), _T("Ctrl"), _T("Alt"), _T("Pause"), _T("Caps Lock"), 
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Escape"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Space"), _T("Page Up"), _T("Page Down"), _T("End"), _T("Home"),
    _T("Left Arrow"), _T("Up Arrow"), _T("Right Arrow"), _T("Down Arrow"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("Insert"), _T("Delete"), _T("?"),
    _T("0"), _T("1"), _T("2"), _T("3"), _T("4"), 
    _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"),
    _T("G"), _T("H"), _T("I"), _T("J"), _T("K"), _T("L"),
    _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), 
    _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z"),
    _T("Left Win"), _T("Right Win"), _T("Application"), _T("?"), _T("?"),
    _T("NumPad 0"), _T("NumPad 1"), _T("NumPad 2"), _T("NumPad 3"), 
    _T("NumPad 4"), _T("NumPad 5"), _T("NumPad 6"), _T("NumPad 7"),
    _T("NumPad 8"), _T("NumPad 9"), _T("Multiply"), _T("Add"), _T("?"),
    _T("Subtract"), _T("Decimal"), _T("Divide"),
    _T("F1"), _T("F2"), _T("F3"), _T("F4"), _T("F5"),_T("F6"),
    _T("F7"), _T("F8"), _T("F9"), _T("F10"), _T("F11"), _T("F12"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Num Lock"), _T("Scroll Lock"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Semicolon"), _T("Equals"), _T("Comma"),
    _T("Minus"), _T("Period"), _T("Slash"), _T("Grave"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Left bracket"), _T("Backslash"), _T("Right bracket"), _T("Apostrophe"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?")
};

// 中文按键名称
LPTCHAR CMs2DInput::s_pszKeyNames_Cn[MS2D_INPUT_MAX_KEY_COUNT] =
{
    _T("鼠标扩展"),
    _T("鼠标左键"), _T("鼠标右键"), _T("未知"), _T("鼠标中键"),
    _T("未知"), _T("未知"), _T("未知"), _T("退格键"), _T("制表键"),
    _T("未知"), _T("未知"), _T("未知"), _T("回车键"), _T("未知"), _T("未知"),
    _T("上档键"), _T("控制键"), _T("功能键"), _T("暂停键"), _T("大小写锁定键"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("退出键"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("空格键"), _T("上翻页键"), _T("下翻页键"), _T("结尾键"), _T("起始键"),
    _T("左光标键"), _T("上光标键"), _T("右光标键"), _T("下光标键"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("插入键"), _T("删除键"),
    _T("未知"),
    _T("0"), _T("1"), _T("2"), _T("3"), _T("4"),
    _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"),
    _T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"),_T("H"),
    _T("I"), _T("J"), _T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"),
    _T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"),
    _T("Y"), _T("Z"),
    _T("左窗口键"), _T("右窗口键"), _T("应用键"), _T("未知"), _T("未知"),
    _T("数字0"), _T("数字1"), _T("数字2"), _T("数字3"), _T("数字4"),
    _T("数字5"), _T("数字6"), _T("数字7"), _T("数字8"), _T("数字9"),
    _T("乘法键"), _T("加法键"), _T("未知"), _T("减法键"), _T("回车键(小区)"),
    _T("除法键"),
    _T("F1"), _T("F2"), _T("F3"), _T("F4"), _T("F5"), _T("F6"),
    _T("F7"), _T("F8"), _T("F9"), _T("F10"), _T("F11"), _T("F12"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("数字区锁定键"), _T("卷轴锁定键"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("分号键"), _T("等号键"), _T("逗号键"), _T("减号键"), _T("句号键"),
    _T("斜杠键)"), _T("波浪号键"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("左括号键"), _T("反斜杠键"), _T("右括号键"), _T("省略号键"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知"), _T("未知"),
    _T("未知"), _T("未知"), _T("未知")
};


// 构造
CMs2DInput::CMs2DInput(
    HINSTANCE hInstance,
    HWND* phWnd,
    LPLONG lplScreenWidth,
    LPLONG lplScreenHeight
    )
: m_hInstance       (hInstance)
, m_phWnd           (phWnd)
, m_lplScreenWidth  (lplScreenWidth)
, m_lplScreenHeight (lplScreenHeight)
, m_dwvKey          (INVALID_UID)
, m_dwChar          (INVALID_UID)
, m_dwFlags         (INVALID_UID)
, m_lZpos           (0)
, m_lXpos           (0)
, m_lYpos           (0)
, m_bCaptured       (FALSE)
, m_bMouseOver      (FALSE)
{
    ZeroMemoryArray(m_keyz);
}

// 析构
CMs2DInput::~CMs2DInput(VOID)
{
}

// 获取鼠标坐标
VOID MSCALL CMs2DInput::Ms2DInput_GetMousePos(LPLONG lpX, LPLONG lpY)
{
    *lpX = m_lXpos;
    *lpY = m_lYpos;
}

// 设置鼠标坐标
VOID MSCALL CMs2DInput::Ms2DInput_SetMousePos(LONG lX, LONG lY)
{
    POINT pt;
    pt.x = lX;
    pt.y = lY;
    if (m_phWnd)
    {
        ::ClientToScreen(*m_phWnd, &pt);  // 客户坐标转换到场景坐标
    }
    else
    {
        AssertEx(AL_NORMAL, m_phWnd, _T("Ms2DInput_SetMousePos->窗口句柄为空"));
    }
    ::SetCursorPos(pt.x, pt.y);     // 设置光标位置
}

// 获取鼠标滚轮值
INT MSCALL CMs2DInput::Ms2DInput_GetMouseWheel(VOID)
{
    return m_lZpos;
}

// 判断鼠标是否穿过
BOOL MSCALL CMs2DInput::Ms2DInput_IsMouseOver(VOID)
{
    POINT   pt;
    RECT    rc;

    ::GetCursorPos(&pt);            // 获取光标坐标
    ::GetClientRect(*m_phWnd, &rc);   // 获取客户区域矩形范围

    ::MapWindowPoints(              // 将窗口坐标点转换成桌面坐标点
        *m_phWnd,
        NULL,
        (LPPOINT)&rc,
        sizeof(RECT)/sizeof(POINT)
        );

    // 窗口是否被捕获
    if (m_bCaptured)
    {
        // 点是否在客户区域矩形内,同时是否在窗口区域内
        if (::PtInRect(&rc, pt) && (::WindowFromPoint(pt) == *m_phWnd))
        {
            m_bMouseOver = TRUE;
        }
    }
    m_bMouseOver = FALSE;
    return m_bMouseOver;
}

// 实时判断按键是否按下
BOOL MSCALL CMs2DInput::Ms2DInput_IsKeyDownRealTime(INT nKey)
{
    return ((::GetKeyState(nKey) & 0x8000) != 0);
}

// 判断指定按键是否有按下事件发生
BOOL MSCALL CMs2DInput::Ms2DInput_IsKeyDownEvent(INT nKey)
{
    return (m_keyz[nKey] & 1) != 0;
}

// 判断指定按键是否有松开事件发生
BOOL MSCALL CMs2DInput::Ms2DInput_IsKeyUpEvent(INT nKey)
{
    return (m_keyz[nKey] & 2) != 0;
}

// 获取指定按键的名称
TCHAR* MSCALL CMs2DInput::Ms2DInput_GetKeyName(INT nKey, DWORD& dwNameLen, BOOL bChineseName)
{
    if (bChineseName)
    {
        dwNameLen = (DWORD)_tcslen(s_pszKeyNames_Cn[nKey]);
        return s_pszKeyNames_Cn[nKey];
    }
    else
    {
        dwNameLen = (DWORD)_tcslen(s_pszKeyNames_Cn[nKey]);
        return s_pszKeyNames_En[nKey];
    }
}

// 获取有事件发生的按键
DWORD MSCALL CMs2DInput::Ms2DInput_GetKey(VOID)
{
    return m_dwvKey;
}

// 获取有事件发生的按键字符码
DWORD MSCALL CMs2DInput::Ms2DInput_GetChar(VOID)
{
    return m_dwChar;
}

// 获取输入事件
BOOL MSCALL CMs2DInput::Ms2DInput_GetEvent(CMs2DInputEvent* pEvent)
{
    AUTO_LOCK(m_InputLock);
    if (m_Ms2DInputEventQueue.m_CMsQueueManager.GetCurrentCount() > 0)
    {
        DWORD dwSize;
        ULONG_PTR ulIndex = (ULONG_PTR)(m_Ms2DInputEventQueue.m_CMsQueueManager.GetData(dwSize));
        m_Ms2DInputEventQueue.m_CMsQueueManager.DeleteData();
        memcpy(pEvent, &m_Ms2DInputEventQueue.m_pInputEventQueue[ulIndex], sizeof(*pEvent));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// 清除下一个输入事件
BOOL MSCALL CMs2DInput::Ms2DInput_ClearNextEvent(VOID)
{
    if (m_Ms2DInputEventQueue.m_CMsQueueManager.GetCurrentCount() > 0)
    {
        m_Ms2DInputEventQueue.m_CMsQueueManager.DeleteData();
    }
    return TRUE;
}

// 清除所有输入事件
BOOL MSCALL CMs2DInput::Ms2DInput_ClearAllEvent(VOID)
{
    ZeroMemoryArray(m_keyz);

    m_dwvKey=0; m_dwChar=0; m_lZpos=0;

    return m_Ms2DInputEventQueue.m_CMsQueueManager.ClearAll();
}

// 建立事件
VOID MSCALL CMs2DInput:: Ms2DInput_BuildEvent(
    E_MS2D_INPUT_TYPE eType,
    DWORD dwKey, SHORT snScan, 
    DWORD eFlags,
    LONG lX, LONG lY
    )
{
    _ENTER_FUN_C;

    AUTO_LOCK(m_InputLock);
    // 获取一个空闲事件
    CMs2DInputEvent* pEvent = m_Ms2DInputEventQueue.GetNextFreeEvent();

    // 获取键盘状态
    BYTE KeyboardState[MS2D_INPUT_MAX_KEY_COUNT];
    if (!::GetKeyboardState(KeyboardState))
    {
        AssertEx(AL_NORMAL, FALSE, _T("GetKeyboardState失败"));
    }

    // 记录光标位置
    POINT pt = {lX, lY};
    //m_lXpos = lX;
    //m_lYpos = lY;
    //if (INVALID_NID == pt.x)
    //{
    //    pEvent->lX = m_lXpos;
    //    pEvent->lY = m_lYpos;
    //}
 /*   else
    {
        if(pt.x < 0)
        {
            pt.x = 0;
        }

        if(pt.y < 0)
        {
            pt.y = 0;
        }

        if(pt.x >= *m_lplScreenWidth)
        {
            pt.x = *m_lplScreenWidth - 1;
        }

        if(pt.y >= *m_lplScreenHeight)
        {
            pt.y = *m_lplScreenHeight - 1;
        }

        pEvent->lX = pt.x;
        pEvent->lY = pt.y;
    }*/

    pEvent->eType   = eType;
    pEvent->wChar   = 0;
    pEvent->dwKey   = dwKey;
    pEvent->nWheel  = 0;
    pEvent->dwFlags = INPUT_FLAG_NO_FLAG;

    // 记录事件
    switch (eType)
    {
    case INPUT_KEYDOWN:
        {
            if((eFlags & INPUT_FLAG_REPEAT) == 0)
            {
                m_keyz[dwKey] |= 1;
            }
            ::ToAscii(dwKey, snScan, KeyboardState, &pEvent->wChar, 0);
            m_dwvKey = dwKey;
            m_dwChar = pEvent->wChar;
        }break;

    case INPUT_KEYUP:
        {
            m_keyz[dwKey] |= 2;
            ::ToAscii(dwKey, snScan, KeyboardState, &pEvent->wChar, 0);
        }break;

    case INPUT_MBUTTONDOWN:
        {
            m_keyz[dwKey] |= 1;

            // 设置窗口被捕获
            ::SetCapture(*m_phWnd);
            m_bCaptured = TRUE;

            m_dwvKey = dwKey;
            m_dwChar = pEvent->wChar;
        }break;

    case INPUT_MBUTTONUP:
        {
            m_keyz[dwKey] |= 2;

            // 设置窗口未捕获
            ::ReleaseCapture();
            m_bCaptured = FALSE;

            //// 更新光标位置
            //this->Ms2DInput_SetMousePos(m_lXpos, m_lYpos);
            //pt.x = m_lXpos;
            //pt.y = m_lYpos;
        }break;

    case INPUT_MOUSEMOVE:
        {
            m_lXpos = pEvent->lX;
            m_lYpos = pEvent->lY;
        }break;

    case INPUT_MOUSEWHEEL:
        {
            pEvent->dwKey = 0;
            pEvent->nWheel = dwKey;
            ::ScreenToClient(*m_phWnd, &pt);
            m_lZpos += pEvent->nWheel;
        }break;

    case INPUT_UNKNOW:
    default:
        {
            ;
        }break;
    }

    // 记录事件标记
    if(KeyboardState[VK_SHIFT]  & 0x80) pEvent->dwFlags |= INPUT_FLAG_SHIFT;
    if(KeyboardState[VK_CONTROL]& 0x80) pEvent->dwFlags |= INPUT_FLAG_CTRL;
    if(KeyboardState[VK_MENU]   & 0x80) pEvent->dwFlags |= INPUT_FLAG_ALT;
    if(KeyboardState[VK_CAPITAL]& 0x1)  pEvent->dwFlags |= INPUT_FLAG_CAPSLOCK;
    if(KeyboardState[VK_SCROLL] & 0x1)  pEvent->dwFlags |= INPUT_FLAG_SCROLLLOCK;
    if(KeyboardState[VK_NUMLOCK]& 0x1)  pEvent->dwFlags |= INPUT_FLAG_NUMLOCK;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}