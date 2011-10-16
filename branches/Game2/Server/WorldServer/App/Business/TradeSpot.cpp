///
/// @file CTradeSpot.cpp
/// @brief 贸易点相关
///
#include "StdAfx.h"
#include "TradeSpot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Business
{
	CTradeSpot::~CTradeSpot()
	{
	}

	bool CTradeSpot::LoadGoodsTemplate( const std::string &file )
	{
		m_Goods.clear();
		CRFile *rfile = rfOpen( file.c_str() );
		if( rfile == NULL )
		{
			return false;
		}

		stringstream stream;
		rfile->ReadToStream( stream );
		rfClose( rfile );
		
		tagGoods goods;
		string tmpstr;
		int vtType = 0;
		int Local = 0;

		while( ReadTo( stream, "*" ) )
		{
			stream >> goods.dwIndex
				>> goods.strOrigName
				>> tmpstr
				>> vtType
				>> goods.dwCredit
				>> goods.dwPersonalInvest
				>> goods.dwCountryInvest
				>> goods.dwSpotDevelop
				>> Local
				>> goods.dwBaseBuyLimit
				>> goods.dwVolume
				>> goods.fVolumeQuo
				>> goods.dwBasePrice
				>> goods.fBuyMinQuo
				>> goods.fBuyMaxQuo
				>> goods.fSellMinQuo
				>> goods.fSellMaxQuo
				>> goods.dwMinVol
				>> goods.dwMaxVol
				>> goods.fBuyMinAddonQuo
				>> goods.fSellMaxAddonQuo
				>> goods.dwFallPrice
				>> goods.fCurVolumeQuo ;
			
			goods.VTType = (BYTE) vtType;
			goods.Local = ( Local != 0 ? 1 : 0 );

			m_Goods.push_back( goods );
		}

		return true;
	}

	void CTradeSpot::EncodeToDataBlock( DBWriteSet &db ) 
	{
		db.AddToByteArray( &m_Property, sizeof( m_Property ) );
		db.AddToByteArray( (long) m_Goods.size() );
		for( GoodsList::const_iterator it = m_Goods.begin(); it != m_Goods.end(); ++ it )
		{
			db.AddToByteArray( (void*)&(*it), sizeof( tagGoods ) );
		}

		// TODO: 在GS连接上WS时，WS并非必然已装载数据库数据，所以以下数据可能为空

		// encode goods list raw data from database
		db.AddToByteArray( (long) m_GoodsRawData.size() );
		if( m_GoodsRawData.size() > 1 )
		{
			db.AddToByteArray( &m_GoodsRawData[1], (long) m_GoodsRawData.size() - 1 );
		}

		// encode country invest raw data from database
		db.AddToByteArray( (long) m_CountryInvestRawData.size() );
		if( m_CountryInvestRawData.size() > 1 )
		{
			db.AddToByteArray( &m_CountryInvestRawData[1], (long) m_CountryInvestRawData.size() - 1 );
		}
	}

}