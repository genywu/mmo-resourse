///
///
///
#include "StdAfx.h"
#include "CTradeArea.h"

namespace Business
{
	void CTradeArea::DecodeFromDataBlock( DBReadSet &db ) 
	{
		db.GetBufferFromByteArray( &m_Property, sizeof( m_Property ) );
	}
}