#pragma once

class CTestRender
    : public CMs2DRender_DX9
{
public:
    CTestRender(
        HWND hWnd,
        DWORD dwWidth,
        DWORD dwHeight
        );
    ~CTestRender(VOID);
    virtual VOID Draw(VOID);                // ªÊ÷∆

private:
    LPCMs2DTexture          m_lpTexture;    // Õº∆¨

    // “ı”∞‰÷»æ—’…´æÿ’Û
    Gdiplus::ColorMatrix    m_ShadowColorBate;

      // “ı”∞‰÷»æª“∂»æÿ’Û
    Gdiplus::ColorMatrix    m_ShadowGrayness;

};