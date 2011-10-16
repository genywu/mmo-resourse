#pragma once
#include "CContainerListener.h"

class CSeekGoodsListener :
	public CContainerListener
{
private:
	DWORD									m_dwIndex;

public:
	vector<CGUID>							m_vGoodsID;

	CSeekGoodsListener						();
	~CSeekGoodsListener						();

	VOID	SetTarget						( CHAR* szGoodsOriginalName );

	virtual BOOL	OnTraversingContainer	(CContainer* pContainer, CBaseObject* pObject);
};
