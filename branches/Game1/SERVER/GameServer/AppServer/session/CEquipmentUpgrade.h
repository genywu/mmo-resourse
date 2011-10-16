// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "..\Container\CEquipmentUpgradeShadowContainer.h"
#include "CPlug.h"

class CEquipmentUpgrade 
: public CPlug
{
private:	
	CEquipmentUpgradeShadowContainer m_cUpgradeContainer;

public:	
	CEquipmentUpgrade();
	
	CEquipmentUpgrade(LONG lID);
	
	~CEquipmentUpgrade();
	
	virtual BOOL IsPlugAvailable();
	
	virtual BOOL OnSessionEnded();
	
	virtual BOOL OnSessionAborted();
	
	virtual BOOL OnPlugInserted();
	
	virtual BOOL OnPlugAborted();
	
	virtual BOOL OnPlugEnded();
	
	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);
	
	virtual VOID Release();
	
	virtual CContainer* GetContainer(LONG lID);
	
	DWORD GetSucceedResult();
	
	DWORD GetFailedResult();
	
	DWORD GetProbability();
	
	DWORD GetUpgradePrice();
	
	BOOL DoesUpgradeValid();
	
	BOOL Upgrade();
	
	BOOL DeleteGoods(CEquipmentUpgradeShadowContainer::UPGRADING_EQUIPMENT_PLACE_CELL uepc);
};