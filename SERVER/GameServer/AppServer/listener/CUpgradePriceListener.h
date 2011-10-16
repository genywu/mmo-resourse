#pragma once
#include "CContainerListener.h"

class CUpgradePriceListener :
	public CContainerListener
{
public:
	DWORD m_dwPrice;

	CUpgradePriceListener();
	~CUpgradePriceListener();

	virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};
