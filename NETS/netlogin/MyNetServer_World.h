#pragma once
#include "..\servers.h"

class CMyNetServer_World:
	public CServer
{
public:

	CMyNetServer_World(void);
	virtual ~CMyNetServer_World(void);

	virtual CServerClient* CreateServerClient();
};
