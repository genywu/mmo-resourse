//////////////////////////////////////////////////////////////////////////
//SocketCommands.h
//Fun:对Socket的操作命令列表
//Create Time:2004.12.17
//Author:wangqiao
//////////////////////////////////////////////////////////////////////////

#pragma once
class CServerClient;
//对网络操作的类型
enum eSocketOperaType
{
	//完成端口用到的
	SCOT_Init,						//初始化
	SCOT_DisConn,					//断开某个连接
	SCOT_DisConnAll,				//断开所有连接
	SCOT_CloseAll,					//关闭所有连接
	SCOT_Send,						//发送消息

	SCOT_OnSend,					//完成端口上发送结束消息
	SCOT_OnRecieve,					//接受消息完成
	SCOT_OnSendZeroByte,			//发送0字节大小的操作
	SCOT_OnClose,					//调节子被关系
	SCOT_OnError,					//发生错误

	SCOT_ExitThread,				//退出运行线程
	
	//事件客户端用到的
	SCOT_Client_Send,
	SCOT_Client_OnSend,				//当客户端可以发送数据时激活
	SCOT_Client_OnRecv,				//当客户端有数据来的时候
	SCOT_Client_ExitThread,			//退出运行的主线程
};

//对socket的操作结构，大小:24Byte
struct tagSocketOper
{
	eSocketOperaType OperaType;
	ulong	 lSocketID;
	char*			 pStrID;					//字符串ID

	void*			 pBuf;						//操作的内容
	long			 lNum1;						//备用值1
	long			 lNum2;						//备用值1
	//CServerClient*	 pServerClient;			//客户端套接字
	
	tagSocketOper(){}

	tagSocketOper(eSocketOperaType eOperType,ulong lID,void* pTemptBuf,
		long lNumber1,long lNumber2=0)
		:OperaType(eOperType)
		,lSocketID(lID)
		,pStrID(NULL)
		,pBuf(pTemptBuf)
		,lNum1(lNumber1)
		,lNum2(lNumber2)
	{
	}


	tagSocketOper(eSocketOperaType eOperType,ulong lID,char* pBufID,void* pTemptBuf,
		long lNumber1,long lNumber2=0)
		:OperaType(eOperType)
		,lSocketID(lID)
		,pStrID(pBufID)
		,pBuf(pTemptBuf)
		,lNum1(lNumber1)
		,lNum2(lNumber2)
	{
	}

	void Init(eSocketOperaType eOperType,ulong lID,void* pTemptBuf,
		long lNumber1 = 0,long lNumber2=0)
	{
		OperaType = eOperType;lSocketID = lID;pBuf=pTemptBuf;
		lNum1 = lNumber1;lNum2 = lNumber2;
	}
};

typedef deque<tagSocketOper*> CommandsQueue;

//此类的消费线程只能有一个

class CSocketCommands
{
private:
	CommandsQueue	 m_Commands;				//命令操作队列
	CRITICAL_SECTION m_CriticalSectionCommans;
	//是否有线程等待
	bool	m_bWait;
	//当队列为空的时候,相应线程的等待事件同志
	HANDLE	m_hWait;
public:
	CSocketCommands(void);
	~CSocketCommands(void);

	bool Push_Back(tagSocketOper* pCommand);	// 压入命令到队列后段

	long GetSize();								// 得到命令队列长度
	void Clear();								// 清空命令队列]

	void CopyAllCommand(CommandsQueue& TemptCommandsQueue);
};
