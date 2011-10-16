///

#ifndef _Gurad_h_
#define _Gurad_h_

#include "Lock.h"

/// Guard
 
template<typename LOCK = Lock>
class Guard
{
public:
    explicit Guard(LOCK &lock);
    virtual ~Guard();
    
	bool Acquire();
    bool Release();
    bool TryAcquire();
private:
    LOCK &_lock;
};

//////////////////////////////////////////////
template<typename LOCK>
inline Guard<LOCK>::Guard(LOCK &lock)
: _lock(lock)
{
	_lock.Acquire();
}

template<typename LOCK>
inline Guard<LOCK>::~Guard()
{
	_lock.Release();
}

template<typename LOCK>
inline bool Guard<LOCK>::Acquire()
{
	return _lock.Acquire();
}

template<typename LOCK>
inline bool Guard<LOCK>::TryAcquire()
{
	return _lock.TryAcquire();
}

template<typename LOCK>
inline bool Guard<LOCK>::Release()
{
	return _lock.Release();
}

#define GUARD() \
 do { \
 Mutex _mutex_; \
 Guard<Mutex> _gurad_(_mutex_); \
 } while(0)

#endif
