///
/// @file ShareObjDef.h
///
#ifndef ___SHARE_OBJ_DEF_H_
#define ___SHARE_OBJ_DEF_H_

namespace StateAI
{
	enum ShareObjType
	{
		SOT_FIGHT_NORMAL = 1,
	};

#define DECL_SO_LIFE_POLICY_FUNC( className, baseClassName ) \
	static baseClassName *Create( AIState::entity_type* ); \
	static void Destroy( baseClassName * )
#define IMPL_SO_LIFE_POLICY_FUNC( className, baseClassName ) \
	baseClassName *className::Create( AIState::entity_type* ) \
	{ \
		return new className(); \
	} \
	void className::Destroy( baseClassName *obj ) \
	{ \
		delete obj; \
	}
}

#endif
