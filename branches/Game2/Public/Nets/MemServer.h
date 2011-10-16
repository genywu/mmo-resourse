// Fox@20090119----------------------------------------------
// File:    MemServer.h
// Brief:   CMemServer作为server监视client(s)内存映射文件.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Jan.19, 2009
// Fox@20090119----------------------------------------------

#pragma once

#include "MsgQueue.h"
#include "MapFile.h"

class CBaseMessage;
class CDataBlockAllocator;

// 映射文件配置
struct tagConf
{
	tagConf(void)
		: dwID(0)
		, hProcThread(NULL)
		, pMapFile(NULL)
	{
		memset(szMapObj, 0, 64);
		memset(szMapFile, 0, 64);
		memset(szOutputDir, 0, 64);
		lTestID = -1;
	}

	const tagConf& operator=(const tagConf& rConf)
	{
		memcpy(this, &rConf, sizeof(rConf));
		return *this;
	}

	DWORD		dwID;
	HANDLE		hProcThread;
	HANDLE		hSendMsgThread;
	CMapFile*	pMapFile;
	char		szMapObj[64];
	char		szMapFile[64];
	char		szOutputDir[64];

	long		lTestID;
};

typedef std::map<DWORD, tagConf> MF_CONFS;
typedef MF_CONFS::iterator ITR_MF_CONF;

class CMemServer
{
public:
	CMemServer(void);
	~CMemServer(void);

	unsigned AddServer(DWORD dwID,
		const char* szMapObj,
		const char* szMapFile,
		const char* szOutputDir);					// 添加SERVER, 返回线程ID.
	bool Initial(CDataBlockAllocator* pDBAllocator);
	bool Release(void);
	CMsgQueue* GetRecvMsgs(void) { return &m_RecvMsgs; }
	bool Send(DWORD dwID,
		CBaseMessage* pMsg,
		eFileMapServerType eType = FMT_Client);
	friend unsigned __stdcall ProcFuncS(void* pArguments);
	friend unsigned __stdcall SendMsgFuncS(void* pArguments);

	void Reset(DWORD dwID);
	void ResetAll(void);

private:
	bool Run(DWORD dwID);
	void Send(DWORD dwID);									// 根据线程ID发送消息
	// 文件操作函数
	tagFileInfo0x* GetFile(DWORD dwID,
		const char* pszFileName);							// 根据文件名得到相应文件句柄
	tagFileInfo0x* AddFile(DWORD dwID,
		const char* pszFileName);							// 添加一个文件
	void OpenFile(DWORD dwID, const char* pszFileName);		// 打开文件
	void CloseFile(DWORD dwID, const char* pszFileName);	// 关闭文件
	void ClearFile(DWORD dwID, const char* pszFileName);	// 清空文件
	void WriteFile(DWORD dwID, const char* pszFileName,
		const char* pszContent);							// 写文件
	CMapFile* GetMapFile(DWORD dwID);						// 根据线程ID获取MapFile对象

private:
	MF_CONFS		m_MapFiles;
	CMsgQueue		m_RecvMsgs;
	CDataBlockAllocator*	m_pDBAllocator;
};
