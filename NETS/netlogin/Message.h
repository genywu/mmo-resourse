#pragma once
#include "..\basemessage.h"
#include "..\public\MsgType.h"
#include "..\..\public\ManageToolMsgType.h"
#include <hash_map>

using namespace stdext;

class CServer;
class CRegion;
class CPlayer;
class CMapFile;


class CMessage :
	public CBaseMessage
{
public:
	CMessage(long lType);
	CMessage(void);
	virtual ~CMessage(void);
private:
	static CServer	*m_pClientServer;
	static CServer *m_pWorldServer;
	static long			m_lASSocket;		//连接AccountServer的客户端
public:

	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static void SetClientServer(CServer *pClientServer);
	static void SetWorldServer(CServer *pWorldServer);
	static void SetASSocket(long lSocket){m_lASSocket=lSocket;}
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

	const char* GetStringIP()
	{
		static char pszIP[256]="";
		_snprintf(pszIP,256, "%d.%d.%d.%d", LOBYTE(LOWORD(m_dwIP)), HIBYTE(LOWORD(m_dwIP)), LOBYTE(HIWORD(m_dwIP)), HIBYTE(HIWORD(m_dwIP)));
		return pszIP;
	}

	const char *GetCdkey();
	long GetWorldID();

	void Reset(void);        //重设消息相关变量

	long SendToClient(long lSocketID);		//发送一个消息给客户端套接字
	long SendToWorld(long lSocketID);		//发送一个消息给WorldServer套接字

	long SendAllClient();					// 发送给所有客户端
	long SendAllWorld();					// 发送给所有WorldServer

	long SendToWorldNumID(long lWorldID);
	long SendToClient(const char *szCdkey);
	
	long SendToSM();
	//! [作为AS客户端] 发送消息
	BOOL SendToAS(VOID);
	
	long DefaultRun();   //缺省的消息解析函数
	long Run();			// 服务器端消息处理

private:
	//客户端套结字和CDkey关联
	typedef stdext::hash_map<long,string>	HMapLongString;
	typedef stdext::hash_map<long,string>::iterator itLS;

	typedef stdext::hash_map<string,long>	HMapStringLong;
	typedef stdext::hash_map<string,long>::iterator itSL;

	
	static HMapLongString	m_SocketCDkey;
	static HMapStringLong	m_CDkeySocket;
	//WorldServer连接套结字和WorldServerID关联

	typedef stdext::hash_map<long,long>	HMapLongLong;
	typedef stdext::hash_map<long,long>::iterator itLL;

	static HMapLongLong	m_SocketWorldID;
	static HMapLongLong	m_WorldIDSocket;

public:
	//映射CDkey和SocketID
	static void MapCDkeySocket(const char* szCdkey,long lSocketID);
	static long GetSocketByCdkey(const char* szCdkey);
	static const char* GetCDkeyBySocket(long lSocketID);
	static void UnCDkeyBySocket(const char* szCdkey);
	//映射WorldID和SocketID
	static void MapWorldIDSocket(long lWorldID,long lSocketID);
	static long GetSocketByWorldID(long lWorldID);
	static long GetWorldIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapCDkeySocket(long lSocketID);
	static void UnMapWorldIDSocket(long lSocketID);


	//用内存影射文件的进程通信机制,实现写文件的异步
	//Author:wangqiao
public:
	//打开文件
	static void AsyOpenFile(const char* pszFileName);
	//关闭文件
	static void AsyCloseFile(const char* pszFileName);
	//清空文件内容
	static void AsyClearFile(const char* pszFileName);
	//异步写文件的接口
	static void AsyWriteFile(const char* pszFileName,const char* pszContent);
private:
	//! 发送到SM的消息顺序
	static long c_SmInfoOrder;
};
