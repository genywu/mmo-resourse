///
/// @file CTradeSpot.h
/// @brief 贸易点相关
///
#pragma once
#include "BusinessCommon.h"
#include "../../../../Public/Common/BusinessGoodsDef.h"

namespace Business
{
	class CTradeSpot 
	{
	public:
		typedef std::vector<tagGoods> GoodsList;
		typedef std::vector<BYTE> RawBufType;
	public:
		CTradeSpot()
		{
			memset( &m_Property, 0, sizeof( m_Property ) );
			m_GoodsRawData.push_back( 0 );
			m_CountryInvestRawData.push_back( 0 );
		}

		CTradeSpot( const tagTradeSpotProperty &p ) : m_Property( p )
		{
			m_GoodsRawData.push_back( 0 );
			m_CountryInvestRawData.push_back( 0 );
		}

		~CTradeSpot();

		///
		/// Comment 'Load goods template config file' here is pretty stupid!
		/// The code will comment itself well.
		///
		bool LoadGoodsTemplate( const std::string &file );
	
		///
		/// Add the goods template list and the base trade area property to the buffer.
		///
		void EncodeToDataBlock( DBWriteSet &db );

		void SetProperty( const tagTradeSpotProperty &p )
		{
			m_Property = p;
		}

		const tagTradeSpotProperty &GetProperty() const 
		{
			return m_Property;
		}

		///
		/// Used in database loading module.
		///
		RawBufType &QueryGoodsRawBuf() 
		{
			return m_GoodsRawData;
		}
		RawBufType &QueryCountryInvestRawBuf()
		{
			return m_CountryInvestRawData;
		}

	private:
		tagTradeSpotProperty m_Property;
		GoodsList m_Goods;
		/// goods list raw data, load from database
		RawBufType m_GoodsRawData;
		/// country invest raw data, load from database
		RawBufType m_CountryInvestRawData;
	};

}