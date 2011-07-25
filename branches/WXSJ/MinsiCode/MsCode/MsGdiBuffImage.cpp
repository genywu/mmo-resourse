#include "Precompiled.h"

// 构造、析构
CMsGdiBuffImage::CMsGdiBuffImage(VOID)
: m_bInit       (FALSE)
, m_hMainWnd    (NULL)
, m_hMainDc     (NULL)
, m_hBuffDc     (NULL)
, m_nDcIndex    (INVALID_NID)
, m_hBrush      (NULL)
, m_cBackColor  (0)
{
    ;
}

CMsGdiBuffImage::~CMsGdiBuffImage(VOID)
{
    this->Close();
}

// 初始化
BOOL CMsGdiBuffImage::Init(HWND hWnd, DWORD dwWidth, DWORD dwHeight, COLORREF cBackColor, BOOL bTransparent)
{
    if (hWnd && dwWidth && dwHeight)
    {
        m_hMainWnd = hWnd;
        m_hMainDc = ::GetDC(hWnd);
        AssertEx(AL_NORMAL, m_hMainDc, _T("获取窗口DC失败"));

        m_dwWidth = dwWidth;
        m_dwHeight = dwHeight;
        m_Rc.left = 0;
        m_Rc.top  = 0;
        m_Rc.right = m_dwWidth;
        m_Rc.bottom = m_dwHeight;

        this->SetBackColor(cBackColor);

        m_hBuffDc = ::CreateCompatibleDC(m_hMainDc);
        AssertEx(AL_NORMAL, m_hBuffDc, _T("缓冲设备建失败"));

        if (bTransparent)
        {
            ::SetBkMode(m_hBuffDc, TRANSPARENT);
        }
        else
        {
            ::SetBkMode(m_hBuffDc, OPAQUE);
        }

        m_hBuffBitmap = ::CreateCompatibleBitmap(m_hMainDc, dwWidth, dwHeight);
        AssertEx(AL_NORMAL, m_hBuffBitmap, _T("缓冲图创建失败"));

        ::SelectObject(m_hBuffDc, m_hBuffBitmap);

        m_bInit = TRUE;

        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, hWnd, _T("窗口不能为空"));
        return FALSE;
    }
}

// 保存图片
BOOL CMsGdiBuffImage::Save(TCHAR* szImageFileName)
{
    USE_PARAM(szImageFileName);
    return TRUE;
}

// 关闭图片
VOID CMsGdiBuffImage::Close(VOID)
{
    if (m_hBuffBitmap)
    {
        ::DeleteObject(m_hBuffBitmap);
        m_hBuffBitmap = NULL;
    }
    if (m_hBuffDc)
    {
        ::DeleteDC(m_hBuffDc);
        m_hBuffDc = NULL;
    }
    if (m_hBrush)
    {
        ::DeleteObject(m_hBrush);
        m_hBrush = NULL;
    }
}

// 获取主设备
HDC CMsGdiBuffImage::GetMainDc(VOID)
{
    return m_hMainDc;
}

// 获取缓冲设备
HDC CMsGdiBuffImage::GetBuffDc(VOID)
{
    m_nDcIndex = ::SaveDC(m_hBuffDc);
    return m_hBuffDc;
}

// 归还DC
VOID CMsGdiBuffImage::RestoreBuffDc(VOID)
{
    if (m_nDcIndex != INVALID_NID)
    {
        ::RestoreDC(m_hBuffDc, m_nDcIndex);
    }
}

// 设置背景色
VOID CMsGdiBuffImage::SetBackColor(COLORREF cBackColor)
{
    m_cBackColor = cBackColor;
    m_hBrush = ::CreateSolidBrush(m_cBackColor);
}

// 准备绘制
BOOL CMsGdiBuffImage::BeginDraw(BOOL bErasure)
{
    if (bErasure)
    {
        ::FillRect(m_hBuffDc, &m_Rc, m_hBrush);
    }
    return TRUE;
}

// 绘制文字
BOOL CMsGdiBuffImage::PrintText(HFONT hFont, RECT* lpRc, TCHAR* szText, COLORREF cColor, INT nTextLen, DWORD dwFormat)
{
    if (hFont)
    {
        ::SetTextColor(m_hBuffDc, cColor);
        HANDLE hObj = ::SelectObject(m_hBuffDc, hFont);
        ::DrawText(m_hBuffDc, szText, nTextLen, lpRc, dwFormat);
        ::SelectObject(m_hBuffDc, hObj);
        return TRUE;
    }
    return FALSE;
}

// 绘制图片
BOOL CMsGdiBuffImage::EndDraw(INT nDrawX, INT nDrawY, DWORD dwRop)
{
    if (m_bInit && m_hMainDc && m_hBuffBitmap && m_hBuffDc)
    {
        return ::BitBlt(m_hMainDc, 0, 0, m_dwWidth, m_dwHeight, m_hBuffDc, nDrawX, nDrawY, dwRop);
    }
    else
    {
        return FALSE;
    }
}

BOOL CMsGdiBuffImage::EndDraw(HDC hDc, INT nDrawX, INT nDrawY, DWORD dwRop)
{
    if (m_bInit && hDc && m_hBuffBitmap && m_hBuffDc)
    {
        return ::BitBlt(hDc, 0, 0, m_dwWidth, m_dwHeight, m_hBuffDc, nDrawX, nDrawY, dwRop);
    }
    else
    {
        return FALSE;
    }
}
