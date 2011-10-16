//=============================================================================
/**
 *  file: ConClient.h
 *
 *  Brief:用完成端口实现的主动连接端。
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-6-11
 */
//=============================================================================
#ifndef __CONCLIENT__H__
#define __CONCLIENT__H__
#pragma once

class CServer;
#include "mysocket.h"

class CConClient :
	public CMySocket
{
protected:
	char *m_pBuffer;					// 接受缓冲
	int m_nIniMsgLength;				// 初始的消息长度
	int m_nBufferSize;					// 缓冲区大小
	int m_nSize;						// 缓冲区中当前数据大小

	char *m_pSendBuffer;				// 发送缓冲区
	long m_lMaxSendBuffSize;			// 最大发送缓冲区
	long m_lSendBufferSize;				// 发送缓冲区大小
	long m_lCurSendBufferSize;			// 当前缓冲区大小
	long m_lSendSize;					// 每次发送的数据大小
public:
	CConClient(CServer*	pServers);
public:
	virtual ~CConClient(void);

private:
	CServer*		m_pServers;			//控制的父套接字

	long	m_lIOOperatorNum;			//当前重叠发送的个数
	bool	m_bCloseFlag;				//是否有删除标记
public:
	//创建一个和完成端口邦定的SOCKET
	virtual bool CreateEx(UINT nSocketPort = 0,
		LPCTSTR lpszSocketAddress = NULL,				
		int nSocketType = SOCK_STREAM	// TCP:SOCK_STREAM UDP:SOCK_DGRAM
		);
	virtual bool Close();

	//连接指定服务器
	bool Connect(LPCTSTR lpszHostAddress, UINT nHostPort,ulong dwTimeOut=0);

	//发送数据
	bool Read(PER_IO_OPERATION_DATA* pIOData);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnReceive(int nErrorCode=0);
	virtual void OnConnect(int nErrorCode=0);
	virtual void OnClose(int nErrorCode=0);
	virtual void OnSend(int nErrorCode=0);

	// 给派生类使用的接口
	virtual void HandleReceive();
	virtual void HandleConnect();
	virtual void HandleClose();

	void AddReceiveData(CHAR* pBuf,int nBufSize);
	bool AddSendData(void* pBuf,int nBuffSize);

	long GetIOOperatorNum()	{return m_lIOOperatorNum;}
	void DecIoOperatorNum()	{m_lIOOperatorNum--;}

	void	SetCloseFlag(bool b)		{m_bCloseFlag = b;}
	bool	GetCloseFlag()				{return m_bCloseFlag;}

	long	GetCurSendBuffSize()		{return m_lCurSendBufferSize;}
	long	GetSendBuffSize()			{return m_lSendBufferSize;}

	CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);
};

#endif