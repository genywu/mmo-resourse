/// Recursive mutex

#ifndef _Mutex_h_
#define _Mutex_h_

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "Lock.h"

/// Mutex class
class Mutex : public Lock
{
public:
	Mutex();
	virtual ~Mutex();

	void Acquire();
#ifndef WIN32
	void Acquire(long sec, long nsec=0);
#endif
	//bool TryAcquire();
	void Release();

#ifndef WIN32
	pthread_mutex_t *GetMutex();
#endif

protected:

#ifdef WIN32
	CRITICAL_SECTION _mutex;
#else
	pthread_mutex_t	_mutex;
#endif
};

////////////////////////////////////////////////////
inline Mutex::Mutex()
{
#ifdef WIN32
	InitializeCriticalSection(&_mutex);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&_mutex, &attr);
#endif
}

inline Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&_mutex);
#else
	pthread_mutex_destroy(&_mutex);
#endif
}

inline void Mutex::Acquire()
{
#ifdef WIN32
	EnterCriticalSection(&_mutex);
#else
	pthread_mutex_lock(&_mutex);
#endif
}

#ifndef WIN32
inline void Mutex::Acquire(long sec, long nsec)
{
	timespec tv;
	tv.tv_sec = sec;
	tv.tv_nsec = nsec;
  pthread_mutex_timedlock(&_mutex, &tv);
}
#endif

//inline bool Mutex::TryAcquire()
//{
//#ifdef WIN32
//	return TryEnterCriticalSection(&_mutex) == TRUE;
//#else
//	return pthread_mutex_trylock(&_mutex) == 0;
//#endif
//}

inline void Mutex::Release()
{
#ifdef WIN32
	LeaveCriticalSection(&_mutex);
#else
	pthread_mutex_unlock(&_mutex);
#endif
}

#ifndef WIN32
inline pthread_mutex_t *Mutex::GetMutex()
{
	return &_mutex;
}
#endif

#endif
