#include "stdafx.h"
#include "CLotterySystem.h"
#include "..\nets\networld\message.h"
CLotterySystem g_LotterySystem;

CLotterySystem::CLotterySystem()
{
   m_lWorldCredit = 0;
   m_lCreditHighest = 0;
   m_strCreditHighestName = "";
   m_dwLastRefreshTime = 0;
   m_bSendWorldCredit = false;
}

CLotterySystem::~CLotterySystem()
{
}

//! 得到全局唯一实例
CLotterySystem& CLotterySystem::GetInstance(void)
{
	return g_LotterySystem;
}

void CLotterySystem::Run()
{
	if(m_bSendWorldCredit)
	{
		DWORD dwCurTime = timeGetTime();
		if((dwCurTime-m_dwLastRefreshTime)>=GetInst(CLotteryXml).GetWorldCreditRefreshTime())
		{
			//m_dwLastRefreshTime = dwCurTime;
			//发送刷新消息
			CMessage msg(MSG_W2S_LOTTERY_SENDWORLDCREDIT);
			msg.Add((long)m_lWorldCredit);
			msg.SendAll();
			m_bSendWorldCredit = false;
		}
	}
}

void CLotterySystem::SendCreditHighest()
{
	CMessage msg(MSG_W2S_LOTTREY_SENDCreditHighest);
	msg.Add((long)m_lCreditHighest);
	msg.Add((char*)m_strCreditHighestName.c_str());
	msg.SendAll();
}

void CLotterySystem::SetIsSendWorldCredit(bool b)
{
	if(!m_bSendWorldCredit)
	{
		m_bSendWorldCredit = b;
		m_dwLastRefreshTime = timeGetTime();
	}
}