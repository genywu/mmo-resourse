///
/// @file PlayerRelation.cpp
/// @author Kevin Lynx
/// @brief Check the relationship between two players.
///
#include "stdafx.h"
#include "AntiCheatCommon.h"
#include "../LinkMan/LinkmanSystem.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../Player.h"

namespace AntiCheat
{
	static CPlayer *FindPlayerByName( const char *name )
	{
		return GetGame()->FindPlayer( name );
	}

#define CHECK_PLAYER( player, ret ) \
	if( player == NULL ) \
		return ret

	///
	/// Check whether the second player is in the friend list of the first player .
	///
	bool IsFriend( CPlayer *first, const char *second_name )
	{
		return LinkmanSystem::GetInstance().CheckRelation( 
			first->GetExID(), second_name, eLGT_Friend ) == TRUE ;	
	}

	///
	/// Check whether the two players are in the same team.
	///
	bool IsInSameTeam( CPlayer *first, const char *second_name )
	{
		const CGUID &team_id = first->GetTeamID();
		if( team_id == NULL_GUID )
		{
			return false;
		}
		GSTeam *team = GetOrganiCtrl()->GetGameTeamByID( team_id );
		if( team == NULL )
		{
			PutoutLog( LOG_FILE, LT_WARNING, "Team for player [%s] has not been found.", 
				first->GetName() );
			return false;
		}
		return team->IsMember( second_name );
	}

	///
	/// Check whether the two players are in the same faction.
	///
	bool IsInSameFaction( CPlayer *first, const char *second_name )
	{
		const CGUID &fac_id = first->GetFactionID();
		if( fac_id == NULL_GUID )
		{
			return false;
		}
		GameFaction *faction = GetOrganiCtrl()->GetFactionOrganizing( fac_id );
		if( faction == NULL )
		{
			PutoutLog( LOG_FILE, LT_WARNING, "Faction for player [%s] has not been found.",
				first->GetName() );
			return false;
		}
		return faction->IsMember( second_name );
	}

	///
	/// Check whether the two players have some relationship between them.
	///
	bool CheckRelation( CPlayer *first, const std::string &second, long check_flag )
	{
		bool ret = false;
		if( check_flag & PR_FRIEND )
		{
			// when ret is false, it will then check the second expression.
			ret = ( ret || IsFriend( first, second.c_str() ) );
		}
		if( check_flag & PR_TEAM )
		{
			ret = ( ret || IsInSameTeam( first, second.c_str() ) );
		}
		if( check_flag & PR_FACTION )
		{
			ret = ( ret || IsInSameFaction( first, second.c_str() ) );
		}
		return ret;
	}
}
