////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   PingSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-10  09:21:42
//  Comment     :   Tcp/ip ping socket header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINGSOCKET_H__752184FA_8742_4259_814E_25442FC6A7ED__INCLUDED_)
#define AFX_PINGSOCKET_H__752184FA_8742_4259_814E_25442FC6A7ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RawSocket.h"
#include "InetStruct.h"

#define PINGEVENT_OCCUROVER                 0x0000
#define PINGEVENT_OCCURSTART                0x0001
#define PINGEVENT_OCCURERROR                0x0002
#define PINGEVENT_HOSTNOTFOUND              0x0003
#define PINGEVENT_REPEAT                    0x0004
#define PINGEVENT_TIMEOUT                   0x0005
#define PINGEVENT_TESTBREAK                 0x0006
#define PINGEVENT_THREADEXIT                0x0007
#define PINGEVENT_THREADIDLE                0x0008


// Ping Echo Request
typedef struct tagPING_ECHOREQ
{
    ICMPHDR icmpHdr;
    
    unsigned int  unSignMain;
    unsigned int  unSignExt;
    unsigned int  unTimeMain;
    unsigned int  unTimeExt;
    
    unsigned char byData[ICMP_DATAMINSIZE];
    
} PING_ECHOREQUEST, *PPING_ECHOREQUEST;

// Ping Echo Reply
typedef struct tagPING_ECHOREP
{
    IPHDR   ipHdr;
    PING_ECHOREQUEST echoRequest;
    
    unsigned char byFillField[128];
    
} PING_ECHOREPLY, *PPING_ECHOREPLY;

#define DEFAULT_PING_ID                     'SK'
#define DEFAULT_PING_SEQUENCE               0x0301

#define DEFAULT_PING_SIGN_MAIN              '3PUK'
#define DEFAULT_PING_SIGN_EXT               'KCSH'


#define DEFAULT_PING_TIMEOUT                1000


#define PINGRESULT_SUCCESS                  0
#define PINGRESULT_TIMEOUT                  1
#define PINGRESULT_TESTBREAK                2
#define PINGRESULT_OCCURERROR               3


class CPingSocket
  : public CRawSocket
{
public:
    CPingSocket();
    virtual ~CPingSocket();
    
public:
    virtual int Create();
    
public:
    int PingToAddress(const char *pcszRemoteAddr, int& nTimeOut);
    int PingToAddress(const INET_ADDRESS *pszRemoteAddr, int& nTimeOut);
    
    int GetLastPingTime();
    
protected:
    virtual int OnPingDataSend(int *pnBufSize, unsigned char **ppbyDataSend);
    virtual int OnPingDataReceive(int *pnBufSize, unsigned char **pbyDataReceive);
    
    virtual int OnPingStatus(int nOccurEvent);
    
protected:
    int DoEventNotify(int nOccurEvent);
    
protected:
    int m_nLastSequence;
    int m_nLastPingTime;
    
private:
    virtual void OnErrorHandler();
};

#endif // !defined(AFX_PINGSOCKET_H__752184FA_8742_4259_814E_25442FC6A7ED__INCLUDED_)
