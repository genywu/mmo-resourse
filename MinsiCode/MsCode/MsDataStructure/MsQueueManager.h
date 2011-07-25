#pragma once                //┃//┃
#ifndef __MSQUEUEMANAGER_H__//┃//┃
#define __MSQUEUEMANAGER_H__//┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsQueueManager.h    //┃
//功能描述：队列管理相关封装    //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 池管理器
class CMsQueueManager
{
public:
    enum E_ERROR_CODE
    {
        ERROR_UNKNOW = -1,          // 未知错误
        ERROR_NO_ERROR = 0,         // 无错误
        ERROR_NO_INIT,              // 队列未初始化
        ERROR_ISFULL,               // 队列满了
        ERROR_ISEMPTY,              // 队列空了
        ERROR_OPERATE_EXCEPTION,    // 运行异常
    };

    // 构造函数
    CMsQueueManager(DWORD dwMaxCount, BOOL bAutoLock = TRUE);

    // 析构函数
    ~CMsQueueManager(VOID);

    VOID operator = (CMsQueueManager){AssertEx(AL_NORMAL, FALSE, _T("禁用赋值运算符"));}
public:
    BOOL    Init(VOID);                                 // 池子管理器初始化函数
    BOOL    Release(VOID);                              // 释放池子管理器
    DWORD   GetCurrentCount(VOID);                      // 获取当前元素个数
    BOOL    IsFull(VOID);                               // 检查是否已经满了
    BOOL    IsEmpty(VOID);                              // 检查是否已经空了
    BOOL    ClearAll(VOID);

    // 压入数据(数据靠内部存储)
    BOOL    PushData(LPVOID lpData, DWORD dwSize);

    // 压入数据(数据靠外部存储)
    BOOL    PushData(LPVOID lpData);
    
    // 获取数据(数据外部存储时,尺寸为0)
    LPVOID          GetData(DWORD& dwSize);
    VOID            DeleteData(VOID);                   // 删除数据
    VOID            QueueLock(VOID);                    // 锁定队列
    VOID            QueueUnLock(VOID);                  // 解锁队列
    ULONG_PTR       GetNextIndex(BOOL bPossess = TRUE); // 获取下个可用索引位置
    E_ERROR_CODE    GetLastErrorCMsQueueManager(VOID);  // 获取错误代码

private:
    MsLock          m_MsQueueLock;              // 队列访问锁
    CONST DWORD     m_dwQueueMemberMaxCount;    // 队列成员最大个数
    LPVOID*         m_lppQueueMemberData;       // 队列成员数组
    LPDWORD         m_lpdwQueueMemberSize;      // 队列成员尺寸数组
    DWORD           m_dwQueueCurrentCount;      // 队列当前成员个数
    DWORD           m_dwQueueCurrentFirstUse;   // 队列当前第一个已用位置
    DWORD           m_dwQueueCurrentFirstFree;  // 队列当前第一个空闲位置
    LPFILETIME      m_lpLandTime;               // 队列成员登陆时间数组
    SYSTEMTIME      m_stLandTime;               // 登陆时间
    E_ERROR_CODE    m_eQueueErrorCode;          // 队列错误代码
    BOOL            m_bInit;                    // 是否已经初始化
    CONST BOOL      m_bLock;                    // 是否进行多线程加锁
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
