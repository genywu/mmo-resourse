#pragma once                //┃//┃
#ifndef __BASENOD_H__       //┃//┃
#define __BASENOD_H__       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：DeskNode.h          //┃
//功能描述：桌子节点            //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

//
////根节点位置
//#define TREE_X 10
//#define TREE_Y 10
//#define TREE_W 40
//#define TREE_H 20
//
////滚动条范围
//#define SCROLLBARUP     40
//#define SCROLLBARDOWN   640
//
////滚动条高度
//#define SCROLLBARHIGHT  60
//
////节点显示范围
//#define TREESHOWUP TREE_Y + TREE_H
//
//#define TREESHOWDOWN 700

// 头节点宏定义
#define HEAD_NODE   (CBaseNode*)INVALID_POINTER

// 尾节点宏定义
#define TAIL_NODE   (CBaseNode*)INVALID_POINTER

// 节点基类
class CBaseNode
{
public:
    CBaseNode(CBaseNode* lpPreviousNode = HEAD_NODE, CBaseNode* lpNextNode = TAIL_NODE);
    virtual ~CBaseNode(VOID);
    virtual VOID Open(BOOL bPrint);     // 打开节点
    virtual VOID Close(BOOL bPrint);    // 关闭节点
    virtual VOID Destroy(VOID);         // 销毁节点
    virtual VOID CutMe(VOID);           // 从链表中断开自己

public:
    LPRECT      GetRect(VOID);          // 节点位置
    BOOL        IsOpen(VOID);           // 是否打开
    DWORD       GetNodeId(VOID);        // 获得节点Id
    CBaseNode*  GetNextNode(VOID);      // 获取下一个节点
    CBaseNode*  GetPreviousNode(VOID);  // 获取前一个节点

protected:
    CBaseNode*  m_lpNextNode;       // 下一个节点
    CBaseNode*  m_lpPreviousNode;   // 前一个节点

    RECT        m_Rect;         // 节点位置
    BOOL        m_bOpen;        // 是否展开节点
    DWORD       m_dwWidth;      // 节点宽度
    DWORD       m_dwHight;      // 节点高度
    DWORD       m_dwIndent;     // 节点缩进
    DWORD       m_dwBetween;    // 节点间距
    DWORD       m_eNodeType;    // 节点类型
    DWORD       m_dwNodeId;     // 节点Id
};
typedef CBaseNode* lpCBaseNode;
//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
