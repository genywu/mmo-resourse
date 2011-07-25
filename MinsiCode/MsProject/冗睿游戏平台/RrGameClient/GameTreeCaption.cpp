#include "Precompiled.h"

CGameTreeCaption::CGameTreeCaption(CMsGdiBuffImage* pBuffImage, HFONT hFont)
: m_pBuffImage  (pBuffImage)
, m_hFont       (hFont)
{
    AssertEx(AL_CATAST, m_pBuffImage, _T("缓冲图不能为空"));
    if (!m_hFont)
    {
        this->SetFont(12, FW_THIN, FALSE, FALSE, NULL);
    }
}

CGameTreeCaption::~CGameTreeCaption(VOID)
{
    m_pBuffImage = NULL;
}

BOOL CGameTreeCaption::SetFont(INT nSize, DWORD dwWeight, BOOL bItalic, BOOL bUnderline, TCHAR* lpszFontName)
{
    if (!lpszFontName)
    {
        lpszFontName = _T("宋体");
    }
    m_hFont = ::CreateFont(
        nSize,
        0,
        0,
        0,
        dwWeight,
        bItalic,
        bUnderline,
        0,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        lpszFontName
        );

    if (m_hFont)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CGameTreeCaption::PrintText(RECT* lpRc, TCHAR* szText, INT nTextLen, COLORREF cColor, DWORD dwFormat)
{
    if (!m_pBuffImage)
    {
        return FALSE;
    }
    return m_pBuffImage->PrintText(m_hFont, lpRc, szText, cColor, nTextLen, dwFormat);
}
