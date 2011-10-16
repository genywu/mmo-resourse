#include "StdAfx.h"
#include "netgameserverclient.h"
#include "message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CNetGameserverClient::CNetGameserverClient(void)
{
}

CNetGameserverClient::~CNetGameserverClient(void)
{
}

void CNetGameserverClient::HandleReceive()
{
	// -----------------------------------------------------------
	// 具体游戏处理
	CMessage* pmsg = (CMessage*)CMessage::GetRecvQueue()->PopMessage();
	if( pmsg )
	{
	}
	// -----------------------------------------------------------

	return;
}

void CNetGameserverClient::HandleClose()
{
	// 服务器断线
	CMessage* msg;
	msg = new CMessage(CMessage::MSG_WORLD_SERVERLOST);
	CMessage::GetRecvQueue()->PushMessage( msg );
}

void CNetGameserverClient::HandleConnect()
{
}