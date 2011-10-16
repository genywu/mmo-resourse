#pragma once
#include "..\myadobase.h"

class CRsSetup :
	public CMyAdoBase
{
public:
	CRsSetup(void);
	virtual ~CRsSetup(void);
	
	void LoadPlayerID();
	bool SavePlayerID(_ConnectionPtr& cn);
	
	//void LoadGoodsID();
	//bool SaveGoodsID(_ConnectionPtr& cn);

	//void LoadOrganizingID();
	//bool SaveOrganizingID(_ConnectionPtr& cn);

	void LoadLeaveWorldID();
	bool SaveLeaveWorldID(_ConnectionPtr& cn);
};