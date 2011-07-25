#include "Precompiled.h"
#include "hgesprite.h"
#include "HgeText.h"

CHgeText::CHgeText(VOID)
: m_DesktopDc       (NULL)
, m_hDcCompatible   (NULL)
, m_phgeSprite      (NULL)
{
    m_phgeSprite = new hgeSprite(NULL, 0.0f, 0.0f, 0.0f, 0.0f);
}

CHgeText::~CHgeText(VOID)
{
    //SAFE_DELETE(m_phgeSprite);
}

// 创建文本客户区
BOOL CHgeText::CreateTextBox(DTS* pDts)
{
    m_pDts = pDts;
    if (m_pDts && m_pDts->m_pHge)
    {
        INT nWidth  = (m_pDts->m_rc.right - m_pDts->m_rc.left);
        INT nHeight = (m_pDts->m_rc.bottom - m_pDts->m_rc.top);
        HWND hWnd = m_pDts->m_pHge->System_GetState(HGE_HWND);
        m_DesktopDc = ::GetDC(hWnd);
        m_hDcCompatible = ::CreateCompatibleDC(m_DesktopDc);
        if (!m_pDts->m_hBitmap)
        {
            m_pDts->m_hBitmap = ::CreateCompatibleBitmap(m_DesktopDc, nWidth, nHeight);
            //AssertEx(m_pDts->m_hBitmap, _T("创建文字图片失败,函数失败"));
        }
        if (!m_pDts->m_BitmapBit)
        {
            m_pDts->m_BitmapBit = new BYTE[nWidth*(nHeight+1)*4 + sizeof(BFH)];
            //AssertEx(m_pDts->m_BitmapBit,_T("m_pDts->m_BitmapBit为空,内存不足"));
        }
        return TRUE;
    }
	return FALSE;
}

// 更新字符串
BOOL CHgeText::UpDataString(VOID)
{
	if (m_pDts && m_pDts->m_pHge)
	{
        //AssertEx(m_pDts->m_hBitmap, _T("文字图片句柄无效,错误逻辑"));
        //AssertEx(m_pDts->m_hFont, _T("文字图片字体句柄无效,错误逻辑"));
        ::SelectObject(m_hDcCompatible, m_pDts->m_hBitmap);
        ::SelectObject(m_hDcCompatible, m_pDts->m_hFont);
        ::SetTextColor(m_hDcCompatible,m_pDts->m_FontColor);
        if (m_pDts->m_bLucencyBack)
        {
            ::SetBkMode(m_hDcCompatible, TRANSPARENT);
        }
        else
        {
            ::SetTextColor(m_hDcCompatible,m_pDts->m_BackColor);
        }
        ::DrawText(m_hDcCompatible, m_pDts->m_szString, (INT)_tcslen(m_pDts->m_szString), &m_pDts->m_rc, m_pDts->m_nFormat);

        INT nWidth  = (m_pDts->m_rc.right - m_pDts->m_rc.left);
        INT nHeight = (m_pDts->m_rc.bottom - m_pDts->m_rc.top);

        BFH bfh(nWidth,nHeight,32);
        memcpy(m_pDts->m_BitmapBit, &bfh, sizeof(bfh));
        ::GetBitmapBits(m_pDts->m_hBitmap, nWidth*nHeight*4, (DWORD*)((DWORD)m_pDts->m_BitmapBit + sizeof(BFH)));

        BYTE* pTempData = m_pDts->m_BitmapBit + sizeof(bfh);
        for (INT i = 0; i < (nHeight / 2); i++)
        {
            memcpy(&pTempData[nHeight*nWidth*4], &pTempData[i*nWidth*4], nWidth*4);
            memcpy(&pTempData[i*nWidth*4], &pTempData[(nHeight-1-i)*nWidth*4], nWidth*4);
            memcpy(&pTempData[(nHeight-1-i)*nWidth*4], &pTempData[nHeight*nWidth*4], nWidth*4);
        }

        if (m_pDts->m_QuadCache.tex)
        {
            m_pDts->m_pHge->Texture_Free(m_pDts->m_QuadCache.tex);
            m_pDts->m_QuadCache.tex = NULL;
        }
        m_pDts->m_QuadCache.tex = m_pDts->m_pHge->Texture_Load((TCHAR*)m_pDts->m_BitmapBit, nWidth*nHeight*4+sizeof(BFH));
        //AssertEx(m_pDts->m_QuadCache.tex, _T("加载文字纹理失败,函数失败"));

        HWND hWnd = m_pDts->m_pHge->System_GetState(HGE_HWND);
        ::ReleaseDC(hWnd, m_DesktopDc);
        ::DeleteDC(m_hDcCompatible);

		return TRUE;
	}
	return FALSE;
}

VOID CHgeText::Render(VOID)
{
	if (m_pDts->m_pHge)
	{
        m_phgeSprite->SetTexture(m_pDts->m_QuadCache.tex);
        m_phgeSprite->SetTextureRect((float)m_pDts->m_rc.left, (float)m_pDts->m_rc.top, (float)m_pDts->m_rc.right, (float)m_pDts->m_rc.bottom);
        m_phgeSprite->Render(0.0f,0.0f);
	}
}