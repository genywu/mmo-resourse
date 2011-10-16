///
/// @file CBusinessGoods.cpp
/// @brief 商业系统商品模板 
///
#include "StdAfx.h"
#include "CBusinessGoods.h"
#include "../goods/CGoodsFactory.h"


namespace Business
{
	CBusinessGoods::CBusinessGoods( CTradeSpot *pSpot ) : m_pFather( pSpot )
	{
		m_PriceState = PRICE_EQUAL;
		memset( &m_BaseProperty, 0, sizeof( m_BaseProperty ) );
	}
	CBusinessGoods::CBusinessGoods( const tagGoods &bp, CTradeSpot *pSpot ) : m_pFather( pSpot )
	{
		m_PriceState = PRICE_EQUAL;
		SetBaseProperty( bp );
	}

	void CBusinessGoods::SetBaseProperty( const tagGoods &bp )
	{
		m_BaseProperty = bp;
		m_Volume = bp.dwVolume ;
		m_MaxVolume = bp.dwVolume ;
		m_fBuyQuo = ( bp.fBuyMaxQuo - bp.fBuyMinQuo ) / bp.dwVolume ;
		m_fSellQuo = ( bp.fSellMaxQuo - bp.fSellMinQuo ) / bp.dwVolume;
		m_Index = CGoodsFactory::QueryGoodsIDByOriginalName( bp.strOrigName );
	
		m_dwBuyPrice = m_dwSellPrice = bp.dwBasePrice ;
		m_dwBuyCount = m_dwSellCount = 0;
		m_PriceState = PRICE_EQUAL;
	}

	void CBusinessGoods::UpdatePrice()
	{
		DWORD dwOldPrice = 0;
		if( m_BaseProperty.Local )
		{
			dwOldPrice = m_dwSellPrice;
		}
		else
		{
			dwOldPrice = m_dwBuyPrice;
		}

		if( m_Volume > 0 && m_Volume <= m_MaxVolume )
		{
			// 正常范围(0, 最大库存容量]
			m_dwBuyCount = m_MaxVolume - m_Volume;
			m_dwSellCount = m_Volume;
			m_fBuyQuo = ( m_BaseProperty.fBuyMaxQuo - m_BaseProperty.fBuyMinQuo ) / m_MaxVolume ;
			m_fSellQuo = ( m_BaseProperty.fSellMaxQuo - m_BaseProperty.fSellMinQuo ) / m_MaxVolume;
			float fBuyAddonQuo = m_dwBuyCount * m_fBuyQuo + 1;
			m_dwBuyPrice = m_BaseProperty.dwBasePrice * fBuyAddonQuo;

			float fSellAddonQuo = m_BaseProperty.fSellMaxQuo - m_dwSellCount * m_fSellQuo;
			m_dwSellPrice = m_BaseProperty.dwBasePrice * fSellAddonQuo;
		}
		else if( m_Volume <= 0 && m_Volume > m_BaseProperty.dwMinVol )
		{
			// 低于库存 ( 承载量min, 0]
			m_dwBuyCount = m_MaxVolume - m_Volume;
			float fBuyQuo = m_BaseProperty.fBuyMinAddonQuo / ( m_BaseProperty.dwMinVol * m_BaseProperty.dwMinVol );
			float fBuyAddon = m_BaseProperty.fBuyMaxQuo + fBuyQuo * 
				( m_dwBuyCount - m_MaxVolume ) * ( m_dwBuyCount - m_MaxVolume );
			m_dwBuyPrice = fBuyAddon * m_BaseProperty.dwBasePrice ;
		}
		else if( m_Volume > m_MaxVolume && m_Volume <= m_BaseProperty.dwMaxVol )
		{
			// 高于库存 (最大库存容量, 承载量max]
			m_dwSellCount = m_Volume - m_MaxVolume;
			float fSellQuo = m_BaseProperty.fSellMaxAddonQuo / 
				( ( m_BaseProperty.dwMaxVol - m_MaxVolume ) * ( m_BaseProperty.dwMaxVol - m_MaxVolume ) );
			float fSellAddon = m_BaseProperty.fSellMinQuo - fSellQuo * 
				( m_dwSellCount ) * ( m_dwSellCount );
			m_dwSellPrice = fSellAddon * m_BaseProperty.dwBasePrice;
		}
		else if( m_Volume > m_BaseProperty.dwMaxVol )
		{
			// 暴跌，公式和高于库存相同

		}
		else if( m_Volume < m_BaseProperty.dwMinVol )
		{
			// 暴涨，公式和低于库存相同
		}

		if( m_BaseProperty.Local )
		{
			SetPriceState( (int)( m_dwBuyPrice - dwOldPrice ) );
		}
		else
		{
			SetPriceState( (int)( m_dwSellPrice - dwOldPrice ) );			
		}
	}

	DWORD CBusinessGoods::GetBuyLimit( const CGUID &player_id )
	{
		BuyLimitTable::const_iterator it = m_BuyLimitRecord.find( player_id );
		if( it != m_BuyLimitRecord.end() )
		{
			return it->second;
		}

		return GetMaxBuyLimit( player_id );
	}

	DWORD CBusinessGoods::GetMaxBuyLimit( const CGUID &player_id )
	{
		// TODO: 更多最大限购额换算规则

		// 基本限购额
		DWORD dwBuyLimit = m_BaseProperty.dwBaseBuyLimit;	

		CPlayer *pPlayer = GetGame()->FindPlayer( player_id );
		if( pPlayer != NULL )
		{
			//‘交易之道’技能对限购额的影响

		}

		return dwBuyLimit;
	}

	void CBusinessGoods::SetBuyLimit( const CGUID &player_id, DWORD dwBuyLimit )
	{
		m_BuyLimitRecord[player_id] = dwBuyLimit;
	}

	bool CBusinessGoods::HasBuyLimitRecord( const CGUID &player_id )
	{
		return m_BuyLimitRecord.find( player_id ) != m_BuyLimitRecord.end();
	}

	void CBusinessGoods::RefreshBuyLimit( const CGUID &player_id )
	{
		// 如果不是本地出售的商品，限购额记录必然为空
		if( HasBuyLimitRecord( player_id ) )
		{
			// TODO: 最大限购额换算
			SetBuyLimit( player_id, GetMaxBuyLimit( player_id ) );
		}
	}

	DWORD CBusinessGoods::SubBuyLimit( const CGUID &player_id, DWORD dwSub )
	{
		BuyLimitTable::iterator it = m_BuyLimitRecord.find( player_id );
		if( it == m_BuyLimitRecord.end() )
		{
			m_BuyLimitRecord.insert( std::make_pair( player_id, m_BaseProperty.dwBaseBuyLimit - dwSub ) );
			return m_BaseProperty.dwBaseBuyLimit - dwSub;
		}
		else
		{
			it->second -= dwSub;
			return it->second;
		}
	}

	void CBusinessGoods::GetTaxScale( CPlayer *pPlayer, int &OrigTaxScale, int &TaxScale )
	{
		// TODO : to calculate the tax
		OrigTaxScale = 20;
		TaxScale = 10;
	}

	void CBusinessGoods::EncodeToSave( vector<BYTE> &buf )
	{
		_AddToByteArray( &buf, (long)m_Index );
		_AddToByteArray( &buf, (long)m_Volume );
		_AddToByteArray( &buf, (long)m_MaxVolume );
	}
	
	void CBusinessGoods::DecodeFromDB( DBReadSet &db )
	{
		m_Volume = db.GetLongFromByteArray();
		m_MaxVolume = db.GetLongFromByteArray();
	}

	void CBusinessGoods::SetPriceState( int sub )
	{
		if( sub < 0 )
		{
			m_PriceState = PRICE_LESS;
		}
		else if( sub == 0 )
		{
			m_PriceState = PRICE_EQUAL;
		}
		else 
		{
			m_PriceState = PRICE_GREAT;
		}
	}

}