#include "StdAfx.h"
#include "CUpgradePriceListener.h"
#include "..\Goods\CGoods.h"


CUpgradePriceListener::CUpgradePriceListener()
{
	m_dwPrice			= 0;
}

CUpgradePriceListener::~CUpgradePriceListener()
{
	m_dwPrice			= 0;
}

BOOL CUpgradePriceListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	/*
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		m_dwPrice += pGoods -> GetAddonPropertyValues( GAP_GOODS_UPGRADE_PRICE, 1 );
	}*/
	return TRUE;
}