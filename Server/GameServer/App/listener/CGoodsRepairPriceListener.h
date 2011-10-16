#pragma once
#include "CContainerListener.h"

class CGoodsRepairPriceListener :
	public CContainerListener
{
public:
	DWORD m_dwNumEquipments;
	DWORD m_dwPrice;

	CGoodsRepairPriceListener();
	~CGoodsRepairPriceListener();

	virtual bool OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};
