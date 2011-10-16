///
///
///
#include "StdAfx.h"
#include "CTradeArea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Business
{
	void CTradeArea::DecodeFromDataBlock( DBReadSet &db ) 
	{
		db.GetBufferFromByteArray( &m_Property, sizeof( m_Property ) );
	}
}