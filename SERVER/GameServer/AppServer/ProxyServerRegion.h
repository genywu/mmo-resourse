
#ifndef _PROXY_SERVER_REGION_
#define _PROXY_SERVER_REGION_

#include "serverregion.h"
class CProxyServerRegion :
	public CServerRegion
{
private:
	CProxyServerRegion(void);
public:
	CProxyServerRegion(const CGUID& id);
	CProxyServerRegion(long id);
	virtual ~CProxyServerRegion(void);

public:
	bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// ½âÂë
};

#endif