#pragma once                    //┃//┃
#ifndef __CMSCLIENTSOCKET_H__   //┃//┃
#define __CMSCLIENTSOCKET_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：CMsClientSocket.h       //┃
//功能描述：网络终端套接字封装      //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 网络终端类
class CMsClientSocket
    : public CMsSocket
{
// 公有
public:

    // 构造、析构
    CMsClientSocket(
        DWORD dwBuffSize,
        DWORD dwLogicThreadId,
        DWORD dwNetThreadId,
        CMsPacketFactory* lpFactory
        );
    virtual ~CMsClientSocket(VOID);

// 公有
public:

    // 设置套接字
    VOID SetSocket(SOCKET Socket);
protected:// 保护
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
