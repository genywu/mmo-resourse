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

	void	SetTarget						( const char* szGoodsOriginalName );

	void	SetTarget						( DWORD dwGoodsIndex );

	virtual bool	OnTraversingContainer	(CContainer* pContainer, CBaseObject* pObject);
};
