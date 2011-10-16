///
/// @file MS_MessageDefs.h
/// @author Kevin Lynx
/// @date 9.3.2008
/// @brief To define some messages communicated between these servers about MonsterServer.
///
#ifndef ___MS_MESSAGE_DEFS_H_
#define ___MS_MESSAGE_DEFS_H_

#include "../MsgType.h"

///
/// Message valus between MonsterServer and WorldServer, GameServer and MonsterServer, etc.
///
enum MS_MessageDefs
{
	/************ WS -> MS ***************************************************************/
	MSG_W2M_NORMAL = MSG_W2M_BEGIN + MAX_SUB * 0,

	MSG_W2M_SENDSETUP, ///< send setup (monsterlist.ini etc)
	MSG_W2M_ADDGS,	///< add a GS

	/************ MS -> WS ****************************************************************/
	MSG_M2W_NORMAL = MSG_M2W_BEGIN + MAX_SUB * 0,
	MSG_M2W_SENDINFO, ///< send monster info to the world server.

	/************ GS -> WS ****************************************************************/
	MSG_G2W_REQUEST = MSG_G2W_BEGIN + MAX_SUB * 0,
	MSG_G2W_MONSTER_REQUEST_DISPATCH, ///< request WS to dispath MS to GS.
	MSG_G2W_LISTEN_ADDRESS,	///< tell the address GS listen on.MS will connect to this address.

	/************ WS -> GS ****************************************************************/
	MSG_W2G_RESPONSE = MSG_W2G_BEGIN + MAX_SUB * 0,
	MSG_W2G_MONSTER_RESPONSE_DISPATCH, ///< response to GS

	/************ GS -> MS ****************************************************************/
	MSG_G2M_NORMAL = MSG_G2M_BEGIN + MAX_SUB * 0,
	MSG_G2M_MONSTER_ADD, ///< add a monster
	MSG_G2M_MONSTER_ADD_DYNAMIC, ///< dynamic add a monster, can add a duplicate monster
	MSG_G2M_MONSTER_ADD_GROUP, ///< add a group of monsters
	MSG_G2M_MONSTER_REMOVE, ///< remove a monster
	MSG_G2M_RESPONSE_REGION, ///< response a region
	MSG_G2M_UPDATE_SHAPE_ATTR, ///< update the attribute of a shape.

	/************ MS -> GS ****************************************************************/
	MSG_M2G_NORMAL = MSG_M2G_BEGIN + MAX_SUB * 0,
	MSG_M2G_RESPONSE_MONSTER_ADD, ///< the response 
	MSG_M2G_SENDINFO,
	MSG_M2G_REQUEST_REGION, ///< request a region
	MSG_M2G_UPDATE_SHAPE_ATTR, ///< update the attribute of a shape.
};

///
/// resource type.
///
enum eSetupType
{
	ST_MONSTERLIST = 0x00000001,
	ST_REGIONLIST  = 0x00000002,
	ST_GS_LIST     = 0x00000004,
};

///
/// Dispatch type, used to identify the type that the GS request WS to dispatch MS.
///
enum eRequestDispathType
{
	RT_CONFORM_MS,
	RT_CREATE_MONSTER,
	//RT_CREATE_REGION_MONSTER
};

#endif // ___MS_MESSAGE_DEFS_H_