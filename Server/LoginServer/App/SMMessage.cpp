#include "stdafx.h"

#include "../Net/Message.h"
#include "../../../Public/Common/ManageToolMsgType.h"

void OnSMMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_SM2GAPP_TESTCONN:
		{
			//SM发送的时间
			long lTime = pMsg->GetLong();
			CMessage msg(MSG_GAPP2SM_TESTCONN);
			long lReturnTime = timeGetTime();
			msg.Add(GetGame()->GetSetup()->dwGappID);
			msg.Add(lTime);
			msg.Add(lReturnTime);
			msg.SendToSM();
		}
		break;
	case MSG_SM2GAPP_SHUTDOWN:
		{
            AppFrame::Exit();
		}
		break;
	default:
		break;
	}
}