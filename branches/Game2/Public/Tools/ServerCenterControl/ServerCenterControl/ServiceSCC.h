#pragma once

//#include "../../../public/UseMemManageSCC.h"
#include "ServiceNet.h"


class ServiceSCC
{
public:
	ServiceSCC(VOID);
	~ServiceSCC(VOID);
public:
	static		ServiceSCC&			GetInstance(VOID);

public:
	//!								初始化程序
	BOOL							Init(VOID);
	//!								执行程序
	VOID							Begin(VOID);
	//!								外部结束程序（不能跟RUN在同一个线程调用）
	VOID							CloseExec(VOID);
	//!								释放程序
	VOID							Relese(VOID);

#ifndef OUT_SERVICE
	//!								执行输入指令
	VOID							RunCommand(VOID);
	//!								外部传入的指令编号
	VOID							InputCommand(DWORD dwCommand);
#endif

public:
	//!								响应SUC连接
	VOID							OnConnSUC(LONG lSocket);
	//!								响应断开SUC
	VOID							OnLostSUC(LONG lSocket);
	//!								响应SM连接
	DWORD							OnConnSM(LONG lSocket, LONG lIP);
	//!								响应断开SM
	VOID							OnLostSM(DWORD dwSMID);

private:
	//!								执行一帧程序，让函数返回FALSE时结束程序（内部结束）
	BOOL							RunOneFrame(VOID);
	//!
	static	unsigned	__stdcall	MainLoopFunc( void* pThis );

private:
	static	ULONG					c_InstanceNum;
	static	ServiceSCC				c_ServiceSCCInstance;
	

private:
	//!								外部通知结束的标志
	BOOL							m_bExotericExit;
	//!								互拆锁
	HANDLE							m_hMutex;
	//!								主线程句柄
	HANDLE							m_hMainThrd;

#ifndef OUT_SERVICE
	DWORD							m_dwCommand;
#endif
};