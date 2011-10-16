#include "StdAfx.h"
#include "CGoodsRepairListener.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoodsRepairListener::CGoodsRepairListener()
{
}

CGoodsRepairListener::~CGoodsRepairListener()
{
}

bool CGoodsRepairListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> CanReparied() )
		{
			CGoodsFactory::RepairEquipment( pGoods );
		}
	}
	return true;
}