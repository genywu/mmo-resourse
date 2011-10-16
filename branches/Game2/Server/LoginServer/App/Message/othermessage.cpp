#include "stdafx.h"

#include "../../Net/Message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应其他消息
void OnOtherMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case 0:
		{

		}
		break;
	}
}
