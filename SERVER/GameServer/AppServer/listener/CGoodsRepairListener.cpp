#include "StdAfx.h"
#include "CGoodsRepairListener.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"



CGoodsRepairListener::CGoodsRepairListener()
{
}

CGoodsRepairListener::~CGoodsRepairListener()
{
}

BOOL CGoodsRepairListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> CanReparied() )
		{
			CGoodsFactory::RepairEquipment( pGoods );
		}
	}
	return TRUE;
}