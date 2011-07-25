#include "Precompiled.h"

// 构造、析构
CMsGdiPlusImage::CMsGdiPlusImage(VOID)
: m_hFile       (NULL)
, m_pMsGraphics (NULL)
, m_pMsImage    (NULL)
, m_bInit       (FALSE)
, m_hDc         (NULL)
, m_dwWidth     (0)
, m_dwHeight    (0)
{
    ZeroMemoryArray(m_szFullPath);
}

CMsGdiPlusImage::~CMsGdiPlusImage(VOID)
{
    // 关闭
    this->Close();
    SAFE_DELETE(m_pMsGraphics);
}

// 初始化
BOOL CMsGdiPlusImage::Init(HDC hDc)
{
    if (m_bInit)
    {
        return TRUE;
    }

    if (!m_pMsGraphics)
    {
        m_hDc = hDc;
        m_pMsGraphics = NEW MsGraphics(m_hDc);
        AssertEx(AL_CATAST, m_pMsGraphics, _T("内存不足"));
    }

    // 重置绘制属性
    m_ImageAttr.Reset();
    m_bInit = TRUE;
    return TRUE;
}

BOOL CMsGdiPlusImage::Init(CMsGdiBuffImage* lpCMsGdiBuffImage)
{
    if (m_bInit)
    {
        return TRUE;
    }

    if (lpCMsGdiBuffImage)
    {
        m_hDc = lpCMsGdiBuffImage->GetBuffDc();
    }

    if (!m_pMsGraphics)
    {
        m_pMsGraphics = NEW MsGraphics(m_hDc);
        AssertEx(AL_CATAST, m_pMsGraphics, _T("内存不足"));
    }

    // 重置绘制属性
    m_ImageAttr.Reset();

    m_bInit = TRUE;
    return TRUE;
}

// 加载图片
BOOL CMsGdiPlusImage::Load(TCHAR* szImageFileName)
{
    BOOL bRet = CreateFullPath(m_szFullPath, szImageFileName);
    AssertEx(AL_NORMAL, bRet, _T("路径解析错误"));

    m_pMsImage = NEW MsImage(m_szFullPath);
    AssertEx(AL_CATAST, m_pMsImage, _T("内存不足"));

    m_dwHeight = m_pMsImage->m_Image.GetHeight();
    m_dwWidth = m_pMsImage->m_Image.GetWidth();

    return TRUE;
}

// 加载图片
BOOL CMsGdiPlusImage::Load(HBITMAP hBitmap)
{
    m_pMsImage = NEW MsImage(hBitmap);
    AssertEx(AL_CATAST, m_pMsImage, _T("内存不足"));

    m_dwHeight = m_pMsImage->m_Image.GetHeight();
    m_dwWidth = m_pMsImage->m_Image.GetWidth();

    return TRUE;
}

// 创建图片
BOOL CMsGdiPlusImage::Create(DWORD dwWidth, DWORD dwHeight)
{
    HBITMAP hBitmap = ::CreateCompatibleBitmap(m_hDc, dwWidth, dwHeight);
    m_pMsImage = NEW MsImage(hBitmap, NULL);
    AssertEx(AL_CATAST, m_pMsImage, _T("内存不足"));

    m_dwHeight = m_pMsImage->m_Image.GetHeight();
    m_dwWidth = m_pMsImage->m_Image.GetWidth();

    return TRUE;
}

// 保存图片
BOOL CMsGdiPlusImage::Save(TCHAR* szImageFileName)
{
    USE_PARAM(szImageFileName);

    return TRUE;
}

// 关闭图片
VOID CMsGdiPlusImage::Close(VOID)
{
    SAFE_DELETE(m_pMsImage);
    ZeroMemoryArray(m_szFullPath);
    m_bInit = FALSE;
}

// 获取主DC
HDC CMsGdiPlusImage::GetDc(VOID)
{
    return m_hDc;
}

// 设置颜色矩阵
VOID CMsGdiPlusImage::SetColorMatrices(Gdiplus::ColorMatrix& CMatrix, Gdiplus::ColorMatrix& GMatrix)
{
    m_ImageAttr.SetColorMatrices(&CMatrix, &GMatrix);
}

// 重置绘制属性
VOID CMsGdiPlusImage::ResetImageAttr(VOID)
{
    m_ImageAttr.Reset();
}

// 绘制图片
BOOL CMsGdiPlusImage::Draw(RECT& DesRc, INT X, INT Y)
{
    if (m_bInit)
    {
        m_DestRect.X = DesRc.left;
        m_DestRect.Y = DesRc.top;
        m_DestRect.Width = DesRc.right - DesRc.left;
        m_DestRect.Height = DesRc.bottom - DesRc.top;
        m_pMsGraphics->m_Graphics.DrawImage(
            &m_pMsImage->m_Image,
            m_DestRect,
            X,
            Y,
            m_DestRect.Width,
            m_DestRect.Height,
            Gdiplus::UnitPixel,
            &m_ImageAttr
            );
        //m_pMsGraphics->m_Graphics.DrawImage(
        //    &m_pMsImage->m_Image,
        //    DesRc.left,
        //    DesRc.top,
        //    X,
        //    Y,
        //    m_DestRect.Width,
        //    m_DestRect.Height,
        //    Gdiplus::UnitPixel
        //    );
    }
    else
    {
        AssertEx(AL_NORMAL, m_bInit, _T("对象未初始化"));
    }
    return TRUE;
}

// 绘制图片
BOOL CMsGdiPlusImage::Draw(INT X, INT Y, INT srcWidth, INT srcHeight, INT srcX, INT srcY)
{
    if (m_bInit)
    {
        if (srcWidth == INVALID_NID)
        {
            srcWidth = m_dwWidth;
        }
        if (srcHeight == INVALID_NID)
        {
            srcHeight = m_dwHeight;
        }

        m_DestRect.X = X;
        m_DestRect.Y = Y;
        m_DestRect.Width = srcWidth;
        m_DestRect.Height = srcHeight;
        m_pMsGraphics->m_Graphics.DrawImage(
            &m_pMsImage->m_Image,
            m_DestRect,
            srcX,
            srcY,
            srcWidth,
            srcHeight,
            Gdiplus::UnitPixel,
            &m_ImageAttr
            );
        //m_pMsGraphics->m_Graphics.DrawImage(
        //    &m_pMsImage->m_Image,
        //    X,
        //    Y,
        //    srcX,
        //    srcY,
        //    srcWidth,
        //    srcHeight,
        //    Gdiplus::UnitPixel
        //    );
    }
    else
    {
        AssertEx(AL_NORMAL, m_bInit, _T("对象未初始化"));
    }
    return TRUE;
}

// 缩放绘制图片
BOOL CMsGdiPlusImage::ZoomDraw(RECT& DesRc, INT srcWidth, INT srcHeight, INT X, INT Y)
{
    if (m_bInit)
    {
        m_DestRect.X = DesRc.left;
        m_DestRect.Y = DesRc.top;
        m_DestRect.Width = DesRc.right - DesRc.left;
        m_DestRect.Height = DesRc.bottom - DesRc.top;
        m_pMsGraphics->m_Graphics.DrawImage(&m_pMsImage->m_Image, m_DestRect, X, Y, srcWidth, srcHeight, Gdiplus::UnitPixel, &m_ImageAttr);
    }
    else
    {
        AssertEx(AL_NORMAL, m_bInit, _T("对象未初始化"));
    }
    return TRUE;
}

BOOL CMsGdiPlusImage::ZoomDraw(INT X, INT Y, INT desWidth, INT desHeight, INT srcWidth, INT srcHeight, INT srcX, INT srcY)
{
    if (m_bInit)
    {
        if (srcWidth == INVALID_NID)
        {
            srcWidth = m_dwWidth;
        }
        if (srcHeight == INVALID_NID)
        {
            srcHeight = m_dwHeight;
        }

        m_DestRect.X = X;
        m_DestRect.Y = Y;
        m_DestRect.Width = desWidth;
        m_DestRect.Height = desHeight;
        m_pMsGraphics->m_Graphics.DrawImage(&m_pMsImage->m_Image, m_DestRect, srcX, srcY, srcWidth, srcHeight, Gdiplus::UnitPixel, &m_ImageAttr);
    }
    else
    {
        AssertEx(AL_NORMAL, m_bInit, _T("对象未初始化"));
    }
    return TRUE;
}