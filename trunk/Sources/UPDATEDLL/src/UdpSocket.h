////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   UdpSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-16  20:13:26
//  Comment     :   Tcp/ip udp socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKET_H__4AA72D6F_0192_4145_ACE6_466897B7F815__INCLUDED_)
#define AFX_UDPSOCKET_H__4AA72D6F_0192_4145_ACE6_466897B7F815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DgmSocket.h"


class CUdpSocket
  : public CDgmSocket
{
public:
    CUdpSocket();
    virtual ~CUdpSocket();
    
public:
    virtual int Create();
};


#endif // !defined(AFX_UDPSOCKET_H__4AA72D6F_0192_4145_ACE6_466897B7F815__INCLUDED_)
