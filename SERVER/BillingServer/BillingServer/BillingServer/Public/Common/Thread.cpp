#include "stdafx.h"
#include "Thread.h"

////////////////////////////////////

ThreadReturn Thread::Routine(void *arg)
{
	((Thread*)(arg))->Run();

#ifdef WIN32
	_endthreadex(0);
#else
	 pthread_exit(0);
#endif
	return 0;
}