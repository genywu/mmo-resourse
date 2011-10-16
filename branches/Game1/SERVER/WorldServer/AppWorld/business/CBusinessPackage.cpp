///
/// @file CBusinessPackage.cpp
/// @brief ÅÜÉÌ±³°üÃèÊö
///
#include "StdAfx.h"
#include "CBusinessPackage.h"
#include "BusinessCommon.h"

#include "../Container/CVolumeLimitGoodsContainer.h"
#include "../goods/CGoodsFactory.h"
#include "../goods/CGoods.h"
#include "../goods/CGoodsBaseProperties.h"
#include "../Player.h"



namespace Business
{
	CPackage::CPackage()
	{
		m_pContainer = MP_NEW CVolumeLimitGoodsContainer();
		m_pGoods = NULL;
	}

	CPackage::~CPackage()
	{
		if( m_pGoods != NULL )
		{
			CGoodsFactory::GarbageCollect( &m_pGoods );
		}
		MP_DELETE(m_pContainer);
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

			BUSINESS_LOG_INFO( FMT_STR( "Decode package for player [%s] ok", pPlayer->GetName() ) );
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