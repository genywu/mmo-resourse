//=============================================================================
/**
 *  file: Message.h
 *
 *  Brief:消息类，包装了构建消息、添加数据 、发送消息的功能
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================
#pragma once
#include "..\App\ClientRegion.h"

//

class CMySocket;
class CServer;
class CPlayer;

class CMessage :
	public CBaseMessage
{
public:
	CMessage(void);
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CServer	*m_pServer;
	static long	m_lWSSocket;		//连接worldserver的客户端
	//[Amend AHC 070719]
	static long	m_lDBSSocket;		//连接DBserver的客户端
	static long m_lLogSocket;		//连接日志服务器的客户端

	//分配一个消息
	static CMessage* AllocMessage();

	//MappingFile 管理器
	static CMemServer *m_pMemServerManager;
public:
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static bool Release();

	static void SetServer(CServer *pServer)	{m_pServer=pServer;}
	static void SetWSSocket(long lSocket){m_lWSSocket=lSocket;}
	static void SetDBSSocket(long lSocket){m_lDBSSocket=lSocket;}
	static void SetLogSocket(long lSocket){m_lLogSocket=lSocket;}
	static void SetMemServerManager(CMemServer *pManager){ m_pMemServerManager = pManager; }
	static CBaseMessage* NewMessage();
	////////////////////////////////////////////////////////////////////////
//	Player & Region
////////////////////////////////////////////////////////////////////////
private:
	//CClientRegion* m_pRegion;
	//CPlayer* m_pPlayer;

public:
	//CClientRegion* GetRegion()		{return m_pRegion;}
	//CPlayer* GetPlayer()			{return m_pPlayer;}
	//void SetPlayer(CPlayer* p)		{m_pPlayer=p;}

	void Reset(void);        //重设消息相关变量

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

// 服务器
private:
	long	m_lSocketID;			//发送消息过来的SocketID;
	DWORD	m_dwIP;					//发送该消息的IP
	CGUID	m_guDiscardFlag;		//消息的丢弃标示

	
public:
	void SetSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetSocketID()					{return m_lSocketID;}

	void SetIP(DWORD dwIP)				{m_dwIP = dwIP;	}
	DWORD GetIP()						{return m_dwIP;	}

	const CGUID& GetPlayerID();


//	void Reset(void);

	long SendBySocket(long lSocketID,bool bClone=true);
//	long SendByPlayer(long lPlayerID);
	long SendByPlayer(const CGUID& lPlayerID);
	//bool SendToClient(unsigned uID);			//on 线程ID
	long DefaultRun();
	long Run();					// 服务器端消息处理
private:
	//WorldServer连接套结字和WorldServerID关联
	typedef stdext::hash_map<long,CGUID>	HMapLongGUID;
	//typedef stdext::hash_map<long,long>::iterator itLL;
	typedef stdext::hash_map<long,CGUID>::iterator itLG;

	typedef stdext::hash_map<CGUID,long,hash_guid_compare>	HMapGUIDLong;
	typedef stdext::hash_map<CGUID,long,hash_guid_compare>::iterator itGL;

	static HMapLongGUID	m_SocketPlayerID;
	static HMapGUIDLong	m_PlayerIDSocket;

public:
	//映射WorldID和SocketID
	//static void MapPlayerIDSocket(long lPlayerID,long lSocketID);
	//static long GetSocketByPlayerID(long lPlayerID);
	static void MapPlayerIDSocket(const CGUID& lPlayerID,long lSocketID);
	static long GetSocketByPlayerID(const CGUID& lPlayerID);
	static const CGUID& GetPlayerIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapPlayerIDSocket(long lSocketID);
};
