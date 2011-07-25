#pragma once                //┃//┃
#ifndef __GAMENODE_H__      //┃//┃
#define __GAMENODE_H__      //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：GameNode.h          //┃
//功能描述：游戏节点            //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 游戏节点
class CGameNode
    : public CBaseNode
{
public:
    CGameNode(VOID);
    ~CGameNode(VOID);

    // 初始化
    virtual BOOL Init(DWORD dwDistricNodeCount);

    // 渲染
    virtual VOID Render(IN OUT INT& nBottom);
private:

};
typedef CGameNode*  lpCGameNode;


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
