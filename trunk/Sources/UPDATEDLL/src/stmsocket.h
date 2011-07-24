////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   StmSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-02-21  10:03:16
//  Comment     :   Tcp/ip streamgram base socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STMSOCKET_H__D1C50B8A_5C1A_4EC7_A504_26671253DBA4__INCLUDED_)
#define AFX_STMSOCKET_H__D1C50B8A_5C1A_4EC7_A504_26671253DBA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IpSocket.h"


#define BACK_LOG_DEFAULT                    5

class CStmSocket
  : public CIPSocket
{
public:
    CStmSocket();
    virtual ~CStmSocket();
    
public:
    virtual int Attach(HSOCKET hInetSocket);
    
public:
    int Connect(const char *pcszRemoteAddr, int nPort = 0);
    int Connect(const INET_ADDRESS *psaRemoteAddr);
    
    int Listen(int nBackLog = BACK_LOG_DEFAULT);
    
    int Accept(HSOCKET *phAcceptSocket);
    int Accept(CStmSocket *pAcceptSocket);
    
protected:
    INET_ADDRESS m_saRemoteAddr;
};

#endif // !defined(AFX_STMSOCKET_H__D1C50B8A_5C1A_4EC7_A504_26671253DBA4__INCLUDED_)
