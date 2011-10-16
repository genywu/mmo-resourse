#pragma once
#include "..\serverclient.h"

class CPlayer;

class CMyServerClient :
	public CServerClient
{
public:
	CMyServerClient(CServer*	pServers);
	virtual ~CMyServerClient(void);

public:
	
	virtual void OnClose(int errorCode=0);		// 与对方断开的时候激活
	//virtual	void OnReceive(int nErrorCode=0);	// 完成端口上受到消息时激活
	virtual bool CheckMsg(CBaseMessage *pMsg);

	void OnOneMessageSizeOver(long lSize,long lPermitSize);
	void OnTotalMessageSizeOver(long lSize,long lPermitSize);
	
};
