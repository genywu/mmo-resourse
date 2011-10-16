///
/// @file StateImplDef.h
/// @author Kevin Lynx
/// @brief Declare AI state implementation types.
///
#ifndef ___STATE_IMPL_DEF_H_
#define ___STATE_IMPL_DEF_H_

namespace StateAI
{
	enum PeaceStateImpl
	{
		PSI_NORMAL = 1,
		PSI_PLAYER,
        PSI_NPC,

        PSI_SCRIPT = 256,
	};

	enum FightStateImpl
	{
		FSI_NORMAL = 1,
		FSI_PLAYER,

        FSI_SCRIPT = 256,
	};

    enum AgonalStateImpl
    {
        ASI_NORMAL = 1,
        ASI_PLAYER,
    };

	enum HangupStateImpl
	{
		HSI_NORMAL = 1,
	};

	enum SearchStateImpl
	{
		SSI_DHF_GLADIATOR = 1,
		SSI_DHF_PASSIVE_GLADIATOR,
        SSI_GUARD,
        SSI_TAUNT_GLADIATOR,
        SSI_TAUNT_PASSIVE_GLADIATOR,

        SSI_SCRIPT = 256,
	};

	enum DeadStateImpl
	{
		DSI_NORMAL = 1,
		DSI_PLAYER,
	};

	enum ReturnStateImpl
	{
		RSI_NORMAL = 1,
	};

	enum EscapeStateImpl
	{
		ESI_NORMAL = 1,

        ESI_SCRIPT = 256,
	};

	enum CycleStateImpl
	{
		CSI_MOVETO_CELL = 2,
		CSI_MOVEBY_STEP,
		CSI_FOLLOW_TARGET,
		CSI_INVALID,
		CSI_WRAPPER = 1,
		CSI_SUB_STATE_COUNT = CSI_INVALID - 1,
	};
}

#endif
