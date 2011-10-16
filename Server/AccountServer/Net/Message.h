#pragma once

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
	static CServer	*m_pClientServer;

public:

	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static void SetClientServer(CServer *pClientServer);

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
public:
	void SetSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetSocketID()					{return m_lSocketID;}

	void SetIP(DWORD dwIP)				{m_dwIP = dwIP;	}
	DWORD GetIP()						{return m_dwIP;	}

	long GetWorldID();

	void Reset(void);        //重设消息相关变量

	long SendToClient(long lSocketID);		//发送一个消息给客户端套接字

	long SendAllClient();					// 发送给所有客户端
	
	long DefaultRun();   //缺省的消息解析函数
	long Run();			// 服务器端消息处理

private:
	//客户端连接套结字和WorldServerID关联

	typedef stdext::hash_map<long,long>	HMapLongLong;
	typedef stdext::hash_map<long,long>::iterator itLL;

	static HMapLongLong	m_SocketWorldID;
	static HMapLongLong	m_WorldIDSocket;

public:
	//映射WorldID和SocketID
	static void MapWorldIDSocket(long lWorldID,long lSocketID);
	static long GetSocketByWorldID(long lWorldID);
	static long GetWorldIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapWorldIDSocket(long lSocketID);
};
