#pragma once
#include "../clients.h"

class CMyNetClient :
	public CClient
{
public:
	CMyNetClient(void);
	~CMyNetClient(void);

	// 给派生类使用的接口
	void HandleReceive();
	void HandleConnect();
	void HandleClose();

	//virtual	void OnReceive(int nErrorCode=0);	// 收到到消息时激活
	virtual CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);
	virtual bool IsCheckByCRC()	{return true;}

	virtual void CMyNetClient::SetSendRevBuf();
};
