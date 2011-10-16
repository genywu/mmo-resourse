#pragma once

#include "..\basemessage.h"
#include "..\..\public\MsgType.h"
#include "..\..\public\ManageToolMsgType.h"
#include <hash_map>
using namespace stdext;

class CServer;
class CMySocket;
class CMapFile;

class CMessage : public CBaseMessage
{
public:
	CMessage(void);
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CServer *m_pClientServer;
	static long m_lServerSocket;
	static long m_lLogSocket;

public:
	// 初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator, long nMaxFreeMsgNum);
	// 释放资源
	static bool Release();

	static void SetServer(CServer* pServer)		{ m_pClientServer = pServer; }
	static void SetServerSocket(long lSocket)	{ m_lServerSocket = lSocket; }
	static void SetLogSocket(long lSocket)		{ m_lLogSocket = lSocket; }

	static CBaseMessage* NewMessage();

protected:
	long  m_lSocketID;			// 发送消息过来的SocketID
	DWORD m_dwIP;				// 发送该消息的IP

public:
	void SetSocketID(long lSocketID)	{ m_lSocketID = lSocketID; }
	long GetSocketID()					{ return m_lSocketID; }

	void SetIP(DWORD dwIP)			{ m_dwIP = dwIP; }
	DWORD GetIP()					{ return m_dwIP; }

	void Reset();			// 重设消息相关变量

	long GetDSID();
	long GetLSID();
	long GetWSID();
	long GetGSID();

	long SendToSocket(long lSocketID, bool bClone=true);
	
	long SendAll();
	long DefaultRun();
	virtual long Run();

	long Send(bool bFront=false);
	long SendToSM();

//////////////////////////////////////////////////////////////////////////
//	时间戳
//////////////////////////////////////////////////////////////////////////
protected:
	DWORD	m_dwRecvTime;			// 收到的时间

public:
	DWORD GetRecvTime()					{ return m_dwRecvTime; }
	void SetRecvTime(DWORD dwRecvTime)	{ m_dwRecvTime = dwRecvTime; }

private:
	typedef stdext::hash_map<long, long> HMapLongLong;
	typedef stdext::hash_map<long, long>::iterator itLL;

	static HMapLongLong m_SocketClientID;
	static HMapLongLong m_ClientIDSocket;

public:
	static void MapGSIDSocket(long lGSID, long lSocketID);
	static long GetSocketByGSID(long lGSID);
	static long GetGSIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID, long lNetFlag);
	static void UnMapGSIDSocket(long lSocketID);

//用内存影射文件的进程通信机制,实现写文件的异步
public:
	//打开文件
	static void AsyOpenFile(const char* pszFileName);
	//关闭文件
	static void AsyCloseFile(const char* pszFileName);
	//清空文件内容
	static void AsyClearFile(const char* pszFileName);
	//异步写文件的接口
	static void AsyWriteFile(const char* pszFileName,const char* pszContent);

};
