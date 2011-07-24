////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   BusyThread.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-10  10:30:13
//  Comment     :   Auxi-work thread header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUSYTHREAD_H__8A849C15_8339_4E14_9BA8_4BD6D2C4BF76__INCLUDED_)
#define AFX_BUSYTHREAD_H__8A849C15_8339_4E14_9BA8_4BD6D2C4BF76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBusyThread
{
public:
    CBusyThread();
    virtual ~CBusyThread();
    
public:
    int IsThreadOK();

	HANDLE GetSafeHandle() {return m_hProcessor;}
  
	virtual int StopThread();
	virtual int StartThread();

	virtual int PreExecution();
    virtual void PostExecution();
    virtual ULONG MainExecution() = 0;
    
private:
    static  DWORD WINAPI InnerThreadProc(LPVOID lpThisParam);
    
protected:
	HANDLE m_hStop;	// Thread running signal
	HANDLE m_hProcessor;		// Thread handle
};

#endif // !defined(AFX_BUSYTHREAD_H__8A849C15_8339_4E14_9BA8_4BD6D2C4BF76__INCLUDED_)
