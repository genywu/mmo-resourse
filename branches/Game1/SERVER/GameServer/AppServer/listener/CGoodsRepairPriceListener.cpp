#include "StdAfx.h"
#include "CGoodsRepairPriceListener.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"


CGoodsRepairPriceListener::CGoodsRepairPriceListener()
{
	m_dwPrice			= 0;
	m_dwNumEquipments	= 0;
}

CGoodsRepairPriceListener::~CGoodsRepairPriceListener()
{
	m_dwPrice			= 0;
	m_dwNumEquipments	= 0;
}

BOOL CGoodsRepairPriceListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> CanReparied() )
		{
			LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
			LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );
			if( lMaxDurability > 0 && lCurrentDurability < lMaxDurability )
			{
				++ m_dwNumEquipments;
				m_dwPrice += CGoodsFactory::CalculateRepairPrice( pGoods );
			}
		}
	}
	return TRUE;
}