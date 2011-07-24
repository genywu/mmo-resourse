////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   StmSocket.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-02-21  10:03:16
//  Comment     :   Tcp/ip streamgram base socket source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "StmSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStmSocket::CStmSocket()
{
    m_nSckType = SOCK_STREAM;
    
    ZeroMemory(&m_saRemoteAddr, sizeof(m_saRemoteAddr));
}

CStmSocket::~CStmSocket()
{
}

int CStmSocket::Attach(HSOCKET hInetSocket)
{
    int nResult = false;
    int nRetCode = false;
    
    nRetCode = CIPSocket::Attach(hInetSocket);
    if (!nRetCode)
        goto Exit0;
    
    ZeroMemory(&m_saRemoteAddr, sizeof(m_saRemoteAddr));
    nRetCode = GetRemoteAddress(&m_saRemoteAddr);
    ASSERT(0 == nRetCode);
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CStmSocket::Connect(const char *pcszRemoteAddr, int nPort /*=0*/)
{
    int nResult = false;
    int nRetCode = 0;
    INET_ADDRESS saRemoteAddr = { 0 };
    
    ASSERT(nPort != 0);
    ASSERT(pcszRemoteAddr != NULL);
    if (
        (0 == nPort) || 
        (NULL == pcszRemoteAddr)
    )
    {
        goto Exit0;
    }
    
    nRetCode = GetSocketAddress(pcszRemoteAddr, &saRemoteAddr);
    ASSERT(nRetCode);
    
    if (nRetCode)
    {
        if (nPort != 0)
            saRemoteAddr.sin_port = ::htons(nPort);
        
        nResult = Connect(&saRemoteAddr);
    }
    
Exit0:
    return nResult;
}

int CStmSocket::Connect(const INET_ADDRESS *psaRemoteAddr)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(psaRemoteAddr != NULL);
    if (NULL == psaRemoteAddr)
        goto Exit0;
    
    ASSERT(IsSocketOK());
    
    if (
        (m_saRemoteAddr.sin_family == psaRemoteAddr->sin_family) && 
        (m_saRemoteAddr.sin_port   == psaRemoteAddr->sin_port) && 
        (m_saRemoteAddr.sin_addr.s_addr == psaRemoteAddr->sin_addr.s_addr)
    )
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
            
            nResult = true;
        }
    }
    
Exit0:
    return nResult;
}

int CStmSocket::Listen(int nBackLog /*=BACK_LOG_DEFAULT*/)
{
    int nResult = false;
    int nRetCode = 0;
    
    ASSERT(IsSocketOK());
    
    nRetCode = ::listen(m_hSckHandle, nBackLog);
    nRetCode = CheckError(nRetCode);
    if (0 == nRetCode)
    {
        nResult = true;
    }
    
    return nResult;
}

int CStmSocket::Accept(HSOCKET *phAcceptSocket)
{
    int nResult = false;
    
    INET_ADDRESS saRemoteAddr = { 0 };
    int nAddrLength = sizeof(saRemoteAddr);
    
    ASSERT(phAcceptSocket != NULL);
    if (NULL == phAcceptSocket)
        goto Exit0;
    
    ASSERT(IsSocketOK());
    
    *phAcceptSocket = ::accept(
        m_hSckHandle,
        (sockaddr *)&saRemoteAddr,
        &nAddrLength
    );
    if (INVALID_SOCKET == *phAcceptSocket)
    {
        CheckError(SOCKET_ERROR);
        goto Exit0;
    }
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CStmSocket::Accept(CStmSocket *pAcceptSocket)
{
    int nResult = false;
    int nRetCode = false;
    
    HSOCKET hAcceptSocket = NULL;
    
    ASSERT(pAcceptSocket != NULL);
    if (NULL == pAcceptSocket)
        goto Exit0;
    
    nRetCode = Accept(&hAcceptSocket);
    if (!nRetCode)
        goto Exit0;
    
    nResult = pAcceptSocket->Attach(hAcceptSocket);
    
Exit0:
    return nResult;
}
