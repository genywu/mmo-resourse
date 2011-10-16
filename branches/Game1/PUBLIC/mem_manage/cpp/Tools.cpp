

#include "../include/Tools.h"
#include "../include/ObjectFactory.h"


namespace XM_Tools
{
#ifdef _LOG_ALLOC_FILE_INFO_
	//! 同步分配内存
	void* SyncAlloc(unsigned long uSize, const char *pInfo)
	{
		return GetMemManage<InvalidLock>().AsyncAlloc(uSize, pInfo);
	}

	//! 异步分配内存
	void* AsyncAlloc(unsigned long uSize, const char *pInfo)
	{
		return GetMemManage<AsyncLock>().AsyncAlloc(uSize, pInfo);;
	}

#else
	//! 同步分配内存
	void* SyncAlloc(unsigned long uSize)
	{
		return GetMemManage<InvalidLock>().AsyncAlloc(uSize);
	}
	//! 异步分配内存
	void* AsyncAlloc(unsigned long uSize)
	{
		return GetMemManage<AsyncLock>().AsyncAlloc(uSize);
	}


#endif


	//! 同步释放内存
	void SyncFree(void *pMem, unsigned long uSize)
	{
		GetMemManage<InvalidLock>().AsyncFree(pMem, uSize);
	}

	//! 异步释放内存
	void AsyncFree(void *pMem, unsigned long uSize)
	{
		GetMemManage<AsyncLock>().AsyncFree(pMem, uSize);
	}
	



};
