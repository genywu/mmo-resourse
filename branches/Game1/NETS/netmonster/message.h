#pragma once

#include "..\basemessage.h"
#include "../../public\MsgType.h"
#include "../../public/guid.h"

class CMySocket;
class CServer;
class CPlayer;

class CMessage :
	public CBaseMessage
{
public:
	CMessage();
	CMessage( long lType, const CGUID &DiscardID )
	{
		assert( 0 && "Not implement!" );
	}
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CServer	*m_pClientServer;
public:
	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	//释放资源
	static bool Release();

	static void SetServer(CServer *pServer)	{m_pClientServer=pServer;}

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

	long SendToSocket(long lSocketID);
	long SendGSID(long lGSID);		

	///////////////////////////////////////////////////////////////////////
	/// Only for compatible....
	long SendToAround( void *p ) {assert( 0 ); return 1l; }
	long SendToAround( ... ) {assert( 0 ); return 1l; }
	long SendToPlayer( const CGUID &guid ) { assert( 0 );return 1l; }
	long SendToRegion( ... ) { assert( 0 ); return 1l ; }
	long SendToArea( ... ) { assert( 0 ); return 1; }
	CGUID GetPlayerID() { assert( 0 ); return CGUID(); }
	void SendAll() { assert( 0 ); }
	CPlayer *GetPlayer() { assert( 0 ); return 0; }
	///////////////////////////////////////////////////////////////////////

	long DefaultRun();
	virtual long Run();					// 服务器端消息处理


	/// 发生消息给WS
	long Send();		// 发送消息

//////////////////////////////////////////////////////////////////////////
//	时间戳
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD	m_dwRecvTime;			// 收到的时间

public:
	DWORD	GetRecvTime()			{return m_dwRecvTime;}
	void	SetRecvTime(DWORD dw)	{m_dwRecvTime=dw;}

};

