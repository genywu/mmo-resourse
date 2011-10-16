/********************************************************************
	created:	2010/10/12
	created:	12:10:2010   15:35
	filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Relive\Relive.cpp
	file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Relive
	file base:	Relive
	file ext:	cpp
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	玩家复活逻辑功能
*********************************************************************/
#include "StdAfx.h"
#include "Message.h"
#include "../MsgBoxSys/MsgEventDef.h"
#include "../MsgBoxSys/MsgEventManager.h"

//请求复活
void  RelivePlayerSelf(MsgEvent& event,eMsgResult UserId)
{     
	if(UserId == S_OK)
	{
		CMessage msg(MSG_C2S_PLAYER_QUEST_RELIVE);
		msg.Send();
	}
}