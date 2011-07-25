#include "Precompiled.h"

DWORD WINAPI CMsThread::CMsThreadCallBack(LPVOID lpThreadParameter)
{
    ((CMsThread*)lpThreadParameter)->m_IsRunning = TRUE;
    BOOL bRet = ((CMsThread*)lpThreadParameter)->ThreadCallBack();
    ((CMsThread*)lpThreadParameter)->m_IsRunning = FALSE;
    return bRet;
}

CMsThread::CMsThread()
: m_IsRunning   (FALSE)
, m_dwThreadId  (INVALID_UID)
, m_hThread     (NULL)
, m_bStop       (TRUE)
{
    m_hThread = ::CreateThread(NULL, 0, CMsThreadCallBack, this, CREATE_SUSPENDED, &m_dwThreadId);
    AssertEx(AL_CATAST, m_hThread, _T("创建线程失败"));
}

CMsThread::~CMsThread()
{
    ;
}

// 运行线程
BOOL CMsThread::Run(VOID)
{
    if (m_bStop)
    {
        m_bStop = FALSE;
        ::ResumeThread(m_hThread);
        if (!m_hThread)
        {
            return FALSE;
        }
    }
    return TRUE;
}

// 挂起
BOOL CMsThread::Suspend(VOID)
{
    DWORD dwRet = ::SuspendThread(m_hThread);
    if (INVALID_UID == dwRet)
    {
        return FALSE;
    }
    return TRUE;
}

// 停止线程
BOOL CMsThread::Stop(BOOL IsBackup, BOOL bForce, DWORD dwWaitMillisecond)
{
    if (!m_bStop)
    {
        if (!this->ForeStop())
        {
            return FALSE;
        }

        m_bStop = TRUE;

        if (IsBackup)
        {
            ::WaitForSingleObject(m_hThread, dwWaitMillisecond);
            if (bForce)
            {
                ::ResumeThread(m_hThread);
            }
            ::WaitForSingleObject(m_hThread, dwWaitMillisecond);
        }
    }
    return TRUE;
}

// 线程回调
BOOL CMsThread::ThreadCallBack(VOID)
{
    while (!m_bStop)
    {
        ::SleepEx(100, TRUE);
    }
    return TRUE;
}

// 获取线程Id
DWORD CMsThread::GetMsThreadId(VOID)
{
    return m_dwThreadId;
}