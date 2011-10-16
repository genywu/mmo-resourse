#pragma once
#include "CContainerListener.h"

class CSeekGoodsListener :
	public CContainerListener
{
private:
	DWORD									m_dwIndex;

public:
	//vector<LONG>							m_vGoodsID;
	vector<CGUID>							m_vGoodsID;


	CSeekGoodsListener						();
	~CSeekGoodsListener						();

	VOID	SetTarget						( CHAR* szGoodsOriginalName );

	VOID	SetTarget						( DWORD dwGoodsIndex );

	virtual BOOL	OnTraversingContainer	(CContainer* pContainer, CBaseObject* pObject);
};
