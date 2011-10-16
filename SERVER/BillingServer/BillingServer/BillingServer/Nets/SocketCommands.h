//////////////////////////////////////////////////////////////////////////
//SocketCommands.h
//Fun:对Socket的操作命令列表
//Create Time:2004.12.17
//Author:Joe
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <deque>
using namespace std;
class CServerClient;

//对网络操作的类型
enum eSocketOperaType
{
	SOCKETOPERATYPE_ADD,			//添加
	SOCKETOPERATYPE_CDKEYJOIN,		//玩家进入
	SOCKETOPERATYPE_PLAYERJOIN,		//玩家进入
	
	SOCKETOPERATYPE_DELBYSOCKETID,	//根据套接字ID删除

	SOCKETOPERATYPE_QUITBYSOCKETID,	//根据套接字ID删除
	SOCKETOPERATYPE_QUITBYMAPID,	//根据映射 ID删除
	SOCKETOPERATYPE_QUITBYMAPSTR,	//根据映射的ID删除
	SOCKETOPERATYPE_QUITALL,		//删除

	SOCKETOPERATYPE_RECIEVE,		//接受数据
	SOCKETOPERATYPE_SENDTOSOCKET,	//根据SocketID发送数据
	SOCKETOPERATYPE_SENDTOMAPID,	//根据MapID发送数据
	SOCKETOPERATYPE_SENDTOMAPSTR,	//根据MapStr发送数据
	SOCKETOPERATYPE_SENDALL,		//发给所有的客户端

	SOCKETOPERATYPE_ONRECEIVE,		//当客户端可以收数据时激活
	SOCKETOPERATYPE_ONSEND,			//当客户端可以发送数据时激活
	SOCKETOPERATYPE_ONCLOSE,		//当客户端被关闭时候
	SOCKETOPERATYPE_ONCONNECT,		//返回客户端的连接情况

	SOCKETOPERATYPE_SENDEND,		//完成端口上发送结束消息
};

//对socket的操作结构，大小:24Byte
struct tagSocketOper
{
	eSocketOperaType OperaType;
	unsigned long	 lSocketID;
	char*			 pStrID;					//字符串ID

	void*			 pBuf;						//操作的内容
	long			 lNum1;						//备用值1
	long			 lNum2;						//备用值1
	//CServerClient*	 pServerClient;				//客户端套接字
	

	tagSocketOper(eSocketOperaType eOperType,unsigned long lID,void* pTemptBuf,
		long lNumber1,long lNumber2=0)
		:OperaType(eOperType)
		,lSocketID(lID)
		,pStrID(NULL)
		,pBuf(pTemptBuf)
		,lNum1(lNumber1)
		,lNum2(lNumber2)
	{
	}


	tagSocketOper(eSocketOperaType eOperType,unsigned long lID,char* pBufID,void* pTemptBuf,
		long lNumber1,long lNumber2=0)
		:OperaType(eOperType)
		,lSocketID(lID)
		,pStrID(pBufID)
		,pBuf(pTemptBuf)
		,lNum1(lNumber1)
		,lNum2(lNumber2)
	{
	}
};

typedef deque<tagSocketOper*> CommandsQueue;

class CSocketCommands
{
private:
	CommandsQueue	 m_Commands;				//命令操作队列
	CRITICAL_SECTION m_CriticalSectionCommans;
public:
	CSocketCommands(void);
	~CSocketCommands(void);

	bool Push_Front(tagSocketOper* pCommand);	// 压入命令到队列前段
	bool Push_Back(tagSocketOper* pCommand);	// 压入命令到队列后段
	tagSocketOper* Pop_Front();					// 从前段弹出命令操作

	long GetSize();								// 得到命令队列长度
	void Clear();								// 清空命令队列]

	void CopyAllCommand(CommandsQueue& TemptCommandsQueue);
	void AddCommandsQueueToFront(CommandsQueue& TemptCommandsQueue);
};
