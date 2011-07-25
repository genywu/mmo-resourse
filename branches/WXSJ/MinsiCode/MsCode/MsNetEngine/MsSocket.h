#pragma once                //┃//┃
#ifndef __CMSSOCKET_H__     //┃//┃
#define __CMSSOCKET_H__     //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：CMsSocket.h         //┃
//功能描述：网络基本套接字封装  //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 临时宏定义接收缓冲封包个数
#define TEMP_PACKET_RECV_COUNT  100

// 基本套接字
class CMsSocket
{
// 公有
public:

    // 构造、析构
    CMsSocket(
        DWORD dwBuffSize,
        DWORD dwLogicThreadId,
        DWORD dwNetThreadId,
        CMsPacketFactory* lpFactory
        );
    virtual ~CMsSocket(VOID);

// 公有
public:
    SOCKET  GetSocket(VOID);// 获取套接字
    DWORD   GetProt(VOID);  // 获取套接字端口
    ACHAR*  GetAddr(VOID);  // 获取套接字地址
    BOOL    Release(VOID);  // 释放
    BOOL    Reset(VOID);    // 重置

    virtual BOOL ExecutePacket(VOID);  // 执行包
    virtual BOOL OnEnter(VOID);        // 进入通知
    virtual BOOL OnExit(VOID);         // 退出通知
    
    // 读取封包,准备执行
    BOOL ReadPacket(VOID);

    // 写入封包,准备发送
    BOOL WritePacket(
        PACKET_TYPE PacketId,
        LPVOID lpData,
        DWORD dwSize = INVALID_UID
        );

    // 写入封包,准备发送
    BOOL WritePacket(
        CMsPacketFactory* lpFactory,
        PACKET_TYPE PacketId,
        LPVOID lpData,
        DWORD dwSize = INVALID_UID
        );

    LPVOID  GetPacketData(VOID);                    // 获取封包数据
    BOOL    OnUpdate(S_IOCP_UPDATE* lpIocpUpdate);  // 更新接收
    DWORD   GetLogicThreadId(VOID);                 // 获取逻辑处理线程Id
    DWORD   GetNetThreadId(VOID);                   // 获取网络处理线程Id
    FILETIME& GetTime(VOID);                        // 获取终端时间
    VOID    UpdataTime(VOID);                       // 更新终端时间
    BOOL    IsValid(VOID);                          // 判断是否有效
    VOID    SetPoolIndex(ULONG_PTR ulIndex);        // 设置池子索引
    ULONG_PTR GetPoolIndex(VOID);                   // 获取池子索引
    S_IOCP_UPDATE* GetRecvIO(VOID);                 // 获取接收IO结构体

// 保护
protected:

    BOOL RecvData(VOID);    // 接收数据(仅限自己分配调用)
    BOOL SendData(VOID);    // 发送数据(仅限自己分配调用)
    BOOL OnUpdateRecv(DWORD dwNumOfBytesUpdate);    // 更新接收(仅限自己分配调用)
    BOOL OnUpdateSend(DWORD dwNumOfBytesUpdate);    // 更新发送(仅限自己分配调用)

    DWORD           m_dwLogicThreadId;  // 逻辑处理线程Id
    DWORD           m_dwNetThreadId;    // 网络处理线程Id

    ULONG_PTR       m_ulIndexInPool;    // 索引

    SOCKET          m_Socket;           // 套接字
    SOCKADDR_IN     m_Sockaddr;         // 套接字地址

    MsRecvBuffer    m_MsRecvBuff;       // 接收缓冲区
    MsSendBuffer    m_MsSendBuff;       // 发送缓冲区

    S_IOCP_UPDATE   m_RecvOverlapped;   // 接收重叠IO
    UINT64          m_u64RecvSize;      // 接收IO流量

    S_IOCP_UPDATE   m_SendOverlapped;   // 发送重叠IO
    UINT64          m_u64SendSize;      // 发送IO流量

    LPVOID          m_lpPacketData;     // 执行包数据
    CMsBasePacket*  m_lpPacket;         // 执行包指针

    FILETIME        m_Time;             // 终端64位时间
    MsLock          m_Lock;             // 终端访问锁

    CMsPacketFactory* m_lpFactory;      // 封包工厂指针
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
