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
	static CServer	*m_pSUCServer;
	static CServer	*m_pSMServer;
	static long	m_lLogingSocket;		//连接loginserver的客户端
	static long	m_lCostSocket;			//连接costserver的客户端
public:
	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	//释放资源
	static bool Release();

	static void SetSUCServer		(CServer *pServer)	{m_pSUCServer=pServer;}
	static void SetSMServer			(CServer *pServer)	{m_pSMServer=pServer;}
	static void SetLogingSocket		(long lSocket)		{m_lLogingSocket=lSocket;}
	static void SetCostSocket		(long lSocket)		{m_lCostSocket=lSocket;}

	static CBaseMessage* NewMessage();

	static void DisSucConn(long lSocket);
	static void DisSmConn(long lSocket);
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

	long SendToSuc(long lSocketID);
	long SendToSm(long lSocketID);

	long DefaultRun();
	virtual long Run();					// 服务器端消息处理


//////////////////////////////////////////////////////////////////////////
//	时间戳
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD	m_dwRecvTime;			// 收到的时间

public:
	DWORD	GetRecvTime()			{return m_dwRecvTime;}
	void	SetRecvTime(DWORD dw)	{m_dwRecvTime=dw;}

private:
	static map<DWORD, LONG>		c_mapSMID2Socket;
	static map<LONG, DWORD>		c_mapSMSocket2ID;

	static map<string, LONG>	c_mapCdkey2Socket;
	static map<LONG, string>	c_mapSocket2Cdkey;

	//!							是否允许SCC连接
	static BOOL					c_bAllowSUCConn;

public:
	static void		OnSUCAccept(long lSocketID);
	static void		OnSUCClose(long lSocketID);
	static void		OnSMAccept(long lSocketID, long lIP);
	static void		OnSMClose(long lSocketID);

	static void		AllowSUC(BOOL bIsAllow){c_bAllowSUCConn = bIsAllow;};

	static LONG		GetSMSocket(DWORD dwSMID);
	static DWORD	GetSMID(long lSMSocket);
};

