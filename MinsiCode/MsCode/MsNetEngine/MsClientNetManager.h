#pragma once                    //┃//┃
#ifndef __MSCLIENTNETMANAGER_H__//┃//┃
#define __MSCLIENTNETMANAGER_H__//┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsClientNetManager.h    //┃
//功能描述：客户端网络管理器        //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

class CMsClientNetManager
    : public CMsMsgThread
{
public:
    // 构造
    CMsClientNetManager(
        TCHAR* szIP,
        WORD wProt,
        DWORD dwLogicThreadId,
        CMsPacketFactory* pCMsPacketFactory
        );

    // 析构
    virtual ~CMsClientNetManager(VOID);

    // 日志
    VOID LogClientNet(TCHAR* szLog);

    // 判断是否有效
    BOOL IsValid(VOID);

    // 连接服务器
    BOOL Connect(VOID);

    // 断开服务器
    BOOL Disconnect(VOID);

    // 停止之前
    virtual BOOL ForeStop(VOID);

    // 消息处理
    virtual VOID OnThreadMsgExecute(LPMSG lpMsg);

    // 空闲处理
    virtual VOID OnThreadFreeWork(VOID);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent);

    // 获取服务器套接字
    lpCMsServerSocket GetServerSocket(VOID);
private:

    // 逻辑处理线程函数
    LPTHREAD_START_ROUTINE  m_lpLogicThread;

    // 逻辑处理线程Id
    DWORD               m_dwLogicThreadId;

    BOOL                m_bConnected;       // 是否已经连接上
    CMsPacketFactory*   m_pCMsPacketFactory;// 封包工厂管理器
    CMsIocpManager*     m_lpCMsIocpManager; // 完成端口管理器
    CMsServerSocket*    m_pMsServerSocket;  // 服务器套接字对象
    ACHAR               m_szIp[IP_SIZE];    // 服务器IP
    WORD                m_wProt;            // 服务器端口
    sockaddr_in         m_ServerAddr;
    CMsLog              m_MsClientNetLog;   // 网络管理器日志
};

typedef CMsClientNetManager* lpCMsClientNetManager;

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
