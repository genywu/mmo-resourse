////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   PingSocket.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-10  09:21:42
//  Comment     :   Tcp/ip ping socket source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "PingSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPingSocket::CPingSocket()
  : CRawSocket()
{
    m_nSckProto = IPPROTO_ICMP;
    
    m_nLastSequence = DEFAULT_PING_SEQUENCE;
    m_nLastPingTime = -1;
}

CPingSocket::~CPingSocket()
{
}

int CPingSocket::Create()
{
    CRawSocket::SetBlockMode(false);
    
    return CRawSocket::Create();
}

int CPingSocket::PingToAddress(const char *pcszRemoteAddr, int& nTimeOut)
{
    int nResult = false;
    int nRetCode = true;
    
    INET_ADDRESS saRemoteAddr = { 0 };
    
    ASSERT(pcszRemoteAddr);
    if (NULL == pcszRemoteAddr)
        goto Exit0;
    
    nRetCode = CRawSocket::GetSocketAddress(pcszRemoteAddr, &saRemoteAddr);
    if (!nRetCode)
    {
        DoEventNotify(PINGEVENT_HOSTNOTFOUND);
        goto Exit0;
    }
    
    nResult = PingToAddress(&saRemoteAddr, nTimeOut);
    
Exit0:
    return nResult;
}

int CPingSocket::PingToAddress(const INET_ADDRESS *psaRemoteAddr, int& nTimeOut)
{
    int nResult = PINGRESULT_TESTBREAK;
    int nRetCode = 0;
    
    int nIsBreak = false;
    int nTimeLeft = nTimeOut;
    ULONG ulTimeStart = 0;
    
    int nSendSize = 0;
    int nRecvSize = 0;
    unsigned char *pSendBuf = NULL;
    unsigned char *pRecvBuf = NULL;
    
    PING_ECHOREQUEST EchoRequest = { 0 };
    PING_ECHOREPLY   EchoReply   = { 0 };
    
    if (!IsSocketOK())
    {
        ASSERT(FALSE);
        
        nResult = PINGRESULT_OCCURERROR;
        goto Exit0;
    }
    
    nRetCode = DoEventNotify(PINGEVENT_TESTBREAK);
    if (nRetCode)
    {
        nIsBreak = true;
        goto Exit0;
    }
    
    nRetCode = CRawSocket::SetRemoteAddress(psaRemoteAddr);
    if (!nRetCode)
    {
        nResult = PINGRESULT_OCCURERROR;
        goto Exit0;
    }
    
    EchoRequest.icmpHdr.ID  = DEFAULT_PING_ID;
    EchoRequest.icmpHdr.Seq = ++m_nLastSequence;
    EchoRequest.unSignMain  = DEFAULT_PING_SIGN_MAIN;
    EchoRequest.unSignExt   = (unsigned int)CRawSocket::GetSafeHandle();
    EchoRequest.unTimeMain  = ::GetTickCount();    
    
    nSendSize = sizeof(PING_ECHOREQUEST);
    pSendBuf  = (unsigned char *)&EchoRequest;
    nRetCode  = OnPingDataSend(&nSendSize, &pSendBuf);
    ASSERT(nRetCode ? ((nSendSize > 0) && (pSendBuf != NULL)) : TRUE);
    if (!nRetCode)
    {
        nResult = PINGRESULT_OCCURERROR;
        goto Exit0;
    }
    
    EchoRequest.icmpHdr.Type     = ICMPTYPE_ECHOREQUEST;
    EchoRequest.icmpHdr.Code     = 0;
    
    nRetCode = DoEventNotify(PINGEVENT_TESTBREAK);
    if (nRetCode)
    {
        nIsBreak = true;
        goto Exit0;
    }
    
    nRecvSize = sizeof(PING_ECHOREPLY);
    pRecvBuf  = (unsigned char *)&EchoReply;
    nRetCode = OnPingDataReceive(&nRecvSize, &pRecvBuf);
    ASSERT(nRetCode ? ((nRecvSize > 0) && (pRecvBuf != NULL)) : TRUE);
    if (!nRetCode)
    {
        nResult = PINGRESULT_OCCURERROR;
        goto Exit0;
    }
    
    EchoRequest.icmpHdr.Checksum = 0;
    EchoRequest.icmpHdr.Checksum = inet_chksum((unsigned short *)pSendBuf, nSendSize);
    
    ulTimeStart = ::GetTickCount();
    nRetCode = SendTo(nSendSize, pSendBuf);
    if (SOCKET_ERROR == nRetCode)
    {
        nResult = PINGRESULT_OCCURERROR;
        goto Exit0;
    }
    
    while (nTimeLeft > 0)
    {
        nRetCode = CRawSocket::Select(true, false, false, nTimeLeft);
        if (0 == nRetCode)
        {
            nTimeOut = (int)(::GetTickCount() - ulTimeStart);
            nResult = PINGRESULT_TIMEOUT;
            goto Exit0;
        }
        else if (SOCKET_ERROR == nRetCode)
        {
            nResult = PINGRESULT_OCCURERROR;
            goto Exit0;
        }
        
        nRetCode = CRawSocket::ReceiveFrom(nRecvSize, pRecvBuf);
        nTimeLeft -= (int)(::GetTickCount() - ulTimeStart);
        
        if (
            (nRetCode >= (sizeof(IPHDR) + sizeof(ICMPHDR))) && 
            (EchoReply.echoRequest.icmpHdr.ID == EchoRequest.icmpHdr.ID) && 
            (EchoReply.echoRequest.icmpHdr.Seq  == EchoRequest.icmpHdr.Seq) && 
            (EchoReply.echoRequest.icmpHdr.Code == EchoRequest.icmpHdr.Code)
        )
        {
            // Cannot receive this package as this routine use point-to-point ICMP
            if (ICMPTYPE_DSTNOTTOUCH == EchoReply.echoRequest.icmpHdr.Type)
            {
                DoEventNotify(PINGEVENT_HOSTNOTFOUND);
                
                nResult = PINGEVENT_HOSTNOTFOUND;
                goto Exit0;
            }
            
            if (
                (nRetCode >= (sizeof(IPHDR) + sizeof(PING_ECHOREQUEST))) && 
                (ICMPTYPE_ECHOREPLY == EchoReply.echoRequest.icmpHdr.Type) && 
                (EchoReply.echoRequest.unSignMain == EchoRequest.unSignMain) && 
                (EchoReply.echoRequest.unSignExt  == EchoRequest.unSignExt )
            )
            {
                break;
            }
        }
        
        nRetCode = DoEventNotify(PINGEVENT_REPEAT);
        if (!nRetCode)
        {
            nResult = PINGRESULT_OCCURERROR;
            goto Exit0;
        }
        
        nRetCode = DoEventNotify(PINGEVENT_TESTBREAK);
        if (nRetCode)
            goto Exit0;
    }
    
    nTimeOut -= nTimeLeft;
    if (nTimeOut < 0)
    {
        ASSERT(FALSE);
        
        nTimeOut = 0;
    }
    
    m_nLastPingTime = nTimeOut;
    
    nResult = PINGRESULT_SUCCESS;
    
Exit0:
    if (nResult != PINGRESULT_SUCCESS)
    {
    }
    
    return nResult;
}

int CPingSocket::GetLastPingTime()
{
    return m_nLastPingTime;
}

int CPingSocket::OnPingDataSend(int *pnBufSize, unsigned char **ppbyDataSend)
{
    return true;
}

int CPingSocket::OnPingDataReceive(int *pnBufSize, unsigned char **pbyDataReceive)
{
    return true;
}

int CPingSocket::OnPingStatus(int nOccurEvent)
{
    int nResult = false;
    
    switch (nOccurEvent)
    {
    case PINGEVENT_TESTBREAK:
        nResult = false;
        
    case PINGEVENT_REPEAT:
        nResult = true;
        
    default:
        nResult = true;
        break;
    }
    
    return nResult;
}

void CPingSocket::OnErrorHandler()
{
    DoEventNotify(PINGEVENT_OCCURERROR);
}

int CPingSocket::DoEventNotify(int nOccurEvent)
{
    return OnPingStatus(nOccurEvent);
}
   
