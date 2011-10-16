#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\player.h"
#include "..\..\gameserver\game.h"
#include "..\AntiCheat\CConfiger.h"
#include "..\AntiCheat\CQuestionManager.h"
#include "..\AntiCheat\CPlayerManager.h"
#include "..\public\Singleton.h"
#include "..\AntiCheat\AntiCheatCommon.h"


void OnAntiCheatMsg(CMessage *msg)
{
	if( !GetInst( AntiCheat::CConfiger ).IsEnable() )
	{
		PutoutLog( LOG_FILE, LT_WARNING, "Received AntiCheat messages [%d] but the module is DISABLED.", 
			msg->GetType() );
		return;
	}

	switch( msg->GetType() )
	{
		case MSG_C2S_AC_ANSWER:
			{
				CPlayer * player = msg->GetPlayer();
				if( player == NULL )
				{
					AntiCheat::AddAccSysError();
					break;
				}
				BYTE    sel		 = msg->GetByte();
				GetInst(AntiCheat::CPlayerManager).PlayerAnswer( player, sel );
			}
			break;				

		case MSG_W2S_AC_UPDATEQUESTIONS:
			{
				DBReadSet db;
				msg->GetDBReadSet( db );
				GetInst( AntiCheat::CQuestionManager ).UpdateQuestions( db );
			}
			break;

		default:
			{
				// some error occures
				PutoutLog( LOG_FILE, LT_WARNING, "Unknown message type [%d].", 
					msg->GetType() );
			}
			break;
	}
}