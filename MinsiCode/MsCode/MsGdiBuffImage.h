#pragma once                //┃//┃
#ifndef __MSGDIBUFFIMAGE_H__//┃//┃
#define __MSGDIBUFFIMAGE_H__//┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsGdiBuffImage.h    //┃
//功能描述：GDI+缓冲图          //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛

class CMsGdiBuffImage
{
public:
    // 构造、析构
    CMsGdiBuffImage(VOID);
    virtual ~CMsGdiBuffImage(VOID);

    // 初始化
    virtual BOOL Init(HWND hWnd, DWORD dwWidth, DWORD dwHeight, COLORREF cBackColor = RGB(0,0,0), BOOL bTransparent = TRUE);

    // 保存图片
    virtual BOOL Save(TCHAR* szImageFileName);

    // 关闭图片
    virtual VOID Close(VOID);

    // 获取主设备
    virtual HDC GetMainDc(VOID);

    // 获取缓冲设备
    virtual HDC GetBuffDc(VOID);

    // 归还DC
    virtual VOID RestoreBuffDc(VOID);

    // 设置背景色
    virtual VOID SetBackColor(COLORREF cBackColor = RGB(0,0,0));

    // 准备绘制
    virtual BOOL BeginDraw(BOOL bErasure = TRUE);

    // 绘制文字
    virtual BOOL PrintText(HFONT hFont, RECT* lpRc, TCHAR* szText, COLORREF cColor, INT nTextLen, DWORD dwFormat);

    // 绘制图片
    virtual BOOL EndDraw(INT nDrawX = 0, INT nDrawY = 0, DWORD dwRop = SRCCOPY);
    virtual BOOL EndDraw(HDC hDc, INT nDrawX = 0, INT nDrawY = 0, DWORD dwRop = SRCCOPY);
private:

    // 主DC设备
    HDC         m_hMainDc;

    // 主窗口句柄
    HWND        m_hMainWnd;

    // 缓冲图句柄
    HBITMAP     m_hBuffBitmap;

    // 缓冲图宽高
    DWORD       m_dwWidth;
    DWORD       m_dwHeight;

    // 缓冲设备
    HDC         m_hBuffDc;

    // 是否已经初始化
    BOOL        m_bInit;

    // 背景颜色
    COLORREF    m_cBackColor;
    RECT        m_Rc;
    HBRUSH      m_hBrush;

    // 保存DC的索引值
    INT         m_nDcIndex;
};  

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
