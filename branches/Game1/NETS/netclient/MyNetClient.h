
#pragma once

#include "..\clients.h"

class CMyNetClient :
	public CClient
{

protected:
	CSocketCommands		m_SocketOperaCommands;		//对网络操作的命令队列
public:
	CMyNetClient(void);
	virtual ~CMyNetClient(void);

	// 给派生类使用的接口
	void HandleReceive();
	void HandleConnect();
	void HandleClose();

	virtual bool IsSameSocketID(long lSocketID1,long lSocketID2)
	{
		return lSocketID1 == lSocketID2;
	}

	CSocketCommands& GetSocketCommand()
	{
		return m_SocketOperaCommands;
	}

	virtual void CMyNetClient::SetSendRevBuf();
};
