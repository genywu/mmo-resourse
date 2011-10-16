
#include "stdafx.h"
#include "AinMessage.h"
//#include <MMSystem.h>

extern void OnAinMessage(AinMessage* pAimMsg);

AinMessage* AinMessage::Create(LONG lType)
{
	AinMessage* pRe = (AinMessage*)M_ALLOC(sizeof(AinMessage));
	if(NULL != pRe)
		new(pRe)AinMessage(lType);
	return pRe;

}
VOID AinMessage::Release(AinMessage **ppAinMsg)
{
	if(NULL != ppAinMsg)
		M_FREE(*ppAinMsg, sizeof(AinMessage));
}

VOID AinMessage::AddByte(BYTE cData)
{
	m_vData.push_back(cData);
}
VOID AinMessage::AddWord(WORD wData)
{
	size_t lSize = m_vData.size();
	m_vData.resize(lSize + sizeof(WORD));
	memcpy(&m_vData[lSize], &wData, sizeof(WORD));
}
VOID AinMessage::AddDword(DWORD dwData)
{
	size_t lSize = m_vData.size();
	m_vData.resize(lSize + sizeof(DWORD));
	memcpy(&m_vData[lSize], &dwData, sizeof(DWORD));
}
VOID AinMessage::AddStr(LPCSTR pStr)
{
	if(NULL == pStr)
	{
		assert(false);
		return;
	}
	WORD wStrLen = (WORD)strlen(pStr);
	AddWord(wStrLen);
	AddEx(pStr, wStrLen);		
}
VOID AinMessage::AddEx(LPCVOID pData, DWORD dwAddSize)
{
	size_t lSize = m_vData.size();
	m_vData.resize(lSize + dwAddSize);
	memcpy(&m_vData[lSize], pData, dwAddSize);
}

BYTE AinMessage::GetByte(VOID)
{
	BYTE cRe = 0;
	size_t lSize = m_vData.size();
	if(m_lPos + sizeof(BYTE) <= lSize)
	{
		cRe = m_vData[m_lPos];
		m_lPos += sizeof(BYTE);
	}
	else
		assert(false);
	return cRe;
}
WORD AinMessage::GetWord(VOID)
{
	WORD wRe = 0;
	size_t lSize = m_vData.size();
	if(m_lPos + sizeof(WORD) <= lSize)
	{
		wRe = *(WORD*)(&m_vData[m_lPos]);
		m_lPos += sizeof(WORD);
	}
	else
		assert(false);
	return wRe;
}
DWORD AinMessage::GetDword(VOID)
{
	DWORD dwRe = 0;
	size_t lSize = m_vData.size();
	if(m_lPos + sizeof(DWORD) <= lSize)
	{
		dwRe = *(DWORD*)(&m_vData[m_lPos]);
		m_lPos += sizeof(DWORD);
	}
	else
		assert(false);
	return dwRe;
}
BOOL AinMessage::GetStr(LPSTR pStr, DWORD lBufMaxSize)
{
	BOOL bRe = FALSE;
	WORD lStrLen = GetWord();
	WORD lLoadLen = min(lStrLen, lBufMaxSize - 1);
	
	size_t lSize = m_vData.size();
	if(m_lPos + lStrLen <= lSize)
	{
		memcpy(pStr, &(m_vData[m_lPos]), lLoadLen);
		m_lPos += lStrLen;
		bRe = TRUE;
	}
	else
		assert(false);
	return bRe;
}
BOOL AinMessage::GetEx(LPVOID pData, DWORD lGetSize)
{
	BOOL bRe = FALSE;
	size_t lSize = m_vData.size();
	if(m_lPos + lGetSize <= lSize)
	{
		memcpy(pData, &(m_vData[m_lPos]), lGetSize);
		m_lPos += lGetSize;
		bRe = TRUE;
	}
	else
		assert(false);
	return bRe;
}

VOID AinMessage::Run(VOID)
{
	OnAinMessage(this);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AinMsgQueue::AinMsgQueue(VOID)
:m_dwMinBeginTime(0xFFFFFFFF)
{
}
AinMsgQueue::~AinMsgQueue(VOID)
{
	
}
VOID AinMsgQueue::Release(VOID)
{
	//! 执行最后队列的消息
	list<AinMessage*> listMsg;
	PopMessage(listMsg);
	for(list<AinMessage*>::iterator ite = listMsg.begin(); listMsg.end() != ite; ite++)
	{
		if( NULL != *ite )
		{
			(*ite)->Run();
			AinMessage::Release(&(*ite));
		}
	}

	//! 定时为到的消息不执行了
	map<DWORD, AinMessage*>::iterator ite = m_timeMsgQueue.begin();
	for (; m_timeMsgQueue.end() != ite; ++ite)
	{
		AinMessage::Release(&(ite->second));
	}
}
//! 得到消息数目
LONG AinMsgQueue::GetSize(VOID)
{
	LONG lRe = 0;
	m_Lock.Lock();
	{
		lRe = (LONG)m_msgQueue.size() + (LONG)m_timeMsgQueue.size();
	}
	m_Lock.UnLock();
	return lRe;
}
//! 压入消息
BOOL AinMsgQueue::PushMessage(AinMessage* pMsg)
{
	if(NULL == pMsg) return FALSE;
	m_Lock.Lock();
	{
		m_msgQueue.push_back(pMsg);
	}
	m_Lock.UnLock();
	return TRUE;
}

//! 压入一个定时消息
BOOL AinMsgQueue::PushTimeMessage(AinMessage* pMsg, DWORD dwMillisecond)
{
	if(NULL == pMsg) return FALSE;
	if(0 == dwMillisecond)
		return PushMessage(pMsg);

	DWORD dwCurrTime = timeGetTime();
	DWORD dwBeginTime = dwCurrTime + dwMillisecond;

	m_Lock.Lock();
	{
		do 
		{
			if(m_timeMsgQueue.end() != m_timeMsgQueue.find(dwBeginTime))
				++dwBeginTime;
			else
				break;
		} while(true);

		m_timeMsgQueue[dwBeginTime] = pMsg;
		m_dwMinBeginTime = min(m_dwMinBeginTime, dwBeginTime);
	}
	m_Lock.UnLock();
	return TRUE;
}

//! 弹出消息
VOID	AinMsgQueue::PopMessage(list<AinMessage*> &listMsg)
{
	//! 普通消息
	m_Lock.Lock();
	{
		listMsg = m_msgQueue;
		m_msgQueue.clear();
	}
	m_Lock.UnLock();

	//! 定时消息
	DWORD dwCurrTime = timeGetTime();
	if(m_dwMinBeginTime <= dwCurrTime)
	{
		m_Lock.Lock();
		{
			while (0 != m_timeMsgQueue.size() && dwCurrTime < m_timeMsgQueue.begin()->first)
			{
				listMsg.push_back(m_timeMsgQueue.begin()->second);
				m_timeMsgQueue.erase(m_timeMsgQueue.begin());
			}
			
			if(0 == m_timeMsgQueue.size())
				m_dwMinBeginTime = 0xFFFFFFFF;
			else
				m_dwMinBeginTime = m_timeMsgQueue.begin()->first;
		}
		m_Lock.UnLock();
	}
}
//! 清空消息（不释放内存）
VOID AinMsgQueue::Clear(VOID)
{
	m_Lock.Lock();
	m_msgQueue.clear();
	m_Lock.UnLock();
}