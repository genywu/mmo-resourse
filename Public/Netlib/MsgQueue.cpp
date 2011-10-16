#include "stdafx.h"
#include "MsgQueue.h"
#include "basemessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgQueue::CMsgQueue()
{
	InitializeCriticalSection(&m_CriticalSectionMsgQueue);
}

CMsgQueue::~CMsgQueue()
{
	Clear();
	DeleteCriticalSection(&m_CriticalSectionMsgQueue);
}

//压入一个消息
bool CMsgQueue::PushMessage(CBaseMessage* pMsg)
{
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	if( pMsg )
	{
		m_msgQueue.push_back(pMsg);
		LeaveCriticalSection(&m_CriticalSectionMsgQueue);
		return true;
	}

	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
	return false;
}

bool CMsgQueue::PushMsgsoFront(msgQueue& queMsgs)
{
	if( queMsgs.size() == 0)	return false;
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	m_msgQueue.insert(m_msgQueue.begin(),queMsgs.begin(),queMsgs.end());
	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
	return true;
}

//弹出一个消息
CBaseMessage* CMsgQueue::PopMessage()
{
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	if (m_msgQueue.empty())
	{
		LeaveCriticalSection(&m_CriticalSectionMsgQueue);
		return NULL;
	}

	CBaseMessage* pMsg = *m_msgQueue.begin();
	m_msgQueue.pop_front();
	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
	return pMsg;
}

void CMsgQueue::GetAllMessage(msgQueue& pTemptMsgQueue)
{
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	pTemptMsgQueue = m_msgQueue;
	m_msgQueue.clear();
	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
}

// 得到消息队列长度
long CMsgQueue::GetSize()
{
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	long lSize = (long)m_msgQueue.size();
	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
	return lSize;
}


//清空消息
void CMsgQueue::Clear()
{
	EnterCriticalSection(&m_CriticalSectionMsgQueue);
	for (msgQueue::iterator it = m_msgQueue.begin(); it!=m_msgQueue.end(); it++)
	{
		delete (*it);
	}
	m_msgQueue.clear();
	LeaveCriticalSection(&m_CriticalSectionMsgQueue);
}