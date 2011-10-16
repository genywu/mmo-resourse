#pragma once

#include "..\basemessage.h"
#include "../../public\MsgType.h"
#include "..\..\public\ManageToolMsgType.h"
#include <hash_map>
using namespace stdext;

class CMySocket;
class CServer;
class CMapFile;

class CMessage :
	public CBaseMessage
{
public:
	CMessage();
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CServer	*m_pClientServer;
	static long	m_lLogingSocket;		//连接loginserver的客户端
	static long	m_lCostSocket;			//连接costserver的客户端
	//[Amend AHC 070719]
	static long	m_lDBSocket;		//连接DBserver的客户端
	static long m_lLogSocket;		//连接日志服务器的客户端
public:
	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	//释放资源
	static bool Release();

	static void SetServer(CServer *pServer)	{m_pClientServer=pServer;}
	static void SetLogingSocket(long lSocket){m_lLogingSocket=lSocket;}
	static void SetCostSocket(long lSocket){m_lCostSocket=lSocket;}
	static void SetDBSocket(long lSocket){m_lDBSocket=lSocket;}
	static void SetLogSocket(long lSocket){m_lLogSocket=lSocket;}

	static CBaseMessage* NewMessage();
////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////
	// 服务器
protected:
	long	m_lSocketID;			//发送消息过来的SocketID;
	DWORD	m_dwIP;					//发送该消息的IP

public:
	void SetSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetSocketID()					{return m_lSocketID;}

	void SetIP(DWORD dwIP)				{m_dwIP = dwIP;	}
	DWORD GetIP()						{return m_dwIP;	}

	void Reset(void);        //重设消息相关变量

	long GetGSID();
	long GetDBSID();
	long GetLOGSID();
	long GetLSID();

	long SendToSocket(long lSocketID,bool bClone=true);	//发送到GS,DBS的Socket
	long SendGSID(long lGSID,bool bClone=true);		//发送到GS,DBS的一个映射ID


	long SendAll();						// 发送给所有客户端
	long DefaultRun();
	virtual long Run();					// 服务器端消息处理


	long Send(bool bFront=false);		// 发送消息
	void SendToCost(bool bFront=false);
	long SendToSM();
	//同步发送到SM,可能丢失,不安全的
	long SyncSendToSM();
//////////////////////////////////////////////////////////////////////////
//	时间戳
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD	m_dwRecvTime;			// 收到的时间

public:
	DWORD	GetRecvTime()			{return m_dwRecvTime;}
	void	SetRecvTime(DWORD dw)	{m_dwRecvTime=dw;}

private:
	//dbserver连接套结字和dbserverID关联
	typedef stdext::hash_map<long,long>	HMapLongLong;
	typedef stdext::hash_map<long,long>::iterator itLL;

	static HMapLongLong	m_SocketGSID;
	static HMapLongLong	m_GSIDSocket;

public:
	//映射WorldID和SocketID
	static void MapGSIDSocket(long lGSID,long lSocketID);
	static long GetSocketByGSID(long lGSID);
	static long GetGSIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapGSIDSocket(long lSocketID);

// Fox@20080922----------------------------------------------
// 网关连接
	static void SetGateSocketID(int nSocket){ m_nGateSocket = nSocket; }
	static int GetGateSocketID(void) { return m_nGateSocket; }

private:
	static int				m_nGateSocket;		// 连接GateServer的SocketID

//用内存影射文件的进程通信机制,实现写文件的异步
//Author:wangqiao
public:
	//打开文件
	static void AsyOpenFile(const char* pszFileName,bool bSync = true);
	//关闭文件
	static void AsyCloseFile(const char* pszFileName,bool bSync = true);
	//清空文件内容
	static void AsyClearFile(const char* pszFileName,bool bSync = true);
	//异步写文件的接口
	static void AsyWriteFile(const char* pszFileName,const char* pszContent,bool bSync = true);
private:
	//! 发送到SM的消息顺序
	static long c_SmInfoOrder;
};

