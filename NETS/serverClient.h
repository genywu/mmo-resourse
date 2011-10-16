//=============================================================================
/**
 *  file: serverClient.h
 *
 *  Brief:主要是服务器端使用的网络封装，用IOCP方式实现的网络接口
 *		当有一个客户端的连接请求到达后，服务器生成一个ServerClient与之关联
 *		本类负责和具体客户端通讯
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================


#ifndef _SERVERCLIENT_H_
#define _SERVERCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
using namespace std;

#include "Servers.h"

class CBaseMessage;

/*-------------------------------------------------------------------
 [CServerClient] 服务器对应每个客户端的连接

 功能：
	仅用于TCP协议，UDP协议不需要该类。
   
--------------------------------------------------------------------*/

#define MAXIMUMSEQUENSENUMBER 10000

class CServerClient : public CMySocket
{
protected:	
	bool m_bLost;						// 是否掉线了的标志
	bool m_bQuit;						// 套接字是否退出
	bool m_bAccept;						// 是否是Accept产生的套接字

	bool m_bSendZeroByteData;			// 是否已经发送了0字节大小的数据

	bool m_bTransfersStatus;			//发送的传输状况,true:表示拥挤,false:表示畅通
	//是否正在使用中
	bool m_bUsing;
	//使用计数
	WORD m_nUseCount;

	long m_bServerType;                 //客户端类型 GameServer Or LogingServer

	CServer*		m_pServers;			//控制的父套接字
	CDataBlockAllocator* m_pDBAllocator;

	typedef list<CBaseMessage*>	 NetMessages;
	typedef list<CBaseMessage*>::iterator itMsg;
	
	NetMessages m_SendMessages;

	typedef list<CDataBlock*>	 ListDataBlock;
	typedef list<CDataBlock*>::iterator itDB;
	ListDataBlock	m_ReadDataBlocks;	//已经读取的网络数据
	int	m_nReadDataSize;				//已经读取数据的大小

	
	// 用于顺序读取的变量
	typedef map<int,CDataBlock*>	DataBlockMap;
	typedef map<int,CDataBlock*>::iterator itDBMap;
	DWORD				m_ReadSequenceNumber;
	DWORD				m_CurrentReadSequenceNumber;
	DataBlockMap		m_ReadBuffer;

public:
	bool IsLost()				{return m_bLost;}
	void SetLost(bool b)		{m_bLost = b;}
	bool IsQuit()				{return m_bQuit;}
	void SetQuit(bool b)		{m_bQuit = b;}

	void SetIsAccpet(bool b)	{m_bAccept = b;}
	bool IsAccept()				{return m_bAccept;}

	bool IsUsing()				{return m_bUsing;}
	void SetUsing(bool bUsing)	{m_bUsing=bUsing;}

	bool IsTransCong()			{return m_bTransfersStatus;}
	void SetTransCong(bool b)	{m_bTransfersStatus=b;}

	WORD GetUseCount()			{return m_nUseCount;}
	void IncUseCount();

	long GetServerType()                { return m_bServerType;}            //获得客户端服务器类型
	void SetServerType(long lservertype){m_bServerType = lservertype;}      //设置客户端服务器类型

	CServer*	GetServer()				{return m_pServers;	}
	virtual void SetSendRevBuf();

public:

	CServerClient(CServer*	pServers,CDataBlockAllocator* pDBAllocator);
	virtual ~CServerClient();

	void Init();
	void Release();
	void SetParam(bool bChecked,long lMaxMsgLen = 0xFFFFFFF,long lMaxBlockSendMsgNum=1024,
				long lMaxPendingWrBufNum=8192,long lMaxPendingRdBufNum=4096,
				long lMaxSendSizePerSecond=8192,long lMaxRcvSizePerSecond=4096,
				long lFlag=0);

	bool AddSendMsg(CBaseMessage* pMsg);
	bool AddReceiveData(long lSequeNumber,CDataBlock *pDB);
	//得到当前消息长度
	long GetCurMsgLen();

	void PushReadDataBack(CDataBlock *pDB);
	CDataBlock *PopReadDataFront();
	void PushReadDataFront(CDataBlock *pDB);

	
	// 连接成功时激活
	virtual void OnAccept(int errorCode=0);
	// 完成端口上受到消息时激活
	virtual	void OnReceive(int nErrorCode=0);
	// 与对方断开的时候激活
	virtual void OnClose(int errorCode=0);
	// 有消息发送的时候激活
	virtual void OnSend(int nErrorCode=0)	{};
	//发送0字节数据返回
	void OnSendZeroByteData();
	//当网络传输变化的时候
	void OnTransferChange();
	
	virtual void OnOneMessageSizeOver(long lSize,long lPermitSize){};
	virtual void OnTotalMessageSizeOver(long lSize,long lPermitSize){};
	bool StartReadData();
	bool ReadFromCompletionPort(PER_IO_OPERATION_DATA* pIOData,CDataBlock *pDB=NULL);  //在完成端口读取数据
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);	// [TCP]发送数据

	virtual bool CheckMsg(CBaseMessage *pMsg)	{return true;}

	friend unsigned __stdcall NetThreadFunc(void* pArguments);
private:
	//当发送数据流量大于限制流量时,发送0字节数据
	//用来在下一个周期中来检测、发送数据
	void SendZeroByteData();
	int Send(CBaseMessage *pMsg,int nFlags);
//统计客户端的数据流量
private:
	//统计接受
	DWORD	m_dwRcvStartTime;					//接受统计开始时间
	long	m_lCurTotalRcvSize;					//接受包的总大小
	long	m_lCurRcvSizePerSecond;				//当前每秒接受的数据大小
	long	m_lMaxRcvSizePerSecond;				//最大每秒接受大小

	//统计发送
	DWORD	m_dwSendStartTime;					//发送统计开始时间
	long	m_lCurTotalSendSize;				//当前总发送大小
	long	m_lCurSendSizePerSecond;			//当前每秒发送大小
	long	m_lMaxSendSizePerSecond;			//每秒最大发送大小

	bool	m_bChecked;							//是否检验消息
	long	m_lMaxMsgLen;						//最大消息长度
	long	m_lPendingWrBufNum;					//当前挂起发送缓冲区的总大小
	long	m_lMaxPendingWrBufNum;				//最大挂起发送缓冲区的总大小
	long	m_lMaxPendingRdBufNum;				//挂起的接受缓冲区的总大小
	long	m_lMaxBlockSendMsnNum;				//最大阻塞的发送消息数
	
	long	m_lSendCounter;
	long	m_lRecvCounter;


	//上次总数据长度
	long	m_lLastMsgTotalSize;
	//上次消息的长度
	long	m_lLastMsgLen;
	//上次消息的类型
	long	m_lLastMsgType;
	//上次消息数据块的处理前个数
	long	m_lLastMsgPreDBNum;
	//上次消息数据块的处理后个数
	long	m_lLastMsgPosDBNum;
	//上次消息操作时最后的长度
	long	m_lLastMsgRemainSize;
	//上次消息移动内存的位置
	long	m_lLastMsgMemMoveDBPos;
	//上次移动的最大
	long	m_lLastMsgMemMoveDBSize;

public:
	//增加接受数据大小
	long AddRecvSize(long lSize);
	//添加发送数据大小
	long AddSendSize(long lSize);
	//得到当前每秒接受大小
	long GetCurRecvSizePerSecond();
	long GetMaxRecvSizePerSecond(){return m_lMaxRcvSizePerSecond;}
	//得到当前每秒发送大小
	long GetCurSendSizePerSecond();

	long GetPendingWrBufNum()			{return m_lPendingWrBufNum;}
	void IncPendingWrBufNum(long lNum)	{m_lPendingWrBufNum += lNum;}
	void DecPendingWrBufNum(long lNum)	{m_lPendingWrBufNum -= lNum;}

	//给读取序列号加1
	void IncrReadSequenceNumber()	{m_ReadSequenceNumber = (m_ReadSequenceNumber+1)%MAXIMUMSEQUENSENUMBER;}
	DWORD GetReadSequenceNumber()	{return m_ReadSequenceNumber;}
	void IncrCurReadSequenceNumber(){m_CurrentReadSequenceNumber = (m_CurrentReadSequenceNumber+1)%MAXIMUMSEQUENSENUMBER;}
	CDataBlock* GetNextReadDB();
};

#endif
