///
/// @file ShareObjTable.h
///
#ifndef ___SHARE_OBJ_TABLE_H_
#define ___SHARE_OBJ_TABLE_H_

#include "AIStateDefs.h"
#include <map>

namespace StateAI
{
	///
	/// It's a manager class to manage some shared objects within some AI states.
	///
	class ShareObjTable
	{
	public:
		typedef std::map<long, AIShareObject*> ShareObjTableT;
		typedef AIShareObject *(*Creator)( AIState::EntityType* );
		typedef void (*Destroyer)( AIShareObject* );
	public:
		~ShareObjTable();

		AIShareObject *Get( long type );

		bool Add( long type, AIState *state, AIState::EntityType *entity, Creator creator );

		bool Add( long type, AIShareObject *obj );

		bool Exist( long type ) const;

		void Remove( long type, Destroyer destroyer );

	private:
		ShareObjTableT m_ShareObjs;
	};
}
 
#endif
