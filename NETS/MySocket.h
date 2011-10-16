//=============================================================================
/**
 *  file: MySocket.h
 *
 *  Brief:网络基类，提供基本的网络接口
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================

#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <iostream>
#include <vector>
#include "winsock2.h"

using namespace std;

const int MAX_MESSAGE_LENGTH = 1048576*2;
class CBaseMessage;
class CDataBlock;

//对套接字的操作类型
enum SocketOperType
{	
	SOT_Send,			//发送
	SOT_Receive,		//接受
	SOT_Accept,			//连接
	SOT_SendZeroByte,	//发送0字节数据

	SOT_CONSend,		//连接端发送
	SOT_CONReceive,		//连接端接受
};

//IO操作结构:大小:44BYTE
typedef class PER_IO_OPERATION_DATA
{
public:
	OVERLAPPED		OverLapped;
	LPWSABUF		pDataBuf;
	long			m_lDataBufNum;
	void*			pParam;
	SOCKET			hSocket;			// 操作的套接字
	SocketOperType	OperationType;

	int				m_nSequenceNumber;	//操作序列号

public:
	PER_IO_OPERATION_DATA(long lWSABUFNum)
	:pParam(NULL)
	,hSocket(INVALID_SOCKET)
	,m_nSequenceNumber(0)
	,m_lDataBufNum(lWSABUFNum)
	{
		pDataBuf = new WSABUF[m_lDataBufNum];
	}

	~PER_IO_OPERATION_DATA()
	{
		if(pDataBuf)
			delete []pDataBuf;
	}

	void ResetDataBufSize(long lWSABUFNum)
	{
		if(lWSABUFNum != m_lDataBufNum)
		{
			if(pDataBuf)
				delete []pDataBuf;
			m_lDataBufNum = lWSABUFNum;
			pDataBuf = new WSABUF[m_lDataBufNum];
		}
	}

}*LPER_IO_OPERATION_DATA;

/*-----------------------------------------------------------
 [CMySocket] WinSock基类
 本类不直接使用，一般派生后使用。
 
 功能：
	* 建立Socket
	* 绑定端口和地址
	* 设置网络事件的响应
	* 发送数据
	* 接收数据
	* 关闭socket
------------------------------------------------------------*/
class CMessage;
class CMySocket
{
public:
	static BOOL MySocketInit();		// 程序启动时调用，初始化Winsock
	static void MySocketClearUp();	// 程序退出时调用

	CMySocket();
	virtual ~CMySocket();

//////////////////////////////////////////////////////////////////////////
// 基本信息
//////////////////////////////////////////////////////////////////////////
protected:
	long m_lProtocol;		// 当前协议	TCP:SOCK_STREAM UDP:SOCK_DGRAM
	long m_lIndexID;		// 该套Socket分配的ID

	//char m_strIP[32];		// socket IP
	//DWORD m_dwIP;			// IP(4byte)
	//DWORD m_dwPort;			// socket端口

	char m_strPeerIP[32];	// 对方的IP
	DWORD m_dwPeerIP;		// IP(4byte)
	DWORD m_dwPeerPort;		// 对方的端口号

	bool m_bClose;			// socket是否关闭
	bool m_bShutDown;		// 是否shutdown

	long m_lFlag;			// 被逻辑层设置的标志
	unsigned char m_key[8];							/// 密钥轮
	unsigned char m_kn[16][6];						/// 密钥轮
	static long s_lEncryptType;						/// 需要通信加密的类型

public:
	void GetKn(unsigned char kn[16][6])  { memcpy(m_kn, kn, 16*6); }

	/// 设置密钥(针对C/S策略不同)
	void SetKey(int h, int l);
	
	void SetIndexID(long lID)	{m_lIndexID=lID;}
	long GetIndexID()			{return m_lIndexID;	}

	long GetProtocol()			{return m_lProtocol;}
	void SetProtocol(long l)	{m_lProtocol=l;}

	bool IsClose()				{return m_bClose;}
	void SetClose(bool b)		{m_bClose = b;}

	bool IsShutDown()			{return m_bShutDown;}
	void SetShutDown(bool b)	{m_bShutDown = b;}
	
	//void SetIP(const char* str)	{ strcpy(m_strIP, str); }
	//char* GetIP(void)			{ return m_strIP; }
	//void SetDWIP(DWORD dw)		{ m_dwIP = dw; }
	//DWORD GetDWIP(void)			{ return m_dwIP; }
	//void SetPort(DWORD dw)		{ m_dwPort = dw; }
	//DWORD GetPort(void)			{ return m_dwPort; }

	void SetPeerIP(const char* str)	{ strcpy(m_strPeerIP, str); }
	char* GetPeerIP(void)		{ return m_strPeerIP; }
	void SetPeerIP(DWORD dw)	{ m_dwPeerIP = dw; }
	DWORD GetDWPeerIP(void)		{ return m_dwPeerIP; }
	void SetPeerPort(DWORD dw)	{ m_dwPeerPort = dw;}
	DWORD GetPeerPort(void)		{ return m_dwPeerPort; }

	void SetFlag(long lFlag)	{m_lFlag = lFlag;}
	long GetFlag()				{return m_lFlag;}
	BOOL IsEncryptType(long lFlag) { return s_lEncryptType & lFlag; }
	static void SetEncryptFlag(long l) { s_lEncryptType = l; }

//////////////////////////////////////////////////////////////////////////
// [UDP] 接收上一条消息的信息
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD m_dwLastRecvPort;		// [UDP] 上一条收到的消息的端口
	char m_strLastRecvIP[32];	// [UDP] 上一条收到的消息的IP

public:
	void SetLastRecvPort(DWORD w)		{m_dwLastRecvPort = w;}
	DWORD GetLastRecvPort(void)			{return m_dwLastRecvPort;}
	void SetLastRecvIP(const char* str)	{strcpy(m_strLastRecvIP,str);}
	char* GetLastRecvIP()				{return m_strLastRecvIP;}

//////////////////////////////////////////////////////////////////////////
// 接口
//////////////////////////////////////////////////////////////////////////
public:
	// 创建一个和事件机制邦定的SOCKET
	virtual BOOL Create(UINT nSocketPort = 0,
				LPCTSTR lpszSocketAddress = NULL,				
				int nSocketType = SOCK_STREAM,	// TCP:SOCK_STREAM UDP:SOCK_DGRAM
				long lEvent = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE,
				bool bAsyncSelect=true);
	
	//创建一个和完成端口邦定的SOCKET
	virtual BOOL CreateEx(	UINT nSocketPort,
					LPCTSTR lpszSocketAddress,
					int nSocketType=SOCK_STREAM);

	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);	// 绑定端口和地址

	//设置socket选项
	virtual void SetSocketOpt();

	//得到本机的IP
	void SetHostName();
	
	virtual BOOL Close();	// 关闭socket，先等待SocketThread结束
	//优雅的关闭套接字
	bool ShutDown();
	
	virtual int Recv(void* lpBuf, int nBufLen, int nFlags = 0);			// [TCP]接收数据
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);	// [TCP]发送数据
	virtual int RecvFrom(void* lpBuf, int nBufLen, char* strIP, DWORD& dwPort, int nFlags = 0);		// [UDP]发送数据
	virtual int Sendto(const void* lpBuf, int nBufLen, const char* strIP, DWORD dwPort, int nFlags = 0);	// [UDP]接收数据

public:
	virtual void OnReceive(int nErrorCode)	{};
	virtual void OnSend(int nErrorCode)		{};
	virtual void OnOutOfBandData(int nErrorCode)	{};
	virtual void OnAccept(int nErrorCode)	{};
	virtual void OnConnect(int nErrorCode)	{};
	virtual void OnClose(int nErrorCode=0);	// 关闭socket (收到FD_CLOSE)
	virtual void SetSendRevBuf(){}

//////////////////////////////////////////////////////////////////////////
// 工具函数
//////////////////////////////////////////////////////////////////////////
public:
	BOOL GetPeerName(char * rPeerAddress, UINT& rPeerPort);		// 获取与套接字相连的地址
	BOOL GetSockName(char * rSocketAddress, UINT& rSocketPort);	// 获取本地地址
	BOOL IOCtl(long lCommand=FIONREAD, DWORD* lpArgument=0);	// 获取现有缓冲区数据的大小
	
//////////////////////////////////////////////////////////////////////////
// SocketThread
//////////////////////////////////////////////////////////////////////////
public:
	SOCKET m_hSocket;			// 套接字
	friend class	CMessage;
};

//得到一个ID分配给Socket
long GetSocketID();
unsigned long STR2UL(const char *szIP);

#endif
