#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <iostream>
#include <vector>
#include "winsock2.h"

using namespace std;

const int MAX_MESSAGE_LENGTH = 1048576;

//对套接字的操作类型
enum SocketOperType
{	
	SOT_Send,			//发送
	SOT_Receive,		//接受
	SOT_Accept,			//连接
};

//IO操作结构:大小:44BYTE
typedef struct PER_IO_OPERATION_DATA
{
	OVERLAPPED	OverLapped;
	WSABUF		DataBuf;

	CHAR*		pBuffer;
	int			nBufSize;

	SOCKET		hSocket;			// 操作的套接字
	SocketOperType	OperationType;

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
	long m_lProtocol;	// 当前协议	TCP:SOCK_STREAM UDP:SOCK_DGRAM
	DWORD m_dwPort;		// socket端口
	char m_strIP[32];	// socket IP

	DWORD m_dwIP;		// IP(4byte)
	bool m_bClose;		// socket是否关闭

	long m_lIndexID;	//该套Socket分配的ID

public:
	void SetIndexID(long lID)	{m_lIndexID=lID;}
	long GetIndexID()			{return m_lIndexID;	}

	long GetProtocol()			{return m_lProtocol;}
	void SetProtocol(long l)	{m_lProtocol=l;}

	void SetPort(DWORD w)		{m_dwPort = w;}
	DWORD GetPort(void)			{return m_dwPort;}
	void SetIP(const char* str)	{strcpy(m_strIP,str);}
	char* GetIP()				{return m_strIP;}
	void SetDwordIP(DWORD ip)	{m_dwIP = ip;}
	DWORD GetDwordIP()			{return m_dwIP;}
	bool IsClose()				{return m_bClose;}
	void SetClose(bool b)		{m_bClose = b;}

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
	// 创建SOCKET
	virtual BOOL Create(UINT nSocketPort = 0,
				LPCTSTR lpszSocketAddress = NULL,				
				int nSocketType = SOCK_STREAM,	// TCP:SOCK_STREAM UDP:SOCK_DGRAM
				long lEvent = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE,
				bool bAsyncSelect=true  //asyncselect flag,if create socket for client,set the flag=true
				);

	//创建 overlapped socket
	BOOL WSACreate(UINT nSocketPort,LPCTSTR lpszSocketAddress,int nSocketType=SOCK_STREAM);

	// winsock2默认 reuseaddr = 1 dontlinger = 1(linger = 0)
	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL, int reuseaddr=1, int dontlinger=1);	// 绑定端口和地址
	
	virtual BOOL Close();	// 关闭socket，先等待SocketThread结束
	
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

#ifdef _DEBUG
#ifdef _CLIENT
//##日志相关
	ofstream		ofLogFile;
	BOOL			bIsLogFileOpened;

	VOID			Log					( CMessage* pMsg, BOOL bIsSendMessage = FALSE );
#endif
#endif

	friend class	CMessage;

};

//得到一个ID分配给Socket
long GetSocketID();

unsigned long STR2UL(const char *szIP);

#endif
