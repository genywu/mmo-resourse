


/*
大块内存池
	池只保存同一个尺寸的内存块对象，不同尺寸的请求组合足够多的块满足返回
	位图方式管理
	不动态分配内存，在不能满足请求时，直接调用_TAllocator返回内存

*/

#include "mmdef.h"

#ifndef _BLOCK_POOL_H_
#define _BLOCK_POOL_H_


namespace XM_Tools
{
	template<typename _TLock, typename _TAllocator>
	class CBlockPool
	{
	public:
		CBlockPool(_TAllocator &Allocator);
		~CBlockPool(void);

	public:
		//!								初始化对象
		bool							Init(void);
		//!
		void							Release(void);
		//!								输出运行时信息
		void							OutInfo(char *pInfoBuf);

	public:
		//!		
		inline void*					Alloc(unsigned long uSize);
		//!		
		inline void						Free(void *pMem);

	private:
		_TAllocator						&m_Allocator;
		//!								初始化标记
		unsigned long					m_uInitFlag;
		//!								剩余空间(byte)
		unsigned long					m_uFreeCount;
#ifdef LOG_ALLOC_FREE_INFO
		//!								池分配内存次数
		unsigned long					m_PoolAllocCount;
#endif
		//!								记录直接分配的内存次数
		unsigned long					m_uFirsthandCount;

		//!								第一块空闲内存的偏移量
		unsigned long					m_FreeBeginOffset;
		//!								最后一块空闲内存的偏移量
		unsigned long					m_FreeEndOffset;

		//!								
		_TLock							m_FlagLock;

		//!
		unsigned char					*m_pFlag;
		//!
		unsigned char					*m_pBuf;
		
		
	private:

		
		
	};
};


#include "../cpp/BlockPool.cpp"


#endif //! _BLOCK_POOL_H_