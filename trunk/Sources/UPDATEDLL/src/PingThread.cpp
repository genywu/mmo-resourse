////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   PingThread.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-10  10:30:13
//  Comment     :   Tcp/ip ping thread source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "PingThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPingThread::CPingThread()
{
    m_nRepeatCount  = PINGTHREAD_PINGCOUNT;
    m_nPingTimeOut  = DEFAULT_PING_TIMEOUT;
    
    m_ulContextWith = 0;
    
    m_hPingThread = NULL;
    m_ulPingThreadId = 0;
    
    m_piCallback    = NULL;
}

CPingThread::~CPingThread()
{
}

void CPingThread::SetSplitTimes(int nTimes)
{
    if (nTimes <= 0)
        nTimes = 1;
    else if (nTimes > 10)
        nTimes = 10;
    
    m_nRepeatCount = nTimes;
}

void CPingThread::SetTimeOut(int nTimeOut)
{
    m_nPingTimeOut = nTimeOut;
}

void CPingThread::SetContext(ULONG ulContext)
{
    m_ulContextWith = ulContext;
}

void CPingThread::SetCallback(IKPingCallback *piCallback)
{
    m_piCallback = piCallback;
}

int CPingThread::StartPingThread()
{
    int nResult = false;
    
    ASSERT(NULL == m_hPingThread);
    
    ASSERT(m_piCallback != NULL);
    
    m_ulPingThreadId = 0;
    
    m_hPingThread = ::CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)&PingThreadProc,
        (LPVOID)this,
        0,
        &m_ulPingThreadId
    );
    if (NULL == m_hPingThread)
        goto Exit0;
    
    TRACE1("Ping Thread Start: 0x%x\n", m_ulPingThreadId);
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CPingThread::StopPingThread(int nForceStop)
{
    int nResult = false;
    
    if (m_hPingThread  != NULL)
    {
        ULONG ulRetCode = 0;
        
        ulRetCode = ::WaitForSingleObject(m_hPingThread, 0);  // -1
        if (ulRetCode != WAIT_OBJECT_0)
        {
            int nRetCode = false;
            
            ASSERT(FALSE);
            
            if (!nForceStop)
                goto Exit0;
            
            nRetCode = ::TerminateThread(m_hPingThread, -1);
            ASSERT(nRetCode);
            
            TRACE1("Ping Thread Terminate: 0x%x\n", m_ulPingThreadId);
        }
        
        TRACE1("Ping Thread Stopped: 0x%x\n", m_ulPingThreadId);
        m_ulPingThreadId = 0;
        
        ::CloseHandle(m_hPingThread);
        m_hPingThread = NULL;
    }
    
    nResult = true;
    
Exit0:
    return nResult;
}

HANDLE CPingThread::GetThreadHandle()
{
    return m_hPingThread;
}

int CPingThread::OnPingStatus(int nOccurEvent)
{
    int nResult = CPingSocket::OnPingStatus(nOccurEvent);
    ULONG ulRetCode = 0;
    
    switch (nOccurEvent)
    {
    case PINGEVENT_TESTBREAK:
        nResult = DoStatusNotify(PINGEVENT_THREADIDLE, (ULONG)this);
        nResult = !nResult;
        break;
        
    case PINGEVENT_REPEAT:
        nResult = DoStatusNotify(PINGEVENT_THREADEXIT, (ULONG)this);
        nResult = !nResult;
        break;
        
    case PINGEVENT_THREADEXIT:
        nResult = DoStatusNotify(nOccurEvent, (ULONG)this);
        break;
        
    case PINGEVENT_OCCURSTART:
    case PINGEVENT_OCCURERROR:
    case PINGEVENT_HOSTNOTFOUND:
    case PINGEVENT_TIMEOUT:
        nResult = DoStatusNotify(nOccurEvent, (ULONG)this);
        break;
        
    default:
        nResult = true;
        break;
    }
    
    return nResult;
}

int CPingThread::DoStatusNotify(ULONG ulOccurEvent, ULONG ulOccurParam)
{
    int nResult = 0;
    ASSERT(m_piCallback != NULL);
    
    if (m_piCallback != NULL)
    {
        nResult = m_piCallback->PingStatus(
            m_ulContextWith,
            ulOccurEvent,
            ulOccurParam
        );
    }
    
    return nResult;
}

ULONG CPingThread::MainExecution()
{
    ULONG ulResult = 0;
    
    int nRetCode = false;
    int nPingStop = false;
    ULONG ulContextWith;
    
    int   nPingLeft;
    int   nPingTime;
    int   nPingUsedTime;
    char  szPingUrl[PINGTHREAD_URLMAXLENGTH];
    
    nRetCode = CPingSocket::Create();
    if (!nRetCode)
        goto Exit0;
    
    while (!nPingStop)
    {
        nRetCode = OnPingStatus(PINGEVENT_THREADEXIT);
        if (nRetCode)
        {
            nPingStop = true;
            break;
        }
        
        ulContextWith = 0;
        ZeroMemory(szPingUrl, sizeof(szPingUrl));
        
        nRetCode = m_piCallback->GetNextUrl(
            &ulContextWith,
            sizeof(szPingUrl),
            szPingUrl
        );
        if (nRetCode <= 0)
        {
            if (0 == nRetCode)
            {
                ASSERT(FALSE);
                continue;
            }
            
            nRetCode = OnPingStatus(PINGEVENT_THREADEXIT);
            if (nRetCode)
            {
                nPingStop = true;
                break;
            }
            
            continue;
        }
        
        SetContext(ulContextWith);
        
        OnPingStatus(PINGEVENT_OCCURSTART);
        
        nPingLeft = m_nRepeatCount;
        nPingUsedTime = 0;
        
        while (nPingLeft-- > 0)
        {
            nPingTime = m_nPingTimeOut;
            
            nRetCode = PingToAddress(szPingUrl, nPingTime);
            if (PINGRESULT_SUCCESS == nRetCode)
            {
                nPingUsedTime += nPingTime;
                
                DoStatusNotify(
                    PINGEVENT_OCCUROVER,
                    nPingUsedTime
                );
                continue;
            }
            if (PINGRESULT_TIMEOUT == nRetCode)
            {
                nPingUsedTime += nPingTime;
                
                DoStatusNotify(
                    PINGEVENT_TIMEOUT,
                    nPingUsedTime
                );
                continue;
            }
            
            break;
        }
        
        if (PINGRESULT_TESTBREAK == nRetCode)
            continue;
        
        // DoStatusNotify(
        //     PINGEVENT_OCCUROVER,
        //     nPingUsedTime
        // );
        
        ulResult++;
    }
    
Exit0:
    nRetCode = CPingSocket::IsSocketOK();
    if (nRetCode)
        CPingSocket::Destroy();
    
    return ulResult;
}

ULONG WINAPI CPingThread::PingThreadProc(LPVOID lpThisParam)
{
    ULONG ulResult = 0;
    CPingThread *pThis = (CPingThread *)lpThisParam;
    
    ASSERT_POINTER(pThis, CPingThread);
    
    ASSERT(pThis->m_piCallback != NULL);
    
    ulResult = pThis->MainExecution();
    
    return ulResult;
}
