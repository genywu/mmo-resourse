///
/// @file AntiCheatCommon.h
/// @author Kevin Lynx
/// @brief to provide some common utilities for AntiCheat module.
///
#ifndef ___ANTI_CHEAT_COMMON_H_ 
#define ___ANTI_CHEAT_COMMON_H_

namespace AntiCheat
{
#ifdef _GAME_SERVER
#define LOG_FILE "AntiCheatGS"
#elif defined _WORLD_SERVER_
#define LOG_FILE "AntiCheatWS"
#endif

	///
	/// runtime information
	///
	struct LogInfo
	{
		unsigned long AccWrong;
		unsigned long AccTimeOut;
		unsigned long AccRight;
		unsigned long AccQues;
		unsigned long AccSysErr;
	};
	const LogInfo *GetLogInfo();
	unsigned long AddAccWrong();
	unsigned long AddAccRight();
	unsigned long AddAccTimeOut();
	unsigned long AddAccQues();
	unsigned long AddAccSysError();
	void ResetLogInfo();

	///
	/// Check whether the second player is in the friend list of the first player .
	///
	bool IsFriend( CPlayer *first, const char *second_name );

	///
	/// Check whether the two players are in the same team.
	///
	bool IsInSameTeam( CPlayer *first, const char *second_name );

	///
	/// Check whether the two players are in the same faction.
	///
	bool IsInSameFaction( CPlayer *first, const char *second_name );

	enum PlayerRelation
	{
		PR_FRIEND = 0x0001,
		PR_TEAM = 0x0002,
		PR_FACTION = 0x0004,
		PR_ALL = PR_FRIEND | PR_TEAM | PR_FACTION,
	};

	///
	/// Check whether the two players have some relationship between them.
	/// friend || same_team || same_faction
	///
	bool CheckRelation( CPlayer *first, const std::string &second, long check_flag );
}

#endif 