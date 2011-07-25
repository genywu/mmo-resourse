#pragma once                //┃//┃
#ifndef __GAMETREE_H__      //┃//┃
#define __GAMETREE_H__      //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：GameTree.h          //┃
//功能描述：游戏列表树          //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 游戏列表树
class CGameTree
{
public:
    CGameTree(CMsGdiBuffImage* lpCMsGdiBuffImage);
    ~CGameTree(VOID);

    // 绘制节点
    VOID Render(VOID);

    // 销毁节点
    VOID Destroy(VOID);

    // 增加游戏类型
    CGameTypeNode* AddGameTypeNode(VOID);

    // 鼠标是否穿过滚动条
    BOOL IsOnScrollbar(POINT& Pos);

    // 记录鼠标位置
    VOID SaveMousePos(POINT& Pos);

    // 记录鼠标位置
    VOID OperationPosOffset(POINT& Pos);

    // 移动滚动条
    VOID MoveScroll(POINT& Pos);

    // 设置滚动条焦点
    VOID GetScrollFocus(VOID);

    // 释放滚动条焦点
    VOID ReleaseScrollFocus(VOID);

    // 是否滚动条焦点
    BOOL IsScrollFocus(VOID);

    // 重置节点
    VOID Reset(VOID);
private:

    // 列表树滚动条
    CGameTreeScrollbar* m_pCGameTreeScrollbar;

    // 滚动条焦点
    BOOL                m_bScrollFocus;

    // 列表树标题
    CGameTreeCaption*   m_pCGameTreeCaption;

    // 标题范围
    RECT                m_CaptionRc;

    // 背景缓冲图
    CMsGdiBuffImage*    m_lpCMsGdiBuffImage;

    // 原鼠标位置
    POINT   m_OldMousePos;

    // 当前鼠标位置
    POINT   m_CurMousePos;

    // 绘制最后位置
    INT     m_nLastBottom;

    // 游戏类型跟节点
    lpCGameTypeNode m_lpCGameTypeRoot;

    // 最后一个节点
    lpCGameTypeNode m_lpLastNode;

    // 游戏类型个数
    DWORD   m_dwGameTypeNodeCount;

    // 树访问锁
    MsLock  m_Lock;
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
