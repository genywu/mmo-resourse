#pragma once                //┃//┃
#ifndef __MSRECVBUFFER_H__  //┃//┃
#define __MSRECVBUFFER_H__  //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsRecvBuffer.h      //┃
//功能描述：接收缓冲区          //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 发送缓冲区
struct MsRecvBuffer
{
    // 构造,析构
    MsRecvBuffer(DWORD dwPacketCount, DWORD dwMaxBuffSize);
    ~MsRecvBuffer(VOID);
    VOID operator = (MsRecvBuffer){AssertEx(AL_NORMAL, FALSE, _T("禁用赋值运算符"));}

    // 获取接收位置、尺寸
    LPBYTE  GetRecvAddr(DWORD& dwSize);

    // 获取空闲尺寸
    DWORD   GetFreeSize(VOID);

    // 重置缓冲区
    VOID    Reset(VOID);

    // 压入数据
    BOOL    Push(DWORD dwSize);

    // 弹出数据
    VOID    Pop(DWORD dwSize);

    // 完成执行
    VOID    CompleteExe(VOID);

    // 获取执行封包
    VOID    GetExePacket(OUT lpPacketHead& lpPacketHead, OUT LPVOID& lpData);

    // 获取完成包个数
    DWORD   GetCompletePacketCount(VOID);

    // 设置套接字指针
    VOID    SetSocket(CMsSocket* lpSocket);

    struct RecvPacketBuff
    {
        PacketHead  m_PacketHead;   // 包头
        LPBYTE      m_lpPacketData; // 包数据
        BOOL        m_IsValid;      // 有效的
    }*              m_lpPacketHead; // 包数组
    CONST DWORD     m_dwPacketCount;// 包头缓冲个数

    DWORD   m_dwCompletePacketCount;    // 完成包个数
    DWORD   m_dwCurrentRecvPacketIndex; // 当前接收包索引
    DWORD   m_dwCurrentExePacketIndex;  // 当前执行包索引
    DWORD   m_dwExeingPacketIndex;      // 当前正在执行包的索引

    enum E_RECV_BUFF_STATE
    {
        RECV_HEAD,
        RECV_DATA,
    }               m_eRecvState;           // 接收状态
    DWORD           m_dwRecvedSize;         // 已接收尺寸

    LPBYTE          m_lpBuffHead;           // 缓冲区头
    LPBYTE          m_lpBuffTail;           // 缓冲区尾
    CONST DWORD     m_dwMaxBuffSize;        // 缓冲区最大尺寸

    CMsSocket*      m_lpSocket;             // 套接字指针
    MsLock          m_Lock;                 // 线程锁

    LPBYTE          m_pDataHead;            // 数据头指针
    LPBYTE          m_pDataTail;            // 数据尾指针
    DWORD           m_dwCurrentDataSize;    // 当前数据尺寸
};


// 接收缓冲区
struct MsSendBuffer
{
    // 构造,析构
    MsSendBuffer(DWORD dwMaxBuffSize);
    ~MsSendBuffer(VOID);
    VOID operator = (MsSendBuffer){AssertEx(AL_NORMAL, FALSE, _T("禁用赋值运算符"));}

    // 获取发送位置、尺寸
    LPBYTE  GetSendAddr(DWORD& dwSize);

    // 获取数据尺寸
    DWORD   GetDataSize(VOID);

    // 获取空闲尺寸
    DWORD   GetFreeSize(VOID);

    // 重置缓冲区
    VOID    Reset(VOID);

    // 压入数据
    BOOL    Push(LPBYTE lpData, DWORD dwSize);

    // 弹出数据
    VOID    Pop(DWORD dwSize);

    // 是否正在发送
    BOOL    IsSending(VOID);

    // 设置是否正在发送
    VOID    SetSending(BOOL bSending);

    LPBYTE          m_lpBuffHead;           // 缓冲区头
    LPBYTE          m_lpBuffTail;           // 缓冲区尾
    CONST DWORD     m_dwMaxBuffSize;        // 缓冲区最大尺寸

    BOOL            m_bSending;             // 是否正在发送
    MsLock          m_Lock;                 // 线程锁

    LPBYTE          m_pDataHead;            // 数据头指针
    LPBYTE          m_pDataTail;            // 数据尾指针
    DWORD           m_dwCurrentDataSize;    // 当前数据尺寸
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
