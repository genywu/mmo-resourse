#ifndef _Thread_h_
#define _Thread_h_

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

/**
 * @class Thread
 * 
 * @author Bugs
 */
class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual void Run() = 0;

	/**
	 * Start
	 */
	void Start();

	void Join();
	/**
	 * Cancel
	 * @brief member function Cancel
	 */
    void Cancel();

private:
#ifdef WIN32
	HANDLE _id;
#else
	pthread_t _id;
#endif

protected:
#ifdef WIN32
#define ThreadReturn unsigned __stdcall
#else
#define ThreadReturn void *
#endif
	static ThreadReturn Routine(void *arg);
};

//////////////////////////////////////
//
//ThreadReturn Thread::Routine(void *arg)
//{
//	((Thread*)(arg))->Run();
//
//#ifdef WIN32
//	_endthreadex(0);
//#else
//	 pthread_exit(0);
//#endif
//	return 0;
//}

//////////////////////////////////////////////////////////////////

inline Thread::Thread()
{
}

inline Thread::~Thread()
{
}
inline void Thread::Start()
{
#ifdef WIN32
	_id = (HANDLE)_beginthreadex( 0, 0, &Routine, (void*)this, 0, 0);
#else
	pthread_create(&_id, 0, Routine, (void*)this);
#endif
}

inline void Thread::Cancel()
{
#ifdef WIN32
	TerminateThread(_id, 0);
	CloseHandle(_id);
#else
	pthread_cancel(_id);
#endif
}

inline void Thread::Join()
{
#ifdef WIN32
	WaitForSingleObject(_id, INFINITE);
	CloseHandle(_id);
#else
	pthread_join(_id,0);
#endif
}

#endif
