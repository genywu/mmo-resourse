

#include "../include/Tools.h"
#include "../include/ObjectFactory.h"


namespace XM_Tools
{
#ifdef LOG_ALLOC_FREE_INFO

	#ifdef MM_DEBUG_NEW

	//!
	void* CMemObject::operator new(size_t Size, const char *pInfo, CDebugNew &DebugNew)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize, pInfo);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}
	void* CMemObject::operator new[](size_t Size, const char *pInfo, CDebugNew &DebugNew)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize, pInfo);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}

	//!
	void CMemObject::operator delete(void *pMem, const char*, CDebugNew &)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}
	void CMemObject::operator delete[](void *pMem, const char*, CDebugNew &)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}

	#else

	//!
	void* CMemObject::operator new(size_t Size, const char *pInfo)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize, pInfo);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}
	void* CMemObject::operator new[](size_t Size, const char *pInfo)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize, pInfo);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}

	//!
	void CMemObject::operator delete(void *pMem, const char*)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}
	void CMemObject::operator delete[](void *pMem, const char*)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}

	#endif
	

#else
	//!
	void* CMemObject::operator new(size_t Size)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}
	void* CMemObject::operator new[](size_t Size)
	{
		unsigned long AllocSize = sizeof(unsigned long) + (unsigned long)Size;
		unsigned long *pBuf = (unsigned long*)GetMemManage<AsyncLock>().AsyncAlloc(AllocSize);
		pBuf[0] = AllocSize;
		return &pBuf[1];
	}
	
	

#endif

	//!
	void CMemObject::operator delete(void *pMem)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}
	void CMemObject::operator delete[](void *pMem)
	{
		unsigned long *pBuf = ((unsigned long*)pMem) - 1;
		__FREE__(pBuf, pBuf[0]);
	}


	
};
