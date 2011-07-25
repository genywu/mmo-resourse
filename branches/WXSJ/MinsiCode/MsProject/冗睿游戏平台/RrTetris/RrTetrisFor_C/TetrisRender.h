#pragma once

// 定义在背景图上开始绘制的位置偏移
#define DRAW_BACK_SCENE_POS_X 176
#define DRAW_BACK_SCENE_POS_Y 139

class CRender
    : public CMs2DRender_DX9
{
public:
    CRender(
        HWND hWnd,
        DWORD dwScreenWidth,
        DWORD dwScreenHeight,
        CT_Scene* pScene,
        CT_Square* pSquare);
    ~CRender(VOID);

    virtual VOID Draw(VOID);                            // 绘制
    BOOL DrawBackScene(VOID);                           // 绘制背景
    VOID ResetBackScene(VOID);                          // 重置背景
    BOOL DrawActivityShape(BOOL IsDrawShadow);          // 绘制前景
    BOOL DrawShapeShadow(UPOS Shape_X, UPOS Shape_Y);   // 绘制阴影
    BOOL DrawNextShape(VOID);                           // 绘制下一个方块
    BOOL DrawText(VOID);                                // 绘制文本信息
    BOOL DrawCenterText(TCHAR* szText, RECT* rcIntegral,  COLORREF cColor);// 居中绘制文本信息

private:
	// 计算需要使用的图元信息
	CMs2DTexture* AccountMetafileInfo(DWORD dwCurrentMetafileIndex, UPOS& uMetafilePosX, UPOS& uMetafilePosY);
    CMs2DTexture*       m_pTextureBack;         // 背景图片
	CMs2DTexture*       m_pTextureMetafile_0;   // 图元图片
	CMs2DTexture*       m_pTextureMetafile_1;   // 图元图片
	CMs2DTexture*       m_pTextureMetafile_2;   // 图元图片
	CMs2DTexture*       m_pTextureMetafile_3;   // 图元图片
    ID3DXFont*          m_lpD3DIntegralFont;    // 积分字体
    ID3DXFont*          m_lpD3DLevelFont;       // 等级字体
    CT_Scene*           m_pScene;               // 场景类指针
    CT_Square*          m_pSquare;              // 当前方块的指针
    BOOL                m_IsShowShadow;         // 是否显示阴影

    // 阴影渲染颜色矩阵
    Gdiplus::ColorMatrix m_ShadowColorBate;

      // 阴影渲染灰度矩阵
    Gdiplus::ColorMatrix m_ShadowGrayness;

};
