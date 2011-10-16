///
/// @file AIUtilities.cpp
/// @author Kevin Lynx
/// @brief To provide some useful utilities for AI module.
///
#include "StdAfx.h"
#include "AIUtils.h"
#include "../../ServerRegion.h"

namespace AIUtils
{
	size_t FindAroundShape( CServerRegion *pRegion, CShape *pFinder, ShapeListType &outList )
	{
		size_t init_size = outList.size();
		pRegion->FindAroundObject( pFinder, TYPE_MONSTER, outList );
		pRegion->FindAroundObject( pFinder, TYPE_PLAYER, outList );
		return outList.size() - init_size;
	}

	CMoveShape *FindShape( CServerRegion *pRegion, const CGUID &targetID )
	{
		CMoveShape *pTarget;
		pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_MONSTER, targetID ));
		if( pTarget == NULL )
		{
			pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_PLAYER,targetID ) );
		}

		return pTarget;
	}
}