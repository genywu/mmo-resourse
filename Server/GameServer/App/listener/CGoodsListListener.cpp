#include "StdAfx.h"
#include "CGoodsListListener.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoodsListListener::CGoodsListListener()
{
	m_bIsAllGoodsExist = true;
}

CGoodsListListener::~CGoodsListListener()
{
	m_vGoodsID.clear();
}

bool CGoodsListListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		m_vGoodsID.push_back( pGoods -> GetExID() );
	}
	return true;
}