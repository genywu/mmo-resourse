

#ifndef _CLIENT_H_
#define _CLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mysocket.h"
#include "MsgQueue.h"
#include "SocketCommands.h"

//const int MAX_MESSAGE_LENGTH = 10240;	// 最大消息长度
const int CONNECT_TIMEOUT = 10000;		// 连接等待时间

/*-----------------------------------------------------------
   [CClient] 客户端套接字类

  功能:
	 处理连接和消息发送

  初始化过程:
	[TCP]
		CClient* pClient = new CClient;
		pClient->Create(dwPort, strIP, nSocketType);	// 创建socket
		pClient->CreateSocketThread();					// 创建SocketThread
		pClient->Connect(strServerIP, lServerPort);		// 连接服务器

  	[UDP]
		CClient* pClient = new CClient;
		pClient->Create(dwPort, strIP, nSocketType);	// 创建socket
		pClient->CreateSocketThread();					// 创建SocketThread

------------------------------------------------------------*/
class CMessage;
class CClient : public CMySocket  
{
protected:
	char m_strServerIP[64];				// 服务器IP
	long m_lServerPort;					// 服务器端口

//	char m_pBuffer[MAX_MESSAGE_LENGTH];	// 接受缓冲

	char *m_pBuffer;					// 接受缓冲
	int m_nBufferSize;					// 缓冲区大小
	int m_nSize;						// 缓冲区中当前数据大小

	HANDLE m_hConnectEventSuccess;		// 连接成功
	bool m_bConnect;					// 是否连接

	void* pFinishCallback;			//异步连接的回调函数

public:
	
	HANDLE m_hSocketEvent[2];	// 0 接受消息 1 结束SocketThread
	HANDLE m_hSocketThread;		// 处理事件的进程
	HANDLE m_hNetClientThread;	// 客户端网络主线程

	BOOL CreateSocketThread();	// 建立独立线程管理socket
	BOOL ExitSocketThread();	//推出ClientSocket的线程
	
	friend unsigned __stdcall SocketThread(void* pArguments);
	void DoSocketThread(CClient* pClient);
	friend unsigned __stdcall NetClientThreadFunc(void* pArguments);		//客户端网络主线程 
	friend void DoNetClientThreadFunc(CClient* pClient);					//客户端网络主线程 

	bool m_bSendData;					//标示客户端是否可以发数据
	bool m_bSocketThreadExit;
	bool m_bNetClientThreadExit;

	CSocketCommands		m_ClientSocketOnSendCommands;		//网络表示可否发送消息的一个命令队列


protected:
	bool m_bControlSend;				//控制发送的一个全局标量
	CMsgQueue			m_RecvMessages;				//接收到的消息队列

public:
	void	SetControlSend(bool bSend)	{m_bControlSend=bSend;}
	bool	GetControlSend()			{return m_bControlSend;}
	virtual bool IsSameSocketID(long lSocketID1,long lSocketID2)
	{
		return true;
	}

	virtual CSocketCommands& GetSocketCommand();

	CMsgQueue*	GetRecvMessages()	{return & m_RecvMessages;}

	// 创建SOCKET
	virtual BOOL Create(UINT nSocketPort = 0,
		LPCTSTR lpszSocketAddress = NULL,				
		int nSocketType = SOCK_STREAM,	// TCP:SOCK_STREAM UDP:SOCK_DGRAM
		long lEvent = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE,
		bool bAsyncSelect=true  //asyncselect flag,if create socket for client,set the flag=true
		);

	BOOL AsyncSelect(long lEvent = FD_READ|FD_WRITE|FD_ACCEPT|FD_CONNECT|FD_CLOSE);	// 设置网络事件的响应
	void SetServerPort(DWORD w)		{m_lServerPort = w;}
	long GetServerPort(void)		{return m_lServerPort;}
	void SetServerIP(const char* str)	{strcpy(m_strServerIP,str);}
	char* GetServerIP()				{return m_strServerIP;}

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);	// [TCP]发送数据
	virtual int SendToServer(const void* lpBuf, int nBufLen, bool bFront,int nFlags = 0);//

	virtual BOOL Close();				// 关闭socket
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnSend(int nErrorCode);

	// 给派生类使用的接口
	virtual void HandleReceive() = 0;
	virtual void HandleConnect() = 0;
	virtual void HandleClose() = 0;

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);	// 连接
	BOOL ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort);//连接
	bool IsConnect();		// 是否连接
	long WaitForConnect();	// 等待连接

	CClient();
	virtual ~CClient();

//////////////////////////////////////////////////////
// Socks5代理
//////////////////////////////////////////////////////
protected:
	enum 
	{
		PROXY_INVALID = 0,		// 不可用
		PROXY_VALID = 1,		// 可用
	};

	bool m_bUseProxy;			// 是否使用代理
	char m_strProxyIP[64];		// 代理服务器IP
	long m_lProxyPort;			// 代理服务器端口
	char m_strProxyUser[32];	// 用户
	char m_strProxyPass[32];	// 密码

	long m_lConnectProxyStep;	// 初始化代理的步骤
	bool m_bProxyIsValid;		// 代理服务器是否可用

public:
	void EnableProxy(bool b)	{ m_bUseProxy = b; }
	bool IsUseProxy()			{ return m_bUseProxy; }
	bool IsProxyValid()			{ return m_bProxyIsValid; }

	void SetProxy(const char* ip, long port, const char* user=NULL, const char* pass=NULL);
	void ConnectProxy(long step);

public:
	struct sock5req1
	{
		char Ver;
		char nMethods;
		char Methods[255];
	};

	struct sock5ans1
	{
		char Ver;
		char Method;
	};

	struct sock5req2
	{
		char Ver;
		char Cmd;
		char Rsv;
		char Atyp;
		char other[1];
	};

	struct sock5ans2
	{
		char Ver;
		char Rep;
		char Rsv;
		char Atyp;
		char other[1];
	};

	struct authreq
	{
		char Ver;
		char Ulen;
		char Name[255];
		char PLen;
		char Pass[255];
	};

	struct authans
	{
		char Ver;
		char Status;
	};
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
