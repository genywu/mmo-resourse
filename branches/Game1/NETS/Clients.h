//=============================================================================
/**
 *  file: Clients.h
 *
 *  Brief:主要是客户端使用的网络封装，用事件机制实现的网络接口
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================

#ifndef _CLIENT_H_
#define _CLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mysocket.h"
#include "MsgQueue.h"
#include "SocketCommands.h"

const int CONNECT_TIMEOUT = 10000;		// 连接等待时间

class CMessage;
class CDataBlockAllocator;

class CClient : public CMySocket  
{
protected:
	CDataBlockAllocator* m_pDBAllocator;

	typedef list<tagSocketOper*>	SocketOpers;
	typedef list<tagSocketOper*>::iterator itSockOP;

	long m_nMaxFreeSockOperNum;
	SocketOpers	m_FreeSocketOpers;
	CRITICAL_SECTION	m_CSSockOper;


	typedef list<CBaseMessage*>	 NetMessages;
	typedef list<CBaseMessage*>::iterator itMsg;
	NetMessages m_SendMessages;

	typedef list<CDataBlock*>	 ListDataBlock;
	typedef list<CDataBlock*>::iterator itDB;
	ListDataBlock	m_ReadDataBlocks;	//已经读取的网络数据
	int	m_nReadDataSize;				//已经读取数据的大小


	HANDLE m_hConnectEventSuccess;		// 连接成功
	bool m_bConnect;					// 是否连接

public:
	void PushReadDataBack(CDataBlock *pDB);
	CDataBlock *PopReadDataFront();
	void PushReadDataFront(CDataBlock *pDB);
	
	HANDLE m_hSocketEvent[2];	// 0 接受消息 1 结束SocketThread
	HANDLE m_hSocketThread;		// 处理事件的进程
	HANDLE m_hNetClientThread;	// 客户端网络主线程

	BOOL CreateSocketThread();	// 建立独立线程管理socket
	BOOL ExitSocketThread();	//推出ClientSocket的线程

	tagSocketOper* AllocSockOper();
	void FreeSockOper(tagSocketOper* pSockOper);
	
	friend unsigned __stdcall SocketThread(void* pArguments);
	friend void DoSocketThread(CClient* pClient);
	friend unsigned __stdcall NetClientThreadFunc(void* pArguments);		//客户端网络主线程 
	friend void DoNetClientThreadFunc(CClient* pClient);					//客户端网络主线程 

	bool m_bSendData;					//标示客户端是否可以发数据
protected:
	bool m_bControlSend;							//控制发送的一个全局标量

	CSocketCommands		m_SocketOperaCommands;		//对网络操作的命令队列
	CMsgQueue			m_RecvMessages;				//接收到的消息队列

public:
	void	SetControlSend(bool bSend)	{m_bControlSend=bSend;}
	bool	GetControlSend()			{return m_bControlSend;}
	
	CSocketCommands& GetSocketCommand()	{return m_SocketOperaCommands;}
	CMsgQueue*	GetRecvMessages()	{return & m_RecvMessages;}

	bool Start(CDataBlockAllocator* pDBAllocator,long nMaxFreeSockOperNum,long lFlag);
	void Exit();

	// 创建一个和事件机制邦定的SOCKET
	virtual BOOL Create(UINT nSocketPort = 0,
		LPCTSTR lpszSocketAddress = NULL,				
		int nSocketType = SOCK_STREAM,	// TCP:SOCK_STREAM UDP:SOCK_DGRAM
		long lEvent = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE,
		bool bAsyncSelect=true  //asyncselect flag,if create socket for client,set the flag=true
		);
	BOOL AsyncSelect(long lEvent = FD_READ|FD_WRITE|FD_ACCEPT|FD_CONNECT|FD_CLOSE);	// 设置网络事件的响应

	int ASend(CBaseMessage *pMsg, int nFlags = 0);
	bool AddSendMsg(CBaseMessage* pMsg);
	int Send();
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);	// [TCP]发送数据

	virtual BOOL Close();				// 关闭socket
	virtual void OnReceive(int nErrorCode);
	void OnReceive(CDataBlock *pDB,long lReadNum);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnSend(int nErrorCode);

	//得到当前消息长度
	long GetCurMsgLen();
	//解码消息
	void DecordMsg();


	// 给派生类使用的接口
	virtual void HandleReceive(){};
	virtual void HandleConnect() {};
	virtual void HandleClose() {};

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);	// 连接
	BOOL ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort);//连接
	bool IsConnect();		// 是否连接
	long WaitForConnect();	// 等待连接

	CClient();
	virtual ~CClient();

//统计流量
protected:
	bool				m_bCheck;					//是否检测

	DWORD				m_dwSendStartTime;			//统计开始时间
	long				m_lTotalSendSize;			//发送的总数量
	long				m_lSendSizePerSecond;		//美秒发送的数量
	LONG64				m_llSendSize;

	DWORD				m_dwRecvStartTime;			//统计开始时间
	long				m_lTotalRecvSize;			//接受总数量
	long				m_lRecvSizePerSecond;		//每秒接受数量
	LONG64				m_llRecvSize;
public:

	bool	GetCheck()	{ return m_bCheck; }

	long	AddSendSize(long lSize);
	long	GetSendSizePerSecond()			{return m_lSendSizePerSecond;}
	LONG64  GetTotalSendSize()				{return m_llSendSize;}

	long	AddRecvSize(long lSize);
	long	GetRecvSizePerSecond()			{return m_lRecvSizePerSecond;}
	LONG64	GetTotalRecvSize()				{return m_llRecvSize;}
};

#endif
