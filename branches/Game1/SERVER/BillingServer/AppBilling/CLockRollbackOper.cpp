#include "stdafx.h"
#include "CLockRollbackOper.h"
#include "../../../dbaccess/BillingDB/RsAccount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_nRollbackSend;

CLockRollbackOper::CLockRollbackOper(eBillOperType eType)
{
	m_eOperType=eType;
}

CLockRollbackOper::~CLockRollbackOper()
{

}

BOOL CLockRollbackOper::DoOpertaion()
{
	//BOOL bSucceed=GetGame()->GetRsAccount()->AccountUnlockRollBack(m_SqlConnection,m_OperGuid,m_strCDKey.c_str(),m_lLockNum);
	BOOL bSucceed=GetGame()->GetRsAccount()->RunConsumeFail(m_SqlConnection,m_OperGuid);
	if(bSucceed)
	{
		if(GetGame()->GetSetup()->bUseLogServer)
		{
			WriteLog();
		}
	}
	g_nRollbackSend++;
	return TRUE;

}

VOID CLockRollbackOper::SetLockNum(LONG lNum)
{
	if(lNum<0)
		lNum=0;
	m_lLockNum=lNum;
}

VOID CLockRollbackOper::WriteLog()
{
	string strTime;
	GetCurrTimeString(strTime);	
	
	//RollbackÈÕÖ¾	
	GetGame()->GetLogBSClient().SendLogToServer(512,strTime.c_str(),m_strCDKey.c_str(),
		&m_OperGuid,m_lLockNum);	
}

