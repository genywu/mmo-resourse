/**
 * @file Condition.h
 * @author Bugs
 */

#ifndef _Condition_h_
#define _Condition_h_

#include "Common.h"
#include "Guard.h"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/**
 * Condition
 */

class Condition
{
#ifdef WIN32
#define CONDITION_TYPE HANDLE
#else
#define CONDITION_TYPE pthread_cond_t
#endif

public:
	/**
	 * Condition
	 */
	explicit Condition(Mutex &mutex);

	/**
	 * ~Condition
	 */
	~Condition();

	/**
	 * Signal
	 */
	void Signal();

	/**
	 * Broadcast
	 */
	void Broadcast();
	
	/**
	 * Wait
	 * @return If successful, the functions shall return; otherwise blocking. 
	 */
	void Wait();
	
	/**
	 * Wait wait by time
	 * @param sec
	 * @param nsec
	 * @return If successful, the functions shall return true; otherwise, timeout return false. 
	 */
	bool Wait(long sec, long nsec=0);
	
private:
	/**
	 * _lock external lock
	 */
	Mutex &_lock;

	/**
	 * _cond condition value
	 */
	CONDITION_TYPE _cond;

#ifdef WIN32
	/**
	 * _internal_lock internal lock WIN32 only
	 */
	Mutex _internal_lock;

	/**
	 * _wait_cnt waiter count WIN32 only
	 */
	unsigned _wait_cnt;
#endif

};

////////////////////////////////////////////
////////////////////////////////////////////
inline Condition::Condition(Mutex &mutex)
: _lock(mutex)
#ifdef WIN32
, _wait_cnt(0)
#endif
{
#ifdef WIN32
	_cond = CreateSemaphore(0, 0, 2147483647, 0);
#else
	pthread_cond_init(&_cond,0);
#endif
}

inline Condition::~Condition()
{
#ifdef WIN32
	CloseHandle(_cond);
#else
	pthread_cond_destroy(&_cond);
#endif
}

inline void Condition::Signal()
{
#ifdef WIN32
	Guard<Mutex> guard(_internal_lock);
	if( _wait_cnt )
	{
		ReleaseSemaphore(_cond, 1, 0);
		_wait_cnt--;
	}
#else
	pthread_cond_signal(&_cond);
#endif
}

inline void Condition::Broadcast()
{
#ifdef WIN32
	Guard<Mutex> guard(_internal_lock);
	if( _wait_cnt )
	{
		ReleaseSemaphore(_cond, _wait_cnt, 0);
		_wait_cnt = 0;
	}
#else
	pthread_cond_broadcast(&_cond);
#endif
}

inline void Condition::Wait()
{
#ifdef WIN32
	{
		Guard<Mutex> guard(_internal_lock);
		_wait_cnt++;
	}
	// unlock external lock
	_lock.Release();
	// internal cond wait
	WaitForSingleObject(_cond, INFINITE);
	// lock external lock
	_lock.Acquire();
#else
	pthread_cond_wait(&_cond, _lock.GetMutex());
#endif
}

inline bool Condition::Wait(long sec, long nsec)
{
#ifdef WIN32
	{
		Guard<Mutex> guard(_internal_lock);
		_wait_cnt++;
	}
	unsigned time, res;
	time = sec * 1000 + nsec / 1000;
	// unlock external lock
	_lock.Release();
	res = WaitForSingleObject(_cond, time);
	// lock external lock
	_lock.Acquire();
	if( res == WAIT_OBJECT_0 ) {
		return 1;
	}
	else { // res == WAIT_OBJECT_0
		if( _wait_cnt )
			_wait_cnt--;
		return 0;
	}
#else
	timespec tv;
	tv.tv_sec = sec;
	tv.tv_nsec = nsec;
	return pthread_cond_timedwait(&_cond, _lock.GetMutex(), &tv) == 0;
#endif
}
	
#endif
