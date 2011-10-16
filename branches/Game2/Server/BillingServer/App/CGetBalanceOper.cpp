#include "stdafx.h"
#include "CGetBalanceOper.h"

#include "../DB/RsAccount.h"
#include "../Net/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CGetBalanceOper::CGetBalanceOper(eBillOperType eType)
{
	SetOperType(eType);
	m_strCDKey="";
	m_lGsId=0;
}

CGetBalanceOper::~CGetBalanceOper()
{

}

BOOL CGetBalanceOper::DoOpertaion()
{
	DWORD dwBalance=GetGame()->GetRsAccount()->RunGetBalanceProc(m_SqlConnection,m_strCDKey.c_str());

	CMessage msg(MSG_B2S_GET_BALANCE);
	msg.Add(m_UserGuid);
	msg.Add(dwBalance);
	msg.SendToGS(m_lGsId);

	return TRUE;
}