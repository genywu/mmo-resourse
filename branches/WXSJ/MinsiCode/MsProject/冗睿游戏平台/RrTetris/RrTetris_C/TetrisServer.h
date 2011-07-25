#pragma once            //┃    //┃
#ifndef __SERVER_H__    //┃    //┃
#define __SERVER_H__    //┃    //┃
//━━━━━━━━━━━━┛    //┃
//文件名称：Player.h            //┃
//功能描述：服务器类封装        //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

#include "MsSocket.h"
class CClientNetManager;

class CServer
    :public CMsSocket
{
public: // 构造、析构

    CServer(VOID);  // 服务器构造函数
    ~CServer(VOID); // 服务器析构函数

public: // 接口

    // 消息处理
    BOOL    OnUpDataed(VOID);  // 数据更新状态
    BOOL    OnExit(VOID);      // 退出状态
    BOOL    OnError(VOID);     // 错误状态
    BOOL    WritePacket(CBasePacket* lpPacket);         // 将发送封包拷贝到待发送缓冲中
    USIZE   ReadPacket(CBasePacket* lpPacket);          // 将收到的数据拷贝到接收缓冲中
    BOOL    ExecutePacket(LPVOID pCClientNetManager);   // 执行收到的封包
private:// 私有

};


//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
