#pragma once

#ifndef _INC_CPLAYERID_H
#define _INC_CPLAYERID_H

#include "../../../DBAccess/worlddb/RsSetup.h"

class CPlayerID
{
private:
	CPlayerID(void) {};
	~CPlayerID(void) {};

public:
	static CPlayerID* GetInstance();
	static void Release();

protected:
	static CPlayerID *instance;

public:
	UINT GeneratePlayerID() { return ++m_nPlayerID; }

	UINT GetPlayerID() { return m_nPlayerID; }
	void SetPlayerID(UINT nID) { m_nPlayerID = nID; }

private:
	UINT m_nPlayerID;
};


#endif // _INC_CPLAYERID_H
