#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_MASTER_INFO_INCLUDED
#define _INC_MASTER_INFO_INCLUDED

struct tagMasterInfo
{
	//##主人的有关信息
	LONG	lMasterType;
	CGUID	MasterID;
	CGUID	MasterGuildID;
	CGUID	MasterTeamID;
	CGUID	MasterUnionID;
	LONG	lMasterCountryID;

	//##主人攻击玩家的授权信息
	BOOL	bPermittedToKillPlayer;
	BOOL	bPermittedToKillTeammate;
	BOOL	bPermittedToKillGuildMember;
	BOOL	bPermittedToKillCriminal;

	tagMasterInfo				();
	tagMasterInfo&	operator =	( tagMasterInfo& tIn );
	BOOL			operator ==	( tagMasterInfo& tIn );
};

#endif