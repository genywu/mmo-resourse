////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   PingThread.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-10  10:30:13
//  Comment     :   Tcp/ip ping thread header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINGTHREAD_H__BF9EABCA_24CC_48A9_8415_DFD06A7F07AD__INCLUDED_)
#define AFX_PINGTHREAD_H__BF9EABCA_24CC_48A9_8415_DFD06A7F07AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PingSocket.h"

class IKPingCallback
{
public:
    virtual int  GetNextUrl(
        ULONG      *pulContextWith,
        int         nUrlBufSize,
        char       *pszUrlTextBuf
    ) = 0;
    virtual int  PingStatus(
        ULONG       ulContextWith,
        ULONG       ulStatusType,
        ULONG       ulStatusParam
    ) = 0;
};


#define PINGTHREAD_PINGCOUNT                3
#define PINGTHREAD_WAITEXITTIME             4000

#define PINGTHREAD_URLMAXLENGTH             1024


class CPingThread
  : public CPingSocket
{
public:
    CPingThread();
    virtual ~CPingThread();
    
public:
    void SetTimeOut(int nTimeOut = DEFAULT_PING_TIMEOUT);
    void SetSplitTimes(int nTimes = PINGTHREAD_PINGCOUNT);
    
    void SetCallback(IKPingCallback *piCallback);
    
    int StartPingThread();
    int StopPingThread(int nForceStop = false);
    
    HANDLE GetThreadHandle();
    
protected:
    int  DoStatusNotify(ULONG ulOccurEvent, ULONG ulOccurParam);
    void SetContext(ULONG ulContext);
    
protected:
    virtual int OnPingStatus(int nOccurEvent);
    
protected:
    ULONG MainExecution();
    
private:
    static ULONG WINAPI PingThreadProc(LPVOID lpThisParam);
    
protected:
    int m_nRepeatCount;
    int m_nPingTimeOut;
    
    HANDLE m_hPingThread;
    ULONG  m_ulPingThreadId;
    
    ULONG m_ulContextWith;
    IKPingCallback *m_piCallback;
};

#endif // !defined(AFX_PINGTHREAD_H__BF9EABCA_24CC_48A9_8415_DFD06A7F07AD__INCLUDED_)
