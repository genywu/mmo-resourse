/**
* @file    Message.h
* @author  Fox (yulefox@gmail.com)
* @date    Mar.04, 2010
*/

#pragma once

#include "NetHandler.h"

class CMapFile;
class CMySocket;
class CServer;

/**
* @class   CMessage
* @author  Fox (yulefox@gmail.com)
* @date    Mar.04, 2010
* @brief   BaseMessage子类, 用于WorldServer消息处理
*/

class CMessage : public CBaseMessage
{
public:
	CMessage();
	CMessage(uint type);
	virtual ~CMessage();

public:
	/// 初始化
	static bool Initial(CDataBlockAllocator* allocator, uint maxFreeMsgNum);

	/// 释放资源
	static bool Release();
	static CBaseMessage* NewMessage();
    static void SetServer(CServer* server) { s_server = server; }
    /// 获取连接的 GS 数量
    static long GetGSNum() { return (long)s_GSIDs.size(); }

    /// GS 是否已连接
    /// @param id 0, 返回是否有GS连接; -1, 返回是否所有GS均已连接;
    /// other, 返回对应GS是否已连接. 
    static bool IsGSReady(uint id);

	virtual void SetSocketID(long socket) { m_socket = socket; }
	virtual long GetSocketID() { return m_socket; }
	virtual void SetIP(ulong IP) { m_IP = IP; }
	virtual ulong GetIP() { return m_IP;	}

    /// 服务器端消息处理
	virtual long DefaultRun();
	virtual long Run();

    /// 重设消息相关变量
	virtual void Reset();

    /// 发送给所有客户端
	void SendToAllGS();

    /// 发送消息到LoginServer
    /// @param flag 服务器标识
    /// @param prior 是否优先发送
	void SendToServer(NetFlag flag, bool prior = false);

	uint GetGSID();

    /// 发送到GS的一个映射ID
	void SendToGS(uint id, bool clone = true);

    /// 发送到GS, LS, DBS的Socket
	void SendToSocket(int socket, bool clone = true);

	void SendToSM();
	uint GetRecvTime() { return m_recvTime; }
	void SetRecvTime(uint time)	{m_recvTime = time; }

public:
	/// 映射GSID和SocketID
	static void MapGSIDSocket(uint id, int socket);
	static int GetSocketByGSID(uint id);
	static uint GetGSIDBySocket(int socket);
	static void UnMapSocket(int socket, NetFlag flag);
	static void UnMapGSIDSocket(int socket);

    /// 用内存影射文件的进程通信机制,实现写文件的异步
    /// Author:wangqiao
	/// 打开文件
	static void AsyOpenFile(const char* file);

	/// 关闭文件
	static void AsyCloseFile(const char* file);

	/// 清空文件内容
	static void AsyClearFile(const char* file);

	/// 异步写文件的接口
	static void AsyWriteFile(const char* file, const char* content);

private:

	int GetServerSocket(NetFlag flag);

	/// dbserver连接套结字和dbserverID关联
	typedef stdext::hash_map<int, uint> Clients;
	typedef Clients::iterator ClientItr;

	static Clients s_GSs;
	static Clients s_GSIDs;

protected:
    /// 发送消息过来的SocketID
	int m_socket;

    /// 发送该消息的IP
	uint m_IP;

    /// 收到的时间
	uint m_recvTime;

    /// 网络模块服务器实例
    static CServer* s_server;
};
