#include "Precompiled.h"

// 构造、析构
CMsPngImage::CMsPngImage(VOID)
: m_lppPngBuff      (NULL)
, m_lpBitData       (NULL)
, m_dwPngWidth      (0)
, m_dwPngHeight     (0)
, m_hFile           (NULL)
, m_PngReadPtr      (NULL)
, m_PngWritePtr     (NULL)
, m_PngReadInfoPtr  (NULL)
, m_PngWriteInfoPtr (NULL)
{
    ZeroMemoryArray(m_szFullPath);

    // 创建读取结构体
    m_PngReadPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    AssertEx(m_PngReadPtr, _T("创建PNG读取结构体失败"));

    // 创建读取信息结构体
    m_PngReadInfoPtr = png_create_info_struct(m_PngReadPtr);
    AssertEx(m_PngReadInfoPtr, _T("创建PNG读取信息结构体失败"));

    // 创建写入结构体
    m_PngWritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    AssertEx(m_PngWritePtr, _T("创建PNG写入结构体失败"));

    // 创建写入信息结构体
    m_PngWriteInfoPtr = png_create_info_struct(m_PngWritePtr);
    AssertEx(m_PngWriteInfoPtr, _T("创建PNG写入信息结构体失败"));

    // 设置PNG 读取回调函数
    png_set_read_fn(m_PngReadPtr, this, ReadPng);
    png_set_write_fn(m_PngWritePtr, this, WritePng, FlushPng);
}

CMsPngImage::~CMsPngImage(VOID)
{
    // 关闭
    this->Close();

    ::DeleteObject(m_hBitmap);

    // 销毁读取信息结构体
    png_destroy_info_struct(m_PngReadPtr, &m_PngReadInfoPtr);

    // 销毁写入信息结构体
    png_destroy_info_struct(m_PngWritePtr, &m_PngWriteInfoPtr);

    // 销毁读取结构体
    png_destroy_read_struct(&m_PngReadPtr, NULL, NULL);

    // 销毁写入结构体
    png_destroy_write_struct(&m_PngWritePtr, NULL);
}

// 打开图片
BOOL CMsPngImage::Open(TCHAR* szImageFileName)
{
    BOOL bRet = CreateFullPath(m_szFullPath, szImageFileName);
    AssertEx(bRet, _T("路径解析错误"));

    // 打开图片文件
    m_hFile = ::CreateFile(
        m_szFullPath,
        FILE_READ_DATA,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        );
    AssertEx(m_hFile, _T("打开图片失败"));
    return TRUE;
}

// PNG 错误回调
VOID PNGAPI CMsPngImage::ErrorPng(png_structp PngPtr, png_const_charp ErrorMsg)
{
    USE_PARAM(PngPtr);
    AssertExA(FALSE, ErrorMsg);
}

// PNG 读取回调
VOID PNGAPI CMsPngImage::ReadPng(png_structp png_ptr, png_bytep data, png_size_t length)
{
    CMsPngImage* pPngImage = (CMsPngImage*)png_get_io_ptr(png_ptr);

    if (pPngImage->m_hFile)
    {
        DWORD dwReadSize;
        BOOL bRet = ::ReadFile(pPngImage->m_hFile, data, length, &dwReadSize, NULL);
        AssertEx(bRet, _T("读取图片失败"));
    }
    else
    {
        AssertEx(pPngImage->m_hFile, _T("读取图片失败"));
    }
}

VOID PNGAPI CMsPngImage::WritePng(png_structp png_ptr, png_bytep data, png_size_t length)
{
    CMsPngImage* pPngImage = (CMsPngImage*)png_get_io_ptr(png_ptr);

    if (pPngImage->m_hFile)
    {
        DWORD dwWriteSize;
        BOOL bRet = ::WriteFile(pPngImage->m_hFile, data, length, &dwWriteSize, NULL);
        AssertEx(bRet, _T("写入图片失败"));
    }
    else
    {
        AssertEx(pPngImage->m_hFile, _T("写入图片失败"));
    }
}
VOID PNGAPI CMsPngImage::FlushPng(png_structp png_ptr)
{
    CMsPngImage* pPngImage = (CMsPngImage*)png_get_io_ptr(png_ptr);

    if (pPngImage->m_hFile)
    {
        BOOL bRet = ::FlushFileBuffers(pPngImage->m_hFile);
        AssertEx(bRet, _T("写入图片失败"));
    }
    else
    {
        AssertEx(pPngImage->m_hFile, _T("写入图片失败"));
    }
}


// 解析图片
BOOL CMsPngImage::Decode(VOID)
{
    png_read_info(m_PngReadPtr, m_PngReadInfoPtr);
    m_dwPngWidth = m_PngReadInfoPtr->width;
    m_dwPngHeight = m_PngReadInfoPtr->height;
    m_lppPngBuff = NEW LPBYTE[m_dwPngHeight];
    m_lpBitData = NEW BYTE[m_dwPngHeight * m_dwPngWidth * m_PngReadInfoPtr->channels];
    if (m_lppPngBuff)
    {
        FOR_x_TO(i, m_dwPngHeight)
        {
            m_lppPngBuff[i] = &m_lpBitData[i * m_dwPngWidth];
        }
    }
    else
    {
        AssertEx(m_lppPngBuff, _T("内存不足"));
    }
    png_read_rows(m_PngReadPtr, m_lppPngBuff, NULL, m_dwPngHeight);
    INT a = 24;
    INT b = 1;
    m_hBitmap = ::CreateBitmap(m_dwPngWidth, m_dwPngHeight, a, b, m_lpBitData);
    //m_hBitmap = (HBITMAP)::LoadImage(g_pCMsWinMain->GetHInstance(), _T("D:/MinsiWork/Debug/MsRes/Project/Exe_冗睿游戏平台/RrRes/Image/00.bmp"), IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
    AssertEx(m_hBitmap, _T("图片转换失败"));
    ::CloseHandle(m_hFile);
    m_hFile = NULL;

    return TRUE;
}

// 编码图片
BOOL CMsPngImage::Encode(VOID)
{

    return TRUE;
}

// 保存图片
BOOL CMsPngImage::Save(TCHAR* szImageFileName)
{
    USE_PARAM(szImageFileName);

    return TRUE;
}

// 关闭图片
VOID CMsPngImage::Close(VOID)
{
    SAFE_DELETE_ARRAY(m_lppPngBuff);
    SAFE_DELETE_ARRAY(m_lpBitData);
    m_dwPngWidth = 0;
    m_dwPngHeight = 0;
    ZeroMemoryArray(m_szFullPath);
}

// 绘制图片
BOOL CMsPngImage::Draw(HDC hDc, INT x, INT y, RECT DesRc)
{
    HDC TmpDC = ::CreateCompatibleDC(hDc);
    ::SelectObject(TmpDC, m_hBitmap);
    ::BitBlt(hDc, DesRc.left, DesRc.top, (DesRc.right - DesRc.left), (DesRc.bottom - DesRc.top), TmpDC, x, y, SRCCOPY);
    return ::DeleteDC(TmpDC);
}