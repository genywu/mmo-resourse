#pragma once                //┃//┃
#ifndef __MSPNGIMAGE_H__    //┃//┃
#define __MSPNGIMAGE_H__    //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsPngImage.h        //┃
//功能描述：PNG图片             //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛
#include "../MsExtern/Exoteric/Png/png.h"
class CMsPngImage
    : public CMsBaseImage
{
public:
    // 构造、析构
    CMsPngImage(VOID);
    virtual ~CMsPngImage(VOID);

    // 创建图片
    //virtual BOOL Create(VOID);

    // 打开图片
    virtual BOOL Open(TCHAR* szImageFileName);

    // 解码图片
    virtual BOOL Decode(VOID);

    // 编码图片
    virtual BOOL Encode(VOID);

    // 保存图片
    virtual BOOL Save(TCHAR* szImageFileName);

    // 关闭图片
    virtual VOID Close(VOID);

    // 绘制图片
    virtual BOOL Draw(HDC hDc, INT x, INT y, RECT DesRc);


    HBITMAP GetBitmap(VOID){return m_hBitmap;};
private:

    // PNG 读取回调
    static VOID PNGAPI ErrorPng(png_structp PngPtr, png_const_charp ErrorMsg);
    static VOID PNGAPI ReadPng(png_structp PngPtr, png_bytep lpData, png_size_t Length);
    static VOID PNGAPI WritePng(png_structp PngPtr, png_bytep lpData, png_size_t Length);
    static VOID PNGAPI FlushPng(png_structp PngPtr);


    // 图片完整路径名
    TCHAR   m_szFullPath[MAX_PATH];
    HANDLE  m_hFile;        // 图片文件句柄
    LPBYTE* m_lppPngBuff;   // 图片缓冲
    LPBYTE  m_lpBitData;    // 图片数据
    DWORD   m_dwPngWidth;   // 图片宽度
    DWORD   m_dwPngHeight;  // 图片高度

    HBITMAP m_hBitmap;      // 位图句柄

    // PNG 读取结构体指针
    png_struct* m_PngReadPtr;

    // PNG 读取信息结构体指针
    png_info*   m_PngReadInfoPtr;

    // PNG 写入结构体指针
    png_struct* m_PngWritePtr;

    // PNG 读取信息结构体指针
    png_info*   m_PngWriteInfoPtr;

    // 位图信息
    BITMAPINFO  m_BitmapInfo;

    // 位图信息头部
    BITMAPINFOHEADER    m_BitmapInfoHeader;
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
