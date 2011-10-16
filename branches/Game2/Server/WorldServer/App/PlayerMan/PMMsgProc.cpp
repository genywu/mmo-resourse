#include "StdAfx.h"
#include "PlayerMan.h"
#include "../../Server/Game.h"
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
	case MSG_S2W_PM_CHG:		// GS请求更新玩家信息
		{
			CGUID ExID;
			pMsg->GetGUID(ExID);
			CPlayer *pPlayer = GetGame().GetMapPlayer(ExID);
			long lType = pMsg->GetLong();
			long lNewVal = pMsg->GetLong();
			if( NULL == pPlayer )
			{
				//char szLog[512];
				char szGUID[128];
				ExID.tostring(szGUID);
				//sprintf(szLog, "请求刷新TYPE:0X%X为%ld的的玩家(GUID为%s)不存在.",
				//	lType,
				//	lNewVal,
				//	szGUID);
				//PutoutLog("PM", LT_ERROR, szLog);

                Log4c::Error(ROOT_MODULE,"%-15s 请求刷新TYPE:0X%X为%ld的的玩家(GUID为%s)不存在.",__FUNCTION__,lType,lNewVal,szGUID);
				break;
			}

			GetInst(CPlayerMan).RefreshElem(lType, pPlayer, lNewVal);
		}
		break;

	case MSG_S2W_PM_REQ:		// GS请求查询玩家信息
		{
			GetInst(CPlayerMan).Query(pMsg);
		}
		break;

	}
}