#include "Precompiled.h"
#include "GdiFont.h"

FontSprite* FontSprite::Create(CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, BOOL bAntialias)
{
    GdiFont* pEngineFontSprite = new GdiFont(lpsFontName,nFaceSize,bBold,bItalic,bUnderline,bAntialias);
    if (NULL == pEngineFontSprite) return NULL;
    
    // 缓冲下面的这些字符，因为是最常用的。
    pEngineFontSprite->StaticCacheCharacter(_T("0123456789"));
    pEngineFontSprite->StaticCacheCharacter(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    pEngineFontSprite->StaticCacheCharacter(_T("abcdefghijklmnopqrstuvwxyz"));
    pEngineFontSprite->StaticCacheCharacter(_T("`-=\\[];\',./~!@#$%^&*()_+|{}:\"<>?"));

    return (FontSprite*)pEngineFontSprite;
}

GdiFont::GdiFont(CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, BOOL bAntialias)
{
    m_pHGE = hgeCreate(HGE_VERSION);

    // 创建GDI相关设备
    HDC hDC = ::GetDC(m_pHGE->System_GetState(HGE_HWND));
    m_hMemDC = ::CreateCompatibleDC(hDC);
    if (NULL == m_hMemDC) return;
    ::ReleaseDC(m_pHGE->System_GetState(HGE_HWND),hDC);

    ::SetMapMode(m_hMemDC, MM_TEXT);
    ::SetTextAlign(m_hMemDC, TA_TOP);
    ::SetTextColor(m_hMemDC,RGB(255,255,255));
    ::SetBkColor(m_hMemDC,RGB(0,0,0));

    m_hBrush = ::CreateSolidBrush(RGB(0,0,0));
    if (NULL == m_hBrush) return;

    m_bItalic = bItalic;
    m_hFont = ::CreateFont(
        nFaceSize,
        0,
        0,
        0,
        (bBold) ? FW_BOLD : FW_NORMAL,
        bItalic,
        bUnderline,
        FALSE,
        GB2312_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        (bAntialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
        FF_DONTCARE | DEFAULT_PITCH,
        lpsFontName);
    if (NULL == (m_hFont)) return;
    ::SelectObject(m_hMemDC, m_hFont);

    HTEXTURE hTexLetter = m_pHGE->Texture_Create(tex_size,tex_size);
    if (NULL == (hTexLetter)) return;
    
    BITMAPINFO bmi;
    memset(&bmi,0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biWidth = m_pHGE->Texture_GetWidth(hTexLetter);;
    bmi.bmiHeader.biHeight = -m_pHGE->Texture_GetHeight(hTexLetter);

    m_pBuffer = NULL;
    m_hBitmap = ::CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, &m_pBuffer, NULL, 0);
    if (NULL == (m_hBitmap)) return;
    ::SelectObject(m_hMemDC, m_hBitmap);

    //
    m_FontTextures.clear();
    memset(m_FontGlyphs,0,sizeof(TENGINEFONTGLYPH)*font_count);

    m_nFontSize = static_cast<FLOAT>(nFaceSize);
    m_nKerningWidth = 0;
    m_nKerningHeight = 0;

    m_pSprite = new hgeSprite( 0, 0, 0, 0, 0 );
    m_pSprite->SetColor( ARGB( 255, 255, 255, 255 ) );

    memset(&m_ptLetter,0,sizeof(m_ptLetter));

    m_FontTextures.push_back(hTexLetter);
    m_hTexLetter = hTexLetter;
}

GdiFont::~GdiFont(VOID)
{

    size_t size = m_FontTextures.size();
    while (m_FontTextures.empty()==FALSE)
    {
        HTEXTURE hTex = m_FontTextures.front();
        m_FontTextures.pop_front();
        m_pHGE->Texture_Free(hTex);
    }

    if ((m_hBitmap)) DeleteObject(m_hBitmap);
    if ((m_hFont)) DeleteObject(m_hFont);
    if ((m_hBrush)) DeleteObject(m_hBrush);
    if ((m_hMemDC)) DeleteDC(m_hMemDC);

    DELETE_OBJECT(m_pSprite);
    RELEASE_OBJECT(m_pHGE);
}

VOID GdiFont::ResetFont( CONST TCHAR* lpsFontName, INT nFaceSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, BOOL bAntialias )
{
    m_bItalic = bItalic;
    if ((m_hFont)) ::DeleteObject(m_hFont);
    m_hFont = ::CreateFont(
        nFaceSize,
        0,
        0,
        0,
        (bBold) ? FW_BOLD : FW_NORMAL,
        bItalic,
        bUnderline,
        FALSE,
        GB2312_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        (bAntialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
        FF_DONTCARE | DEFAULT_PITCH,
        lpsFontName);
    if (NULL == (m_hFont)) return;
    SelectObject(m_hMemDC, m_hFont);
}


VOID GdiFont::StaticCacheCharacter(CONST TCHAR* text)
{
    while(*text)
    {
        GetGlyphByCharacter(*text);
        ++text;
    }
}

// 释放一个TypeFont精灵对象
VOID GdiFont::Release()
{
    DELETE_SELF(this);
}

// 渲染文本
VOID GdiFont::Printf( FLOAT x, FLOAT y, CONST TCHAR *format, ... )
{
    va_list l;
    va_start( l, format );

    TCHAR text[10240];
    _vstprintf( text, format, l );

    Render( x, y, text );
}

VOID GdiFont::Render(FLOAT x, FLOAT y, CONST TCHAR* text )
{
    FLOAT offsetX = x;
    FLOAT offsetY = y;

    while(*text)
    {
        if (*text == _T('\n') || *text == _T('\r'))
        {
            offsetX = x;
            offsetY += (m_nFontSize + m_nKerningHeight);
        }
        else
        {
            UINT idx = GetGlyphByCharacter(*text);
            if ( idx > 0)
            {
                m_pSprite->SetTexture( m_FontGlyphs[idx].t );
                m_pSprite->SetTextureRect( m_FontGlyphs[idx].x, m_FontGlyphs[idx].y, m_FontGlyphs[idx].w, m_FontGlyphs[idx].h );
                m_pSprite->Render(offsetX, offsetY);
                offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
            }
            else
            {
                offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
            }
        }

        ++text;
    }
}

VOID GdiFont::RenderEx( FLOAT x, FLOAT y, CONST TCHAR* text, FLOAT scale )
{
    Render(x,y,text);
}


// 设置与获取颜色
VOID GdiFont::SetColor( DWORD dwColor, INT i )
{
    m_pSprite->SetColor(dwColor,i);
}

DWORD GdiFont::GetColor(INT i)
{
    return m_pSprite->GetColor(i);
}

// 获取文本宽高
SIZE GdiFont::GetTextSize( CONST TCHAR* text )
{
    SIZE dim = {0, static_cast<LONG>(m_nFontSize)};
    FLOAT nRowWidth = 0;

    while(*text)
    {
        if (*text == _T('\n') || *text == _T('\r'))
        {
            dim.cy += static_cast<LONG>(m_nFontSize + m_nKerningHeight);
            if (dim.cx < static_cast<LONG>(nRowWidth))
                dim.cx = static_cast<LONG>(nRowWidth);
            nRowWidth = 0;
        }
        else
            nRowWidth += (GetWidthFromCharacter(*text) + m_nKerningWidth);
        ++text;
    }

    if (dim.cx < static_cast<LONG>(nRowWidth))
        dim.cx = static_cast<LONG>(nRowWidth);

    return dim;
}

// 根据坐标获取字符
TCHAR GdiFont::GetCharacterFromPos( CONST TCHAR* text, FLOAT pixel_x, FLOAT pixel_y )
{
    FLOAT x = 0;
    FLOAT y = 0;

    while (*text)
    {
        if (*text == _T('\n') || *text == _T('\r'))
        {
            x = 0;
            y += (m_nFontSize+m_nKerningHeight);
            text++;

            if (!(*text))
                break;
        }

        FLOAT w = this->GetWidthFromCharacter(*text);
        if (pixel_x > x && pixel_x <= x + w &&
            pixel_y > y && pixel_y <= y + m_nFontSize)
            return *text;

        x += (w+m_nKerningWidth);

        text++;
    }

    return _T('\0');
}

// 设置字间距
VOID GdiFont::SetKerningWidth( FLOAT kerning )
{
    m_nKerningWidth = kerning;
}
VOID GdiFont::SetKerningHeight( FLOAT kerning )
{
    m_nKerningHeight = kerning;
}

// 获取字间距
FLOAT GdiFont::GetKerningWidth()
{
    return m_nKerningWidth;
}
FLOAT GdiFont::GetKerningHeight()
{
    return m_nKerningHeight;
}    

// 字体大小
FLOAT GdiFont::GetFontSize()
{
    return m_nFontSize;
}

// 根据字符获取轮廓
UINT GdiFont::GetGlyphByCharacter( TCHAR c )
{
    UINT idx = (UINT)c;
    if (NULL == (m_FontGlyphs[idx].t)) CacheCharacter(idx,c);
    return idx;
}
inline FLOAT GdiFont::GetWidthFromCharacter( TCHAR c )
{
    UINT idx = GetGlyphByCharacter(c);
    if (idx > 0 && idx < font_count)
        return m_FontGlyphs[idx].w;

    if (c >= 0x2000)
    {
        return    m_nFontSize;
    }
    else
    {
        return    _floor(m_nFontSize / 2);
    }
}
inline VOID GdiFont::CacheCharacter(UINT idx, TCHAR c)
{
    if (idx < font_count && NULL == (m_FontGlyphs[idx].t))
    {
        WCHAR sChar[2];
        sChar[0] = c;
        sChar[1] = _T('\0');

        SIZE szChar = {0,0};
        ::GetTextExtentPoint32W(m_hMemDC,sChar,1,&szChar);

        INT font_w = szChar.cx;
        INT font_h = szChar.cy;

        if (m_ptLetter.x + font_w >= tex_size)
        {
            m_ptLetter.x = 0;
            if(m_ptLetter.y + font_h >= tex_size - font_h)
            {
                m_ptLetter.y = 0;

                HTEXTURE hTexLetter = m_pHGE->Texture_Create(tex_size,tex_size);
                if (NULL == (hTexLetter)) return;
                m_FontTextures.push_back(hTexLetter);
                m_hTexLetter = hTexLetter;

                RECT rcFill = {0,0,tex_size,tex_size};
                ::FillRect(m_hMemDC,&rcFill,m_hBrush);
            }
            else
                m_ptLetter.y += font_h;
        }
        
        ::TextOut(m_hMemDC,m_ptLetter.x,m_ptLetter.y,(LPCTSTR)sChar,1);

        DWORD *target_pixels = m_pHGE->Texture_Lock(m_hTexLetter,FALSE);
        
        if (target_pixels)
        {
            BYTE* pbm = (BYTE*)m_pBuffer;

            INT tex_w = m_pHGE->Texture_GetWidth(m_hTexLetter);
            INT tex_h = m_pHGE->Texture_GetHeight(m_hTexLetter);

            // 不带背景色绘制字体
            for (INT y=0;y<tex_w;y++)
            {
                for (INT x=0;x<tex_h;x++)
                {
                    BYTE alpha = 0;

                    // 绘制字体
                    target_pixels[y*tex_w+x] = ARGB(0xFFFFFF == RGB(pbm[2],pbm[1],pbm[0])?0xFF:0x0,0xFF,0xFF,0xFF);

                    pbm+=4;
                }
            }
        }

        m_pHGE->Texture_Unlock(m_hTexLetter);

        m_FontGlyphs[idx].x = static_cast<FLOAT>(m_ptLetter.x);
        m_FontGlyphs[idx].y = static_cast<FLOAT>(m_ptLetter.y);
        m_FontGlyphs[idx].w = static_cast<FLOAT>(font_w);
        m_FontGlyphs[idx].h = static_cast<FLOAT>(font_h);
        m_FontGlyphs[idx].t = m_hTexLetter;
        m_FontGlyphs[idx].c = c;

        m_ptLetter.x += font_w;
    }
}
