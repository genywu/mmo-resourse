////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DgmSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-16  20:13:26
//  Comment     :   Tcp/ip datagram/raw base socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGMSOCKET_H__F1424660_D3D0_4326_9EF0_2E02384CE95C__INCLUDED_)
#define AFX_DGMSOCKET_H__F1424660_D3D0_4326_9EF0_2E02384CE95C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IpSocket.h"


class CDgmSocket
  : public CIPSocket
{
public:
    CDgmSocket();
    virtual ~CDgmSocket();
    
public:
    virtual int Attach(HSOCKET hInetSocket);
    
public:
    int GetPrivateMode();
    int GetPrivateRemote(INET_ADDRESS *psaRemoteAddr);
    int SetRemoteAddress(const char *pcszRemoteAddr, int nPort = 0);
    int SetRemoteAddress(const INET_ADDRESS *psaRemoteAddr);
    
    int SendTo(int nBufSize, const unsigned char *pbyBuffer);
    int ReceiveFrom(int nBufSize,  unsigned char *pbyBuffer);
    
protected:
    int m_nInPrivate;
    INET_ADDRESS m_saRemoteAddr;
};

#endif // !defined(AFX_DGMSOCKET_H__F1424660_D3D0_4326_9EF0_2E02384CE95C__INCLUDED_)
