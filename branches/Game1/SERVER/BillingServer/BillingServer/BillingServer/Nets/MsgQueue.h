#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
using namespace std;

class CBaseMessage;
typedef deque<CBaseMessage*> msgQueue;

/* -------------------------------------------
  [CMsgQueue] 消息队列

  功能:
	存储接收到的消息，等待处理。

---------------------------------------------*/
class CMsgQueue  
{
private:
	msgQueue m_msgQueue;	// 队列
	CRITICAL_SECTION m_CriticalSectionMsgQueue;

public:
	CMsgQueue();
	virtual ~CMsgQueue();

	bool PushMessage(CBaseMessage* pMsg);	// 压入消息
	CBaseMessage* PopMessage();				// 弹出消息
	void	GetAllMessage(msgQueue& pTemptMsgQueue);

	long GetSize();							// 得到消息队列长度
	void Clear();							// 清空消息
};

#endif