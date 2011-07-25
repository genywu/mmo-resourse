#pragma once                //┃//┃
#ifndef __MS2DRENDER_DX9_H__//┃//┃
#define __MS2DRENDER_DX9_H__//┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：Ms2DRender_DX9.h    //┃
//功能描述：2D游戏引擎DX9渲染   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

class CMs2DRender_DX9
    :public CMsThread
{
public: // 构造、析构
    CMs2DRender_DX9(
        HWND hWnd,
        DWORD dwScreenWidth,
        DWORD dwScreenHeight
        );
    ~CMs2DRender_DX9(VOID);

// 接口
public:
    BOOL Render(VOID);                      // 渲染
    virtual VOID Draw(VOID) = 0;            // 绘制
    DWORD GetFrameCount(FLOAT);             // 获取帧数
    DOUBLE GetFrameCount(DWORD);            // 获取帧数

	// 画字
	INT DrawFont(ID3DXFont* lpD3DFrameFont, TCHAR* szText, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color);

    // 关渲染帧数
    VOID DisableFrame(VOID);

    // 开渲染帧数
    VOID EnableFrame(
        ID3DXFont* lpD3DFrameFont,
        BOOL bEven,
        RECT& rc,
        TCHAR* szFormatEven = _T("帧:%d每秒\n"),
        TCHAR* szFormat = _T("帧:%4.3lf每秒\n帧:%d每秒\n耗:%4.3lf毫秒\n"),
        D3DCOLOR D3DColor = ARGB(255, 255, 0, 0)
        );

// 私有
protected:
    virtual BOOL ForeStop(VOID);        // 停止之前
    virtual BOOL ThreadCallBack(VOID);  // 渲染线程
    VOID UpdataFrame(VOID);             // 更新帧数

    LPDIRECT3DDEVICE9   m_lpDirectDevice9;  // D3D9设备指针
    BOOL                m_bInit;            // 是否已初始化
    CMsIni*             m_pMsIni;           // 配置文件
    CMsLog*             m_pMsLog;           // 日志管理
    BOOL                m_bQuit;            // 是否退出
    HWND                m_hWnd;
    DWORD               m_dwScreenWidth;
    DWORD               m_dwScreenHeight;

    ID3DXFont*          m_lpD3DFrameFont;   // 帧数显示字体
    DOUBLE              m_lfConsumeTime;    // 渲染耗时
    RECT                m_FrameRect;        // 帧数显示位置
    BOOL                m_bRenderFrame;     // 是否渲染帧数
    BOOL                m_bEven;            // 是否平滑显示
    TCHAR               m_szFrameBuff[MAX_PATH];
    TCHAR               m_szFormatFrame[MAX_PATH];
    D3DCOLOR            m_D3DColor;         // 帧数显示颜色

    INT64               m_n64TimeEvenA;     // 平滑渲染计时A
    INT64               m_n64TimeEvenB;     // 平滑渲染计时B
    DWORD               m_dwFrameEvenCount; // 平滑帧数

    INT64               m_n64TimeA;         // 渲染计时A
    INT64               m_n64TimeB;         // 渲染计时B
    DWORD               m_dwFrameCount;     // 同一时间渲染帧数
    DWORD               m_dwRenderFrame;    // 渲染帧数
    DOUBLE              m_lfRenderFrame;    // 渲染帧数
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
