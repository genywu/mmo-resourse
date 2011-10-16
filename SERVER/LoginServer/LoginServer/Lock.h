#ifndef _Lock_h_
#define _Lock_h_

#ifdef WIN32
#	ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0501
#		include <windows.h>
#	endif
#endif


class Lock
{
public:
	Lock(void);
	~Lock(void);

	void Acquire(void);

	void Release(void);

	//int AttemptAcquire(void);

protected:
#ifdef WIN32
	CRITICAL_SECTION			_lock;
#else
	static int					attr_initalized;
	static pthread_mutexattr_t	attr;
	pthread_mutex_t				_lock;
#endif

};

#endif