#include "StdAfx.h"
#include "CGoodsParticularPropertyListener.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoodsParticularPropertyListener::CGoodsParticularPropertyListener()
{
	m_gapID = GAP_UNKNOW;
}

CGoodsParticularPropertyListener::~CGoodsParticularPropertyListener()
{
	m_vGoodsID.clear();
}

bool CGoodsParticularPropertyListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> GetAddonPropertyValues( m_gapID, 1 ) != 0 )
		{
			m_vGoodsID.push_back( pGoods -> GetExID() );
		}
	}
	return true;
}