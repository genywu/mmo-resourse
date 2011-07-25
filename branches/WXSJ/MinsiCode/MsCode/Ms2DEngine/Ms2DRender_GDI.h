#pragma once                //┃//┃
#ifndef __MS2DRENDER_GDI_H__//┃//┃
#define __MS2DRENDER_GDI_H__//┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：Ms2DRender_GDI.h    //┃
//功能描述：2D游戏引擎GDI渲染   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

class CMs2DRender_GDI
{
public: // 构造、析构
    CMs2DRender_GDI(VOID);
    ~CMs2DRender_GDI(VOID);
public: // 接口

    // 初始化
    BOOL Init(VOID);

    // 释放
    BOOL Release(VOID);

protected:// 私有

    BOOL        m_bInit;        // 是否已初始化
    CMsIni*     m_pMsIni;       // 配置文件
    CMsLog*     m_pMsLog;       // 日志管理
    LONG        m_lScreenWidth; // 窗口宽度
    LONG        m_lScreenHeight;// 窗口高度
    BOOL        m_bQuit;        // 是否退出

    // 窗口名称
    TCHAR       m_szWndName[MAX_PATH];

    BOOL        m_bActive;      // 是否被激活
    BOOL        m_bHideCursor;  // 是否隐藏光标
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
