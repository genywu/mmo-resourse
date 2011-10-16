#include "StdAfx.h"
#include "CSeekGoodsListener.h"
#include "../Goods/CGoods.h"

#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSeekGoodsListener::CSeekGoodsListener()
{
	m_dwIndex = 0;
}

CSeekGoodsListener::~CSeekGoodsListener()
{
	m_vGoodsID.clear();
}

void CSeekGoodsListener::SetTarget( const char* szGoodsOriginalName )
{
	if( szGoodsOriginalName )
	{
		m_dwIndex = GoodsSetup::QueryGoodsIDByOriginalName( szGoodsOriginalName );
	}
}

void CSeekGoodsListener::SetTarget( DWORD dwGoodsIndex )
{
	if( dwGoodsIndex )
	{
		m_dwIndex = dwGoodsIndex;
	}
}


bool CSeekGoodsListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> GetBasePropertiesIndex() == m_dwIndex )
		{
			m_vGoodsID.push_back( pGoods -> GetExID() );
		}
	}
	return true;
}