


/*
小块内存池
	一个池只保存同一个尺寸的内存块对象
	自由链表方式实现
*/

#include "MemList.h"


#ifndef _TILE_POOL_H_
#define _TILE_POOL_H_


namespace XM_Tools
{
	template<typename _TLock, typename _TAllocator>
	class CTilePool
	{
	public:
		inline CTilePool(_TAllocator &Allocator, unsigned long uDefaultAddSize = TILE_DEFAULT_ADD_SIZE);
		inline ~CTilePool(void);

	public:
		//!											初始化对象
		inline bool									InitForSize(unsigned long uSize, unsigned long uProvideCount);
		//!
		inline void									Release(void);

		//!											输出运行时信息
		void										OutInfo(char *pInfoBuf);

	public:
		//!
		inline void*								Alloc(unsigned long uSize);
		//!
		inline void									Free(void *pMem);


	private:
		//! 管理一个尺寸内存的结构
		struct tagTileManageInfo 
		{
			//! 所在数组的索引
			unsigned long uIndex;
			//!	总大小（包括自身信息）
			unsigned long uSize;
			//! 分块总数
			unsigned long uTileSum;
			//! 已使用块数
			unsigned long uUseCount;
			//! 上次申请的块数
			unsigned long uLastAllocCount;
			//!
			_TLock InfoLock;
			//! 已获得的系统内存
			CMemList<void, _TLock> SysMemList;
			//! 空闲链表
			CMemList<unsigned char, _TLock> FreeList;
		};

	private:		
		//!											得到尺寸对用的索引
		inline unsigned long						GetListIndex(unsigned long uSize);
		//!											得到默认一次增加的块数
		inline unsigned long						GetDefaultAddCount(unsigned long uIndex);
		//!											增加新的空闲块
		inline bool									AddNewTile(tagTileManageInfo &TileManageInfo, unsigned long uAddCount);

	private:
		//!											
		_TAllocator									&m_Allocator;
		//!											
		const unsigned long							m_uDefaultAddSize;
		//!											小块内存内存池数组
		tagTileManageInfo							m_arrMemList[TILE_POOL_NUM];

#ifdef LOG_ALLOC_FREE_INFO
	public:
	private:
#endif

	};
};


#include "../cpp/TilePool.cpp"

#endif //! _TILE_POOL_H_