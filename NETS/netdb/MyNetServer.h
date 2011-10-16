#pragma once
#include "..\servers.h"

class CMyServerClient;

class CMyNetServer:
	public CServer
{

public:
	CMyNetServer(void);
	~CMyNetServer(void);
public:
	virtual CServerClient* CreateServerClient();
};
