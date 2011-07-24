////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   TcpSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-02-21  15:42:21
//  Comment     :   Tcp/ip tcp socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPSOCKET_H__867466D7_7475_44D4_8585_979B8C90C3A5__INCLUDED_)
#define AFX_TCPSOCKET_H__867466D7_7475_44D4_8585_979B8C90C3A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StmSocket.h"


class CTcpSocket
  : public CStmSocket
{
public:
    CTcpSocket();
    virtual ~CTcpSocket();
    
public:
    virtual int Create();
};

#endif // !defined(AFX_TCPSOCKET_H__867466D7_7475_44D4_8585_979B8C90C3A5__INCLUDED_)
