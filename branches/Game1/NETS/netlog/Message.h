#pragma once
#include "..\basemessage.h"
#include "..\..\public\MsgType.h"
#include <hash_map>
using namespace stdext;

class CMySocket;
class CServer;


class CMessage :
	public CBaseMessage
{
public:
	CMessage();
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CServer	*m_pClientServer;
	static long	m_lCostSocket;			//连接costserver的客户端
public:
	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	//释放资源
	static bool Release();

	static void SetServer(CServer *pServer)	{m_pClientServer=pServer;}
	static void SetLogingSocket(long lSocket){}
	static void SetCostSocket(long lSocket){m_lCostSocket=lSocket;}

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

	long SendToSocket(long lSocketID);	//发送到Gameserver的Socket
	long SendGSID(long lGSID);		//发送到Gameserver的一个映射ID

	long SendAll();						// 发送给所有客户端
	long DefaultRun();
	virtual long Run();					// 服务器端消息处理

	void SendToSM();
//////////////////////////////////////////////////////////////////////////
//	时间戳
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD	m_dwRecvTime;			// 收到的时间

public:
	DWORD	GetRecvTime()			{return m_dwRecvTime;}
	void	SetRecvTime(DWORD dw)	{m_dwRecvTime=dw;}

private:
	//logServer连接套结字和logServerID关联
	typedef stdext::hash_map<long,long>	HMapLongLong;
	typedef stdext::hash_map<long,long>::iterator itLL;

	static HMapLongLong	m_SocketGSID;
	static HMapLongLong	m_GSIDSocket;

public:
	//映射logID和SocketID
	static void MapGSIDSocket(long lGSID,long lSocketID);
	static long GetSocketByGSID(long lGSID);
	static long GetGSIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapGSIDSocket(long lSocketID);
};

