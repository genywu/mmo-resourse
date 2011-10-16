/*

*/
#ifndef _Lock_h_
#define _Lock_h_

/**
 * Lock
 * @brief Lock class.
 * @remarks Lock is a lock.
 * @par test.
 */
class Lock
{
public:
	Lock();
	virtual ~Lock();
	
	virtual void Acquire();
	virtual void Release();
	virtual bool TryAcquire();
};

////////////////////////////////////////////////
inline Lock::Lock()
{
}

inline Lock::~Lock()
{
}

inline void Lock::Acquire()
{
}

inline bool Lock::TryAcquire()
{
	return 1;
}

inline void Lock::Release()
{
}

#endif
