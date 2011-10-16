#include "StdAfx.h"
#include "GetBounsOper.h"

extern long g_lObtainBakNum;


CGetBounsOper::CGetBounsOper(eOperType eOt)
{
	SetOperType(eOt);
}

CGetBounsOper::~CGetBounsOper(void)
{
}

BOOL CGetBounsOper::DoOperator()
{
    long lRet = -1;
	HRESULT hRt = GetGame()->GetRsBouns()->GetBouns(m_cnConnPtr, m_guidBouns, m_guidPlayer, m_strPlayerName.c_str(), m_dwWSID, m_strPlayerIP.c_str(), lRet);
    if(hRt != S_OK)
        return FALSE;

	//领取操作，说明奖励已经派发成功给玩家了，这里只是一个数据状态操作确认
	if(lRet == 0)
	{
		//说明数据操作失败，记录日志
		char szLog[512];
		char szGuidPlayer[128],szGuidBouns[128];
		m_guidPlayer.tostring(szGuidPlayer);
		m_guidBouns.tostring(szGuidBouns);
		sprintf(szLog, "bounsguid[%s], playerguid[%s], PlayeName[%s],WSID[%d]",szGuidBouns, szGuidPlayer, m_strPlayerName.c_str(),m_dwWSID);
		PutoutLog("Bouns_log",LT_ERROR,szLog);
	}
	
	g_lObtainBakNum++;	
	RefeashInfoText();	

	return TRUE;
}
