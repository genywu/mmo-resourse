


#ifndef _TILE_POOL_CPP_
#define _TILE_POOL_CPP_


#include "../include/TilePool.h"

namespace XM_Tools
{
	template<typename _TLock, typename _TAllocator>
	CTilePool<_TLock, _TAllocator>::CTilePool(_TAllocator &Allocator, unsigned long uDefaultAddSize)
		:m_Allocator(Allocator)
		,m_uDefaultAddSize(uDefaultAddSize)
	{
#ifndef USE_TILE_MANAGE
		return;
#endif
		for (unsigned long i = 0; i < TILE_POOL_NUM; ++i)
		{
			//! 所在数组的索引
			m_arrMemList[i].uIndex			= i;
			//!	总大小（包括自身信息）
			m_arrMemList[i].uSize			= 0;
			//! 分块总数
			m_arrMemList[i].uTileSum		= 0;
			//! 已使用块数
			m_arrMemList[i].uUseCount		= 0;
			//! 上次申请的块数
			m_arrMemList[i].uLastAllocCount	= 0;
		}
	}

	template<typename _TLock, typename _TAllocator>
	CTilePool<_TLock, _TAllocator>::~CTilePool(void)
	{
		Release();	
	}

	/*
	初始化对象

	*/
	template<typename _TLock, typename _TAllocator>
	bool CTilePool<_TLock, _TAllocator>::InitForSize(unsigned long uSize, unsigned long uProvideCount)
	{
#ifndef USE_TILE_MANAGE
		return true;
#endif
		unsigned long uIdx = GetListIndex(uSize);
		if(0xFFFFFFFF == uIdx || 0 == uProvideCount)
			RETURN_THROW(false);

		tagTileManageInfo &TileManageInfo = m_arrMemList[uIdx];
		//! 一次分配的块数不能超过0xFFFF
		uProvideCount = (0xFFFF < uProvideCount) ? 0xFFFF : uProvideCount;

		//! 数量对齐
		uProvideCount = SNAP_TO_GRID(uProvideCount, GetDefaultAddCount(uIdx));
		AddNewTile(TileManageInfo, uProvideCount);

		return true;
	}

	//!
	template<typename _TLock, typename _TAllocator>
	void CTilePool<_TLock, _TAllocator>::Release(void)
	{
#ifndef USE_TILE_MANAGE
		return;
#endif
		for (unsigned long i = 0; i < TILE_POOL_NUM; ++i)
		{
			//! 检测内存泄露
			MM_ASSERY(0 == m_arrMemList[i].uUseCount);
			//! 输出相关信息
#ifdef LOG_ALLOC_FREE_INFO

#endif 

			//! 释放内存
#ifdef DESTRUTOR_FREE_SYSMEM
			CListNode<void> *pCListNode = m_arrMemList[i].SysMemList.PopNode();
			while(NULL != pCListNode)
			{
				m_Allocator.Free(pCListNode);
				pCListNode = m_arrMemList[i].SysMemList.PopNode();
			}
#endif
		}
	}

	//! 输出运行时信息
	template<typename _TLock, typename _TAllocator>
	void CTilePool<_TLock, _TAllocator>::OutInfo(char *pInfoBuf)
	{
#ifdef LOG_ALLOC_FREE_INFO
	#ifndef USE_TILE_MANAGE
		_snprintf(pInfoBuf, INFO_BUF_SIZE, "CTilePool:\r\n	当前片池没有进行内存分配！\r\n");
	#else
		_snprintf(pInfoBuf, INFO_BUF_SIZE, "CTilePool:\r\n");
		for (unsigned long i = 0; i < TILE_POOL_NUM; ++i)
		{
			_snprintf(&pInfoBuf[strlen(pInfoBuf)], INFO_BUF_SIZE - strlen(pInfoBuf), "	【%u】byte分配【%u】块，消耗【%u】KB、已使用【%u】块、\r\n", 
				MIN_GRANULARITY * (i + 1), m_arrMemList[i].uTileSum, m_arrMemList[i].uSize / 1024, m_arrMemList[i].uUseCount);
		}
	#endif
#endif
	}

	/*
	分配一个指定大小的内存
	*/
	template<typename _TLock, typename _TAllocator>
	void* CTilePool<_TLock, _TAllocator>::Alloc(unsigned long uSize)
	{
#ifndef USE_TILE_MANAGE
		return malloc(uSize);
#endif
		unsigned long uIdx = GetListIndex(uSize);
		if(0xFFFFFFFF == uIdx)
			RETURN_THROW(NULL);

		tagTileManageInfo &TileManageInfo = m_arrMemList[uIdx];
		if(0 == TileManageInfo.FreeList.GetNodeNum())
		{
			if(!AddNewTile(TileManageInfo, GetDefaultAddCount(uIdx)))
				RETURN_THROW(NULL);
		}
		
		CListNode<unsigned char> *pFreeNode = TileManageInfo.FreeList.PopNode();
		MM_ASSERY(NULL != pFreeNode);
		//! 已使用块数
		TileManageInfo.InfoLock.Lock();
		++TileManageInfo.uUseCount;
		TileManageInfo.InfoLock.UnLock();

		unsigned char cInfo = (unsigned char)TileManageInfo.uIndex;
		pFreeNode->SetInfo(cInfo);
		return pFreeNode->GetReBuf();
	}

	/*
	*/
	template<typename _TLock, typename _TAllocator>
	void CTilePool<_TLock, _TAllocator>::Free(void *pMem)
	{
#ifndef USE_TILE_MANAGE
		free(pMem);
		return;
#endif
		//! 内存偏移到节点信息
		CListNode<unsigned char> *pFreeNode = (CListNode<unsigned char>*)(((char*)pMem) - SNAP_TO_GRID(sizeof(unsigned char), LIST_NODE_GRID_SIZE));
		unsigned long uIdx = pFreeNode->GetInfo();
		if(0xFFFFFFFF == uIdx)
			RETURN_THROW_VOID;
		
		tagTileManageInfo &TileManageInfo = m_arrMemList[uIdx];
		//! 已使用块数
		TileManageInfo.InfoLock.Lock();
		--TileManageInfo.uUseCount;
		TileManageInfo.InfoLock.UnLock();

		m_arrMemList[uIdx].FreeList.PushNode(pFreeNode);
	}

	/*
	得到一个尺寸对应的索引
	失败返回0xFFFFFFFF
	*/
	template<typename _TLock, typename _TAllocator>
	unsigned long CTilePool<_TLock, _TAllocator>::GetListIndex(unsigned long uSize)
	{
		if(TILE_MAX_SIZE < uSize || 0 == uSize)
			return 0xFFFFFFFF;
		return uSize / MIN_GRANULARITY - ((0 == uSize % MIN_GRANULARITY) ? 1 : 0);
	}

	/*
	得到默认一次增加的块数
	*/
	template<typename _TLock, typename _TAllocator>
	unsigned long CTilePool<_TLock, _TAllocator>::GetDefaultAddCount(unsigned long uIndex)
	{
		MM_ASSERY(TILE_POOL_NUM > uIndex);
		return m_uDefaultAddSize / (MIN_GRANULARITY * (uIndex + 1));
	}


	//! 增加新的空闲块
	template<typename _TLock, typename _TAllocator>
	bool CTilePool<_TLock, _TAllocator>::AddNewTile(tagTileManageInfo &TileManageInfo, unsigned long uAddCount)
	{
		MM_ASSERY(GetDefaultAddCount(TileManageInfo.uIndex) >= uAddCount && 0 != uAddCount && 0 == (uAddCount % GetDefaultAddCount(TileManageInfo.uIndex)));
		//! 节点长度 = 对齐的节点信息长度 + 复用的内存（Next指针、可用内存长度）
		unsigned long uNodeSize = SNAP_TO_GRID(sizeof(unsigned char), LIST_NODE_GRID_SIZE) + max(sizeof(CListNode<unsigned char>), (TileManageInfo.uIndex + 1) * MIN_GRANULARITY);

		//! 分配所需内存
		char *pMem = (char*)m_Allocator.Alloc(sizeof(CListNode<void>) + uNodeSize * uAddCount);
		if(NULL == pMem)
			return false;

		//! 添加到系统内存管理
		TileManageInfo.SysMemList.PushNode((CListNode<void>*)pMem);
		pMem += sizeof(CListNode<void>);

		//! 添加到空闲链表
		for (unsigned long i = 0; i < uAddCount; ++i)
		{
			CListNode<unsigned char> *pFreeNode = (CListNode<unsigned char>*)(pMem + i * uNodeSize);
#ifdef _TEST_ALLOC_TILE_
			static stdext::hash_set<void*> HasMem;
			stdext::hash_set<void*>::iterator itrMem = HasMem.find(pFreeNode);
			if (itrMem != HasMem.end())
			{
				throw;
			}
			HasMem.insert(pFreeNode);
#endif	
			pFreeNode->GetInfo() = (unsigned char)TileManageInfo.uIndex;
			TileManageInfo.FreeList.PushNode(pFreeNode);
		}

		TileManageInfo.InfoLock.Lock();
		{
			//!	总大小（包括自身信息）
			TileManageInfo.uSize			+= uNodeSize * uAddCount;
			//! 分块总数
			TileManageInfo.uTileSum			+= uAddCount;
			//! 上次申请的块数
			TileManageInfo.uLastAllocCount	= uAddCount;
		}
		TileManageInfo.InfoLock.UnLock();
		
		return true;
	}
	
};


#endif //! _TILE_POOL_CPP_