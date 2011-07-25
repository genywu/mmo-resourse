#pragma once

#define STR_TEXT_MAX_COUNT 1024
#define DEFAULT_FONT_TYPE _T("楷体")

// 位图文件头
#pragma pack(1)
typedef struct BITMAP_FILE_HEAD
{
    BITMAP_FILE_HEAD(DWORD dwMapWidth, DWORD dwMapHeight, WORD wMapBitCount)
        : m_BM              (0x4D42)
        , m_FileLength      (0x00000036)
        , m_Reserve         (0x00000000)
        , m_BFHLength       (0x00000036)

        , m_MapHLength      (0x00000028)
        , m_MapWidth        (dwMapWidth)
        , m_MapHeight       (dwMapHeight)
        , m_MapDcLevel      (0x0001)
        , m_MapBitCount     (wMapBitCount)
        , m_MapIsCompress   (0x00000000)
        , m_MapLength       (dwMapWidth*dwMapHeight*wMapBitCount/8)
        , m_MapPelsPerMeterX(0x00000000)
        , m_MapPelsPerMeterY(0x00000000)
        , m_ClrUsed         (0x00000000)
        , m_ClrImportant    (0x00000000){}
    WORD    m_BM;               // BM标记
    DWORD   m_FileLength;       // 文件长度
    DWORD   m_Reserve;          // 保留
    DWORD   m_BFHLength;        // 图信息起始位置

    DWORD   m_MapHLength;       // 图信息头长度
    DWORD   m_MapWidth;         // 图宽
    DWORD   m_MapHeight;        // 图高
    WORD    m_MapDcLevel;       // 位图设备级别
    WORD    m_MapBitCount;      // 位图位率
    DWORD   m_MapIsCompress;    // 是否压缩
    DWORD   m_MapLength;        // 位图数据尺寸
    DWORD   m_MapPelsPerMeterX; // 横向分辨率(像素每米)
    DWORD   m_MapPelsPerMeterY; // 纵向分辨率(像素每米)
    DWORD   m_ClrUsed;          // 位图实际使用的颜色表中的颜色数
    DWORD   m_ClrImportant;     // 位图显示过程中重要的颜色数
}BFH;
#pragma pack(4)

typedef struct DRAW_TEXT_STRUCT
{
    DRAW_TEXT_STRUCT(VOID)
        : m_hBitmap         (NULL)
        , m_bLucencyBack    (TRUE)
        , m_FontColor       (RGB(255,255,255))
        , m_BackColor       (0)
        , m_nFormat         (DT_LEFT)
        , m_pHge            (NULL)
        , m_BitmapBit       (NULL)
    {
        //ZeroMemoryStruct(m_QuadCache);
        //ZeroMemoryArray(m_szString);
        m_hFont = ::CreateFont(
            10,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            GB2312_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FF_DONTCARE|DEFAULT_PITCH,
            DEFAULT_FONT_TYPE);
    }
    ~DRAW_TEXT_STRUCT(VOID)
    {
        //if (m_pHge && m_QuadCache.tex)
        //{
        //    m_pHge->Texture_Free(m_QuadCache.tex);
        //    m_QuadCache.tex = NULL;
        //}
        if(m_hBitmap)
        {
            ::DeleteObject(m_hBitmap);
            m_hBitmap = NULL;
        }
        //SAFE_DELETE(m_BitmapBit);
    }

    HBITMAP     m_hBitmap;                      // 文字图片句柄
    hgeQuad     m_QuadCache;                    // HGE嵌块
    TCHAR       m_szString[STR_TEXT_MAX_COUNT]; // 文字内容
    BOOL        m_bLucencyBack;                 // 背景是否透明
    COLORREF    m_FontColor;                    // 字体颜色
    COLORREF    m_BackColor;                    // 背景颜色
    HFONT       m_hFont;                        // 字体
    RECT        m_rc;                           // 文字图片位置
    UINT        m_nFormat;                      // 文字格式
    HGE*        m_pHge;                         // HGE系统指针
    BYTE*       m_BitmapBit;                    // 位图数据

}DTS;

class CHgeText
{
public:
	CHgeText(VOID);
	~CHgeText(VOID);

protected:
    HDC         m_DesktopDc;
    HDC         m_hDcCompatible;
    DTS*        m_pDts;
    hgeSprite*  m_phgeSprite;

public:

	BOOL CreateTextBox(DTS* pDts);
	BOOL UpDataString(VOID);
	VOID Render(VOID);
};
