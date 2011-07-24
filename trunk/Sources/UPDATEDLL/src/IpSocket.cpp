////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   IpSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-16  16:41:53
//  Comment     :   Tcp/ip socket class for internet source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "IpSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPSocket::CIPSocket()
{
    m_nSckType  = SOCK_UNKNOWN;
    m_nSckProto = IPPROTO_IP;
    
    m_nSckFlags  = 0;
    m_hSckHandle = INVALID_SOCKET;
    
    m_nBlockMode = true;
    
    m_nLastError = 0;
}

CIPSocket::~CIPSocket()
{
}

int CIPSocket::Create()
{
    if (m_hSckHandle != INVALID_SOCKET)
    {
        ASSERT(FALSE);
        return false;
    }
    
    m_hSckHandle = ::socket(AF_INET, m_nSckType, m_nSckProto);
    if (INVALID_SOCKET == m_hSckHandle)
    {
        ASSERT(FALSE);
        return false;
    }
    
    if (!m_nBlockMode)
    {
        int nRetCode = 0;
        int nIsNonBlock = !m_nBlockMode;
        
        nRetCode = ::ioctlsocket(m_hSckHandle, FIONBIO, (ULONG *)&nIsNonBlock);
        ASSERT(0 == nRetCode);
        
        if (nRetCode != 0)
            m_nBlockMode = true;
    }
    
    return true;
}

void CIPSocket::Destroy()
{
    int nRetCode = false;
    
    if (m_hSckHandle != INVALID_SOCKET)
    {
        nRetCode = ::closesocket(m_hSckHandle);
        m_hSckHandle = INVALID_SOCKET;
    }
}

int CIPSocket::ShutDown(int nModeHow)
{
    int nResult = false;
    int nRetCode = 0;
    
    if (INVALID_SOCKET == m_hSckHandle)
        goto Exit0;
    
    nRetCode = ::shutdown(m_hSckHandle, nModeHow);
    nResult = CheckError(nRetCode);
    
Exit0:
    return nResult;
}

int CIPSocket::Attach(HSOCKET hInetSocket)
{
    Destroy();
    
    m_hSckHandle = hInetSocket;
    return true;
}

HSOCKET CIPSocket::Detach()
{
    HSOCKET hSckReturn = m_hSckHandle;
    
    m_hSckHandle = INVALID_SOCKET;
    return hSckReturn;
}

HSOCKET CIPSocket::GetSafeHandle()
{
    ASSERT_POINTER(this, CIPSocket);
    
    return ((this != NULL) ? m_hSckHandle : INVALID_SOCKET);
}

void CIPSocket::OnSendHandler(int nBufSize, const unsigned char *pbySendBuf)
{
    return;
}

void CIPSocket::OnReceiveHandler(int nBufSize, const unsigned char *pbyReceiveBuf)
{
    return;
}

void CIPSocket::OnErrorHandler()
{
    return;
}

int CIPSocket::GetLastError()
{
    return m_nLastError;
}

int CIPSocket::SetLastError(int nErrorNum)
{
    m_nLastError = nErrorNum;
    return m_nLastError;
}

int CIPSocket::SetBlockMode(int nIsBlock)
{
    int nResult = 0;
    int nRetCode = 0;
    
    if (!nIsBlock != !m_nBlockMode)
    {
        if (INVALID_SOCKET == m_hSckHandle)
            m_nBlockMode = (nIsBlock != 0);
        else
        {
            int nIsNonBlock = !nIsBlock;
            
            nRetCode = ::ioctlsocket(m_hSckHandle, FIONBIO, (ULONG *)&nIsNonBlock);
            
            nResult = CheckError(nRetCode);
            if (0 == nResult)
            {
                m_nBlockMode = !nIsNonBlock;
            }
        }
    }
    
    return nResult;
}

int CIPSocket::GetSocketAddress(const char *pcszAddr, INET_ADDRESS *pSocketAddr)
{
    int nResult = false;
    ULONG ulAddrIp = inet_addr(pcszAddr);
    
    ASSERT_POINTER(pSocketAddr, INET_ADDRESS);
    
    if (INADDR_NONE == ulAddrIp)
    {
        HOSTENT *pHostEnt = NULL;
        
        pHostEnt = gethostbyname(pcszAddr);
        if (NULL == pHostEnt)
            goto Exit0;
        
        ulAddrIp = (*(ULONG *)(pHostEnt->h_addr));
    }
    
    pSocketAddr->sin_family = AF_INET;
    pSocketAddr->sin_port   = 0;
    pSocketAddr->sin_addr.s_addr = ulAddrIp;
    
    nResult = true;
    
Exit0:
    return nResult;
}
    
int CIPSocket::GetLocalAddress(HSOCKET hInetSocket, INET_ADDRESS *psaAddrLocal)
{
    int nResult = false;
    int nAddrLength = sizeof(INET_ADDRESS);
    
    ASSERT_POINTER(psaAddrLocal, INET_ADDRESS);
    
    nResult = ::getsockname(
        hInetSocket,
        (sockaddr *)psaAddrLocal,
        &nAddrLength
    );
    
    return nResult;
}

int CIPSocket::GetRemoteAddress(HSOCKET hInetSocket, INET_ADDRESS *psaAddrRemote)
{
    int nResult = false;
    int nAddrLength = sizeof(INET_ADDRESS);
    
    ASSERT_POINTER(psaAddrRemote, INET_ADDRESS);
    
    nResult = ::getpeername(
        hInetSocket,
        (sockaddr *)psaAddrRemote,
        &nAddrLength
    );
    
    return nResult;
}

int CIPSocket::GetLocalAddress(INET_ADDRESS *psaAddrLocal)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    nRetCode = GetLocalAddress(m_hSckHandle, psaAddrLocal);
    nResult = CheckError(nRetCode);
    
    return nResult;
}

int CIPSocket::GetRemoteAddress(INET_ADDRESS *psaAddrRemote)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    nRetCode = GetRemoteAddress(m_hSckHandle, psaAddrRemote);
    nResult = CheckError(nRetCode);
    
    return nResult;
}

int CIPSocket::Select(int nReadReady, int nWriteReady, int nErrorReady, int nTimeOut)
{
    int nResult = 0;
    int nRetCode = 0;
    
    FD_SET *pReadFds = NULL;
    FD_SET *pWriteFds = NULL;
    FD_SET *pErrorFds = NULL;
    TIMEVAL *pTimeVal = NULL;
    FD_SET fdsRead, fdsWrite, fdsError;
    TIMEVAL tvTimeOut = { 0 };
    
    ASSERT(m_hSckHandle != INVALID_SOCKET);
    
    if (nReadReady)
    {
        pReadFds = &fdsRead;
        
        FD_ZERO(pReadFds);
        FD_SET(m_hSckHandle, pReadFds);        
    }
    if (nWriteReady)
    {
        pWriteFds = &fdsWrite;
        
        FD_ZERO(pWriteFds);
        FD_SET(m_hSckHandle, pWriteFds);
    }
    if (nErrorReady)
    {
        pErrorFds = &fdsError;
        
        FD_ZERO(&pErrorFds);
        FD_SET(m_hSckHandle, pErrorFds);
    }
    if (nTimeOut >= 0)
    {
        if (nTimeOut != 0)
        {
            tvTimeOut.tv_sec = nTimeOut / 1000;
            tvTimeOut.tv_usec = nTimeOut % 1000;
        }
        
        pTimeVal = &tvTimeOut;
    }
    
    nRetCode = ::select((int)m_hSckHandle + 1, pReadFds, pWriteFds, pErrorFds, pTimeVal);
    nResult = CheckError(nRetCode);
    
    return nResult;
}

int CIPSocket::Bind(const INET_ADDRESS *pAddrLocal)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    ASSERT_POINTER(pAddrLocal, INET_ADDRESS);
    
    nRetCode = ::bind(
        m_hSckHandle,
        (const sockaddr *)pAddrLocal,
        sizeof(INET_ADDRESS)
    );
    nResult = CheckError(nRetCode);
    
    return nResult;
}

int CIPSocket::Send(int nBufSize, const unsigned char *pbyBuffer)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    ASSERT(nBufSize > 0);
    ASSERT(pbyBuffer != NULL);
    
    nRetCode = ::send(
        m_hSckHandle,
        (const char *)pbyBuffer,
        nBufSize,
        m_nSckFlags
    );
    
    nResult = CheckError(nRetCode);
    if (nResult > 0)
    {
        OnSendHandler(nResult, pbyBuffer);
    }
    
    return nResult;
}

int CIPSocket::Receive(int nBufSize, unsigned char *pbyBuffer)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    ASSERT(nBufSize > 0);
    ASSERT(pbyBuffer != NULL);
    
    nRetCode = ::recv(
        m_hSckHandle,
        (char *)pbyBuffer,
        nBufSize,
        m_nSckFlags
    );
    
    nResult = CheckError(nRetCode);
    if (nResult > 0)
    {
        OnReceiveHandler(nResult, pbyBuffer);
    }
    
    return nResult;
}

int CIPSocket::SendTo(int nBufSize, const unsigned char *pbyBuffer, const INET_ADDRESS *pAddrTo)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    ASSERT(nBufSize > 0);
    ASSERT(pbyBuffer != NULL);
    ASSERT_POINTER(pAddrTo, INET_ADDRESS);
    
    nRetCode = ::sendto(
        m_hSckHandle,
        (const char *)pbyBuffer,
        nBufSize,
        m_nSckFlags,
        (sockaddr *)pAddrTo,
        sizeof(INET_ADDRESS)
    );
    
    nResult = CheckError(nRetCode);
    if (nResult > 0)
    {
        OnSendHandler(nResult, pbyBuffer);
    }
    
    return nResult;
}

int CIPSocket::ReceiveFrom(int nBufSize, unsigned char *pbyBuffer, const INET_ADDRESS *pAddrFrom)
{
    int nResult = false;
    int nRetCode = 0;
    int nAddrSize = sizeof(INET_ADDRESS);
    
    ASSERT(IsSocketOK());
    
    ASSERT(nBufSize > 0);
    ASSERT(pbyBuffer != NULL);
    ASSERT_POINTER(pAddrFrom, INET_ADDRESS);
    
    nRetCode = ::recvfrom(
        m_hSckHandle,
        (char *)pbyBuffer,
        nBufSize,
        m_nSckFlags,
        (sockaddr *)pAddrFrom,
        &nAddrSize
    );
    
    nResult = CheckError(nRetCode);
    if (nResult > 0)
    {
        OnReceiveHandler(nResult, pbyBuffer);
    }
    
    return nResult;
}

int CIPSocket::GetProtocol()
{
    return m_nSckProto;
}

int CIPSocket::SetProtocol(int nProtocol)
{
    int nOldProto = m_nSckProto;
    
    ASSERT(INVALID_SOCKET == m_hSckHandle);
    
    m_nSckProto = nProtocol;
    return nOldProto;
}

int CIPSocket::GetWorkFlags()
{
    return m_nSckFlags;
}

int CIPSocket::SetWorkFlags(int nSocketFlags)
{
    int nOldFlags = m_nSckFlags;
    
    m_nSckFlags = nSocketFlags;
    
    return nOldFlags;
}

int CIPSocket::IsSocketOK()
{
    return (m_hSckHandle != INVALID_SOCKET);
}

int CIPSocket::CheckError(int nRetValue)
{
    int nResult = 0;
    
    nResult = nRetValue;
    
    if (SOCKET_ERROR == nRetValue)
    {
        SetLastError(::WSAGetLastError());
        
        OnErrorHandler();
    }
    
    return nResult;
}
