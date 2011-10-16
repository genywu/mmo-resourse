#include "StdAfx.h"
#include "CSeekGoodsListener.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"

CSeekGoodsListener::CSeekGoodsListener()
{
	m_dwIndex = 0;
}

CSeekGoodsListener::~CSeekGoodsListener()
{
	m_vGoodsID.clear();
}

VOID CSeekGoodsListener::SetTarget( CHAR* szGoodsOriginalName )
{
	if( szGoodsOriginalName )
	{
		m_dwIndex = CGoodsFactory::QueryGoodsIDByOriginalName( szGoodsOriginalName );
	}
}

VOID CSeekGoodsListener::SetTarget( DWORD dwGoodsIndex )
{
	if( dwGoodsIndex )
	{
		m_dwIndex = dwGoodsIndex;
	}
}


BOOL CSeekGoodsListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods -> GetBasePropertiesIndex() == m_dwIndex )
		{
			m_vGoodsID.push_back( pGoods -> GetExID() );
		}
	}
	return TRUE;
}