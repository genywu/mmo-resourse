#ifndef _SERVER_H_
#define _SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySocket.h"
#include "MsgQueue.h"
#include ".\socketcommands.h"

#define CP_TIMEOUT	200

class CServerClient;
const int MAX_CLIENTS = 2048;

typedef map<long,CServerClient*> SocketIDClient;

// client info
struct AllowedClient
{
	char			_ip[32];
	unsigned short	_port;
	unsigned long	_sock;
};

class CServer : public CMySocket
{
protected:
	long m_lClientSocketCount;							// 连上了服务器的套接字数量

	SocketIDClient				 m_Clients;				//连接到服务器的客户端套接字列表

	map<long, AllowedClient *>	 m_MapIDSocketID;		//玩家ID对应的套接字列表<_sock, AllowedClient>
	map<string,long>			 m_MapStrSocketID;	    //玩家Str对应的套接字

	vector<HANDLE> m_hWorkerThreads;			//工作者线程句柄数组
	HANDLE m_hNetMainTheads;					//服务器网络主线程
	HANDLE m_hAcceptThread;						//acceptThread

	bool m_bNetMainThreadExit;					//是否主线程退出
	bool m_bAcceptThreadExit;					//是否结束AcceptThread
	bool m_bWorkerThreadsExit;					//是否结束工作者线程

	DWORD m_dwAcceptThreadSleepTime;			//接受线程睡眠时间
	DWORD m_dwNetMainThreadSleepTime;			//主线程睡眠时间

	bool AddAClient(long lSocketID,CServerClient* pClient);	// 添加一个客户端
	CServerClient* FindOneClient(long lSocketID);			// 查找一个客户端
	bool DelOneClient(long lSocketID);						// 删除一个客户端
	bool						_cl_available;				// clients list is available

public:
	 HANDLE m_hCompletionPort;   //completion port

public:
	bool GetClientCheck() { return _cl_available; }
	bool LoadAllowedClient( const char *filename );
	long CheckAllowedClient( const struct sockaddr_in &addr );
	AllowedClient *GetClientByID( unsigned long socket );
	long GetClientSocketCount()	{ return m_lClientSocketCount;}
	HANDLE* GetWorkerThread()	{ return &m_hWorkerThreads[0];}
	HANDLE GetAcceptThread()	{ return m_hAcceptThread; }
	BOOL Host(UINT dwPort, const char* strIP,DWORD nSocketType=SOCK_STREAM,bool bUseCP=false);	// 创建Server的套接字，进入监听状态，等待客户端的连接
	BOOL Listen(int nConnectionBacklog=SOMAXCONN);			// listen
	virtual int SendBySocketID(long lSocketID, void *buffer, unsigned long size);			// 向某个SocketID发送数据
	virtual int SendByMapID(long lMapID, void *buffer, unsigned long size);					// 向某个MapID发送数据
	virtual int SendByMapStr(const char* strMap, void *buffer, unsigned long size);			//向某个MapStr发送数据
	int SendAll(void* buffer,unsigned long size);						// 发送广播
	int Sendto(void *buffer, unsigned long size, const char* strIP, DWORD dwPort);	// [UDP] 对某一个ip发送消息
	int SendtoAll(void* buffer,unsigned long size)	{};	// [UDP] 发送广播

	int QuitClientBySocketID(long lSocketID);								// 断掉某个套接字
	int QuitClientByMapID(long lMapID);										// 断掉某个映射的ID的客户端
	int QuitClientByMapStr(const char* strMap);								// 断掉某个映射的ID的客户端

	int OnQuitOneClient(long lSocketID);									//
	int OnQuitAllClient();

	int SetClientMapID(long lSocketID,long lMapID = 0);						//设置客户端套接字的PlayerID
	long GetSocketIDByMapID(long lMapID);									//根据MapID得到SocketID

	int SetClientMapStr(long lSocketID,const char* strMap);					//设置客户端套接字的PlayerID
	int GetSocketIDByMapStr(const char* strMap);							//设置客户端套接字的PlayerID

	AllowedClient *IsExistMapID(long lID);										//是否存在对应MapID的客户端
	bool IsExistMapStr(const char* strMap);									//是否存在对应Mapstr的客户端

	void Exit();															//退出服务端套接字
	void ExitWorkerThread(void);// {m_bWorkerThreadsExit = true;}			//设置退出完成端口工作者线程标志
	
	virtual void OnClose(int nErrorCode);	// [TCP] Close的时候会调用
	virtual void OnError(int errorCode);	// error的时候会调用
	virtual void OnReceive(int nErrorCode);	// [UDP] 在接收到消息时调用
	virtual void OnAccept(int nErrorCode); //SERVER在完成端口模式下
	virtual CServerClient* CreateServerClient();

	virtual void OnMapIDError(long lID)	{};
	virtual void OnMapStrError(const char* strMap){};

	BOOL CreateNetMainThread();					//创建网络主线程
	BOOL CreateWorkerThreads(int nProcNum);		//创建在完成端口上等待的工作者线程
	BOOL CreateAcceptThread();					//创建接收客户Socket的线程

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
	bool				m_bCheckMsgCon;				//是否检测消息内容
	long				m_lSizePerSecond;			//客户端的数据流量标准
	long				m_lForbidTime;				//屏蔽IP的时间
	long				m_lMaxMessageLength;		//消息包的最大长度

	DWORD				m_dwStartTime;				//统计开始时间
	long				m_lTotalSendSize;			//发送的总数量
	long				m_lSendSizePerSecond;		//美秒发送的数量
	LONG64				m_llSendSize;

	DWORD				m_dwRecvStartTime;			//统计开始时间
	long				m_lTotalRecvSize;			//接受总数量
	long				m_lRecvSizePerSecond;		//每秒接受数量
	LONG64				m_llRecvSize;

	long				m_lMaxClientConNum;			//最大的客户端连接数量
	long				m_lMaxIPOperatorNum;		//在每个客户端最大的发送操作数量
	long				m_lPermitMaxClientSendBufSize;//最大的客户端发送缓冲区
	map<u_long,DWORD>	m_ForbidIPs;				//屏蔽IP列表
	long				m_lMaxBlockConnetNum;
	long				m_lSendInterTime;
	

	map<DWORD,DWORD>	g_NewAcceptSockets;
public:

	bool	GetCheck()	{ return m_bCheck; }
	bool	GetCheckMsgCon()	{return m_bCheckMsgCon;	}
	long	GetSizePerSecond(){ return m_lSizePerSecond; }
	long	GetMaxMsgLen()		{ return m_lMaxMessageLength; }

	void	SetPraram(bool bCheck,bool bCheckCon,long lSizePerSecond,long lForbidTime,long lMaxMsgLen,
		long lMaxConNums,long lSendIONums,long lMaxClientSendBufSize)
	{
		m_bCheck = bCheck;m_bCheckMsgCon=bCheckCon;
		m_lSizePerSecond = lSizePerSecond;
		m_lForbidTime = lForbidTime;m_lMaxMessageLength=lMaxMsgLen;
		m_lMaxClientConNum = lMaxConNums;m_lMaxIPOperatorNum = lSendIONums;
		m_lPermitMaxClientSendBufSize = lMaxClientSendBufSize;
	}

	void	AddForbidIP(u_long laddr);
	bool	FindForbidIP(u_long laddr);


	long	AddPackageSize(long lSize);
	long	GetSendSizePerSecond()			{return m_lSendSizePerSecond;}
	LONG64  GetTotalSendSize()				{return m_llSendSize;}

	long	AddRecvSize(long lSize);
	long	GetRecvSizePerSecond()			{return m_lRecvSizePerSecond;}
	LONG64	GetTotalRecvSize()				{return m_llRecvSize;}

	long	GetMaxConNum()				{return m_lMaxClientConNum;}
	long	GetMaxIOOperNum()			{return m_lMaxIPOperatorNum;}

	long	GetMaxClientSendBufSize()	{return m_lPermitMaxClientSendBufSize;}

	void	SetParamEx(long lBlockConNum,long lSendInterTime)
	{m_lMaxBlockConnetNum=lBlockConNum;m_lSendInterTime=lSendInterTime;}

	void	AddNewAcceptSocket(DWORD dwSocketID);
	void	RemoveNewAcceptSocket(DWORD dwSocketID);
	void	DoNewAcceptSocket();
};

#endif