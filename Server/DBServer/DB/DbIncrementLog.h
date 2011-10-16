#pragma once
#include "..\myadobase.h"
#include "../public/GUID.h"
#include "winbase.h"

class CMessage;
class CDbIncrementLog :
	public CMyAdoBase
{
public:
	CDbIncrementLog(void);
	~CDbIncrementLog(void);

	bool LoadAll();
	bool Save(BYTE byType, DWORD dwMoney, 
		const char* szDesc, const CGUID& PlayerID, 
		const char* szAcc, SYSTEMTIME* logTime , _ConnectionPtr& cn);
	bool Load(const CGUID& guid, CMessage* msg, _ConnectionPtr& cn);
};