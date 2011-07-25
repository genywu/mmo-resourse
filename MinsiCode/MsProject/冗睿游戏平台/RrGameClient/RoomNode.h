#pragma once                //┃//┃
#ifndef __ROOMNODE_H__      //┃//┃
#define __ROOMNODE_H__      //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：RoomNode.h          //┃
//功能描述：大区节点            //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 房间节点
class CRoomNode
    : public CBaseNode
{
public:
    CRoomNode(VOID);
    ~CRoomNode(VOID);

    // 初始化
    virtual BOOL Init(DWORD dwDeskNodeCount);

    // 渲染
    virtual VOID Render(IN OUT INT& nBottom);
private:

};
typedef CRoomNode*  lpCRoomNode;


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
