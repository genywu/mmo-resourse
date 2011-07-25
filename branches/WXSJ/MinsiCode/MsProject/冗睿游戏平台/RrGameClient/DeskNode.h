#pragma once                //┃//┃
#ifndef __DESKNODE_H__      //┃//┃
#define __DESKNODE_H__      //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：DeskNode.h          //┃
//功能描述：桌子节点            //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 桌子节点
class CDeskNode
    : public CBaseNode
{
public:
    CDeskNode(VOID);
    ~CDeskNode(VOID);

    // 初始化
    BOOL Init(DWORD dwPlayerCount);

    // 渲染
    VOID Render(HDC hDC);

private:
    DWORD   m_dwPlayerCount;

};
typedef CDeskNode*  lpCDeskNode;


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
