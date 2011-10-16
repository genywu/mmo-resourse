//=============================================================================
/**
*  file: MapFile.h
*
*  Brief:通过Windows的文件影射机制，包装了简单的影射文件操作功能
*			用于文件日志记录
*
*  Authtor:wangqiao
*	
*	Datae:2008-10-29
*/
//=============================================================================

#ifndef __MAP_FILE_H__
#define __MAP_FILE_H__
#pragma once

#include "MsgQueue.h"

class CDataBlock;
class CBaseMessage;
class CDataBlockAllocator;

const long MAX_FILENAME_LEN = 256;
const long MAX_CONTENT_LEN = 1024*10;
const long MAX_MAP_VIEW_SIZE = 1048576*10;

//影射文件的打开方式
enum eFileMapServerType
{
	FMT_No = 0,
	FMT_Client=0x00000001,//客户方式
	FMT_Server=0x00000002,//服务方式
};

enum MSG_DEFAULT_TYPE
{
	MSG_MEM_OPEN_FILE		= 0x0001,		// 打开文件
	MSG_MEM_WRITE_FILE		= 0x0002,		// 写入文件
	MSG_MEM_CLEAR_FILE		= 0x0003,		// 清空文件
	MSG_MEM_CLOSE_FILE		= 0x0004,		// 关闭文件
};

//消息中文件的操作方式
enum eFileOpType
{
	File_No		= 0,
	File_Open	= 1,	// 打开文件
	File_Write	= 2,	// 写文件
	File_Clear	= 3,	// 清空文件
	File_Close	= 4,	// 关闭文件
	End=0xFF,//结束处理,退出读数据线程
};

struct tagFileInfo0x
{	
	tagFileInfo0x(void)
		: dwLastWriteTime(0)
		, pFile(NULL)
	{
	}

	string strFileName;		// 文件路径
	DWORD dwLastWriteTime;	// 最后一次操作时间
	FILE* pFile;			// 文件句柄
};

typedef map<string, tagFileInfo0x*> MAP_FILES;
typedef MAP_FILES::iterator ITR_MAP_FILE;

class CMapFile
{
public:
	CMapFile(void);
	~CMapFile(void);

private:
	eFileMapServerType m_eType;			// 影射对象类型
	HANDLE	m_hFile;					// 影射到地址空间的文件句柄
	HANDLE	m_hMapObject;				// 影射文件句柄
	LPVOID m_pszMapViewS;				// 内存影射文件的地址空间(for Server)
	LPVOID m_pszMapViewC;				// 内存影射文件的地址空间(for Client)
	HANDLE m_hMutexS;					// 用于读写的互斥体(for Server)
	HANDLE m_hEventS;					// 互斥的事件句柄(for Server)
	HANDLE m_hMutexC;					// 用于读写的互斥体(for Client)
	HANDLE m_hEventC;					// 互斥的事件句柄(for Client)
	CDataBlockAllocator* m_pDBAllocator;
	MAP_FILES m_Files;					// 写数据的所有文件句柄
	char m_szCurDir[256];				// 当前工作目录
	bool volatile m_bExit;				// 线程退出标志
	HANDLE m_hSendEvent;				// 发送消息事件通知
	CMsgQueue m_SendMsgs;				// 接收到的消息队列

	//封装了读写数据的功能
private:
	// 释放所有文件
	void ReleaseAllFiles(void);
	//检查空间大小是否满足
	bool CheckViewSpace(long lNeedSize);
	//返回下次写数据的位置
	int WriteData(int nPos,const char *pData,int nLen);
	//范围下次读数据的位置
	int ReadData(int nPos,char *pData,int nLen);
	void SendExitMsg(void);						// 发送线程退出

	enum eRetSendMsg
	{
		//发送成功
		SendMsg_Ok = 1,
		//发送失败,影射文件空间不足
		SendMsg_NoSpace,
		//其他错误
		SendMsg_Error,
	};
	//理解发送一个消息
	eRetSendMsg SendMsgimmediately(CBaseMessage* pMsg);

public:
	void SetDBAllocator(CDataBlockAllocator* pDBA);
	//初始化
	bool Initial(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName=NULL);
	//重新设置
	bool Reset(eFileMapServerType eFMType,const char* pszMapObjName,const char* pFileName=NULL);
	void Reset(void);

	//释放资源
	void Release();
	void Exit(void);
	long SendMsg(void);							// 写消息(线程统一发送), 返回发送消息数量
	void SendMsg(CBaseMessage* pMsg);			// 写消息数据
	// 同步写消息数据,当消息队列满时,该消息有可能丢弃
	void SyncSendMsg(CBaseMessage* pMsg);		
	CBaseMessage* RecvMsg(void);				// 读消息数据

	// 关闭长时间(8s)没有I/O操作的文件
	void ClosePassiveFiles(void);

	// 根据文件名得到相应文件句柄
	tagFileInfo0x* GetFile(const char* pszFileName);

	// 添加文件句柄: pszFileName不能为NULL
	tagFileInfo0x* AddFile(const char* pszFileName);

	// 写文件
	void WriteFile(const char* pszFileName, const char* pszContent);

	// 清空文件
	void ClearFile(const char* pszFileName);

	// 根据文件名关闭相应文件句柄
	void CloseFile(const char* pszFileName);

	// 设置当前线程读写路径
	void SetCurDir(const char* szDir);

	// 获取当前线程读写路径
	const char* GetCurDir(void);
};

#endif
