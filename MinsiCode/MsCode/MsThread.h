#pragma once            //┃//┃
#ifndef __MSTHREAD_H__  //┃//┃
#define __MSTHREAD_H__  //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsThread.h      //┃
//功能描述：线程基本对象    //┃
//平台相关：WINDOWS         //┃
//━━━━━━━━━━━━━━┛

// 线程事件通知
enum E_THREAD_EVENT_NOTIFY
{
    E_ON_THREAD_RUN,
    E_ON_THREAD_SUSPEND,
};

class CMsThread
{
// 公有
public:
    CMsThread();
    virtual ~CMsThread();

    // 运行线程
    virtual BOOL Run(VOID);

    // 挂起
    virtual BOOL Suspend(VOID);

    // 停止之前
    virtual BOOL ForeStop(VOID) = 0;

    // 停止线程
    virtual BOOL Stop(BOOL IsBackup = FALSE, BOOL bForce = FALSE, DWORD dwWaitMillisecond = 100);

    // 线程回调
    virtual BOOL ThreadCallBack(VOID);

    // 获取线程Id
    DWORD GetMsThreadId(VOID);

// 私有
protected:

    static DWORD WINAPI CMsThreadCallBack(LPVOID lpThreadParameter);

    // 是否正在运行
    BOOL    m_IsRunning;

    // 是否停止运行
    BOOL    m_bStop;

    // 线程ID
    DWORD   m_dwThreadId;

    // 线程句柄
    HANDLE  m_hThread;
};









//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
