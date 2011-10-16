#include "StdAfx.h"

#include "../Player.h"
#include "../AntiCheat/AntiCheatCommon.h"
#include "../AntiCheat/CConfiger.h"
#include "../AntiCheat/CPlayerManager.h"
#include "../AntiCheat/CQuestionManager.h"
#include "../../Server/Game.h"


void OnAntiCheatMsg(CMessage *msg)
{
	if( !GetInst( AntiCheat::CConfiger ).IsEnable() )
	{
		Log4c::Warn(ROOT_MODULE,"Received AntiCheat messages [%d] but the module is DISABLED.", msg->GetType() );
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
				Log4c::Warn(ROOT_MODULE, "Unknown message type [%d].", msg->GetType() );
			}
			break;
	}
}