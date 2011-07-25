#include "Precompiled.h"

CMsMsgThread::CMsMsgThread(VOID)
{
    ZeroMemoryStruct(m_Msg);
    ::PeekMessage(&m_Msg, NULL, 0, 0, 0);
}

CMsMsgThread::~CMsMsgThread(VOID)
{
    ;
}

// 运行线程
BOOL CMsMsgThread::Run(VOID)
{
    BOOL bRet = FALSE;
    if (m_bStop)
    {
        bRet = CMsThread::Run();
        if (bRet)
        {
            bRet = FALSE;
            while (!bRet)
            {
                bRet = ::PostThreadMessage(m_dwThreadId, MSM_MSG_THREAD_ON_RUN, 0, 0);
                ::SleepEx(10, TRUE);
            }
        }
    }
    return TRUE;
}


// 挂起
BOOL CMsMsgThread::Suspend(VOID)
{
    BOOL bRet = FALSE;
    bRet = CMsThread::Suspend();
    if (bRet)
    {
        return ::PostThreadMessage(m_dwThreadId, MSM_MSG_THREAD_ON_SUSPEND, 0, 0);
    }
    return FALSE;
}

// 停止线程
BOOL CMsMsgThread::Stop(BOOL IsBackup, BOOL bForce, DWORD dwWaitMillisecond)
{
    if (!m_bStop)
    {
        return CMsThread::Stop(IsBackup, bForce, dwWaitMillisecond);
    }
    return TRUE;
}

// 线程回调
BOOL CMsMsgThread::ThreadCallBack(VOID)
{
    BOOL bRet = TRUE;
    // 停止之前循环处理
    while (!m_bStop)
    {
        m_Msg.message = MSM_NOTHING;
        bRet = ::PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE);
        if (!bRet)
        {
            this->OnThreadFreeWork();
            ::SleepEx(10, TRUE);
            continue;
        }
        switch (m_Msg.message)
        {
            // 停止消息线程
        case MSM_MSG_THREAD_STOP:
            {
                return this->Stop();
            }
            break;

            // 无事通知
        case MSM_NOTHING:
            {
                ;
            }break;

            // 通知线程运行
        case MSM_MSG_THREAD_ON_RUN:
            {
                this->OnEventNotify(E_ON_THREAD_RUN);
            }break;

            // 通知线程挂起
        case MSM_MSG_THREAD_ON_SUSPEND:
            {
                this->OnEventNotify(E_ON_THREAD_SUSPEND);
            }break;

            // 默认处理
        default:
            {
                this->OnThreadMsgExecute(&m_Msg);
            }
        }
    }
    return TRUE;
}

// 投递消息给线程
BOOL CMsMsgThread::PostMsg(E_MSM e_Msm, WPARAM wParam, LPARAM lParam)
{
    if (m_bStop)
    {
        return INVALID_UID;
    }
    else
    {
        return ::PostThreadMessage(m_dwThreadId, e_Msm, wParam, lParam);
    }
}