#pragma once
#include "..\servers.h"

class CMyNetServer_Client:
	public CServer
{
public:

	CMyNetServer_Client(void);
	virtual ~CMyNetServer_Client(void);
	

	virtual CServerClient* CreateServerClient();
	virtual void OnMapStrError(const char* strMap);
};
