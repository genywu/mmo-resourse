////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   RawSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-09  10:46:38
//  Comment     :   Tcp/ip raw socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAWSOCKET_H__CD65788A_C8BC_4AF2_82D9_43E226948C73__INCLUDED_)
#define AFX_RAWSOCKET_H__CD65788A_C8BC_4AF2_82D9_43E226948C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DgmSocket.h"


class CRawSocket
  : public CDgmSocket
{
public:
    CRawSocket();
    virtual ~CRawSocket();
    
public:
    int SetRawProtocol(int nProtocolType);
    
public:
    virtual int Create();
};


#endif // !defined(AFX_RAWSOCKET_H__CD65788A_C8BC_4AF2_82D9_43E226948C73__INCLUDED_)
