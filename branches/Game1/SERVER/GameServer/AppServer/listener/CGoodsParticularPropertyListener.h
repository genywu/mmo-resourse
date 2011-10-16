#pragma once
#include "..\Goods\CGoodsBaseProperties.h"
#include "CContainerListener.h"

class CGoodsParticularPropertyListener :
	public CContainerListener
{
public:
	GOODS_ADDON_PROPERTIES	m_gapID;
	//vector<LONG>									m_vGoodsID;
	vector<CGUID>									m_vGoodsID;

	CGoodsParticularPropertyListener				();
	~CGoodsParticularPropertyListener				();

	virtual BOOL	OnTraversingContainer			(CContainer* pContainer, CBaseObject* pObject);
};
