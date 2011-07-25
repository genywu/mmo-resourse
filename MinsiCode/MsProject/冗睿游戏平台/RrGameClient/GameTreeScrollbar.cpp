#include "Precompiled.h"

CGameTreeScrollbar::CGameTreeScrollbar(CMsGdiBuffImage* lpCMsGdiBuffImage, INT nBasePosX, INT nBasePosY, DWORD dwWidth, DWORD dwHeight, DWORD eDirection)
: m_eDirection  (eDirection)
, m_nBasePosX   (nBasePosX)
, m_nBasePosY   (nBasePosY)
, m_nOffsetX    (0)
, m_nOffsetY    (0)
, m_dwWidth     (dwWidth)
, m_dwHeight    (dwHeight)
, m_nMaxOffsetX (0)
, m_nMaxOffsetY (0)
{
    this->Init(lpCMsGdiBuffImage);
}

CGameTreeScrollbar::~CGameTreeScrollbar(VOID)
{
    ;
}

// 鼠标是否穿过滚动条
BOOL CGameTreeScrollbar::IsOnScrollbar(POINT& Pos)
{
    RECT rc = {(m_nBasePosX + m_nOffsetX), (m_nBasePosY + m_nOffsetY), (m_nBasePosX + m_nOffsetX + m_dwWidth), (m_nBasePosY + m_nOffsetY + m_dwHeight)};
    return ::PtInRect(&rc, Pos);
}

// 移动滚动条
VOID CGameTreeScrollbar::MoveScrollX(INT nOffsetX)
{
    if (m_eDirection & TRANSVERSE)
    {
        m_nOffsetX += nOffsetX;
        if (m_nMaxOffsetX > 0)
        {
            if (m_nOffsetX > m_nMaxOffsetX)
            {
                m_nOffsetX = m_nMaxOffsetX;
            }
            else if (m_nOffsetX < 0)
            {
                m_nOffsetX = 0;
            }
        }
        else if (m_nMaxOffsetX < 0)
        {
            if (m_nOffsetX < m_nMaxOffsetX)
            {
                m_nOffsetX = m_nMaxOffsetX;
            }
            else if (m_nOffsetX > 0)
            {
                m_nOffsetX = 0;
            }
        }
        else
        {
            m_nOffsetX = 0;
        }
    }
}

VOID CGameTreeScrollbar::MoveScrollY(INT nOffsetY)
{
    if (m_eDirection & LOGNITUDINAL)
    {
        m_nOffsetY += nOffsetY;
        if (m_nMaxOffsetY > 0)
        {
            if (m_nOffsetY > m_nMaxOffsetY)
            {
                m_nOffsetY = m_nMaxOffsetY;
            }
            else if (m_nOffsetY < 0)
            {
                m_nOffsetY = 0;
            }
        }
        else if (m_nMaxOffsetY < 0)
        {
            if (m_nOffsetY < m_nMaxOffsetY)
            {
                m_nOffsetY = m_nMaxOffsetY;
            }
            else if (m_nOffsetY > 0)
            {
                m_nOffsetY = 0;
            }
        }
        else
        {
            m_nOffsetY = 0;
        }
    }
}

VOID CGameTreeScrollbar::AddMaxMoveScrollX(INT nMaxOffset)
{
    m_nMaxOffsetX += nMaxOffset;
}

VOID CGameTreeScrollbar::SubMaxMoveScrollX(INT nMaxOffset)
{
    m_nMaxOffsetX -= nMaxOffset;
}

VOID CGameTreeScrollbar::AddMaxMoveScrollY(INT nMaxOffset)
{
    m_nMaxOffsetY += nMaxOffset;
}

VOID CGameTreeScrollbar::SubMaxMoveScrollY(INT nMaxOffset)
{
    m_nMaxOffsetY -= nMaxOffset;
}

// 绘制滚动条
BOOL CGameTreeScrollbar::DrawScrollbar(VOID)
{
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 255, 255, 128));
    Gdiplus::Rect rc ((m_nBasePosX + m_nOffsetX), (m_nBasePosY + m_nOffsetY), m_dwWidth, m_dwHeight);
    m_pMsGraphics->m_Graphics.FillRectangle(&blackBrush, rc);
    return TRUE;
}
