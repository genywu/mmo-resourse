#pragma once
#include "..\serverclient.h"

class CMyServerClient :
	public CServerClient
{
public:
	CMyServerClient(CServer* pServer);
	virtual ~CMyServerClient(void);
};
