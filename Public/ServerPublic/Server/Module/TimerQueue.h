//=============================================================================
/**
*  file: TimerQueue.h
*
*  Brief:通过时间轮原理，实现定时器功能。
*
*  Authtor:wangqiao
*	
*	Datae:2007-5-15
*/
//=============================================================================

#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEzUE_H__
#pragma once


template <class TYPE>
class CTimerQueue;

const int MAX_TIME = 0x8FFFFFFF;
//#define OUTPUTLOG(X) AppFrame::AddStdLog(X)

#define OUTPUTLOG(X) Log4c::Trace(ROOT_MODULE,X)

/**
* class: TimerNode
*
* Brief: 保存了定时器中一个节点的相关数据
*
*/

template <class TYPE>
class TimerNode
{
private:
    //存储在数据库的对象类型
    TYPE			m_Type;
    //和定时器相关联的完成标示
    const	void*	m_pArg;
    //触发的时间值(毫秒)
    ulong 	m_dwTimeValue;
    //如果是一个周期性的定时器将保存下一次将要触发的值
    ulong 	m_dwInterval;	
    //定时器ID
    ulong 	m_dwTimerID;
    //实现双向链表
    TimerNode<TYPE> *m_pRev;
    TimerNode<TYPE> *m_pNext;
public:
    TimerNode(void);
    ~TimerNode(void);

    //设置相关的数据值
    void Set(const TYPE &type,
        const	void* arg,
        ulong 	dwTimeValue,
        ulong 	dwInterval,
        ulong 	dwTimerID,
        TimerNode<TYPE> *p,
        TimerNode<TYPE> *n);

    //得到type
    TYPE &GetType(void);
    //设置type
    void SetType(TYPE &type);
    //得到用户数据
    const void *GetArg(void);
    //设置用户数据
    void SetArg(void *arg);
    //得到定时器时间值
    ulong GetTimerValue(void) const;
    //设置定时器时间值
    void SetTimerValue(ulong timervalue);
    //得到定时器间隔
    ulong GetInterval(void) const;
    //设置定时器间隔
    void SetInterval(ulong interval);

    TimerNode<TYPE> *GetPrev(void);
    void SetPrev(TimerNode<TYPE> *prev);

    TimerNode<TYPE> *GetNext(void);
    void SetNext(TimerNode<TYPE> *next);
    //得到定时器ID
    long GetTimerID(void) const;
    //设置定时器ID
    void SetTimerID(long timerid);
    //时间期满时触发这个函数
    void OnTimerOut(ulong);
    //取消定时器时触发这个函数
    void OnTimerCancel();
    //输出详细信息
    void OutpuInfo(void) const;
};


/**
* class: CTimerQueue
*
* Brief: 一个Iterator,用来遍历Wheel中的所有节点.
*			不会按照时间超时值增长的顺序遍历
*/

template <class TYPE>
class CTimerQueueIterator
{
private:
    //指定要遍历的定时器
    CTimerQueue<TYPE>&	m_TimerQueue;
    //在时间轮中的当前位置
    ulong	m_nSpoke;
    //指向当前遍历位置上的节点指针
    TimerNode<TYPE>	*m_pCurNode;
private:
    void GoToNext(ulong nSartSpoke);
public:
    typedef CTimerQueue<TYPE> Queue;
    typedef TimerNode<TYPE> Node;

    CTimerQueueIterator(CTimerQueue<TYPE> &);
    ~CTimerQueueIterator(void);

    //定位于第一个spoke的第一个节点
    void First (void);
    //指向当前节点的下一个节点
    void Next (void);
    //当在遍历序列中再也没有节点时,返回true.
    int IsDone (void) const;
    //返回处在遍历序列的当前节点
    TimerNode<TYPE>* Item (void);
};


/**
* class:	CTimerQueueIterator
*
* Brief:	用时间定时轮原理实现的一个定时器.
*			实现使用了一个包含以绝对时间排序双向链表的一个Hash表,
*			定时节点也具有预分配功能
*
*/

template <class TYPE>
class CTimerQueue
{
private:
    //时间轮
    TimerNode<TYPE>** m_pSpokes;
    //时间轮大小
    ulong	m_nSpokeCount;
    //用于轮辐的TimerID的位数
    ulong	m_nSpokeBits;
    //每个轮辐的定时器最大数量
    ulong	m_nMaxPerSpoke;
    //时间轮的解析度(用微妙)
    long			m_nResBits;
    //最近时间的列表索引
    ulong	m_nEarliestSpoke;
    //定时器的迭代器
    CTimerQueueIterator<TYPE>* pIterator;
    //重复一次时间滚轮所需的时间总数(resolution * m_nSpokeCount)
    ulong	m_nWheelTime;
    //当前的时间定时器总数
    ulong	m_nTimerCount;

    //保存预分配节点的列表
    list<TimerNode<TYPE>*> m_FreeList;
private:
    //得到一个2次幂值
    int Power2Bits (int n, int minbits, int maxbits);
    TimerNode<TYPE>* AllocNode();
    void FreeNode(TimerNode<TYPE>*);

    //申明禁止使用拷贝构造函数和赋值操作符
    CTimerQueue(const CTimerQueue &);
    void operator=(const CTimerQueue&);
private:

    TimerNode<TYPE>* GetFirstI(void) const;
    TimerNode<TYPE>* RemoveFirstExpired(ulong now);
    void OpenI(	size_t prealloc,
        ulong spokes,
        ulong res);
    void Reschedule(TimerNode<TYPE> *);
    TimerNode<TYPE>* FindSpokeNode(	ulong spoke,
        long timerid) const;
    TimerNode<TYPE>* FindNode(long timerid) const;
    ulong CalculateSpoke(ulong expire) const;
    long GenerateTimerID(ulong spoke);
    void ScheduleI(TimerNode<TYPE>* n,
        ulong spoke,
        ulong expire);
    void CancelI(TimerNode<TYPE>* n);
    void Unlink(TimerNode<TYPE>* n);
    void RecalcEarliest(ulong last);

    long ScheduleI(const TimerNode<TYPE>& type,
        const void* arg,
        ulong futuretime,
        ulong interval);
public:

    //带有设置spoke大小、resolution、预分配节点大小构造函数
    CTimerQueue (ulong spokecount,
        ulong resolution,
        ulong prealloc = 0);
    ~CTimerQueue (void);

    //如果对列为空返回true,否则false
    int IsEmpty(void) const;
    //返回最早触发的定时器ID
    ulong EarliestTime(void) const;
    //改变一个定时器的间隔(interval==0:设置成非周期性的定时器,
    //反之设置成该间隔值的周期性定时节点)
    int ResetInterval(long timerid,ulong interval);

    //取消所有等于<type>的定时期
    //返回值表示取消的定时器个数
    int Cancel(const TYPE& type);

    //取消单个<m_dwTimerID>等于<timerid>的定时器
    //如果arg不会空,将要设置调度时候设置的<arg>参数
    //返回值表示取消的定时器个数
    int Cancel(long timerid, const void** arg = 0);

    //运行timerGetTime()函数,为所有定时器小于curtime时间的触发超时函数
    //返回取消的定时期个数
    int Expire(void);
    //依传入的curtime为准,为所有定时器小于curtime时间的触发超时函数
    //返回取消的定时期个数
    int Expire(ulong);

    //对<type>类型预定一个定时器,这个定时器将要在绝对时间futuretime触发
    //interval:如果大于0,表示这是一个周期性定时器,是一个相对时间
    //表示定时器下一次触发的间隔时间,该函数返回-1表示调度失败
    //arg是一个用户自定义参数	
    long Schedule(	const TYPE &type,
        const void *arg,
        ulong futuretime,
        ulong interval = 0);

    //返回一个CTimerQueue的Iterator指针
    CTimerQueueIterator<TYPE>& Iter(void);
    //移出最早触发的时间节点,并返回它
    TimerNode<TYPE>* RemoveFirst(void);
    //得到最早触发的时间节点,并返回它
    TimerNode<TYPE>* GetFirst(void);
    //输出详细信息
    void OutpuInfo(void) const;

public:
    //申明CTimerQueueIterator为友元类
    friend class CTimerQueueIterator<TYPE>;
};

//包含定义
#include "TimerQueue.tcc"

#endif