////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   RawSocket.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-09  10:46:38
//  Comment     :   Tcp/ip raw socket source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "RawSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRawSocket::CRawSocket()
{
    m_nSckType = SOCK_RAW;
    m_nSckProto = IPPROTO_IP;
}

CRawSocket::~CRawSocket()
{
}

int CRawSocket::Create()
{
    return CDgmSocket::Create();
}

int CRawSocket::SetRawProtocol(int nProtocolType)
{
    int nResult = false;
    
    if (
        (nProtocolType >= 256) || 
        (GetSafeHandle() != INVALID_SOCKET)
    )
    {
        ASSERT(false);
        goto Exit0;
    }
    
    m_nSckProto = nProtocolType;
    
    nResult = true;
    
Exit0:
    return nResult;
}
