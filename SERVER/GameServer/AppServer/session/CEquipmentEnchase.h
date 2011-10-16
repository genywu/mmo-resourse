// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif


#include "..\Container\CEquipmentEnchaseShadowContainer.h"
#include "CPlug.h"

class CEquipmentEnchase
: public CPlug
{
private:
	
	CEquipmentEnchaseShadowContainer m_cEnchaseContainer;

public:	
	CEquipmentEnchase();
	
	CEquipmentEnchase(LONG lID);
	
	~CEquipmentEnchase();
	
	virtual BOOL IsPlugAvailable();
	
	virtual BOOL OnSessionEnded();
	
	virtual BOOL OnSessionAborted();
	
	virtual BOOL OnPlugInserted();
	
	virtual BOOL OnPlugAborted();
	
	virtual BOOL OnPlugEnded();

	virtual BOOL OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData);
	
	virtual VOID Release();
	
	virtual CContainer* GetContainer(LONG lID);

	CEquipmentEnchaseShadowContainer* GetShadowContainer(){return &m_cEnchaseContainer;}

	CGoods* GetGoodsByPos(DWORD dwPos);
	
	DWORD GetSucceedResult();
	
	DWORD GetFailedResult();
	
	DWORD GetProbability();
	
	DWORD GetUpgradePrice();
	
	BOOL DoesUpgradeValid();
	
	BOOL Enchase();
	
	BOOL DeleteGoods(CEquipmentEnchaseShadowContainer::ENCHASE_EQUIPMENT_PLACE_CELL uepc);
};