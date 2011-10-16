#pragma once
#include "CContainerListener.h"

class CServerRegion;
class CGoodsListListener :
	public CContainerListener
{
public:
	//vector<LONG>							m_vGoodsID;
	vector<CGUID>							m_vGoodsID;
	typedef vector<CGUID>::iterator			itGoodsID;
	bool									m_bIsAllGoodsExist;

	CGoodsListListener						();
	~CGoodsListListener						();

	virtual bool	OnTraversingContainer	(CContainer* pContainer, CBaseObject* pObject);
};
