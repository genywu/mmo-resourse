#include "stdafx.h"
#include "CAccountUnlockOper.h"

#include "CSession.h"
#include "CSessionFactory.h"

#include "../DB/RsAccount.h"
#include "../Net/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_nUnlockSend;

CAccountUnlockOper::CAccountUnlockOper(eBillOperType eType)
{
	SetOperType(eType);
	m_strCDKey="";
	m_strUserName="";
	m_lGsId=0;
	m_lUnlockNum=0;	
	m_dwGameId=1;
	m_dwWorldId=0;
	m_strGoodsName="";
}

CAccountUnlockOper::~CAccountUnlockOper()
{
	m_vecItemData.clear();
}

VOID CAccountUnlockOper::SetUnlockNum(LONG lNum)
{
	if(lNum<0)
		lNum=0;
	m_lUnlockNum=lNum;
}

BOOL CAccountUnlockOper::DoOpertaion()
{
	//GetGame()->GetRsAccount()->AccountUnlock(m_SqlConnection,m_OperGuid,m_strCDKey.c_str(),m_lUnlockNum);
	GetGame()->GetRsAccount()->RunConsumeSuccessProc(m_SqlConnection,m_OperGuid);

	
	CMessage msg(MSG_B2S_ACCOUNT_UPDATED);
	msg.Add(m_strCDKey.c_str());
	//msg.Add(m_BillSessionGuid);
	msg.SendToGS(m_lGsId);
	g_nUnlockSend++;
	if(GetGame()->GetSetup()->bUseLogServer)
	{
		WriteLog();
	}
		
	return TRUE;
	
}

VOID CAccountUnlockOper::WriteLog()
{
	string strTime;
	GetCurrTimeString(strTime);	
	int nConsumeType=0;
	if(m_vecItemData.size()>1)
		nConsumeType=1;
	//UNLOCK»’÷æ
	for(vector<tagItemData>::size_type i=0;i<m_vecItemData.size();i++)
	{
		//GetGame()->GetLogBSClient().SendLogToServer(511,strTime.c_str(),&m_UserGuid,m_strUserName.c_str(),m_strCDKey.c_str(),
		//	nConsumeType,&m_OperGuid,m_vecItemData[i].dwItemId,m_vecItemData[i].dwItemNum,m_lUnlockNum,m_dwWorldId,m_dwGameId,m_strGoodsName.c_str());
	}	
}