#include "Precompiled.h"

CMs2DRender_DX9::CMs2DRender_DX9(
    HWND hWnd,
    DWORD dwScreenWidth,
    DWORD dwScreenHeight
    )
: m_bInit           (FALSE)
, m_pMsIni          (NULL)
, m_pMsLog          (NULL)
, m_bQuit           (TRUE)
, m_bRenderFrame    (TRUE)
, m_lfRenderFrame   (0.0f)
, m_dwRenderFrame   (0)
, m_lfConsumeTime   (0.0f)
, m_n64TimeA        (0)
, m_n64TimeB        (0)
, m_n64TimeEvenA    (0)
, m_n64TimeEvenB    (0)
, m_lpDirectDevice9 (NULL)
, m_dwFrameCount    (0)
, m_dwFrameEvenCount(0)
, m_lpD3DFrameFont  (NULL)
, m_D3DColor        (ARGB(255, 255, 0, 0))
, m_hWnd            (hWnd)
, m_dwScreenWidth   (dwScreenWidth)
, m_dwScreenHeight  (dwScreenHeight)
{
    CMs2DEngine::s_CreateD3D9(m_hWnd, dwScreenWidth, dwScreenHeight);
    m_lpDirectDevice9 = CMs2DEngine::s_GetDirectDevice9();

    ZeroMemoryArray(m_szFrameBuff);
    ZeroMemoryArray(m_szFormatFrame);
    m_FrameRect.left = 0;
    m_FrameRect.top = 0;
    m_FrameRect.right = 100;
    m_FrameRect.bottom = 100;
}

CMs2DRender_DX9::~CMs2DRender_DX9(VOID)
{
    SAFE_RELEASE(m_lpD3DFrameFont);
}

// 渲染
BOOL CMs2DRender_DX9::Render(VOID)
{
    this->UpdataFrame();

    if (m_bRenderFrame && m_lpD3DFrameFont)
    {
        if (m_bEven)
        {
            _stprintf_s(
                m_szFrameBuff,
                m_szFormatFrame,
                m_dwFrameEvenCount
                );
			this->DrawFont(m_lpD3DFrameFont, m_szFrameBuff, (INT)_tcslen(m_szFrameBuff), &m_FrameRect, DT_LEFT, m_D3DColor);
		}
        else
        {
            _stprintf_s(
                m_szFrameBuff,
                m_szFormatFrame,
                m_lfRenderFrame,
                m_dwRenderFrame,
                m_lfConsumeTime
                );
			this->DrawFont(m_lpD3DFrameFont, m_szFrameBuff, (INT)_tcslen(m_szFrameBuff), &m_FrameRect, DT_LEFT, m_D3DColor);
		}
    }

    HRESULT hRet = m_lpDirectDevice9->EndScene();
    if (FAILED(hRet))
    {
        return FALSE;
    }
    hRet = m_lpDirectDevice9->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hRet))
    {
        return FALSE;
    }
    hRet = m_lpDirectDevice9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x8E, 0x87, 0xB2), 1.0f, 0);
    
    if (FAILED(hRet))
    {
        return FALSE;
    }
    hRet = m_lpDirectDevice9->BeginScene();
    if (FAILED(hRet))
    {
        return FALSE;
    }
    return TRUE;
}

// 停止之前
BOOL CMs2DRender_DX9::ForeStop(VOID)
{
    return TRUE;
}

// 线程回调
BOOL CMs2DRender_DX9::ThreadCallBack(VOID)
{
    UPDATA_FILETIME_MILLISECOND(m_n64TimeA);
    UPDATA_FILETIME_MILLISECOND(m_n64TimeEvenA);
    m_lpDirectDevice9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x8E, 0x87, 0xB2), 1.0f, 0);
   
    m_lpDirectDevice9->BeginScene();
    while (!m_bStop)
    {
        this->Draw();
        this->Render();
    }
    m_lpDirectDevice9->EndScene();
    return TRUE;
}

// 更新帧数
VOID CMs2DRender_DX9::UpdataFrame(VOID)
{
    // 更新时间
    UPDATA_FILETIME_MILLISECOND(m_n64TimeB);

    if (m_bEven)
    {
        // 平滑帧数
        static DWORD s_dwCount = 0;
        m_n64TimeEvenB = m_n64TimeB;
        if (m_n64TimeEvenB - m_n64TimeEvenA >= 1000)
        {
            m_dwFrameEvenCount = s_dwCount;
            s_dwCount = 0;
            m_n64TimeEvenA = m_n64TimeEvenB;
        }
        else
        {
            s_dwCount++;
        }
    }
    else
    {
        m_lfConsumeTime = (DOUBLE)(m_n64TimeB - m_n64TimeA);
        if (m_lfConsumeTime > 0.0f)
        {
            m_lfRenderFrame = (DOUBLE)m_dwFrameCount / m_lfConsumeTime * 1000.0f;
            m_dwRenderFrame = (DWORD)m_lfRenderFrame;
            m_dwFrameCount = 1;
        }
        else
        {
            m_dwFrameCount++;
        }
        m_n64TimeA = m_n64TimeB;
    }
}

// 获取帧数
DWORD CMs2DRender_DX9::GetFrameCount(FLOAT)
{
    return m_dwRenderFrame;
}

// 获取帧数
DOUBLE CMs2DRender_DX9::GetFrameCount(DWORD)
{
    return m_lfRenderFrame;
}

// 开渲染帧数
VOID CMs2DRender_DX9::EnableFrame(ID3DXFont* lpD3DFrameFont, BOOL bEven, RECT& rc, TCHAR* szFormatEven, TCHAR* szFormat, D3DCOLOR D3DColor)
{
    m_lpD3DFrameFont = lpD3DFrameFont;
    m_FrameRect = rc;
    m_bEven = bEven;
    m_D3DColor = D3DColor;
    if (m_bEven)
    {
        _tcscpy_s(m_szFormatFrame, MAX_PATH, szFormatEven);
    }
    else
    {
        _tcscpy_s(m_szFormatFrame, MAX_PATH, szFormat);
    }
    m_bRenderFrame = TRUE;
}

// 关渲染帧数
VOID CMs2DRender_DX9::DisableFrame(VOID)
{
    m_bRenderFrame = FALSE;
}

// 画字
INT CMs2DRender_DX9::DrawFont(ID3DXFont* lpD3DFrameFont, TCHAR* szText, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color)
{
	if (lpD3DFrameFont)
	{
		return lpD3DFrameFont->DrawText(NULL, szText, Count, pRect, Format, Color);
	}
	return 0;
}