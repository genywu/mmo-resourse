#pragma once
#include "..\servers.h"

class CMyNetServer :
	public CServer
{
private:
protected:
public:
	CMyNetServer(void);
	virtual ~CMyNetServer(void);

	virtual CServerClient* CreateServerClient();
	virtual void OnMapIDError(long lID);
public:
	SocketIDClient& GetClients() {return m_Clients;}
};
