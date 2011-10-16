//=============================================================================
/**
 *  file: TimerQueue.cpp
 *
 *  Brief:通过时间轮原理，实现定时器功能。
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-5-15
 */
//=============================================================================
#include "StdAfx.h"
#ifndef __TIMER_QUEUE_CPP__
#define __TIMER_QUEUE_CPP__
#pragma once
#include "TimerQueue.h"
#include "GameEvent.h"

//TimerQueue实现说明:

//每一个定时器都是通过TimerNode来表示
//时间轮被分成一定数量的"spokes",在时间轮中总共有m_nSpokeCount个"spokes",
//每一个定时器被散列到相应的spokes中,按照触发的时间增长排序
//在每一个"spoke"中所有的定时器相互之间通过双向链表连接,
//每一个"spoke"的第一个节点作为"特殊节点",用来标示链表的结束.其中的timerid用于计数。

//被调度的定时器的ID,是通过spoke的位置和被插入到链表当中的数量值组成的。
//32bit的m_dwTimerID，其中N位用于确定spoke,M位作为计数值。当一个新的节点插入到
//spoke中后，"特殊节点中"的timerid域加1,当这个计数域产生溢出后,该"spoke"必须搜索
//所有节点,以便于产生一个不重复的timerid,为了避免耗时的搜索,将保存额外的数据在"特殊节点"
//里.

///////////////////////////////////////////////////////////////////////////
// TimerNode

template <class TYPE>
TimerNode<TYPE>::TimerNode (void)
{  
}

template <class TYPE>
TimerNode<TYPE>::~TimerNode (void)
{
}

template <class TYPE> inline
void TimerNode<TYPE>::Set(const TYPE &type,
						  const	void* arg,
						  unsigned long 	dwTimeValue,
						  unsigned long 	dwInterval,
						  unsigned long 	dwTimerID,
						  TimerNode<TYPE> *p,
						  TimerNode<TYPE> *n)
{
	m_Type = type;
	m_pArg = arg;
	m_dwTimeValue=dwTimeValue;
	m_dwInterval=dwInterval;
	m_dwTimerID=dwTimerID;
	m_pRev=p;
	m_pNext=n;
}

template <class TYPE> inline
TYPE &TimerNode<TYPE>::GetType(void)
{
	return m_Type;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetType(TYPE &type)
{
	m_Type = type;
}

template <class TYPE> inline
const void *TimerNode<TYPE>::GetArg(void)
{
	return m_pArg;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetArg(void *arg)
{
	m_pArg = arg;
}

template <class TYPE> inline
unsigned long TimerNode<TYPE>::GetTimerValue(void) const
{
	return m_dwTimeValue;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetTimerValue(unsigned long timervalue)
{
	m_dwTimeValue=timervalue;
}

template <class TYPE> inline
unsigned long TimerNode<TYPE>::GetInterval(void) const
{
	return m_dwInterval;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetInterval(unsigned long interval)
{
	m_dwInterval=interval;
}

template <class TYPE> inline
TimerNode<TYPE> *TimerNode<TYPE>::GetPrev(void)
{
	return m_pRev;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetPrev(TimerNode<TYPE> *prev)
{
	m_pRev = prev;
}

template <class TYPE> inline
TimerNode<TYPE> *TimerNode<TYPE>::GetNext(void)
{
	return m_pNext;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetNext(TimerNode<TYPE> *next)
{
	m_pNext = next;
}

template <class TYPE> inline
long TimerNode<TYPE>::GetTimerID(void) const
{
	return m_dwTimerID;
}

template <class TYPE> inline
void TimerNode<TYPE>::SetTimerID(long timerid)
{
	m_dwTimerID= timerid;
}

template <class TYPE> inline
void TimerNode<TYPE>::OnTimerOut(unsigned long curtime)
{
	m_Type->OnTimeOut(m_dwTimerID,curtime,m_pArg);
}
//取消定时器时触发这个函数
template <class TYPE> inline
void TimerNode<TYPE>::OnTimerCancel()
{
	m_Type->OnTimerCancel(m_dwTimerID,m_pArg);
}

template <class TYPE>
void TimerNode<TYPE>::OutpuInfo(void) const
{
	//OUTPUTLOG("NodeStart");
	char str[64];
	//sprintf(str,"m_dwTimerID = %d",m_dwTimerID);
	//OUTPUTLOG(str);
	_snprintf(str, 64, "m_dwTimeValue = %d",m_dwTimeValue);
	OUTPUTLOG(str);
	//sprintf(str,"m_dwInterval = %d",m_dwInterval);
	//OUTPUTLOG(str);
	//sprintf(str,"m_pArg = %d",(int)m_pArg);
	//OUTPUTLOG(str);
	//OUTPUTLOG("NodeEnd");
}

///////////////////////////////////////////////////////////////////////////
// CTimerQueueIterator

template <class TYPE>
CTimerQueueIterator<TYPE>::CTimerQueueIterator(CTimerQueue<TYPE> &queue)
:m_TimerQueue(queue)
{
	First();
}

template <class TYPE>
CTimerQueueIterator<TYPE>::~CTimerQueueIterator(void)
{
}

//定位于第一个spoke的第一个节点
template <class TYPE>
void CTimerQueueIterator<TYPE>::First (void)
{
	GoToNext(0);
}

//指向当前节点的下一个节点
template <class TYPE>
void CTimerQueueIterator<TYPE>::Next (void)
{
	if (IsDone())
		return;
	TimerNode<TYPE>* n = m_pCurNode->GetNext();
	TimerNode<TYPE>* root = m_TimerQueue.m_pSpokes[m_nSpoke];
	if (n == root)
		GoToNext(m_nSpoke + 1);
	else
		m_pCurNode = n;
}

//当在遍历序列中再也没有节点时,返回true.
template <class TYPE>
int CTimerQueueIterator<TYPE>::IsDone (void) const
{
	return m_pCurNode == 0;
}

template <class TYPE>
TimerNode<TYPE>* CTimerQueueIterator<TYPE>::Item (void)
{
	return m_pCurNode;
}

//函数First()和Next()的公共使用部分
template <class TYPE>
void CTimerQueueIterator<TYPE>::GoToNext(unsigned long nSartSpoke)
{
	//查找第一个非空的条目
	unsigned long sc = m_TimerQueue.m_nSpokeCount;
	for (unsigned long  i = nSartSpoke; i < sc; ++i)
	{
		TimerNode<TYPE>* root = m_TimerQueue.m_pSpokes[i];
		TimerNode<TYPE>* n = root->GetNext ();
		if (n != root)
		{
			m_nSpoke= i;
			m_pCurNode = n;
			return;
		}
	}
	//如果是空
	m_nSpoke = sc;
	m_pCurNode = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//CTimerQueue
/**
* 构造函数,设置wheel大小、spoke数量、预分配一些节点。
*
* spokecount	定时轮列表的数量
* resolution:   被hansh函数使用时间分辨率,用毫秒表示。
* prealloc:     预分配节点的数量
*
*/
template <class TYPE>
CTimerQueue<TYPE>::CTimerQueue(	unsigned long spokecount,
							   unsigned long resolution,
							   unsigned long prealloc)
							   :m_pSpokes(0)
							   ,m_nSpokeCount(0) //在OpenI中计算
							   ,m_nSpokeBits(0)
							   ,m_nResBits(0)
							   ,m_nEarliestSpoke(0)
							   ,pIterator(0)
							   ,m_nTimerCount(0)
{
	OpenI(prealloc, spokecount, resolution);
}

/**
* 析构函数,用来释放相应的内存.
*/
template <class TYPE>
CTimerQueue<TYPE>::~CTimerQueue(void)
{
	delete pIterator;
	unsigned long i = 0;
	for (; i < this->m_nSpokeCount; ++i)
	{
		//从根节点开始，释放所有节点
		TimerNode<TYPE>* root = m_pSpokes[i];
		for (TimerNode<TYPE>* n = root->GetNext (); n != root;)
		{
			TimerNode<TYPE>* next = n->GetNext ();
			FreeNode(n);
			n = next;
		}
		delete root;
	}
	delete[] m_pSpokes;

	list<TimerNode<TYPE>*>::iterator it = m_FreeList.begin();
	for(;it != m_FreeList.end();it++)
	{
		TimerNode<TYPE>* pNode = (*it);
		if(pNode)
			delete pNode;
	}
	m_FreeList.clear();
}

/**
* 得到一个2次幂值
*
* n			要得到幂的数值
* minbits	允许的最小位数
* maxbits   允许的最大位数
*
*/
template <class TYPE>
int CTimerQueue<TYPE>::Power2Bits (int n, int minbits, int maxbits)
{
	int max = (1 << maxbits) - 1;
	if (n > max)
		return maxbits;

	//计算N的位数
	int i = 0;
	int tmp = n;
	do
	{
		tmp >>= 1;
		++i;
	}
	while (tmp != 0);

	if (i <= minbits)
		return minbits;

	//选出最靠近的那个
	int a = (1 << i) - n;
	int b = (1 << (i - 1)) - n;
	if (b < 0)
		b = -b;
	if (b < a)
		return i - 1;
	return i;
}


// 分配一个节点，先从预分配对列中查找。如果没有立即new
template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::AllocNode()
{
	if(m_FreeList.size() == 0)
		return new TimerNode<TYPE>;
	TimerNode<TYPE>* pNode = m_FreeList.front();
	m_FreeList.pop_front();
	return pNode;
}

template <class TYPE>
void CTimerQueue<TYPE>::FreeNode(TimerNode<TYPE>* pNode)
{
	if(m_FreeList.size() <= 10240)
		m_FreeList.push_back(pNode);
	else
		delete pNode;
}


// 使用已经确定的信息，来初始化定时器对列。

template <class TYPE>
void CTimerQueue<TYPE>::OpenI(size_t prealloc, unsigned long spokes, unsigned long res)
{
	//设置spokes的个数为2的N次幂-1;(例如:1,3,7,15,..)
	const int MIN_SPOKE_BITS = 3;  // 允许 8 到 4096 个spokes
	const int MAX_SPOKE_BITS = 12;
	const int MAX_RES_BITS = 20; 

	m_nSpokeBits = Power2Bits(spokes, MIN_SPOKE_BITS, MAX_SPOKE_BITS);
	m_nResBits = Power2Bits(res, 1, MAX_RES_BITS);

	m_nSpokeCount = 1 << m_nSpokeBits;

	unsigned long i = 0;
	for(;i < prealloc + m_nSpokeCount;i ++)
	{
		TimerNode<TYPE> *pNode = new TimerNode<TYPE>;
		m_FreeList.push_back(pNode);
	}

	m_nWheelTime = (1 << (m_nResBits + m_nSpokeBits));
	m_pSpokes = new TimerNode<TYPE>*[m_nSpokeCount];
	//创建根节点，这个节点用作特殊使用。
	for (i = 0; i < m_nSpokeCount; ++i)
	{
		TimerNode<TYPE>* root = AllocNode();
		root->Set(0, 0, 0, 0, 0,root, root);
		m_pSpokes[i] = root;
	}

	pIterator = new CTimerQueueIterator<TYPE>(*this);
}

//根据timerid,在某个指定的spoke里搜索,并返回找到的节点.
template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::FindSpokeNode(unsigned long spoke, long timerid) const
{
	if(spoke >= m_nSpokeCount)
	{
		char strError[256]="";
		_snprintf(strError,256,"Timer Wheel Varialbe Overflow(spoke:%d,m_nSpokeCount:%d).",spoke,m_nSpokeCount);
		PutDebugString(strError);
		return 0;
	}

	TimerNode<TYPE>* root = m_pSpokes[spoke];
	for (TimerNode<TYPE>* n = root->GetNext();
		n != root;
		n = n->GetNext())
	{
		if (n->GetTimerID() == timerid)
			return n;
	}
	return 0;
}
//根据timerid,搜索所有的spokes,寻找相应的节点。
template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::FindNode(long timerid) const
{
	if (timerid == -1)
		return 0;
	// 先搜寻原始调度的timerid里包含的spoke
	unsigned long spokemask = m_nSpokeCount - 1;
	unsigned long start = timerid & spokemask;
	TimerNode<TYPE>* n = FindSpokeNode (start, timerid);
	if (n != 0)
		return n;
	// 搜寻剩余的spokes
	for (unsigned long i = 0; i < m_nSpokeCount; ++i)
	{
		if (i != start)
		{
			n = FindSpokeNode(i, timerid);
			if (n != 0)
				return n;
		}
	}
	return 0;
}

//检查wheel是否为空
template <class TYPE>
int CTimerQueue<TYPE>::IsEmpty(void) const
{
	return m_nTimerCount == 0;
}

//得到第一个最近的时间值
template <class TYPE>
unsigned long CTimerQueue<TYPE>::EarliestTime(void) const
{
	TimerNode<TYPE>* n = GetFirstI();
	if (n != 0)
		return n->GetTimerValue();
	return 0;
}

//基于所需要的expire值，用了一个简单的hash算法来求出相应的spoke
template <class TYPE>
unsigned long CTimerQueue<TYPE>::CalculateSpoke(unsigned long expire) const
{
	return static_cast<unsigned long> ((expire >> m_nResBits) & (m_nSpokeCount- 1));
}

//根据所给的spoke,产生一个独一无二的timerid,如果根节点的计数器溢出,
//这个时候就要遍历所有的节点，以得到一个没有使用的timerid.
template <class TYPE>
long CTimerQueue<TYPE>::GenerateTimerID(unsigned long spoke)
{
	int cntbits = sizeof (long) * 8 - m_nSpokeBits;
	long maxcnt = ((long)1 << cntbits) - 1;
	if (spoke == m_nSpokeCount)
		--maxcnt;

	TimerNode<TYPE>* root = m_pSpokes[spoke];
	if (root == root->GetNext ())
		root->SetArg(0);
	//使用这个字段来跟踪下一个计算值
	long nextcnt = reinterpret_cast<long> (root->GetArg());
	//这个字段用于计数，而不是timerid.
	long cnt = root->GetTimerID();

	if (cnt >= maxcnt && root == root->GetNext())
	{
		OUTPUTLOG("In a space queue the counter overflow .");
		//如果在一个空的spoke上溢出的时候
		root->SetTimerID(1);
		return spoke;
	}
	else if (cnt >= maxcnt)
	{ //溢出，从0开始寻找
		cnt = 0; 
	}
	else if (nextcnt == 0 || cnt < nextcnt)
	{
		root->SetTimerID(cnt + 1);
		return (cnt << m_nSpokeBits) | spoke;
	}

	OUTPUTLOG("remove counter, not alloc TimerID.");
	//如果溢出了。寻找整个队列，找出一个未使用的timerid
	for (; cnt < maxcnt - 1; ++cnt)
	{
		long id = (cnt << m_nSpokeBits) | spoke;
		TimerNode<TYPE>* n = FindSpokeNode(spoke, id);
		if (n == 0)
		{
			root->SetTimerID(cnt + 1);
			nextcnt = 0;
			n=root->GetNext();
			for (; n != root; n = n->GetNext())
			{
				long tmp = n->GetTimerID() >> m_nSpokeBits;
				if (tmp > cnt && (tmp < nextcnt || nextcnt == 0))
					nextcnt = tmp;
			}
			root->SetArg (reinterpret_cast<void*> (nextcnt));
			return id;
		}
	}
	OUTPUTLOG("queue full,not alloc TimerID.");
	//该spoke全满,产生ID失败.
	return -1;
}


/**
*根据传入的参数，产生一个TimerNode节点，并产生一个timerid
*然后返回一个timerid
*
* type			超时调用的实际对象
* arg			用户定义的数据,用作超时回调处理
* futuretime	定时器预订的超时时间(毫秒),用绝对时间表示
* interval		表示是否是一个周期性的定时器,，如果等于0,表示只调度一次.
*				如果大于0,将要按照这个值进行周期性调度
*/
template <class TYPE>
long CTimerQueue<TYPE>::Schedule(const TYPE& type,
								 const void* arg,
								 unsigned long futuretime,
								 unsigned long interval)
{
	TimerNode<TYPE>* n = AllocNode ();
	if (n != 0)
	{
		unsigned long spoke = CalculateSpoke(futuretime);
		long id = GenerateTimerID(spoke);

		if (id != -1)
		{
			n->Set(type, arg, futuretime, interval,id, 0, 0);
			this->ScheduleI(n, spoke, futuretime);
		}
		return id;
	}
	OUTPUTLOG("Schedule() failed,not enough memory.");
	return -1;
}

/**
*重新预订一个记时器,找出相应的spoke,并插入到正确的位置
*
* n		被Reschedule的节点
*/
template <class TYPE>
void CTimerQueue<TYPE>::Reschedule(TimerNode<TYPE> *n)
{
	unsigned long expire = n->GetTimerValue ();
	unsigned long spoke = CalculateSpoke (expire);
	ScheduleI(n, spoke, expire);
}

//提供给 Schedule()和Reschedule()使用的公用函数
template <class TYPE>
void CTimerQueue<TYPE>::ScheduleI(TimerNode<TYPE>* n, unsigned long spoke, unsigned long expire)
{
	//检查是否更新最早的时间
	if (IsEmpty() || expire < EarliestTime ())
		m_nEarliestSpoke = spoke;

	TimerNode<TYPE>* root = m_pSpokes[spoke];
	TimerNode<TYPE>* last = root->GetPrev();

	++m_nTimerCount;

	//如果这个spoke是空的
	if (last == root) {
		n->SetPrev(root);
		n->SetNext(root);
		root->SetPrev(n);
		root->SetNext(n);
		return;
	}
	//比较节点时间，按序插入到相应的位置
	TimerNode<TYPE>* p = root->GetPrev ();
	while (p != root && p->GetTimerValue () > expire)
		p = p->GetPrev ();

	n->SetPrev(p);
	n->SetNext(p->GetNext ());
	p->GetNext()->SetPrev (n);
	p->SetNext(n);
}

/**
* 根据ID查找出相应的节点指针，使用SetInterval()函数重新设置定时器的间隔值
* timerid	定时器的唯一标示
* interval	新的定时器间隔值
*
* 返回1表示成功,-1表示失败。.
*/
template <class TYPE>
int CTimerQueue<TYPE>::ResetInterval(long timerid,unsigned long interval)
{
	TimerNode<TYPE>* n = FindNode (timerid);
	if (n != 0)
	{
		n->SetInterval(interval);
		return 0;
	}
	return -1;
}

/**
*
*遍历时间轮中所有的节点,匹配相应的type值，如果相等则移出。如果移出了最近的定时器
*将重新计算最早触发的定时器
*
* type	表示搜寻的值
*
* 返回取消的定时器个数
*/
template <class TYPE>
int CTimerQueue<TYPE>::Cancel(const TYPE& type)
{
	int numcanceled = 0;

	if(!IsEmpty())
	{
		TimerNode<TYPE>* first = GetFirst();
		unsigned long last = first->GetTimerValue ();
		int recalc = 0;
		
		//遍历所有的spokes
		unsigned long i = 0;
		for (; i < this->m_nSpokeCount; ++i)
		{
			TimerNode<TYPE>* root = m_pSpokes[i];
			for (TimerNode<TYPE>* n = root->GetNext (); n != root; )
			{
				if (n->GetType() == type)
				{
					++numcanceled;
					if (n == first)
						recalc = 1;

					TimerNode<TYPE>* tmp = n;
					n = n->GetNext ();

					//触发取消操作
					tmp->OnTimerCancel();

					CancelI(tmp);
				}
				else
				{
					n = n->GetNext();
				}
			}
		}
		//重新计算最早触发的定时器
		if (recalc)
			RecalcEarliest(last);
	}
	return numcanceled;
}

/**
*
*根据timerid,移出单个的定时器。
*
* timerid   定时器的唯一标识
* arg		用来返回用户定义的数据,如果arg!=0,将要设置arg值到该参数
*
* 返回1表示成功,-1表示失败
*/
//
template <class TYPE>
int CTimerQueue<TYPE>::Cancel(long timerid, const void** arg)
{
	TimerNode<TYPE>* n = FindNode(timerid);
	//是否找到对应的节点
	if (n != 0)
	{
		unsigned long last = n->GetTimerValue();

		int recalc = (GetFirstI() == n);
		if (arg != 0)
			*arg = n->GetArg();

		//触发取消操作
		n->OnTimerCancel();

		CancelI(n);

		if (recalc)
			RecalcEarliest(last);
		return 1;
	}
	return 0;
}

//两个Cancel()函数的公共子集
template <class TYPE>
void CTimerQueue<TYPE>::CancelI(TimerNode<TYPE>* n)
{
	Unlink(n);
	FreeNode(n);
}

//计算出下一个expire的定时器
template <class TYPE>
void CTimerQueue<TYPE>::RecalcEarliest(unsigned long last)
{
	if (IsEmpty ())
		return;

	unsigned long et = 0;
	unsigned long es = 0;
	unsigned long spoke = m_nEarliestSpoke;

	last = last>>m_nResBits;
	last = last<<m_nResBits;
	//遍历wheel一次
	unsigned long i = 0;
	for (;i < m_nSpokeCount; ++i)
	{
		TimerNode<TYPE>* root = m_pSpokes[spoke];
		TimerNode<TYPE>* n = root->GetNext();
		if (n != root)
		{
			unsigned long t = n->GetTimerValue ();
			if (t < (last + m_nWheelTime))
			{
				m_nEarliestSpoke = spoke;
				return;
			}
			else if (et == 0 || t < et)
			{
				et = t;
				es = spoke;
			}
		}
		if (++spoke >= m_nSpokeCount)
			spoke = 0;
	}
	m_nEarliestSpoke = es;
}

//移出最早节点，寻找最新的<m_nEarliestSpoke>
template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::RemoveFirst(void)
{
	RemoveFirstExpired (MAX_TIME);
}

template <class TYPE>
void CTimerQueue<TYPE>::Unlink(TimerNode<TYPE>* n)
{
	--m_nTimerCount;
	n->GetPrev()->SetNext(n->GetNext());
	n->GetNext()->SetPrev(n->GetPrev());
	n->SetPrev(0);
	n->SetNext(0);
}

template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::RemoveFirstExpired(unsigned long now)
{
	TimerNode<TYPE>* n = GetFirst();
	if (n != 0 && n->GetTimerValue() <= now)
	{
		Unlink(n);
		RecalcEarliest(n->GetTimerValue());
		return n;
	}
	return 0;
}

//得到最近的节点，不会移出
template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::GetFirst(void)
{
	return GetFirstI();
}

template <class TYPE>
TimerNode<TYPE>* CTimerQueue<TYPE>::GetFirstI(void) const
{
	TimerNode<TYPE>* root = m_pSpokes[m_nEarliestSpoke];
	TimerNode<TYPE>* first = root->GetNext();
	if (first != root)
		return first;
	return 0;
}

template <class TYPE>
CTimerQueueIterator<TYPE>& CTimerQueue<TYPE>::Iter(void)
{
	pIterator->First();
	return *pIterator;
}


//驱动定时器接口
//默认用现在的时钟周期驱动
template <class TYPE>
int CTimerQueue<TYPE>::Expire(void)
{
	unsigned long curtime = timeGetTime();
	return Expire(curtime);
}

/**
* 依传入的curtime为准,为所有定时器小于curtime时间的触发超时函数
*
* curtime 表示当前时间.
*
* 返回取消的定时期个数
*/
template <class TYPE>
int CTimerQueue<TYPE>::Expire(unsigned long curtime)
{
	int expcount = 0;
	TimerNode<TYPE>* n = RemoveFirstExpired(curtime);

	while (n != 0)
	{
		bool cancel = true;
		++expcount;
		if (n->GetInterval() > 0)
		{
			do
			{
				n->SetTimerValue(n->GetTimerValue()+n->GetInterval());
			}
			while (n->GetTimerValue () <= curtime);

			cancel = false;
			Reschedule(n);
		}
		
		//触发超时操作
		n->OnTimerOut(curtime);
		//调用推出函数
		if(cancel)
		{
			n->OnTimerCancel();
			FreeNode(n);
		}

		n = RemoveFirstExpired(curtime);
	}
	return expcount;
}

template <class TYPE>
void CTimerQueue<TYPE>::OutpuInfo(void) const
{
	OUTPUTLOG("ACE_Timer_Wheel_T::OutpuInfo");
	OUTPUTLOG("Start");

	char str[128];
	_snprintf(str, 128, "m_nSpokeCount = %d",m_nSpokeCount);
	OUTPUTLOG(str);
	_snprintf(str, 128, "m_nResBits = %d",m_nResBits);
	OUTPUTLOG(str);
	OUTPUTLOG("wheel = \n");

	unsigned long i = 0;
	for (; i < m_nSpokeCount; ++i)
	{
		_snprintf(str, 128, "%d",i);
		OUTPUTLOG(str);
		TimerNode<TYPE>* root = m_pSpokes[i];
		for (TimerNode<TYPE>* n = root->GetNext ();
			n != root;
			n = n->GetNext ())
		{
			n->OutpuInfo();
		}
	}
	OUTPUTLOG("End");
}

#endif