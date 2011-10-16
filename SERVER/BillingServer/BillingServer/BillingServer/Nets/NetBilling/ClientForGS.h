#pragma once
#include "..\serverclient.h"

class CClientForGS :
	public CServerClient
{
public:
	CClientForGS(CServer* pServer);
	virtual ~CClientForGS(void);

public:
	virtual void OnClose(int errorCode=0);		// 与对方断开的时候激活
	virtual	void OnReceive(int nErrorCode=0);	// 完成端口上受到消息时激活

	virtual void SetSendRevBuf();
};
