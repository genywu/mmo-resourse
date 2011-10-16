///
/// @file CTradeArea.cpp
/// @brief 贸易区相关
///
#include "StdAfx.h"
#include "CTradeArea.h"

namespace Business
{
	void CTradeArea::EncodeToDataBlock( DBWriteSet &db )
	{
		db.AddToByteArray( &m_Property, sizeof( m_Property ) );
	}
}