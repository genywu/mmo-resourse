#pragma once                    //┃//┃
#ifndef __CMSSERVERSOCKET_H__   //┃//┃
#define __CMSSERVERSOCKET_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：CMsServerSocket.h       //┃
//功能描述：网络终端套接字封装      //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 网络终端类
class CMsServerSocket
    : public CMsSocket
{
// 公有
public:

    // 构造、析构
    CMsServerSocket(
        DWORD dwBuffSize,
        DWORD dwLogicThreadId,
        DWORD dwNetThreadId,
        CMsPacketFactory* lpFactory
        );
    virtual ~CMsServerSocket(VOID);

protected:// 保护
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
