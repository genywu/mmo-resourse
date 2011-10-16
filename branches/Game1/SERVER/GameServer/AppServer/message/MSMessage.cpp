///
/// @file MSMessage.cpp
/// @author Kevin Lynx
/// @date 9.9.2008
/// @brief To process the messages from MonsterServer and WorldServer(about MS)
///
#include "stdafx.h"
//#include "../Nets/NetServer/Message.h"
//#include "../Public/Message_defs/MS_MessageDefs.h"
//#include "GameServer/MSDisResponse.h"
//#include "GameServer/MSHandler.h"
//
//static void _OnGetMSInfo( CMessage *pMsg )
//{
//	long msID = pMsg->GetLong();
//	CMSHandler::getSingleton().AddMS( msID, pMsg->GetSocketID() );
//	AddLogText( "A monster sever [%d] connected.", msID );
//}
//
/////
///// The message handler entry, will be called in CMessage::Run
/////
//void OnMSMessage( CMessage *pMsg )
//{
//	switch( pMsg->GetType() )
//	{
//	case MSG_W2G_MONSTER_RESPONSE_DISPATCH:
//		HandleDisResponse( pMsg );
//		break;
//
//	case MSG_M2G_SENDINFO:
//		_OnGetMSInfo( pMsg );
//		break;
//	}
//} 