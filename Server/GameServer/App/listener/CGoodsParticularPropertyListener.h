#pragma once
#include "CContainerListener.h"

class CGoodsParticularPropertyListener :
	public CContainerListener
{
public:
	eGoodsAddonProperties	m_gapID;
	//vector<LONG>									m_vGoodsID;
	vector<CGUID>									m_vGoodsID;

	CGoodsParticularPropertyListener				();
	~CGoodsParticularPropertyListener				();

	virtual bool	OnTraversingContainer			(CContainer* pContainer, CBaseObject* pObject);
};
