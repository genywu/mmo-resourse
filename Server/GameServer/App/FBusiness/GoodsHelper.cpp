///
/// @file GoodsHelper.cpp
/// @brief To provide some helper functions to query goods properties.
///
#include "StdAfx.h"
#include "GoodsHelper.h"
#include "../Goods/CGoodsFactory.h"

#include "../../../../Public/Setup/GoodsSetup.h"

namespace FBusiness
{
	namespace GoodsHelper
	{
		long GetPrice( long lIndex )
		{
			CGoodsBaseProperties *pProperty = GoodsSetup::QueryGoodsBaseProperties( lIndex );
			if( pProperty == NULL )
			{
				return -1;
			}
			
			if( !pProperty->IsHasAddonPropertie( GAP_FBUSINESS_PRICE ) )
			{
				return -1;
			}
			DWORD dwRet = pProperty->GetAddonPropertyValue( GAP_FBUSINESS_PRICE, 1 );
			return (long) dwRet;
		}
	}
}
