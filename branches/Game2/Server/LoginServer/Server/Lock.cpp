#include "StdAfx.h"
#include "Lock.h"

#ifdef WIN32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* Windows Critical Section Implementation */
Lock::Lock(void)
{
	InitializeCriticalSection(&_lock);
}

Lock::~Lock(void)
{
	DeleteCriticalSection(&_lock);
}

void Lock::Acquire(void)
{
	EnterCriticalSection(&_lock);
}
void Lock::Release(void)
{
	LeaveCriticalSection(&_lock);
}

//int Lock::AttemptAcquire(void)
//{
//	return TryEnterCriticalSection(&_lock);
//}

#else

* this is done slightly differently on bsd-variants */
#if defined(__FreeBSD__) ||  defined(__APPLE_CC__) || defined(__OpenBSD__)
#	define RECURSIVE_MUTEX_FLAG PTHREAD_MUTEX_RECURSIVE
#else
#	define RECURSIVE_MUTEX_FLAG PTHREAD_MUTEX_RECURSIVE_NP
#endif

* Linux mutex implementation */
bool Mutex::attr_initalized = false;
pthread_mutexattr_t Mutex::attr;
Lock::Lock(void)
{
	if( !attr_initalized ) {
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, RECURSIVE_MUTEX_FLAG);
		attr_initalized = true;
	}

	pthread_mutex_init(&_lock, &attr);
}

Lock::~Lock(void)
{
	pthread_mutex_destroy(&_lock);
}

void Lock::Acquire(void)
{
	pthread_mutex_lock(&_lock);
}
void Lock::Release(void)
{
	pthread_mutex_unlock(&_lock);
}
int Lock::AttemptAcquire(void)
{
	return (pthread_mutex_trylock(&_lock) == 0);
}


#endif
