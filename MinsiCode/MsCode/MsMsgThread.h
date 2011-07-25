#pragma once                //┃//┃
#ifndef __MSMSGTHREAD_H__   //┃//┃
#define __MSMSGTHREAD_H__   //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsMsgThread.h       //┃
//功能描述：消息线程对象        //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛

// 等待线程启动最大超时时间
#define MAX_SLEEP_THREAD_TIME_MILLISECOND   1000

class CMsMsgThread
    : public CMsThread
{
// 公有
public:
    CMsMsgThread(VOID);
    ~CMsMsgThread(VOID);

    // 运行线程
    virtual BOOL Run(VOID);

    // 挂起
    virtual BOOL Suspend(VOID);

    // 停止线程
    virtual BOOL Stop(BOOL IsBackup = FALSE, BOOL bForce = FALSE, DWORD dwWaitMillisecond = 100);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent) = 0;

    // 空闲处理
    virtual VOID OnThreadFreeWork(VOID) = 0;

    // 消息处理
    virtual VOID OnThreadMsgExecute(LPMSG lpMsg) = 0;

    // 投递消息给线程
    BOOL PostMsg(E_MSM e_Msm, WPARAM wParam, LPARAM lParam);

// 私有
protected:
    MSG m_Msg;  // 消息

private:
    // 线程回调
    virtual BOOL ThreadCallBack(VOID);

};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
