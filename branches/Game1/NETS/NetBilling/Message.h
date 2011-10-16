#pragma once
#include "..\basemessage.h"
#include "..\..\public\MsgType.h"
#include <hash_map>

using namespace stdext;

class CServer;
class CRegion;
class CPlayer;


class CMessage :
	public CBaseMessage
{
public:
	CMessage(long lType);
	CMessage(void);
	virtual ~CMessage(void);
private:
	
	static CServer *m_pGSServer;
public:

	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	
	static void SetGSServer(CServer *pGSServer);
	//释放资源
	static bool Release();
	static CBaseMessage* NewMessage();

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

	// 服务器
private:
	long	m_lSocketID;			//发送消息过来的SocketID;
	DWORD	m_dwIP;

	static long m_lLogSocket;		//连接日志服务器的客户端
public:
	void SetSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetSocketID()					{return m_lSocketID;}

	void SetIP(DWORD dwIP)				{m_dwIP = dwIP;	}
	DWORD GetIP()						{return m_dwIP;	}

	
	long GetWorldID();

	void Reset(void);        //重设消息相关变量

	
	long SendToGS(long lSocketID);		//发送一个消息给WorldServer套接字

	
	long SendAllGS();					// 发送给所有WorldServer

	long SendToWorldNumID(long lWorldID);	

	long SendToSocket(long lSocketID);	
	
	long DefaultRun();   //缺省的消息解析函数
	long Run();			// 服务器端消息处理

	static void SetLogSocket(long lSocket){m_lLogSocket=lSocket;}

private:	
	//GameServer连接套结字和WorldServerID关联

	typedef stdext::hash_map<long,long>	HMapLongLong;
	typedef stdext::hash_map<long,long>::iterator itLL;

	static HMapLongLong	m_SocketGSID;
	static HMapLongLong	m_GSIDSocket;

public:	
	//映射GameServer ID和SocketID
	static void MapGSIDSocket(long lGSID,long lSocketID);
	static long GetSocketByGSID(long lGSID);
	static long GetGSIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);	
	static void UnMapGSIDSocket(long lSocketID);
};
