#ifndef _SERVERCLIENT_H_
#define _SERVERCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
using namespace std;

#include "Servers.h"

//const int MAX_MESSAGE_LENGTH = 10240;

/*-------------------------------------------------------------------
 [CServerClient] 服务器对应每个客户端的连接

 功能：
	仅用于TCP协议，UDP协议不需要该类。
    当有一个客户端的连接请求到达后，服务器生成一个ServerClient与之关联
 本类负责和具体客户端通讯
--------------------------------------------------------------------*/



class CServerClient : public CMySocket
{
protected:
//	char m_pBuffer[MAX_MESSAGE_LENGTH];	// 接收消息的缓存

	char *m_pBuffer;					// 接受缓冲
	int m_nBufferSize;					// 缓冲区大小
	int m_nSize;						// 缓冲区中当前数据大小

	char *m_pSendBuffer;				// 发送缓冲区
	long m_lMaxSendBuffSize;			// 最大发送缓冲区
	long m_lSendBufferSize;				// 发送缓冲区大小
	long m_lCurSendBufferSize;			// 当前缓冲区大小
	long m_lSendSize;					// 每次发送的数据大小
	
	bool m_bLost;						// 是否掉线了的标志
	bool m_bQuit;						// 套接字是否退出

	long m_bServerType;                 //客户端类型 GameServer Or LogingServer
	long m_lMapID;						//给该Client映射的外部ID
	string m_strMap;					//给该Client映射的外部string

	CServer*		m_pServers;			//控制的父套接字


	bool	m_bCloseFlag;				//是否有删除标记

public:
	bool IsLost()						{return m_bLost;}
	void SetLost(bool b)				{m_bLost = b;}
	bool IsQuit()						{return m_bQuit;}
	void SetQuit(bool b)				{m_bQuit = b;}

	long GetServerType()                { return m_bServerType;}            //获得客户端服务器类型
	void SetServerType(long lservertype){m_bServerType = lservertype;}      //设置客户端服务器类型

	CServer*	GetServer()				{return m_pServers;	}

	void SetMapID(long lID)				{m_lMapID=lID;}
	long GetMapID()						{return m_lMapID;}

	void SetMapStr(const char* str)		{m_strMap=str;}
	const char* GetMapStr()				{return m_strMap.c_str();}

	virtual void SetSendRevBuf();

	void	SetCloseFlag(bool b)		{m_bCloseFlag = b;}
	bool	GetCloseFlag()				{return m_bCloseFlag;}

public:

	CServerClient(CServer*	pServers);
	virtual ~CServerClient();

	virtual void AddReceiveData(CHAR* pBuf,int nBufSize);
	virtual bool AddSendData(void* pBuf,int nBuffSize);
	long	GetCurSendBuffSize()		{return m_lCurSendBufferSize;}
	long	GetSendBuffSize()			{return m_lSendBufferSize;}

	virtual void OnAccept(int errorCode=0)	{};	// 连接成功时激活
	virtual	void OnReceive(int nErrorCode=0);// 完成端口上受到消息时激活
	virtual void OnClose(int errorCode=0)	{};	// 与对方断开的时候激活
	virtual void OnSend(int nErrorCode=0)	{};	// 有消息发送的时候激活

	virtual void OnOneMessageSizeOver(long lSize,long lPermitSize){};
	virtual void OnTotalMessageSizeOver(long lSize,long lPermitSize){};

	bool ReadFromCompletionPort(PER_IO_OPERATION_DATA* pIOData);  //在完成端口读取数据

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);	// [TCP]发送数据

	friend unsigned __stdcall NetThreadFunc(void* pArguments);

//统计客户端的数据流量
private:
	DWORD	m_dwStartTime;						//统计开始时间
	long	m_lTotalSize;						//接受包的总大小
	long	m_lSizePerSecond;					//每秒接受的数据大小

	//统计发送
	DWORD	m_dwSendStartTime;					//发送统计开始时间
	long	m_lTotalSendSize;					//总发送流量
	long	m_lIOOperatorNum;					//当前重叠发送的个数
public:
	long	AddPackageSize(long lSize);
	long	GetCurMsgLength()					//得到当前消息的长度
	{
		if(m_nSize >= sizeof(long) )
		{
			return *((long*)(&m_pBuffer[0]));
		}
		return 0;
	}

	long AddSendPackageSize(long lSize)
	{
		m_lTotalSendSize = m_lTotalSendSize+lSize;
	}

	long GetSendSizePerSecond();

	long GetIOOperatorNum()	{return m_lIOOperatorNum;}
	void DecIoOperatorNum()	{m_lIOOperatorNum--;}
};

#endif
