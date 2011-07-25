//////////////////////////////////////////////////////////////////////////////////////
// 精灵机构
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "hgesprite.h"

//////////////////////////////////////////////////////////////////
// 公共定义
#define DELETE_SELF(p)                { if(p) { delete p; } };
#define DELETE_OBJECT(p)            { if(p) { delete p; p=NULL; } };
#define DELETE_OBJECTARRAY(p)        { if(p) { delete [] p; p=NULL; } };
#define RELEASE_OBJECT(p)            { if(p) { p->Release(); p=NULL; } };

__inline FLOAT _floor(FLOAT f)
{
    static INT _n;
    //_asm fld f
    //_asm fistp _n
    return (FLOAT)_n;
}

//////////////////////////////////////////////////////////////////////////////////////
// 字体精灵
//////////////////////////////////////////////////////////////////////////////////////

class FontSprite
{
public:
    // 获取像素字体指针
    // CONST CHAR* lpsFontName    - 字体名称
    // INT nFaceSize            - 字体大小
    // BOOL bBold                - 是否粗体
    // BOOL bItalic                - 是否斜体
    // BOOL bUnderline            - 是否有下划线
    static FontSprite* Create(CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);

    // 获取适量字体指针
    // CONST CHAR* lpsTTFile    - 矢量字体路径文件名
    // FLOAT nFontSize            - 矢量字体大小
    static FontSprite* CreateEx(CONST TCHAR* lpsTTFile, FLOAT nFontSize);

    // 销毁字体
    virtual VOID    Release() = 0;

    // 渲染文本
    virtual VOID    Printf( FLOAT x, FLOAT y, CONST TCHAR *format, ... ) = 0;
    virtual VOID    Render( FLOAT x, FLOAT y, CONST TCHAR* text ) = 0;
    virtual VOID    RenderEx( FLOAT x, FLOAT y, CONST TCHAR* text, FLOAT scale = 1.0f ) = 0;

    // 设置与获取颜色
    virtual VOID    SetColor( DWORD dwColor, INT i = -1 ) = 0;
    virtual DWORD    GetColor( INT i = 0 ) = 0;

    // 获取文本区域大小
    virtual SIZE    GetTextSize( CONST TCHAR* text ) = 0;

    // 根据相对坐标获取字符
    virtual TCHAR    GetCharacterFromPos( CONST TCHAR* text, FLOAT pixel_x, FLOAT pixel_y ) = 0;

    // 设置字间距
    virtual VOID    SetKerningWidth( FLOAT kerning ) = 0;
    virtual VOID    SetKerningHeight( FLOAT kerning ) = 0;

    // 获取字间距
    virtual FLOAT    GetKerningWidth() = 0;
    virtual FLOAT    GetKerningHeight() = 0;

    // 获取字体大小
    virtual FLOAT    GetFontSize() = 0;

protected:
    // 方式该抽象类被new
    virtual ~FontSprite(VOID) { };

};//class FontSprite
