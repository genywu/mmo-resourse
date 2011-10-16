#include "StdAfx.h"
#include "SeekGoodsListener.h"
#include "../Goods/Goods.h"

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

VOID CSeekGoodsListener::SetTarget( CHAR* szGoodsOriginalName )
{
	if( szGoodsOriginalName )
	{
		m_dwIndex = GoodsSetup::QueryGoodsIDByOriginalName( szGoodsOriginalName );
	}
}

BOOL CSeekGoodsListener::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		if( pGoods->GetBasePropertiesIndex() == m_dwIndex )
		{
			m_vGoodsID.push_back( pGoods->GetExID() );
		}
	}
	return TRUE;
}