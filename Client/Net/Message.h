#pragma once
#include "../../Public/Nets/BaseMessage.h"
#include "../../Public/Nets/Clients.h"
class CMySocket;
class CClient;
class CRegion;
class CPlayer;
class CMemClient;

extern	enum OPERATION_TYPE
{
	OT_ROLL_BACK		= 0,
	OT_MOVE_OBJECT,
	OT_NEW_OBJECT,
	OT_DELETE_OBJECT,
	OT_SWITCH_OBJECT,
	OT_SWITCH_FILL,
	OT_CHANGE_AMOUNT,
};

class CMessage :
	public CBaseMessage
{
public:
	CMessage(void);
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CClient	*m_pNetClient;
    static CClient* s_pNetClient_ChatServer;
public:

	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	static bool Release();

	static void SetClient(CClient *pClient)	{m_pNetClient=pClient;}

    static void SetClient4Cs(CClient* p) {s_pNetClient_ChatServer = p;}
    static CClient* GetClient4Cs() {return s_pNetClient_ChatServer;}
    static void ResetClient4Cs()
    {
        if (s_pNetClient_ChatServer)
        {
            s_pNetClient_ChatServer->Exit();
            s_pNetClient_ChatServer->Close();
            delete s_pNetClient_ChatServer;
            s_pNetClient_ChatServer = NULL;
        }
    }

	static CBaseMessage* NewMessage();

////////////////////////////////////////////////////////////////////////
//	Player & Region
////////////////////////////////////////////////////////////////////////
private:
	CRegion* m_pRegion;
	CPlayer* m_pPlayer;

public:
	CRegion* GetRegion()	{ return m_pRegion; }
	CPlayer* GetPlayer()	{ return m_pPlayer; }


	void Reset(void);        //重设消息相关变量
////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////
	virtual void SetSocketID(long lSocketID){}
	virtual long GetSocketID(){return 0;}

	virtual void SetIP(DWORD dwIP){}
	virtual DWORD GetIP(){return 0;}

	long Send(CClient* p=NULL);		// 发送消息
    bool SendToChatServer();        // 发送消息给聊天服务器

	long DefaultRun();   //缺省的消息解析函数
	virtual long Run();					// 消息处理

//用内存影射文件的进程通信机制,实现写文件的异步
//Author:wangqiao
private:
	//影射文件实例
	//static CMapFile *m_pMapFile;
	static CMemClient *m_pMemClient;
public:
	static void SetMemClient(CMemClient *pMemClient)	{m_pMemClient = pMemClient;}
	//打开文件
	static void AsyOpenFile(const char* pszFileName);
	//关闭文件
	static void AsyCloseFile(const char* pszFileName);
	//清空文件内容
	static void AsyClearFile(const char* pszFileName);
	//异步写文件的接口
	static void AsyWriteFile(const char* pszFileName,const char* pszContent);
	// 向测试程序发送消息
	long SendMemToTest(CMemClient *pMemClient = NULL);
	// 测试程序发送的消息处理
	long TestRun();
};