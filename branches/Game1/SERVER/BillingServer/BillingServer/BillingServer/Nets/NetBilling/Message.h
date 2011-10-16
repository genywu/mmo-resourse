#pragma once
#include "..\basemessage.h"

class CMySocket;
class CRegion;
class CPlayer;


class CMessage :
	public CBaseMessage
{
public:
	CMessage(long lType);
	virtual ~CMessage(void);
public:

	CRegion* m_pRegion;
	CPlayer* m_pPlayer;
	CRegion* GetRegion()	{ return m_pRegion; }
	CPlayer* GetPlayer()	{ return m_pPlayer; }

	static CMessage* CreateMessage(void* pRLEBuf, unsigned long lRLESize);	// 收到Socket接受到的包后将包组装成消息
	static CMessage* CreateMessageWithoutRLE(void* pBuf, unsigned long nSize);	// 收到Socket接受到的包后将包组装成消息

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

	// 服务器
protected:
	long	m_lMapID;				// 发送消息的PlayerID
	long	m_lSocketID;			// 发送消息过来的SocketID;
	DWORD	m_dwIP;					// 发送该消息的IP
	string  m_strCdkey;				// 发送消息过来的Cdkey
public:
	void SetClientSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetClientSocketID()				{return m_lSocketID;}

	void SetIP(DWORD dwIP)					{m_dwIP = dwIP;	}
	DWORD GetIP()							{return m_dwIP;	}

	void SetMapID(long lPlayerID)		{m_lMapID =lPlayerID;}
	long GetMapID()						{return m_lMapID;}

	void SetCdkey(const char* strCdkey)		{m_strCdkey = strCdkey;}
	const char* GetCdkey()					{return m_strCdkey.c_str();}

	long SendToGS(long lSocketID);					// 发送一个消息给客户端套接字
	long SendToGS(const char* strCdkey);			// 发送消息给Cdkey

	long SendToWorldNumID(long lNumID);				//发送一个消息给WorldID

	long SendToAllGS();								// 发送给所有客户端
	
	virtual long Run();								// 服务器端消息处理
};
