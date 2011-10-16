#include "StdAfx.h"
#include "../Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 响应STATE消息
void OnStateMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_STATE_END:
		{
		//	//结束状态
		//	DWORD dwStateID =  pMsg->GetDWord();
		//	CPlayer *pPlayer = pMsg->GetPlayer();

		//	map<DWORD, CState*> mapState = pPlayer->GetStatesList();
		//	
		//	for (map<DWORD, CState*>::iterator it=mapState.begin(); it!=mapState.end(); ++it)
		//	{
		//		CNewState *pNewState = dynamic_cast<CNewState*>(it->second);
	
		//		if(pNewState)
		//		{
		//			if (pNewState->GetID() == dwStateID)
		//			{
		//				pNewState->End();

		//				//pPlayer->RemoveState(pNewState);
		//			}
		//		}
		//	}
		}
		break;

	}
}