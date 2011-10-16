#ifndef _Semaphore_h_
#define _Semaphore_h_

#ifdef WIN32
#else
#include <semaphore.h>
#endif

/**
 * @class Semaphore
 * 
 * @author Bugs
 */
class Semaphore
{
public:
	Semaphore(int num = 0);
	virtual ~Semaphore();

	void Wait();
	void Post();

    bool Wait(long sec, long nsec=0);
#ifndef WIN32
	bool TryWait();
#endif

	

private:
#ifdef WIN32
	HANDLE _sem;
#else
	sem_t _sem;
#endif
};

///////////////////////////////////////////////////////
inline Semaphore::Semaphore(int num)
{
#ifdef WIN32
	_sem = CreateSemaphore(0, 0, 10000, 0);
#else
    sem_init(&_sem, 0, num);
#endif
}

inline Semaphore::~Semaphore()
{
#ifdef WIN32
	CloseHandle(_sem);
#else
	sem_destroy(&_sem);
#endif
}

inline void Semaphore::Wait()
{
#ifdef WIN32
	WaitForSingleObject(_sem, INFINITE);
#else
	sem_wait(&_sem);
#endif
}

inline bool Semaphore::Wait(long sec, long nsec)
{
#ifdef WIN32
	unsigned time;
	time = sec * 1000 + nsec / 1000;
	return WaitForSingleObject(_sem, time) == WAIT_OBJECT_0;
#else
	timespec tv;
	tv.tv_sec = sec;
	tv.tv_nsec = nsec;

    return sem_timedwait(&_sem, &tv) == 0;
#endif
}

#ifndef WIN32
inline bool Semaphore::TryWait()
{
    return sem_trywait(&_sem);
}
#endif

inline void Semaphore::Post()
{
#ifdef WIN32
	ReleaseSemaphore(_sem, 1, 0);
#else
    sem_post(&_sem);
#endif
}

#endif
