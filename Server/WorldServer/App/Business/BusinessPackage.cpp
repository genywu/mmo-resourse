///
/// @file CBusinessPackage.cpp
/// @brief ÅÜÉÌ±³°üÃèÊö
///
#include "StdAfx.h"
#include "BusinessPackage.h"
#include "BusinessCommon.h"
#include "../Player.h"
#include "../Container/VolumeLimitGoodsContainer.h"
#include "../Goods/GoodsFactory.h"
#include "../Goods/Goods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Business
{
	CPackage::CPackage()
	{
		m_pContainer = new CVolumeLimitGoodsContainer();
		m_pGoods = NULL;
	}

	CPackage::~CPackage()
	{
		if( m_pGoods != NULL )
		{
			CGoodsFactory::GarbageCollect( &m_pGoods );
		}
		delete m_pContainer;
	}
	
	void CPackage::DecodeFromGS( CPlayer *pPlayer, DBReadSet &db )
	{
		BYTE flag = db.GetByteFromByteArray();
		if( flag == 1 )
		{
			if( m_pGoods != NULL )
			{
				CGoodsFactory::GarbageCollect( &m_pGoods );
			}
			m_pGoods = CGoodsFactory::UnserializeGoods( db,35 );		
			m_pContainer->Release();
			m_pContainer->SetContainerVolume( m_pGoods );
			m_pContainer->Unserialize( db );

			Log4c::Trace(ROOT_MODULE,FMT_STR( "Decode package for player [%s] ok", pPlayer->GetName() ) );
		}
		else
		{
			if( m_pGoods != NULL )
			{
				CGoodsFactory::GarbageCollect( &m_pGoods );
			}
			m_pContainer->Release();
		}
	}

	void CPackage::EncodeForGS( DBWriteSet &db )
	{
		if( m_pGoods != NULL )
		{
			db.AddToByteArray( (BYTE)1 );
			m_pGoods->Serialize( db );
			m_pContainer->Serialize( db );
		}
		else
		{
			db.AddToByteArray( (BYTE)0 );
		}
	}

	void CPackage::DecodeFromDB( CGoods *pContainerGoods )
	{
		if( m_pGoods != NULL )
		{
			// some error occures
		}

		m_pGoods = pContainerGoods ;
		DWORD dwVolume = m_pGoods->GetAddonPropertyValues( GAP_PACK_CAPACITY, 1 );
		m_pContainer->SetContainerVolume( dwVolume );
	}
}