#pragma once                    //┃//┃
#ifndef __MSSERVERNETMANAGER_H__//┃//┃
#define __MSSERVERNETMANAGER_H__//┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsServerNetManager.h    //┃
//功能描述：服务器网络管理器        //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 每帧监听连接的最大个数
#define MAX_COUNT_LISTEN_IN_ONE_FRAME   5

// 每帧处理完成端口更新最大个数
#define MAX_COUNT_UPDATE_IOPC_MSG       100

class CMsServerNetManager
    : public CMsMsgThread
{
public:
    // 构造
    CMsServerNetManager(
        TCHAR* szIP,
        WORD wProt,
        DWORD dwLogicThreadId,
        DWORD dwMaxClientCount,
        CMsPacketFactory* pCMsPacketFactory
        );

    // 析构
    virtual ~CMsServerNetManager(VOID);

    // 记录日志
    VOID LogServerNet(TCHAR* szLog);

    // 判断是否有效
    BOOL IsValid(VOID);

    // 接受客户端
    virtual BOOL NetAccept(VOID);

    // 停止之前
    virtual BOOL ForeStop(VOID);

    // 消息处理
    virtual VOID OnThreadMsgExecute(LPMSG lpMsg);

    // 空闲处理
    virtual VOID OnThreadFreeWork(VOID);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent);

    // 获取指定客户端套接字
    lpCMsClientSocket GetClientSocket(DWORD dwIndexByPool);

    // 获取当前客户端连接个数
    DWORD GetCurrentClientCount(VOID);

    // 断开服务器
    BOOL StopNet(VOID);

private:

    // 启动服务器
    BOOL RunNet(VOID);


    // 接受连接
    BOOL Accept(VOID);

    // 删除一个客户端
    BOOL RemoveClient(CMsClientSocket* pCMsClientSocket);

    DWORD               m_dwLogicThreadId;  // 逻辑处理线程Id
    BOOL                m_bRunning;         // 是否已经运行中
    CMsPacketFactory*   m_pCMsPacketFactory;// 封包工厂管理器
    CMsIocpManager*     m_lpCMsIocpManager; // 完成端口管理器
    CMsServerSocket*    m_pMsServerSocket;  // 服务器套接字对象
    lpCMsClientSocket*  m_ppMsClientSocket; // 客户端套接字数组
    CMsPoolManager*     m_pCMsPoolManager;  // 索引池子管理器
    ACHAR               m_szIp[IP_SIZE];    // 服务器IP
    WORD                m_wProt;            // 服务器端口
    sockaddr_in         m_ServerAddr;
    CMsLog              m_MsServerNetLog;   // 网络管理器日志
};

typedef CMsServerNetManager* lpCMsServerNetManager;

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛