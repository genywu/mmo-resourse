#pragma once
#include "../clients.h"

class CNetGameserverClient :
	public CClient
{
public:
	CNetGameserverClient(void);
	~CNetGameserverClient(void);

	// 给派生类使用的接口
	void HandleReceive();
	void HandleConnect();
	void HandleClose();
};
