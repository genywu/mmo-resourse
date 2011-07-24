////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DgmSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-16  20:13:26
//  Comment     :   Tcp/ip datagram/raw base socket source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DgmSocket.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDgmSocket::CDgmSocket()
{
    m_nSckType = SOCK_DGRAM;
    
    m_nInPrivate = false;
    ZeroMemory(&m_saRemoteAddr, sizeof(m_saRemoteAddr));
}

CDgmSocket::~CDgmSocket()
{
}

int CDgmSocket::Attach(HSOCKET hInetSocket)
{
    int nResult = false;
    int nRetCode = false;
    
    nRetCode = CIPSocket::Attach(hInetSocket);
    if (!nRetCode)
        goto Exit0;
    
    ZeroMemory(&m_saRemoteAddr, sizeof(m_saRemoteAddr));
    nRetCode = GetRemoteAddress(&m_saRemoteAddr);
    if (0 == nRetCode)
        m_nInPrivate = true;
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CDgmSocket::GetPrivateMode()
{
    return m_nInPrivate;
}

int CDgmSocket::GetPrivateRemote(INET_ADDRESS *psaRemoteAddr)
{
    if (!m_nInPrivate)
        return false;
    
    ASSERT(psaRemoteAddr != NULL);
    if (NULL == psaRemoteAddr)
        return false;
    
    *psaRemoteAddr = m_saRemoteAddr;
    return true;
}

int CDgmSocket::SetRemoteAddress(const char *pcszRemoteAddr, int nPort /*=0*/)
{
    int nResult = false;
    int nRetCode = 0;
    INET_ADDRESS saRemoteAddr = { 0 };
    
    if (NULL == pcszRemoteAddr)
        nResult = SetRemoteAddress(&saRemoteAddr);
    else
    {
        nRetCode = GetSocketAddress(pcszRemoteAddr, &saRemoteAddr);
        ASSERT(nRetCode);
        
        if (nRetCode)
        {
            if (nPort != 0)
                saRemoteAddr.sin_port = ::htons(nPort);
            
            nResult = SetRemoteAddress(&saRemoteAddr);
        }
    }
    
    return nResult;
}

int CDgmSocket::SetRemoteAddress(const INET_ADDRESS *psaRemoteAddr)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(psaRemoteAddr != NULL);
    if (NULL == psaRemoteAddr)
        goto Exit0;
    
    ASSERT(IsSocketOK());
    
    if (0 == psaRemoteAddr->sin_addr.s_addr)
    {
        if (!m_nInPrivate)
            nResult = true;
        else
        {
            nRetCode = ::connect(
                m_hSckHandle,
                (sockaddr *)psaRemoteAddr,
                sizeof(INET_ADDRESS)
            );
            nRetCode = CheckError(nRetCode);
            if (0 == nRetCode)
            {
                m_saRemoteAddr = *psaRemoteAddr;
                m_nInPrivate = false;
                
                nResult = true;
            }
        }
    }
    else
    {
        if (
            (m_saRemoteAddr.sin_family == psaRemoteAddr->sin_family) && 
            (m_saRemoteAddr.sin_addr.s_addr == psaRemoteAddr->sin_addr.s_addr)
        )
            nResult = true;
        else
        {
            nResult = ::connect(
                m_hSckHandle,
                (sockaddr *)psaRemoteAddr,
                sizeof(INET_ADDRESS)
            );
            nRetCode = CheckError(nRetCode);
            if (0 == nRetCode)
            {
                m_saRemoteAddr = *psaRemoteAddr;
                m_nInPrivate = true;
            
                nResult = true;
            }
        }
    }
    
Exit0:
    return nResult;
}

int CDgmSocket::SendTo(int nBufSize, const unsigned char *pbyBuffer)
{
    int nResult = false;
    
    ASSERT(m_nInPrivate);
    
    if (m_nInPrivate)
        nResult = Send(nBufSize, pbyBuffer);
    
    return nResult;
}

int CDgmSocket::ReceiveFrom(int nBufSize, unsigned char *pbyBuffer)
{
    int nResult = false;
    
    ASSERT(m_nInPrivate);
    
    if (m_nInPrivate)
        nResult = Receive(nBufSize, pbyBuffer);
    
    return nResult;
}
