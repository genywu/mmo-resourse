#pragma once                    //┃//┃
#ifndef __CMSIOCPMANAGER_H__    //┃//┃
#define __CMSIOCPMANAGER_H__    //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：CMsIocpManager.h        //┃
//功能描述：完成端口管理器          //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 完成端口返回的状态枚举
enum PORT_STATUS
{
    COMPLETIONPORT_STATUS_UNKNOW = 0,   // 未知状态
    COMPLETIONPORT_STATUS_NOTHING,      // 无事状态
    COMPLETIONPORT_STATUS_UPDATAED,     // 有数据更新
    COMPLETIONPORT_STATUS_EXIT,         // 连接被主动退出
    COMPLETIONPORT_STATUS_ERROR,        // 连接错误

    COMPLETIONPORT_STATUS_COERCE_DWORD = 0x7FFFFFFF,
};

// 完成端口更新通知结构体
struct S_IOCP_UPDATE
    : public OVERLAPPED
{
    ULONG_PTR   m_ulpCompletionKey;
    DWORD       m_dwNumOfBytesComplete;
    PORT_STATUS m_PortStatus;
};

class CMsIocpManager
    : public CMsThread
{
public: // 构造、析构
    CMsIocpManager(TCHAR* szIocpLogFileName, DWORD dwThreadId);
    virtual ~CMsIocpManager(VOID);

public: // 接口
    BOOL AssociateDevice(   // 绑定指定句柄到完成端口对象
        HANDLE hDevice,
        ULONG_PTR CompletionKey
        );
    BOOL AssociateDevice(   // 绑定指定套接字到完成端口对象
        SOCKET hSocket,
        ULONG_PTR CompletionKey
        );

    // 获取完成包状态
    PORT_STATUS GetCompletionStatus(
        ULONG_PTR* pCompletionKey,
        DWORD& dwNumOfBytesReceive,
        S_IOCP_UPDATE** lppOverlapped,
        DWORD dwWaitTime
        );

    // 停止之前
    virtual BOOL ForeStop(VOID);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent, BOOL bReady);

    S_IOCP_UPDATE* GetRecvOverlapped(VOID); // 获取接收重叠IO结构体
    S_IOCP_UPDATE* GetSendOverlapped(VOID); // 获取发送重叠IO结构体

    BOOL PostCompletionStatus(              // 投递完成包状态
        ULONG_PTR CompletionKey,
        DWORD dwNumBytes,
        S_IOCP_UPDATE *pOverlapped
        );

    // 线程回调
    virtual BOOL ThreadCallBack(VOID);

private:// 私有

    BOOL Init(VOID);            // 初始化完成端口
    BOOL Release(VOID);         // 释放完成端口
    VOID LogIocp(TCHAR* szLog); // 记录日志

    BOOL            m_IsInit;               // 是否已经初始化
    HANDLE          m_hCompetionPort;       // 完成端口句柄
    CMsLog          m_MsIocpLog;            // 完成端口日志
    DWORD           m_dwParentThreadId;     // 调用者的线程Id
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
