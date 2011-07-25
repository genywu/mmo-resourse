#pragma once                    //┃//┃
#ifndef __GAMETREESCROLLBAR_H__ //┃//┃
#define __GAMETREESCROLLBAR_H__ //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：GameTreeScrollbar.h     //┃
//功能描述：游戏列表滚动条          //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 滚动条宽度
#define SCROLL_BAR_WIDTH    20
// 滚动条高度
#define SCROLL_BAR_HEIGHT   50

enum E_DIRECTION
{
    TRANSVERSE      = 0x0001,   // 横向的
    LOGNITUDINAL    = 0x0002,   // 纵向的
};

// 游戏列表滚动条
class CGameTreeScrollbar
    : public CMsGdiPlusImage
{
public:
    CGameTreeScrollbar(CMsGdiBuffImage* lpCMsGdiBuffImage, INT nBasePosX, INT nBasePosY, DWORD dwWidth, DWORD dwHeight, DWORD eDirection);
    ~CGameTreeScrollbar(VOID);

    // 鼠标是否穿过滚动条
    BOOL IsOnScrollbar(POINT& Pos);

    // 移动滚动条
    VOID MoveScrollX(INT nOffsetX);
    VOID MoveScrollY(INT nOffsetY);

    // 滚动条最大值
    VOID AddMaxMoveScrollX(INT nMaxOffset);
    VOID SubMaxMoveScrollX(INT nMaxOffset);
    VOID AddMaxMoveScrollY(INT nMaxOffset);
    VOID SubMaxMoveScrollY(INT nMaxOffset);

    // 绘制滚动条
    BOOL DrawScrollbar(VOID);
private:
    DWORD   m_eDirection;   // 滚动条方向
    INT     m_nBasePosX;    // 滚动条位置X
    INT     m_nBasePosY;    // 滚动条位置Y
    INT     m_nOffsetX;     // 滚动条偏移X
    INT     m_nOffsetY;     // 滚动条偏移Y
    INT     m_nMaxOffsetX;  // 滚动条最大偏移X
    INT     m_nMaxOffsetY;  // 滚动条最大偏移Y
    DWORD   m_dwWidth;      // 滚动条宽度
    DWORD   m_dwHeight;     // 滚动条高度
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
