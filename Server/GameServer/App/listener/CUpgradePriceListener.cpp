#include "StdAfx.h"
#include "CUpgradePriceListener.h"
#include "../Goods/CGoods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUpgradePriceListener::CUpgradePriceListener()
{
	m_dwPrice			= 0;
}

CUpgradePriceListener::~CUpgradePriceListener()
{
	m_dwPrice			= 0;
}

bool CUpgradePriceListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	/*
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		m_dwPrice += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GOODS_UPGRADE_PRICE, 1 );
	}*/
	return true;
}