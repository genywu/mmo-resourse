#pragma once
#include <list>
#include <vector>

using namespace std;

#include "../serverClient.h"


//const int MAX_MESSAGE_LENGTH = 10240;


class CMyServerClient:
	public CServerClient
{
public:
	CMyServerClient(CServer* pServer);
	~CMyServerClient(void);

	virtual void OnClose(int errorCode=0);		// 与对方断开的时候激活
	//virtual	void OnReceive(int nErrorCode=0);// 完成端口上受到消息时激活
	virtual CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);	//创建消息

	virtual void SetSendRevBuf();
};
