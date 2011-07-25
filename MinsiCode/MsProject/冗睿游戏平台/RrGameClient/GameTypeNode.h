#pragma once                //┃//┃
#ifndef __GAMETYPENODE_H__  //┃//┃
#define __GAMETYPENODE_H__  //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：GameTypeNode.h      //┃
//功能描述：游戏类型节点        //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 类型节点
class CGameTypeNode
    : public CBaseNode
{
public:
    CGameTypeNode(CMsGdiBuffImage* lpCMsGdiBuffImage, CBaseNode* lpPreviousNode, CBaseNode* lpNextNode);
    ~CGameTypeNode(VOID);

    virtual VOID Open(BOOL bPrint);             // 打开节点
    virtual VOID Close(BOOL bPrint);            // 关闭节点
    virtual VOID Destroy(VOID);                 // 销毁节点
    virtual VOID CutMe(VOID);                   // 从链表中断开自己

public:

    // 初始化
    BOOL Init(S_GAME_TYPE_INFO* pInfo);

    // 绘制节点 
    VOID Render(IN OUT INT& nBottom);

private:
    S_GAME_TYPE_INFO    m_GameTypeInfo;     // 游戏类型结构体
    CMsGdiPlusImage*    m_pIocnImage;       // 光标图片对象
    CMsGdiPlusImage*    m_pLinkImage;       // 连接图片对象
};

typedef CGameTypeNode* lpCGameTypeNode;

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
