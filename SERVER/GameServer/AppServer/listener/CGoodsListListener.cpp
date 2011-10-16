#include "StdAfx.h"
#include "CGoodsListListener.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"


CGoodsListListener::CGoodsListListener()
{
	m_bIsAllGoodsExist = TRUE;
}

CGoodsListListener::~CGoodsListListener()
{
	m_vGoodsID.clear();
}

BOOL CGoodsListListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		m_vGoodsID.push_back( pGoods -> GetExID() );
	}
	return TRUE;
}