#pragma once                //┃//┃
#ifndef __DISTRICNODE_H__   //┃//┃
#define __DISTRICNODE_H__   //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：DistricNode.h       //┃
//功能描述：大区节点            //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 大区节点
class CDistricNode 
    : public CBaseNode
{
public:
    CDistricNode(VOID);
    ~CDistricNode(VOID);

    // 初始化
    virtual BOOL Init(DWORD dwRoomNodeCount);

    // 渲染
    virtual VOID Render(IN OUT INT& nBottom);
private:

};
typedef CDistricNode*  lpCDistricNode;


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
