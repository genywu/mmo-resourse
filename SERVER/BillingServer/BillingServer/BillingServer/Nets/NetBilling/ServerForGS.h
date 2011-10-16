#pragma once
#include "..\Servers.h"

class CServerForGS : public CServer
{
public:
	CServerForGS(void);
	virtual ~CServerForGS(void);

	virtual CServerClient* CreateServerClient();
};
