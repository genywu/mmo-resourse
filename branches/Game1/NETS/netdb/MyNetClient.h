#pragma once
#include "../clients.h"

class CMyNetClient :
	public CClient
{
public:
	CMyNetClient(void);
	~CMyNetClient(void);

	void HandleClose();

	void HandleReceive() {}
	void HandleConnect() {}

	//virtual	void OnReceive(int nErrorCode=0);	// 收到到消息时激活
	virtual CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);
	virtual bool IsCheckByCRC()	{return true;}

	virtual void SetSendRevBuf();
};
