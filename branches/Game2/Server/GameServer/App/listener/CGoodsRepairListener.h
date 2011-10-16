#pragma once
#include "CContainerListener.h"

class CGoodsRepairListener :
	public CContainerListener
{
public:
	CGoodsRepairListener();
	~CGoodsRepairListener();

	virtual bool OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};
