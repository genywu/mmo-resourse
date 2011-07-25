#pragma once            //┃//┃
#ifndef __MS2DINPUT_H__ //┃//┃
#define __MS2DINPUT_H__ //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：Ms2DInput.h     //┃
//功能描述：2D引擎输入模块  //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

class CMs2DInput
{
public:
    // 英文按键名称
    static LPTCHAR s_pszKeyNames_En[MS2D_INPUT_MAX_KEY_COUNT];

    // 中文按键名称
    static LPTCHAR s_pszKeyNames_Cn[MS2D_INPUT_MAX_KEY_COUNT];

    // 构造、析构
    CMs2DInput(
        HINSTANCE hInstance, HWND* phWnd,
        LPLONG lplScreenWidth, LPLONG lplScreenHeight);

    ~CMs2DInput(VOID);

    // 获取鼠标坐标
    VOID MSCALL Ms2DInput_GetMousePos(LPLONG lpX, LPLONG lpY);

    // 设置鼠标坐标
    VOID MSCALL Ms2DInput_SetMousePos(LONG lX, LONG lY);

    // 获取鼠标滚轮值
    INT MSCALL Ms2DInput_GetMouseWheel(VOID);

    // 判断鼠标是否穿过
    BOOL MSCALL Ms2DInput_IsMouseOver(VOID);

    // 实时判断按键是否按下
    BOOL MSCALL Ms2DInput_IsKeyDownRealTime(INT nKey);

    // 判断指定按键是否有按下事件发生
    BOOL MSCALL Ms2DInput_IsKeyDownEvent(INT nKey);

    // 判断指定按键是否有松开事件发生
    BOOL MSCALL Ms2DInput_IsKeyUpEvent(INT nKey);

    // 获取指定按键的名称
    TCHAR* MSCALL Ms2DInput_GetKeyName(INT nKey, DWORD& dwNameLen, BOOL bChineseName = FALSE);

    // 获取有事件发生的按键虚码
    DWORD MSCALL Ms2DInput_GetKey(VOID);

    // 获取有事件发生的按键字符码
    DWORD MSCALL Ms2DInput_GetChar(VOID);

    // 获取输入事件
    BOOL MSCALL Ms2DInput_GetEvent(CMs2DInputEvent* pEvent);

    // 清除下一个输入事件
    BOOL MSCALL Ms2DInput_ClearNextEvent(VOID);

    // 清除所有输入事件
    BOOL MSCALL Ms2DInput_ClearAllEvent(VOID);

    // 建立事件
    VOID MSCALL Ms2DInput_BuildEvent(
        E_MS2D_INPUT_TYPE dwType,
        DWORD dwKey, SHORT snScan, 
        DWORD dwFlags,
        LONG lX, LONG lY
        );

private:

    HINSTANCE           m_hInstance;        // 程序实例句柄
    HWND*               m_phWnd;            // 窗口句柄
    LPLONG              m_lplScreenWidth;   // 窗口宽度
    LPLONG              m_lplScreenHeight;  // 窗口高度

    // Input
    DWORD               m_dwvKey;               // 按键虚码
    DWORD               m_dwChar;               // 字符码
    DWORD               m_dwFlags;              // 事件标记
    LONG                m_lZpos;                // 鼠标滚轮值
    LONG                m_lXpos;                // 鼠标x坐标
    LONG                m_lYpos;                // 鼠标y坐标
    BOOL                m_bCaptured;            // 是否被捕获
    BOOL                m_bMouseOver;           // 鼠标是否穿过
    Ms2DInputEventQueue m_Ms2DInputEventQueue;  // 输入事件队列
    MsLock              m_InputLock;            // 访问锁

    // 按键状态数组
    BYTE                m_keyz[MS2D_INPUT_MAX_KEY_COUNT*sizeof(TCHAR)];
};


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
