#include "Precompiled.h"

LPDIRECT3D9 CMs2DEngine::m_lpDirect9                = NULL; // D3D9对象指针
LPDIRECT3DDEVICE9 CMs2DEngine::m_lpDirectDevice9    = NULL; // D3D9设备指针
D3DPRESENT_PARAMETERS CMs2DEngine::m_d3dPP          = {0};  // 重置参数
BOOL CMs2DEngine::m_bInit                           = FALSE;// 是否初始化
HWND CMs2DEngine::m_hWnd                            = NULL; // 窗口句柄
CMsIni* CMs2DEngine::m_pMsIni                       = NULL; // 配置文件
CMsLog* CMs2DEngine::m_pMsLog                       = NULL; // 日志管理
LONG CMs2DEngine::m_lScreenWidth                    = 0;    // 窗口宽度
LONG CMs2DEngine::m_lScreenHeight                   = 0;    // 窗口高度
BOOL CMs2DEngine::m_bActive                         = FALSE;// 是否被激活
BOOL CMs2DEngine::m_bHideCursor                     = FALSE;// 是否隐藏光标
MsLock CMs2DEngine::m_Lock;                                 // 操作锁

LPDIRECT3D9 CMs2DEngine::s_GetDirect3D9(VOID)
{
    AUTO_LOCK(m_Lock);
    AssertEx(AL_NORMAL, m_lpDirect9, _T("危险操作"));
    return m_lpDirect9;
}

LPDIRECT3DDEVICE9 CMs2DEngine::s_GetDirectDevice9(VOID)
{
    AUTO_LOCK(m_Lock);
    AssertEx(AL_NORMAL, m_lpDirectDevice9, _T("危险操作"));
    return m_lpDirectDevice9;
}
D3DPRESENT_PARAMETERS* CMs2DEngine::s_GetDirect3DPP(VOID)
{
    AUTO_LOCK(m_Lock);
    return &m_d3dPP;
}

BOOL CMs2DEngine::s_CreateD3D9(HWND hWnd, DWORD dwScreenWidth, DWORD dwScreenHeight)
{
    AUTO_LOCK(m_Lock);
    if (m_bInit)
    {
        return TRUE;
    }
    m_hWnd = hWnd;
    m_lpDirect9 = ::Direct3DCreate9(D3D_SDK_VERSION);
    if (m_lpDirect9)
    {
        ZeroMemoryStruct(m_d3dPP);
        m_d3dPP.Windowed   = TRUE;
        m_d3dPP.BackBufferWidth = dwScreenWidth;
        m_d3dPP.BackBufferHeight = dwScreenHeight;
        m_d3dPP.BackBufferCount = 1;
        m_d3dPP.hDeviceWindow = m_hWnd;
        m_d3dPP.SwapEffect = D3DSWAPEFFECT_COPY;
        m_d3dPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        m_d3dPP.BackBufferFormat = MS_D3DFORMAT;
        //m_d3dPP.Flags = D3DPRESENTFLAG_VIDEO;

        HRESULT hRet = m_lpDirect9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &m_d3dPP,
            &m_lpDirectDevice9
            );
        AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("创建D3D9设备失败"));
        m_bInit = TRUE;
        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, m_lpDirect9, _T("创建D3D9对象失败"));
        return FALSE;
    }
}

VOID CMs2DEngine::s_DeleteD3D9(VOID)
{
    SAFE_RELEASE(m_lpDirectDevice9);
    SAFE_RELEASE(m_lpDirect9);
    m_bInit = FALSE;
}

// 创建D3D9字体
ID3DXFont* CMs2DEngine::s_CreateD3D9Font(
    DWORD dwSize,           // 尺寸
    DWORD dwWeight,         // 字重
    DWORD dwMipLevels,      // Mip等级
    BOOL bItalic,           // 是否斜体
    LPCTSTR szFaceName,     // 字体名称
    DWORD dwCharSet,        // 字符集
    DWORD dwOutputPrecision,// 输出精度
    DWORD dwQuality,        // 品质
    DWORD dwPitchAndFamily  // 角度
    )
{
    AssertEx(AL_NORMAL, m_lpDirectDevice9, _T("危险操作"));
    ID3DXFont* lpID3DXFont = NULL;
    ::D3DXCreateFont(
        m_lpDirectDevice9,
        dwSize,
        0,
        dwWeight,
        dwMipLevels,
        bItalic,
        dwCharSet,
        dwOutputPrecision,
        dwQuality,
        dwPitchAndFamily,
        szFaceName,
        &lpID3DXFont
        );
    return lpID3DXFont;
}

// 释放
VOID CMs2DEngine::s_Release(VOID)
{
    SAFE_DELETE(m_pMsIni);
    SAFE_DELETE(m_pMsLog);
}

// 记录日志
BOOL CMs2DEngine::s_Log(TCHAR* szText)
{
    if (m_pMsLog)
    {
        return m_pMsLog->Log(szText);
    }
    return TRUE;
}

// 记录日志
BOOL CMs2DEngine::s_Logf(TCHAR* szText, ...)
{
    va_list ap;
    va_start(ap, szText);
    if (m_pMsLog)
    {
        return m_pMsLog->Logf(szText, ap);
    }
    return TRUE;
}

// 开关日志
VOID CMs2DEngine::s_EnableLog(BOOL bEnable)
{
    m_pMsLog->SetEnableLog(bEnable);
    m_pMsLog->SetSaveLog(bEnable);
}

// 获取按键标记
DWORD CMs2DEngine::s_GetKeyFlag(VOID)
{
    DWORD dwKeyFlag = INPUT_FLAG_NO_FLAG;

    if (::GetKeyState(MS2DK_SHIFT & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_SHIFT;
    }

    if (::GetKeyState(MS2DK_CTRL & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_CTRL;
    }

    if (::GetKeyState(MS2DK_ALT & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_ALT;
    }

    if (::GetKeyState(MS2DK_CAPSLOCK & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_CAPSLOCK;
    }

    if (::GetKeyState(MS2DK_SCROLLLOCK & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_SCROLLLOCK;
    }
    if (::GetKeyState(MS2DK_NUMLOCK & 0x8000))
    {
        dwKeyFlag |= INPUT_FLAG_NUMLOCK;
    }
    return dwKeyFlag;
}

// 获取Ms2DEngine属性
BOOL CMs2DEngine::s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_BOOL)
{
    return TRUE;
}

Ms2DCBFun CMs2DEngine::s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_FUNC)
{
    return NULL;
}

HWND CMs2DEngine::s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_HWND e_hWnd)
{
    switch(e_hWnd)
    {
    case M2DE_HWND:
        {
            return m_hWnd;
        }break;
    case M2DE_HWNDPARENT:
        {
            return NULL;
        }break;
    }
    return NULL;
}

INT CMs2DEngine::s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_INT)
{
    return 0;
}

CONST LPTCHAR CMs2DEngine::s_Ms2DEngine_GetState(E_MS2DENGINE_STATE_STRING)
{
    return NULL;
}

