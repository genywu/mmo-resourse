#pragma once                //┃//┃
#ifndef __MSGDIPLUSIMAGE_H__//┃//┃
#define __MSGDIPLUSIMAGE_H__//┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsGdiPlusImage.h    //┃
//功能描述：GDI+图片            //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛

class CMsGdiPlusImage
{
public:
    // 构造、析构
    CMsGdiPlusImage(VOID);
    virtual ~CMsGdiPlusImage(VOID);

    // 初始化
    virtual BOOL Init(HDC hDc);

    virtual BOOL Init(CMsGdiBuffImage* lpCMsGdiBuffImage);

    // 加载图片
    virtual BOOL Load(TCHAR* szImageFileName);

    // 加载图片
    virtual BOOL Load(HBITMAP hBitmap);

    // 创建图片
    virtual BOOL Create(DWORD dwWidth, DWORD dwHeight);

    // 保存图片
    virtual BOOL Save(TCHAR* szImageFileName);

    // 关闭图片
    virtual VOID Close(VOID);

    // 获取主DC
    virtual HDC  GetDc(VOID);

    // 设置颜色矩阵
    virtual VOID SetColorMatrices(Gdiplus::ColorMatrix& CMatrix, Gdiplus::ColorMatrix& GMatrix);

    // 重置绘制属性
    virtual VOID ResetImageAttr(VOID);
    
    // 绘制图片
    virtual BOOL Draw(RECT& DesRc, INT X = 0, INT Y = 0);

    virtual BOOL Draw(INT X, INT Y, INT srcWidth = INVALID_NID, INT srcHeight = INVALID_NID, INT srcX = 0, INT srcY = 0);

    // 缩放绘制图片
    virtual BOOL ZoomDraw(RECT& DesRc, INT srcWidth, INT srcHeight, INT X, INT Y);

    virtual BOOL ZoomDraw(INT X, INT Y, INT desWidth, INT desHeight, INT srcWidth, INT srcHeight, INT srcX, INT srcY);
protected:

    // 图片完整路径名
    TCHAR       m_szFullPath[MAX_PATH];
    HANDLE      m_hFile;        // 图片文件句柄
    HDC         m_hDc;          // 主DC
    DWORD       m_dwWidth;      // 宽度
    DWORD       m_dwHeight;     // 高度
    MsGraphics* m_pMsGraphics;  // 画法对象
    MsImage*    m_pMsImage;     // 图片对象
    BOOL        m_bInit;        // 是否已经初始化

    Gdiplus::ImageAttributes m_ImageAttr;   // 绘制属性
    Gdiplus::Rect m_DestRect;   // 目标区域
};  

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
