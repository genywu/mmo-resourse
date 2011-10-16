#include "stdafx.h"
#include "..\nets\netlogin\message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应其他消息
void OnGMMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2L_GM_BANPLAYER:
		{
			char szCdkey[256];
			pMsg->GetStr(szCdkey,256);
			long lTime=pMsg->GetLong();
			LONG lEndTime = (0 == lTime) ? 0 : (time(NULL) + lTime);
			GetGame()->GetAccountClient().RejectAccount(szCdkey, lEndTime);
		}
		break;
	}
}
