



/*
本文件定义了用于MemManage的几种锁
	无效的锁
	win32的锁

*/

#include "assert.h"
#include <Windows.h>

#ifndef _LOCK_H_
#define _LOCK_H_


#define CACHE_LINES 32

namespace XM_Tools
{
	/////////////////////////////////////////////////////////////////////////////
	//! 一个无效的锁，用于不需要同步的对象
	class _InvalidLock
	{
	public:
		_InvalidLock(void)			{}
		~_InvalidLock(void)			{}
	public:
		void	Lock(void)			{}
		void	UnLock(void)		{}
	};

	/////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
	//! 一个WIN32临界区实现的锁
	class LockOfWin32
	{
	public:
		LockOfWin32(void)			{InitializeCriticalSection(&m_CriticalSection);}
		~LockOfWin32(void)			{DeleteCriticalSection(&m_CriticalSection);}
	public:
		void	Lock(void)			{EnterCriticalSection(&m_CriticalSection);}
		void	UnLock(void)		{LeaveCriticalSection(&m_CriticalSection);}
	private:
		CRITICAL_SECTION			m_CriticalSection;
	};
#endif

	/////////////////////////////////////////////////////////////////////////////
	//! 一个利用原子操WIN32API作实现的锁（不使用于线程数较多，碰撞激烈的场合）
	//! 用于加锁时间较短的操作
	class LockOfWinWhile
	{
	public:
		LockOfWinWhile(void)		:m_lLockFlag(0){}
		~LockOfWinWhile(void)		{}
	public:
		void	Lock(void)			{while(1 == InterlockedCompareExchange(&m_lLockFlag, 1, 0)){__asm rep nop };}
		void	UnLock(void)		{if(0 == InterlockedCompareExchange(&m_lLockFlag, 0, 1))assert(false);}
	private:
		char				m_FrontCacheLineFill[CACHE_LINES];
		long				m_lLockFlag;
		char				m_BackCacheLineFill[CACHE_LINES - sizeof(long)];
	};

	/////////////////////////////////////////////////////////////////////////////
	//! LockOfWinWhile的一个优化处理（用于线程数较多，碰撞激烈的场合）
	class LockOfWinWhileSleep
	{
	public:
		LockOfWinWhileSleep(void):m_lLockFlag(0), m_lWaitCount(0){}
		~LockOfWinWhileSleep(void)	{}

	public:
		void	Lock(void)			
		{
			bool bIncrement = false;
			while(1 == InterlockedCompareExchange(&m_lLockFlag, 1, 0))
			{
				if(bIncrement || 2 > m_lWaitCount)
				{
					if(!bIncrement)
					{
						InterlockedIncrement(&m_lWaitCount);
						bIncrement = true;
					}
					__asm rep nop
				}
				else
					Sleep(1);
			}

			if(bIncrement)
				InterlockedDecrement(&m_lWaitCount);
		}

		void	UnLock(void)		
		{
			if(0 == InterlockedCompareExchange(&m_lLockFlag, 0, 1))
			{
				assert(false);
			}
		}
	private:
		char				m_FrontCacheLineFill[CACHE_LINES];
		long				m_lLockFlag;
		long				m_lWaitCount;
		char				m_BackCacheLineFill2[CACHE_LINES - sizeof(long) * 2];
	};


};

#endif // _LOCK_H_

