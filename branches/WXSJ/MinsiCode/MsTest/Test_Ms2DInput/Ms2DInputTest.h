#pragma once
#include "resource.h"

class CMsInputTest
    : public CMs2DEngine
{
public:
    // 创建
    virtual BOOL Ms2D_CallBack_Create(OUT S_2DEngineWndInfo& s_ewi);

    // 销毁
    virtual BOOL Ms2D_CallBack_Destroy(VOID);

    // 绘制
    virtual VOID Ms2D_CallBack_Render(HWND hWnd, HDC hDc);

    // 逻辑
    virtual VOID Ms2D_CallBack_LogicTick(CMs2DInputEvent* pEvent);

    // 激活
    virtual VOID Ms2D_CallBack_Activate(VOID);


};