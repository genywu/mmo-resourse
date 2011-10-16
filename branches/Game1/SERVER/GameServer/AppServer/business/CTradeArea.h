///
///
///
#pragma once

#include <vector>
#include "BusinessCommon.h"

namespace Business
{

	class CTradeArea : public BaseMemObj
	{
	public:
		CTradeArea()
		{
			memset( &m_Property, 0, sizeof( m_Property ) );
		}

		CTradeArea( const tagTradeAreaProperty &property ) : m_Property( property )
		{
		}

		void SetProperty( const tagTradeAreaProperty &property )
		{
			m_Property = property;
		}
		
		const tagTradeAreaProperty &GetProperty() const
		{
			return m_Property;
		}

		void DecodeFromDataBlock( DBReadSet &db );

	private:
		tagTradeAreaProperty m_Property;
	};
}