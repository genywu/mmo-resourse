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
	BOOL									m_bIsAllGoodsExist;

	CGoodsListListener						();
	~CGoodsListListener						();

	virtual BOOL	OnTraversingContainer	(CContainer* pContainer, CBaseObject* pObject);
};
