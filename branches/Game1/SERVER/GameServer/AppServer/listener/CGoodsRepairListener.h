#pragma once
#include "CContainerListener.h"

class CGoodsRepairListener :
	public CContainerListener
{
public:
	CGoodsRepairListener();
	~CGoodsRepairListener();

	virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};
