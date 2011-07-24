////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   CheckThread.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-17  10:24:27
//  Comment     :   KAVuser-net check thread header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKTHREAD_H__188A7CAB_A1DA_474F_94CA_E8E2BC751447__INCLUDED_)
#define AFX_CHECKTHREAD_H__188A7CAB_A1DA_474F_94CA_E8E2BC751447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BusyThread.h"


#define defCHECK_SERVER_PORT                6868
#define defCHECK_RECEIVE_MAX_SIZE           1000

#define defCHECK_TIME_SEP                   60
#define defCHECK_TIME_OUT                   60000
#define defCHECK_TRY_TIMES                  5


class IKCheckCallback
{
public:
    virtual int GetCheckInfo(
        ULONG   ulContextWith,
        int     *pnSendSize,
        unsigned char **ppbySendInfo
    ) = 0;
    virtual int CheckRecvInfo(
        ULONG   ulContextWith,
        int     nRecvSize,
        unsigned char  *pbyRecvInfo
    ) = 0;
    virtual void CheckResult(
        ULONG   ulContextWith,
        int     nServerResult
    ) = 0;
};


class CCheckThread
  : public CBusyThread
{
public:
    CCheckThread();
    virtual ~CCheckThread();
    
public:
    int SetCheckCallback(IKCheckCallback *piCallback, ULONG ulContextWith);
    int AddCheckServer(const char *pszCheckServer, int nCheckPort = defCHECK_SERVER_PORT);
    
public:
    virtual int Create();
    virtual void Destroy();
    
public:
    virtual int StartThread();
    
    virtual int RemindStop();
    
protected:
    virtual int QueryStop();
    
    virtual ULONG MainExecution();
    
private:
    HANDLE  m_hKillEvent;
    
    ULONG   m_ulContextWith;
    IKCheckCallback *m_piCallback;
    
    CPtrArray m_paServerAddress;
    CWordArray m_waServerPort;
    
    int     m_nTimeOut;
    int     m_nTryTimes;
};

#endif // !defined(AFX_CHECKTHREAD_H__188A7CAB_A1DA_474F_94CA_E8E2BC751447__INCLUDED_)
