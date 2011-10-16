#include "StdAfx.h"
#include "PlayerMan.h"
#include "../../Net/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应玩家管理消息
void OnPMMessage(CMessage* pMsg)
{
	assert(NULL != pMsg);

	switch(pMsg->GetType())
	{
	case MSG_C2S_PM_REQ:		// CLIENT请求查询玩家信息
		{
			GetInst(CPlayerMan).Query(pMsg);
		}
		break;

	case MSG_W2S_PM_ACK:		// WS应答查询玩家信息
		{
			pMsg->SetType(MSG_S2C_PM_ACK);
			CGUID ExID;
			pMsg->GetGUID(ExID);
			pMsg->SendToPlayer(ExID);
		}
		break;
	}
}