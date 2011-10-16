///
/// @file AIStateType.h
/// @author Kevin Lynx
/// @brief Def some AI state types shared by GS and WS.
///
#ifndef ___AISTATE_TYPE_H_
#define ___AISTATE_TYPE_H_

namespace StateAI
{
	/// The main AI state type.
	enum StateType
	{
		ST_HANGUP = 0,
		ST_PEACE,
		ST_FIGHT,
		ST_ESCAPE,
		ST_RETURN,
		ST_DEAD,
		ST_SEARCH,
		ST_CYCLE,
        ST_AGONAL,
		ST_INVALID
	};

    /// The script AI impl type is >=256
    enum { SCRIPTAI_BASE_IMPL = 256 };
}

#endif
