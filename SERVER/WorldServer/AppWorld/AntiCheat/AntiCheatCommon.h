///
/// @file AntiCheatCommon.h
/// @author Kevin Lynx
/// @brief to provide some common utilities for AntiCheat module.
///
#ifndef ___ANTI_CHEAT_COMMON_H_ 
#define ___ANTI_CHEAT_COMMON_H_

#include "../../../../public/tools.h"

namespace AntiCheat
{
#ifdef _GAME_SERVER
#define LOG_FILE "AntiCheatGS"
#elif defined _WORLD_SERVER_
#define LOG_FILE "AntiCheatWS"
#endif

	/// 
	/// to write logs:
	/// PutoutLog( LOG_FILE, LT_ERROR, "some test logs %s", str ); etc
	///
}

#endif 