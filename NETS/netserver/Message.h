#pragma once
#include "..\basemessage.h"
#include "..\..\public\MsgType.h"
#include "..\..\public\ManageToolMsgType.h" 
#include "..\..\public\GUID.h"
#include "..\..\server\gameserver\gameserver\game.h"
#include <hash_map>
using namespace stdext;

class CMySocket;
class CServerRegion;
class CArea;
class CShape;
class CPlayer;
class CServer;
class CMapFile;

class CMessage :
	public CBaseMessage
{
public:
	CMessage();
	CMessage(long lType);
	CMessage(long lType,const CGUID& guDiscardFlag);
	virtual ~CMessage(void);
private:
	static CServer	*m_pServer;
	static long	m_lWSSocket;		//连接worldserver的客户端
	static long m_lBSSocket;		//连接BILLINGSERVER的客户端
	//[Amend AHC 070719]
	static long	m_lDBSSocket;		//连接DBserver的客户端
	static long m_lLogSocket;		//连接日志服务器的客户端
	static long m_lBounsSocket;		//连接BounsServer的客户端

public:
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static bool Release();

	static void SetServer(CServer *pServer)	{m_pServer=pServer;}
	static void SetWSSocket(long lSocket)	{m_lWSSocket=lSocket;}
	static long GetWSSocket(void)			{return m_lWSSocket;}
	static void SetDBSSocket(long lSocket)	{m_lDBSSocket=lSocket;}
	static void SetLogSocket(long lSocket)	{m_lLogSocket=lSocket;}
	static void SetBSSocket(long lSocket)	{m_lBSSocket=lSocket;}
	static void SetBounsSocket(long lSocket){ m_lBounsSocket = lSocket; }


	static CBaseMessage* NewMessage();
////////////////////////////////////////////////////////////////////////
//	Player & Region
////////////////////////////////////////////////////////////////////////
private:
	CServerRegion* m_pRegion;
	CPlayer* m_pPlayer;

public:
	CServerRegion* GetRegion()		{return m_pRegion;}
	CPlayer* GetPlayer()			{return m_pPlayer;}
	void SetPlayer(CPlayer* p)		{m_pPlayer=p;}

	void Reset(void);        //重设消息相关变量

////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

// 服务器
protected:
	long	m_lSocketID;			//发送消息过来的SocketID;
	DWORD	m_dwIP;					//发送该消息的IP
	CGUID	m_guDiscardFlag;		//消息的丢弃标示

public:
	void SetSocketID(long lSocketID)	{ m_lSocketID=lSocketID;}
	long GetSocketID()					{return m_lSocketID;}

	void SetIP(DWORD dwIP)				{m_dwIP = dwIP;	}
	DWORD GetIP()						{return m_dwIP;	}

	void SetDiscardFlag(const CGUID& guid)	{m_guDiscardFlag=guid;}
	const CGUID& GetDiscardFlag()		{return m_guDiscardFlag;}
	//判断两个消息的丢弃标记是否一样
	virtual bool IsDiscardFlagEqual(CBaseMessage* pMsg);
	//分配一个消息
	static CMessage* AllocMessage();

	const CGUID& GetPlayerID();

	long SendToSocket(long lSocketID,bool bClone=true);			//发送一个消息给套接字
	long SendToPlayer(const CGUID& PlayerID,bool bClone=true);	// 发送一个消息给万家
	long SendToBS();
	long SendToRegion(CServerRegion* =NULL, CPlayer* =NULL);	// 发送给一个场景的所有玩家
	long SendToArea(CArea* =NULL, CPlayer* =NULL);				// 发送给一个AREA的所有玩家
	long SendToAround(CShape* pMain,CPlayer* pExceptionPlayer=NULL,
		bool bExceptTranCongSocket=false);	// [服务器] 发送给一个玩家周围的玩家(pExceptionPlayer=例外)
	long SendToAround(CServerRegion* pRegion,int x, int y, CShape* pMain=NULL, 
		CPlayer* pExceptionPlayer=NULL,bool bExceptTranCongSocket=false);	// [服务器] 发送给一个点周围的玩家(pExceptionPlayer=例外)
	//发消息给指定数量的玩家
	long SendAroundLimitPlayer(CMoveShape* pMoveShape,const CGUID& guidExeptID,bool bExceptTranCongSocket=false);
	//给指定距离里的玩家发送消息
	long SendToNearestPlayer(CShape* pMain,bool bExceptTranCongSocket=false,CPlayer* pExceptionPlayer=NULL);	
	//发送消息给指定玩家
	long SendToPlayers(hash_set<CGUID,hash_guid_compare>& hsPlayers,bool bExceptTranCongSocket=false);

	//从列表中排除网络阻塞的套结字
	void RemoveTransCongSockets(list<long>&	SocketList);
	long SendToAroundTeamate(CShape* pMain,CShape* pDest);					//发给周围队友
	long SendAll();						// 发送给所有客户端
	long Send(bool bFront=false);		// 发送消息
	long SendToBounsServer();			// 发送消息给BounsServer
	long SendToSM();
	//同步发送到SM,可能丢失,不安全的
	long SyncSendToSM();
	long DefaultRun();
	virtual long Run();					// 服务器端消息处理
private:
	//WorldServer连接套结字和WorldServerID关联
	typedef stdext::hash_map<long,CGUID>				HMapLongGuid;
	typedef stdext::hash_map<CGUID,long, hash_guid_compare>				HMapGuidLong;
	typedef stdext::hash_map<long,CGUID>::iterator		iteLG;
	typedef stdext::hash_map<CGUID,long, hash_guid_compare>::iterator		iteGL;

	static HMapLongGuid	m_SocketPlayerID;
	static HMapGuidLong	m_PlayerIDSocket;

	typedef set<long>	Sockets;
	typedef set<long>::iterator itTCSocket;
	//网络传输拥挤的sockets集合
	static Sockets	m_TransCongSokcets;

public:
	//映射WorldID和SocketID
	static void MapPlayerIDSocket(const CGUID& PlayerGuid,long lSocketID);
	static long GetSocketByPlayerID(const CGUID& PlayerGuid);
	static const CGUID& GetPlayerIDBySocket(long lSocketID);

	static void UnMapSocket(long lSocketID,long lNetFlag);
	static void UnMapPlayerIDSocket(long lSocketID);


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