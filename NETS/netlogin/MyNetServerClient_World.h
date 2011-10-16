#pragma once
#include "myserverclient.h"

class CMyNetServerClient_World :
	public CMyServerClient
{
public:
	CMyNetServerClient_World(CServer* pServer);
	virtual ~CMyNetServerClient_World(void);

private:

public:
	virtual void OnClose(int errorCode=0);		// 与对方断开的时候激活
	//virtual	void OnReceive(int nErrorCode=0);// 完成端口上受到消息时激活
	virtual CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);	//创建消息

	virtual void SetSendRevBuf();
};
