#include "Precompiled.h"
#include "Ms2DEngineDef.h"
#include "Ms2DInput.h"
#include "Ms2DEngine.h"
#include "Ms2DInputTest.h"

CMsInputTest g_CMsInputTest;

// 创建窗口
BOOL CMsInputTest::Ms2D_CallBack_Create(S_2DEngineWndInfo& s_ewi)
{
    s_ewi.m_dwWidth = 800;
    s_ewi.m_dwHeight= 600;
    return this->Init(_T("Config.ini"), _T("Log\\Ms2DInputTest.log"), _T("2D引擎输入测试"));
}

// 销毁窗口
BOOL CMsInputTest::Ms2D_CallBack_Destroy(VOID)
{
    return TRUE;
}

VOID CMsInputTest::Ms2D_CallBack_Render(HWND hWnd, HDC hDc)
{
    ::TextOut(hDc, 0, 0, _T("渲染"), 2);
}

// 逻辑
VOID CMsInputTest::Ms2D_CallBack_LogicTick(CMs2DInputEvent* pEvent)
{
    HDC hDc = ::GetDC(this->Ms2DEngine_GetState(M2DE_HWND));
    switch (pEvent->eType)
    {
    case INPUT_KEYDOWN:
        {
            DWORD dwNameLen;
            TCHAR* lpKeyName = m_pCMs2DInput->Ms2DInput_GetKeyName(pEvent->dwKey, dwNameLen, TRUE);
            ::TextOut(hDc, 0, 30, _T("　　　　　　　　　　　　"), 12);
            ::TextOut(hDc, 0, 30, lpKeyName, dwNameLen);
        }break;
    case INPUT_MBUTTONDOWN:
        {
            switch (pEvent->dwKey)
            {
            case MS2DK_EBUTTON:
                ::TextOut(hDc, 0, 0, _T("鼠标扩展按下"), 6);
                break;
            case MS2DK_LBUTTON:
                ::TextOut(hDc, 0, 0, _T("鼠标左键按下"), 6);
                break;
            case MS2DK_RBUTTON:
                ::TextOut(hDc, 0, 0, _T("鼠标右键按下"), 6);
                break;
            case MS2DK_MBUTTON:
                ::TextOut(hDc, 0, 0, _T("鼠标中键按下"), 6);
                break;
            }
        }break;
    }
}

// 激活
VOID CMsInputTest::Ms2D_CallBack_Activate(VOID)
{
    HDC hDc = ::GetDC(this->Ms2DEngine_GetState(M2DE_HWND));
    ::TextOut(hDc, 0, 20, _T("窗口被激活"), 5);
}