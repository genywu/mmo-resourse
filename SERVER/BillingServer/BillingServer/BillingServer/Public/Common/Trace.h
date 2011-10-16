#ifndef _Trace_h_
#define _Trace_h_

#include "Lock.h"
#include "Mutex.h"
#include "Guard.h"
#include <stdio.h>

/**
 * Trace
 */
template<typename LOCK=Lock>
class Trace
{
public:
	explicit Trace(const char *info, const char *file, const char *func, int line);
	virtual ~Trace();
private:
	static unsigned _count;
	static LOCK _lock;

	int         _line;
	const char *_info;
	const char *_file;
	const char *_func;
};

////////////////////////////////////////////////
template<typename LOCK>
unsigned Trace<LOCK>::_count;

template<typename LOCK>
LOCK Trace<LOCK>::_lock;

/////////////////////////////////////////////////
template<typename LOCK>
inline Trace<LOCK>::Trace(const char *info, const char *file, const char *func, int line)
:_line(line), _info(info),_file(file), _func(func)
{
	Guard<LOCK> guard(_lock);
	++_count;
	printf("<TRACE>: ");
	for(unsigned i = 0; i < _count; ++i) printf ("  ");
	printf("(%d)calling `%s::%s` in file `%s` on line `%d`\n", _count, _info, _func, _file, line);
}

template<typename LOCK>
inline Trace<LOCK>::~Trace()
{
	Guard<LOCK> guard(_lock);
	printf("<TRACE>: ");
	for(unsigned i = 0; i < _count; ++i) printf("  ");
	printf("(%d)leaving `%s::%s`\n", _count, _info, _func);
	--_count;
}

#ifdef DEBUG
# define TRACE(X)
#else

#ifdef WIN32
# define TRACE(X) Trace<Mutex> _trace_((X), __FILE__, __FUNCTION__, __LINE__)
#else
# define TRACE(X) Trace<Mutex> _trace_((X), __FILE__, __func__, __LINE__)
#endif
#endif

#endif
