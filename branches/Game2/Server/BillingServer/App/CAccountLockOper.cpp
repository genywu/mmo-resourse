#include "stdafx.h"
#include "CAccountLockOper.h"

#include "CSession.h"
#include "CSessionFactory.h"

#include "../DB/RsAccount.h"
#include "../Net/Message.h"
#include "../../../Public/Common/GoodsExDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_nLockSend;

CAccountLockOper::CAccountLockOper(eBillOperType eType)
{
	SetOperType(eType);
	m_strCDKey="";
	m_lGsId=0;
	m_lLockNum=0;
	
}

CAccountLockOper::~CAccountLockOper()
{
	m_vecItemData.clear();
}

VOID CAccountLockOper::SetLockNum(LONG lNum)
{
	if(lNum<0)
		lNum=0;
	m_lLockNum=lNum;
}

BOOL CAccountLockOper::DoOpertaion()
{
	CGUID operGuid;
	CGUID::CreateGUID(operGuid);
	//eAccountLockRetVal eRetVal=GetGame()->GetRsAccount()->AccountLock(m_SqlConnection,operGuid,m_strCDKey.c_str(),m_lLockNum);	
	eAccountLockRetVal eRetVal=GetGame()->GetRsAccount()->RunConsumeProc(m_SqlConnection,operGuid,m_strCDKey.c_str(),m_lLockNum);
	
	switch(eRetVal)
	{
	case ACCOUNT_LOCK_OK:
		{
			if(GetGame()->GetSetup()->bUseLogServer)
			{
				WriteLog(operGuid);
			}
		}
		break;
	case ACCOUNT_LOCK_ERROR_CDKEY:
		{
		}
		break;
	case ACCOUNT_LOCK_NOT_ENOUGH:
		{
		}
		break;
	case ACCOUNT_LOCK_UNKNOWN_ERROR:
		{
		}
		break;
	}
	//返回帐户锁定消息
	CMessage msg(MSG_B2S_ACCOUNT_LOCK);
	msg.Add((long)eRetVal);
	msg.Add(m_btConsumeType);
	msg.Add(operGuid);//oper guid
	msg.Add(m_UserGuid);
	msg.Add(m_AttachGuid);
	
	msg.Add(m_strCDKey.c_str());
	msg.Add((DWORD)m_vecItemData.size());
	for(uint i=0;i<m_vecItemData.size();i++)
	{
		msg.Add(m_vecItemData[i].dwItemId);
		msg.Add(m_vecItemData[i].dwItemNum);
	}	
	msg.Add(m_lLockNum);
	msg.SendToGS(m_lGsId);
	g_nLockSend++;
	return TRUE;
}

VOID CAccountLockOper::WriteLog(const CGUID& operGuid)
{
	string strTime;
	GetCurrTimeString(strTime);	
	int nConsumeType=0;
	if(m_vecItemData.size()>1)
		nConsumeType=1;
	//UNLOCK日志
	for(vector<tagItemData>::size_type i=0;i<m_vecItemData.size();i++)
	{
		//GetGame()->GetLogBSClient().SendLogToServer(510,strTime.c_str(),m_strCDKey.c_str(),nConsumeType,&m_UserGuid,
		//	&operGuid,m_vecItemData[i].dwItemId,m_vecItemData[i].dwItemNum,m_lLockNum);
	}	
}