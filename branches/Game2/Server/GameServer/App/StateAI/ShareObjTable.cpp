///
/// @file ShareObjTable.cpp
///
#include "stdafx.h"
#include <assert.h>
#include "ShareObjTable.h"

namespace StateAI
{
	ShareObjTable::~ShareObjTable()
	{
		assert( m_ShareObjs.size() == 0 );
	}

	AIShareObject *ShareObjTable::Get( long type )
	{
		ShareObjTableT::iterator it = m_ShareObjs.find( type );
		return it == m_ShareObjs.end() ? NULL : it->second;
	}

	bool ShareObjTable::Add( long type, AIState *state, AIState::EntityType *entity, Creator creator )
	{
		if( Exist( type ) )
		{
			return false;
		}
		AIShareObject *obj = creator( entity );
		assert( obj != NULL );
		return Add( type, obj );
	}

	bool ShareObjTable::Add( long type, AIShareObject *obj )
	{
		if( Exist( type ) )
		{
			return false;
		}
		m_ShareObjs.insert( ShareObjTableT::value_type( type, obj ) );
		return true;
	}

	bool ShareObjTable::Exist( long type ) const
	{
		ShareObjTableT::const_iterator it = m_ShareObjs.find( type );
		return it != m_ShareObjs.end() ;
	}

	void ShareObjTable::Remove( long type, Destroyer destroyer )
	{
		AIShareObject *obj = Get( type );
		if( obj != NULL )
		{
			destroyer( obj );
		}
	}
}
