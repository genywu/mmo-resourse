#ifndef GDIFONT_H
#define GDIFONT_H
#include "Sprites.h"
#include <list>

class GdiFont :
    public FontSprite
{
public:
    GdiFont(CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);
    ~GdiFont(VOID);

    VOID StaticCacheCharacter(CONST TCHAR* text);

public:
    // 释放一个GdiFont精灵对象
    virtual VOID    Release();

    virtual VOID    ResetFont( CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE );

    // 渲染文本
    virtual VOID    Printf( FLOAT x, FLOAT y, CONST TCHAR *format, ... );
    virtual VOID    Render( FLOAT x, FLOAT y, CONST TCHAR* text );
    virtual VOID    RenderEx( FLOAT x, FLOAT y, CONST TCHAR* text, FLOAT scale = 1.0f );

    // 设置与获取颜色
    virtual VOID    SetColor( DWORD dwColor, INT i = -1 );
    virtual DWORD   GetColor( INT i = 0 );

    // 获取文本宽高
    virtual SIZE    GetTextSize( CONST TCHAR* text );

    // 根据坐标获取字符
    virtual TCHAR   GetCharacterFromPos( CONST TCHAR* text, FLOAT pixel_x, FLOAT pixel_y );

    // 设置字间距
    virtual VOID    SetKerningWidth( FLOAT kerning );
    virtual VOID    SetKerningHeight( FLOAT kerning );

    // 获取字间距
    virtual FLOAT   GetKerningWidth();
    virtual FLOAT   GetKerningHeight();

    // 字体大小
    virtual FLOAT   GetFontSize();

private:
    // 根据字符获取轮廓
    UINT            GetGlyphByCharacter( TCHAR c );
    inline FLOAT    GetWidthFromCharacter( TCHAR c );
    inline VOID     CacheCharacter(UINT idx, TCHAR c);

    typedef struct tagEngineFontGlyph
    {
        FLOAT       x;
        FLOAT       y;
        FLOAT       w;
        FLOAT       h;
        HTEXTURE    t;
        TCHAR       c;
    }TENGINEFONTGLYPH;

    static CONST UINT   font_count = 0xFFFF;// = sizeof(TCHAR);
    static CONST INT    tex_size = 256;

    std::list<HTEXTURE> m_FontTextures;
    TENGINEFONTGLYPH    m_FontGlyphs[font_count];
    FLOAT               m_nFontSize;
    FLOAT               m_nKerningWidth;
    FLOAT               m_nKerningHeight;
    BOOL                m_bItalic;

    HGE*                m_pHGE;
    hgeSprite*          m_pSprite;

    // GDI设备
    HDC                 m_hMemDC;
    HBRUSH              m_hBrush;
    HFONT               m_hFont;
    HBITMAP             m_hBitmap;
    VOID*               m_pBuffer;

    // 临时构造字模纹理信息
    HTEXTURE            m_hTexLetter;
    POINT               m_ptLetter;
};
#endif//GDIFONT_H
