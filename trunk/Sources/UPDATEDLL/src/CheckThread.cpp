////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   CheckThread.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-17  10:37:18
//  Comment     :   KAVuser-net check thread source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "CheckThread.h"
#include "DataDefine.h"
#include "UdpSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCheckThread::CCheckThread()
{
    m_nTimeOut = defCHECK_TIME_OUT;
    m_nTryTimes = defCHECK_TRY_TIMES;
    
    m_ulContextWith = 0;
    m_piCallback = NULL;
    
    m_hKillEvent = NULL;
}

CCheckThread::~CCheckThread()
{
}

int CCheckThread::SetCheckCallback(IKCheckCallback *piCallback, ULONG ulContextWith)
{
    int nResult = false;
    int nRetCode = false;
    
    nRetCode = CBusyThread::IsThreadOK();
    if (nRetCode)
        goto Exit0;
    
    ulContextWith = ulContextWith;
    m_piCallback = piCallback;
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CCheckThread::AddCheckServer(const char *pcszCheckServer, int nCheckPort)
{
    int nResult = false;
    int nAddServer = -1;
    
    TRY
    {
        nAddServer = m_paServerAddress.Add((void *)pcszCheckServer);
        
        m_waServerPort.Add((WORD)nCheckPort);
    }
    CATCH_ALL(pException);
    {
        pException->Delete();
        goto Exit0;
    }
    END_CATCH_ALL;
    
    nResult = true;
    
Exit0:
    if ((!nResult) && (nAddServer != -1))
    {
        m_paServerAddress.RemoveAt(nAddServer);
    }
    return nResult;
}

int CCheckThread::Create()
{
    int nResult = false;
    int nRetCode = false;
    
    nRetCode = IsThreadOK();
    if (nRetCode)
        goto Exit0;
    
	ASSERT(NULL == m_hKillEvent);
    
    m_hKillEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == m_hKillEvent)
    {
        ASSERT(FALSE);
        goto Exit0;
    }
    
    nResult = true;
    
Exit0:
    return nResult;
}

void CCheckThread::Destroy()
{
    ASSERT(!IsThreadOK());
    
    if (m_hKillEvent != NULL)
    {
        ::CloseHandle(m_hKillEvent);
        m_hKillEvent = NULL;
    }
}

int CCheckThread::StartThread()
{
    int nResult = false;
    ULONG ulRetCode = 0;
    
    if (
        (NULL == m_hKillEvent) || 
        (NULL == m_piCallback)
    )
    {
        ASSERT(FALSE);
        goto Exit0;
    }
    
    ulRetCode = ::WaitForSingleObject(m_hKillEvent, 0);
    if (WAIT_OBJECT_0 == ulRetCode)
        ::ResetEvent(m_hKillEvent);
    
    nResult = CBusyThread::StartThread();
    
Exit0:
    return nResult;
}

int CCheckThread::QueryStop()
{
    int nResult = false;
    ULONG ulRetCode = 0;
    
    ASSERT(m_hKillEvent != NULL);

    if (m_hKillEvent != NULL)
    {
        ulRetCode = ::WaitForSingleObject(m_hKillEvent, 0);
        
        nResult = (WAIT_OBJECT_0 == ulRetCode);
    }
    
    return nResult;
}

int CCheckThread::RemindStop()
{
    int nResult = false;
    
    if (m_hKillEvent != NULL)
    {
        nResult = ::SetEvent(m_hKillEvent);
    }
    
    return nResult;
}

ULONG CCheckThread::MainExecution()
{
    ULONG ulResult = 0;
    int nRetCode = 0;
    int nDataReady = false;
    int nExitDirect = false;    
    CUdpSocket CheckSocket;
    
    int nAddrIndex = 0;
    int nCheckResult = defConnectServerFailed - 1;
    
    int nCheckServerPort = 0;
    const char *pcszCheckServerAddr = NULL;
    
    int nTimeOut = 0;
    int nTryTimes = 0;
    int nCheckServerCount = 0;
    
    int nSendBufSize;
    unsigned char *pbySendBuffer = NULL;
    
    int nRecvBufSize = defCHECK_RECEIVE_MAX_SIZE;
    unsigned char byRecvBuffer[defCHECK_RECEIVE_MAX_SIZE];
    
    if (NULL == m_piCallback)
        goto Exit0;
    
    nRetCode = m_piCallback->GetCheckInfo(
        m_ulContextWith,
        &nSendBufSize,
        &pbySendBuffer
    );
    if (
        (!nRetCode) ||
        (nSendBufSize <= 0) ||
        (NULL == pbySendBuffer)
    )
        goto Exit0;
    
    nRetCode = QueryStop();
    if (nRetCode)
    {
        nExitDirect = true;
        goto Exit0;
    }
    
    nRetCode = CheckSocket.Create();
    if (!nRetCode)
        goto Exit0;
    
    nCheckServerCount = m_paServerAddress.GetSize();
    for (nAddrIndex = 0; nAddrIndex < nCheckServerCount; nAddrIndex++)
    {
        nRetCode = QueryStop();
        if (nRetCode)
        {
            nExitDirect = true;
            goto Exit0;
        }
        
        nCheckServerPort = m_waServerPort.GetAt(nAddrIndex);
        if (nCheckServerPort <= 0)
            nCheckServerPort = defCHECK_SERVER_PORT;
        pcszCheckServerAddr = (const char *)m_paServerAddress.GetAt(nAddrIndex);
        
        nRetCode = CheckSocket.SetRemoteAddress(pcszCheckServerAddr, nCheckServerPort);
        if (!nRetCode)
            continue;
        
        nTimeOut = m_nTimeOut;
        nTryTimes = m_nTryTimes;
        
        while (nTryTimes-- > 0)
        {
            int nSeparate = defCHECK_TIME_SEP;
            
            nRetCode = QueryStop();
            if (nRetCode)
            {
                nExitDirect = true;
                goto Exit0;
            }
            
            nRetCode = CheckSocket.Send(nSendBufSize, pbySendBuffer);
            if (nRetCode <= 0)
            {
                ASSERT(FALSE);
                continue;
            }
            
            nTimeOut /= nSeparate;
            while (nSeparate-- > 0)
            {
                nDataReady = CheckSocket.Select(true, false, false, nTimeOut);
                if (SOCKET_ERROR == nDataReady)
                    break;

                nRetCode = QueryStop();
                if (nRetCode)
                {
                    nExitDirect = true;
                    goto Exit0; 
                }

                if (nDataReady <= 0)
                    continue;
                
                break;
            }
            if (SOCKET_ERROR == nDataReady)
                break;
            
            if (nDataReady > 0)
            {
                ZeroMemory(byRecvBuffer, nRecvBufSize);
                nRetCode = CheckSocket.ReceiveFrom(nRecvBufSize, byRecvBuffer);
                if (nRetCode <= 0)
                    break;
                
                nCheckResult = m_piCallback->CheckRecvInfo(
                    m_ulContextWith,
                    nRetCode,
                    byRecvBuffer
                );
                if (nCheckResult >= defInhibitiveSerialNumber)
                    goto Exit1;
            }
        }
    }
    
Exit1:
    ulResult = true;
    
Exit0:
    if (!nExitDirect && (m_piCallback != NULL))
    {
        m_piCallback->CheckResult(m_ulContextWith, nCheckResult);
    }
    
    nRetCode = CheckSocket.IsSocketOK();
    if (nRetCode)
        CheckSocket.Destroy();
    
    return ulResult;
}
