#pragma once            //┃//┃
#ifndef __MS2DENGINE_H__//┃//┃
#define __MS2DENGINE_H__//┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：Ms2DEngine.h    //┃
//功能描述：2D游戏引擎框架  //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

class CMs2DEngine
{

// 构造、析构
public:
    CMs2DEngine(VOID);
    ~CMs2DEngine(VOID);
// 接口
public:

    static VOID                     s_Release(VOID);          // 释放
    static LPDIRECT3D9              s_GetDirect3D9(VOID);     // 获取D3D9对象
    static LPDIRECT3DDEVICE9        s_GetDirectDevice9(VOID); // 获取D3D9设备
    static D3DPRESENT_PARAMETERS*   s_GetDirect3DPP(VOID);    // 获取D3D9参数

    // 创建D3D9
    static BOOL s_CreateD3D9(HWND hWnd, DWORD dwScreenWidth, DWORD dwScreenHeight);

    // 删除D3D9
    static VOID s_DeleteD3D9(VOID);

    // 创建D3D9字体
    static ID3DXFont* s_CreateD3D9Font(
        DWORD dwSize,                                           // 尺寸
        DWORD dwWeight = FW_BOLD,                               // 字重
        DWORD dwMipLevels = 1,                                  // Mip等级
        BOOL bItalic = FALSE,                                   // 是否斜体
        LPCTSTR szFaceName = _T("宋体"),                        // 字体名称
        DWORD dwCharSet = DEFAULT_CHARSET,                      // 字符集
        DWORD dwOutputPrecision = OUT_DEFAULT_PRECIS,           // 输出精度
        DWORD dwQuality = DEFAULT_QUALITY,                      // 品质
        DWORD dwPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE    // 角度
        );

    // 设置Ms2DEngine属性
    static VOID s_Ms2DEngine_SetState(
        E_MS2DENGINE_STATE_BOOL eState,
        BOOL bValue
        );

    static VOID s_Ms2DEngine_SetState(
        E_MS2DENGINE_STATE_FUNC eState,
        Ms2DCBFun funValue
        );

    static VOID s_Ms2DEngine_SetState(
        E_MS2DENGINE_STATE_HWND eState,
        HWND hValue
        );

    static VOID s_Ms2DEngine_SetState(
        E_MS2DENGINE_STATE_INT eState,
        INT nValue
        );

    static VOID s_Ms2DEngine_SetState(
        E_MS2DENGINE_STATE_STRING eState,
        CONST LPTCHAR szValue
        );

    // 获取Ms2DEngine属性
    static BOOL             s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_BOOL);
    static Ms2DCBFun        s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_FUNC);
    static HWND             s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_HWND);
    static INT              s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_INT);
    static CONST LPTCHAR    s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_STRING);

    static CMsIni*  s_GetIni(VOID){return m_pMsIni;}// 获取配置管理器
    static BOOL     s_Log(TCHAR* szText);           // 记录日志
    static BOOL     s_Logf(TCHAR* szText, ...);     // 记录日志
    static VOID     s_EnableLog(BOOL bEnable);      // 开关日志
    static DWORD    s_GetKeyFlag(VOID);             // 获取按键标记

// 私有
private:
    static LPDIRECT3D9              m_lpDirect9;        // D3D9对象指针
    static LPDIRECT3DDEVICE9        m_lpDirectDevice9;  // D3D9设备指针
    static D3DPRESENT_PARAMETERS    m_d3dPP;            // 重置参数
    static BOOL         m_bInit;        // 是否初始化
    static HWND         m_hWnd;         // 窗口句柄
    static CMsIni*      m_pMsIni;       // 配置文件
    static CMsLog*      m_pMsLog;       // 日志管理
    static LONG         m_lScreenWidth; // 窗口宽度
    static LONG         m_lScreenHeight;// 窗口高度
    static BOOL         m_bActive;      // 是否被激活
    static BOOL         m_bHideCursor;  // 是否隐藏光标
    static MsLock       m_Lock;         // 操作锁
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
