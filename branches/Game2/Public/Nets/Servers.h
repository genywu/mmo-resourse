//=============================================================================
/**
 *  file: Servers.h
 *
 *  Brief:主要是服务器端使用的网络封装，用IOCP方式实现的网络接口
 *			提供了网络监听和主动连接功能
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================


#ifndef _SERVER_H_
#define _SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySocket.h"
#include "MsgQueue.h"
#include "socketcommands.h"

#define CP_TIMEOUT	200

class CServerClient;
class CBaseMessage;
class CDataBlockAllocator;
const int MAX_CLIENTS = 2048;


class CServer : public CMySocket
{
private:
	uchar	m_bMode;	//启动模式

	typedef list<tagSocketOper*>	SocketOpers;
	typedef list<tagSocketOper*>::iterator itSockOP;

	uint m_nMaxFreeSockOperNum;
	SocketOpers	m_FreeSocketOpers;
	CRITICAL_SECTION	m_CSSockOper;

	typedef list<PER_IO_OPERATION_DATA*>	ListIOOpers;
	typedef list<PER_IO_OPERATION_DATA*>::iterator itIOOper;

	uint m_nMaxFreeIOOperNum;
	ListIOOpers	m_FreeListIOOpers;
	CRITICAL_SECTION	m_CSIOOper;

	//最大的空闲连接客户端数量
	long	m_lMaxFreeConClientNum;

	CRITICAL_SECTION	m_CSClient;
	CServerClient	**m_pClients;
	long	m_lTotalClientsNum;


	CDataBlockAllocator*	m_pDBAllocator;

	//每次IO操作的DataBufNum数量
	long	m_lIOOperDataBufNum;
public:
	//启动网络服务器
	bool	Start(	uchar bMode,CDataBlockAllocator* pDBAllocator,
					long nMaxFreeSockOperNum = 100,long nMaxFreeIOOperNum=100,long lIOOperDataBufNum = 2,
					bool bCheck=false,long lForbidTime=1000,
					long lMaxMsgLen=102400,long lMaxConNums=1000,long lTotalClientsNum=10,
					long lPendingWrBufNum=4096,long lPendingRdBufNum=2048,
					long lMaxSendSizePerSecond=8192,long lMaxRecvSizePerSecond =4096,
					long lMaxBlockSendMsgNum = 1024,
					long lConPendingWrBufNum=10240,long lConPendingRdBufNum=10240,
					long lConMaxSendSizePerSecond=81920,long lConMaxRecvSizePerSecond=81920,
					long lConMaxBlockSendMsgNum=1024);

	void InitClients();
	void ReleaseClients();

	//退出服务端套接字
	void Exit();
	//退出完成端口工作者线程
	void ExitWorkerThread(void);

	tagSocketOper* AllocSockOper();
	void FreeSockOper(tagSocketOper* pSockOper);

	PER_IO_OPERATION_DATA* AllocIoOper();
	void FreeIoOper(PER_IO_OPERATION_DATA* pPerIOData);

	//分配一个客户端
	CServerClient*	AllocClient(bool bAccept);
	void FreeClient(CServerClient* pClient);
	// 查找一个客户端
	CServerClient* FindClient(long lSocketID);
	// 删除一个客户端
	bool FreeClient(long lSocketID);

protected:
	// 连上了服务器的套接字数量
	long m_lCurClientCount;

	typedef map<long,CServerClient*> AcceptClients;
	typedef map<long,CServerClient*>::iterator itAClient;
	//被动接受的套接字
	AcceptClients	m_AClients;

	//预分配的AccClients
	typedef list<CServerClient*> FreeAccClients;
	typedef list<CServerClient*>::iterator itFAClient;
	//最大的预分配AccClients数量
	long	m_nMaxFreeAccClientNum;
	FreeAccClients	m_FreeAClients;


	vector<HANDLE> m_hWorkerThreads;			//工作者线程句柄数组
	HANDLE m_hNetMainTheads;					//服务器网络主线程
	HANDLE m_hAcceptThread;						//acceptThread

	bool m_bAcceptThreadExit;					//是否结束AcceptThread
	bool m_bWorkerThreadsExit;					//是否结束工作者线程
public:
	 HANDLE m_hCompletionPort;   //completion port

public:
	long GetClientCount()	{ return m_lCurClientCount;}

	HANDLE* GetWorkerThread()	{ return &m_hWorkerThreads[0];}
	HANDLE GetAcceptThread()	{ return m_hAcceptThread; }
	// 创建Server的套接字，进入监听状态，等待客户端的连接
	bool Host(UINT dwPort, const char* strIP,long lFlag,ulong nSocketType=SOCK_STREAM);
	bool Listen(int nConnectionBacklog=SOMAXCONN);			// listen


	//异步发送消息
	int ASend(long lIndexID,CBaseMessage *pMsg);	
		
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnAccept(int nErrorCode); 

	bool  DisConn(long lIndexID);
	bool  DisConnAll();
	//关闭所有Socket
	bool  CloseAll();
	//初始化操作
	bool OnClientInit(CServerClient* pClient);
	// error的时候会调用
	void OnError(PER_IO_OPERATION_DATA *pPerIOData,long lIndexID,int errorCode);
	void OnClose(PER_IO_OPERATION_DATA *pPerIOData,long lIndexID);
	void OnReceive(PER_IO_OPERATION_DATA *pPerIOData,long lIndexID,long lReadNum);
	void OnSend(PER_IO_OPERATION_DATA *pPerIOData,long lIndexID,long lSendNum);
	void OnSendZeorByte(PER_IO_OPERATION_DATA *pPerIOData,long lIndexID);
	void OnDisConn(long lIndexID);
	long OnDisConnAll();
	long OnCloseAll();

	bool CreateNetMainThread();					//创建网络主线程
	bool CreateWorkerThreads(int nProcNum);		//创建在完成端口上等待的工作者线程
	bool CreateAcceptThread();					//创建接收客户Socket的线程
	bool AssociateSocketWithCP(SOCKET socket,ulong dwCompletionKey);	//用于邦定一个SOCKET到完成端口

	CServer();
	virtual ~CServer();

	friend unsigned __stdcall NetThreadFunc(void* pArguments);
	friend void DoNetThreadFunc(CServer* pServer);
	friend unsigned __stdcall WorkerThreadFunc(void* pArguments);
	friend void DoWorkerThreadFunc(CServer* pServer);
	friend unsigned __stdcall AcceptThreadFunc(void* pArguments);


//
private:
	CSocketCommands		m_SocketOperaCommands;		//对网络操作的命令队列
	CMsgQueue			m_RecvMessages;				//接收到的消息队列

public:
	CMsgQueue* GetRecvMessages()	{return &m_RecvMessages;}

//统计客户端的数据流量
protected:
	bool				m_bCheck;					//是否检测客户端
	long				m_lForbidTime;				//屏蔽IP的时间
	long				m_lMaxMessageLength;		//消息包的最大长度

	long				m_lMaxClientConNum;			//最大的客户端连接数量
	long				m_lPendingWrBufNum;			//等待的最大写操作数量
	long				m_lPendingRdBufNum;			//等待的最大读操作数量
	long				m_lMaxSendSizePerSecond;	//限制的每秒钟发送大小
	long				m_lMaxRecvSizePerSecond;	//限制的每秒钟接受大小 
	long				m_lMaxBlockSendMsgNum;		//最大阻塞发送的消息个数

	//针对连接端的统计
	long				m_lConPendingWrBufNum;		//连接端等待的最大写操作数量
	long				m_lConPendingRdBufNum;		//连接端等待的最大读操作数量	
	long				m_lConMaxSendSizePerSecond;	//连接端限制每秒钟发送数据的大小
	long				m_lConMaxRecvSizePerSecond;	//连接端限制每秒钟接受数据的大小
	long				m_lConMaxBlockSendMsgNum;	//连接端最大阻塞发送的消息个数


	//针对各种连接的数据统计
	struct tagDataStat
	{
		ulong dwSendStartTime;			//统计开始时间
		long lTotalSendSize;			//发送的总数量
		ulong dwRecvStartTime;			//统计开始时间
		long lTotalRecvSize;			//接受总数量
	};

	typedef map<long,tagDataStat>	mapNetDataStat;
	typedef map<long,tagDataStat>::iterator itDataStat;
	mapNetDataStat	m_NetDataStats;


	//具体消息数据统计
	struct tagMsgStat
	{
		long	lNum;//发送或接收的数理那个
		long	lTotalSize;//总大小
	};

	typedef map<long,tagMsgStat>	mapMsgStat;
	typedef map<long,tagMsgStat>::iterator itMsgStat;
	//发送消息的统计
	mapMsgStat	m_SendMsgsStat;
	//接受消息的统计
	mapMsgStat	m_RecvMsgsStat;

	typedef map<long,long>	mapMaxMsgLen;
	typedef map<long,long>::iterator itMaxMsgLen;
	mapMaxMsgLen	m_SendMsgMaxLen;
	mapMaxMsgLen	m_RecvMsgMaxLen;

	CRITICAL_SECTION	m_CSMsgStat;

	
	map<u_long,ulong>	m_ForbidIPs;				//屏蔽IP列表
	ulong				m_lMaxBlockConnetNum;		//最大的阻塞监听数量
	ulong				m_lSendInterTime;			//建立连接以后到接受到数据的最大间隔事件

	ulong				m_dwWorkThreadTick;			//完成端口工作线程心跳计数
	ulong				m_dwNetThreadTick;			//网络线程心跳计数
	ulong				m_dwAcceptThreadTick;		//监听接受线程心跳计数
	

	map<ulong,ulong>	g_NewAcceptSockets;
public:

	inline void IncWorkThreadTick()	{m_dwWorkThreadTick++;}
	inline void IncNetThreadTick()	{m_dwNetThreadTick++;}
	inline void IncAcceptThreadTick()	{m_dwAcceptThreadTick++;}

	ulong	GetWorkThreadTick()	{return m_dwWorkThreadTick;}
	ulong	GetNetThreadTick()	{return m_dwNetThreadTick;}
	ulong	GetAcceptThreadTick() {return m_dwAcceptThreadTick;}

	bool	GetCheck()	{ return m_bCheck; }
	long	GetMaxMsgLen()		{ return m_lMaxMessageLength; }

	void	AddForbidIP(u_long laddr);
	bool	FindForbidIP(u_long laddr);

	long	AddSendSize(long lFlagID,long lSize);
	long	GetSendSizePerSecond(long lFlagID);

	long	AddRecvSize(long lFlagID,long lSize);
	long	GetRecvSizePerSecond(long lFlagID);

	//添加发送消息数据统计
	void	AddSendMsgStat(long lMsgType,long lSize);
	//添加接受消息数据统计
	void	AddRecvMsgStat(long lMsgType,long lSize);
	//输出消息统计信息到net.log文件
	void	OutputMsgStatInfo(ulong lIntelTime =1800000);

	long	GetMaxConNum()				{return m_lMaxClientConNum;}
	long	GetPendingWrBufNum()		{return m_lPendingWrBufNum;}
	long	GetPendingRdBufNum()		{return m_lPendingRdBufNum;}

	long	GetConPendingWrBufNum()		{return m_lConPendingWrBufNum;}
	long	GetConPendingRdBufNum()		{return m_lConPendingRdBufNum;}

	void	SetParamEx(long lBlockConNum,long lSendInterTime)
	{m_lMaxBlockConnetNum=lBlockConNum;m_lSendInterTime=lSendInterTime;}

	void	AddNewAcceptSocket(ulong dwSocketID);
	void	RemoveNewAcceptSocket(ulong dwSocketID);
	void	DoNewAcceptSocket();


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//主动连接端
private:
	
public:	
	//主动连接服务器
	long Connect(LPCTSTR lpszHostAddress, UINT nHostPort,long lFlag,ulong dwTimeOut=0);
	//阻塞连接
	long Connect(CServerClient* pConClient,LPCTSTR lpszHostAddress, UINT nHostPort,ulong dwTimeOut=0);
	//主动关闭连接
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

};

#endif