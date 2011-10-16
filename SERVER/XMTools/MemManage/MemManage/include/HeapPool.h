


/*
堆内存池
	向系统申请内存，并进行管理


*/


#include "MemList.h"

#ifndef _HEAP_POOL_H_
#define _HEAP_POOL_H_


namespace XM_Tools
{
	template<typename _TLock>
	class CHeapPool
	{
	public:
		CHeapPool(void);
		~CHeapPool(void);

	public:
		//!
		void						Release(void);
		//!							输出运行时信息
		void						OutInfo(char *pInfoBuf);

	public:
		//!		
		inline void*				Alloc(unsigned long uSize);
		//!		
		inline void					Free(void *pMem);

	private:
		struct tagHeapMemInfo 
		{
			unsigned long uSize;

			tagHeapMemInfo(unsigned long uInSize):uSize(uInSize){}
			inline bool operator < (const tagHeapMemInfo &Info) const
			{
				return this->uSize < Info.uSize;
			}
		};
		class HeapMemInfoLikeCompare
		{
		public:
			inline bool operator( )( const tagHeapMemInfo& Info1, const tagHeapMemInfo& Info2 ) const
			{
				if(Info1.uSize >= Info2.uSize && HEAP_MIN_GRANULARITY >= (Info1.uSize - Info2.uSize))
					return true;
				return false;
			}
		};

	private:
		//!
		CMemSerialBList<tagHeapMemInfo, _TLock, HeapMemInfoLikeCompare>			m_UsingList;
		//!
		CMemSerialBList<tagHeapMemInfo, _TLock, HeapMemInfoLikeCompare>			m_FreeList;
		//!

#ifdef LOG_ALLOC_FREE_INFO
		//!
		_TLock																	m_InfoLock;
		//!																		内存分配的信息
		std::map<unsigned long, unsigned long>									m_mapAllocInfo;
		//!
		unsigned long															m_uAllocSize;

#endif 

	};
};


#include "../cpp/HeapPool.cpp"


#endif //! _HEAP_POOL_H_