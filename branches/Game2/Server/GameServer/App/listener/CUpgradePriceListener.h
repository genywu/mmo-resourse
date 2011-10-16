#pragma once
#include "CContainerListener.h"

class CUpgradePriceListener :
	public CContainerListener
{
public:
	DWORD m_dwPrice;

	CUpgradePriceListener();
	~CUpgradePriceListener();

	virtual bool OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};
