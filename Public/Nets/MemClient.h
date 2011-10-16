// Fox@20090119----------------------------------------------
// File:    MemClient.h
// Brief:   CMemClient作为client对内存映射文件进行操作.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Jan.19, 2009
// Fox@20090119----------------------------------------------

#pragma once

#include "MsgQueue.h"
#include "MapFile.h"

class CBaseMessage;
class CDataBlockAllocator;

class CMemClient
{
public:
	CMemClient(void);
	~CMemClient(void);

	bool Initial(const char* szMapObjName,
		CDataBlockAllocator* pDBAllocator);
	bool Release(void);
	CMsgQueue* GetRecvMsgs(void) { return &m_RecvMsgs; }
	bool Send(CBaseMessage* pMsg,
		eFileMapServerType eType = FMT_Server);
	//同步发送消息，(当消息队列满时,有可能丢弃该消息)
	bool SyncSend(CBaseMessage* pMsg,
		eFileMapServerType eType = FMT_Server);
	void AsyOpenFile(const char* pszFileName);		// 打开文件
	void AsyCloseFile(const char* pszFileName);		// 关闭文件
	void AsyClearFile(const char* pszFileName);		// 清空文件内容
	void AsyWriteFile(const char* pszFileName,
		const char* pszContent);					// 异步写文件的接口
	friend unsigned __stdcall ProcFunc(void* pArguments);
	friend unsigned __stdcall SendMsgFunc(void* pArguments);

private:
	bool Recv(void);
	void Send(void);

	HANDLE			m_hProcThread;
	HANDLE			m_hSendMsgThread;
	CMapFile*		m_pMapFile;
	CMsgQueue		m_RecvMsgs;
};