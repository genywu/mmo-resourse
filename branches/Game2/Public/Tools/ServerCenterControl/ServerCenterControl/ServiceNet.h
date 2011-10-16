#pragma once

#include "..\Net\AinMessage.h"

class CServer;
class CDataBlockAllocator;
struct tagTempConnSetup;

class ServiceNet
{
public:
	ServiceNet(VOID);
	~ServiceNet(VOID);

public:
	static		ServiceNet&			GetInstance(VOID);

public:
	//!								初始化
	BOOL							Init(VOID);
	//!
	VOID							ProcessMessage(VOID);
	//!
	VOID							Release(VOID);

	//!
	AinMsgQueue&					GetMsgQueue(VOID) {return m_AinMsgQueue;}

public:
	/*
	//!								连接所有SM
	VOID							ConnectAllSM(VOID);
	//!								连接一个SM
	VOID							ConnectSM(DWORD dwSMID);
	//!								响应SM连接成功
	VOID							OnConnectSM(VOID);
	*/

private:
	//!								初始化网络服务
	BOOL							InitNetServer_SUC(VOID);
	BOOL							InitNetServer_SM(VOID);
	/* 主动连接SM的接口
	//!								连接一个服务器
	BOOL							ConnectSM(tagTempConnSetup &TempConnSetup);
	//!								添加一个SM配置到异步连接
	VOID							AddToAsynConnectSM(tagTempConnSetup &TempConnSetup);

private:
	//!
	static	unsigned	__stdcall	ConnectSMFunc( void* pThis );
	*/

private:
	//!								数据块分配器
	CDataBlockAllocator				*m_pDBAllocator;
	//!								负责接受客户端消息的网络服务
	CServer							*s_pNetServer_SUC;
	//!								负责接受客户端消息的网络服务
	CServer							*s_pNetServer_SM;
	//!								作为SM的客户端
	set<LONG>						m_mapMsSocket;

	//!
	AinMsgQueue						m_AinMsgQueue;

	

private:
	struct tagSocketAndID 
	{
		DWORD	dwID;
		LONG	lSocket;
		tagSocketAndID(DWORD dwInID, LONG lInSocket)
			:dwID(dwInID), lSocket(lInSocket)
		{}
	};
	

	/* 主动连接SM的属性
	//!
	BOOL							m_bIsWorking;
	//!					
	LockOfWin32						m_ConnLock;
	//!								没有连上的SM
	list<tagTempConnSetup>			m_listLostSM;
	//!								正在连接SM的线程
	HANDLE							m_hConnThrd;
	//!								结束连接线程标记
	BOOL							m_bEndConnThrd;

	//!								响应连接成功的锁
	LockOfWin32						m_OnConnLock;
	//!								连接成功的Socket
	list<tagSocketAndID>			m_listSocket;
	*/

private:
	static	ServiceNet				c_ServiceNet;
};