#pragma once
#include "..\basemessage.h"
#include "..\..\public\MsgType.h"
#include "../../GameManageTools/GMTPublic/def/MsgGMT_SCC2SUC.h"
#include <hash_map>
using namespace stdext;

class CMySocket;
class CServer;
class CClient;

extern enum eOperationType
{
	eOT_Roll_Back	= 0,
	eOT_Move_Object,
	eOT_New_Object,
	eOT_Delete_Object,
	eOT_Switch_Object,
	eOT_Switch_Fill,
	eOT_Change_Amount,
};


class CMessage :
	public CBaseMessage
{
public:
	CMessage();
	CMessage(long lType);
	virtual ~CMessage(void);

private:
	static CClient* m_pNetClient;
public:
	//初始化
	static bool Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum);
	//释放资源
	static bool Release();

	static void SetClient(CClient* pClient)	{ m_pNetClient = pClient; }

	static CBaseMessage* NewMessage();

public:
	virtual void SetSocketID(long lSocketID)	{}//	{ m_lSocketID=lSocketID;}
	virtual long GetSocketID()		  {return 0;} //{return m_lSocketID;}

	virtual	void SetIP(DWORD dwIP)				{}//{m_dwIP = dwIP;	}
	virtual	DWORD GetIP()				{return 0;}//		{return m_dwIP;	}

	void Reset(void);        //重设消息相关变量

	long Send(CClient * pClient = NULL );

	long DefaultRun();

	virtual long Run();					// 服务器端消息处理

};

