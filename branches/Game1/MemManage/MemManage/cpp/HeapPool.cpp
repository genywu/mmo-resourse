


#ifndef _HEAP_POOL_CPP_
#define _HEAP_POOL_CPP_

#include "../include/HeapPool.h"

namespace XM_Tools
{
	template<typename _TLock>
	CHeapPool<_TLock>::CHeapPool(void)
#ifdef LOG_ALLOC_FREE_INFO
		:m_uAllocSize(0)
#endif
	{

	}
	template<typename _TLock>
	CHeapPool<_TLock>::~CHeapPool(void)
	{
		Release();
	}

	//!
	template<typename _TLock>
	void CHeapPool<_TLock>::Release(void)
	{
		MM_ASSERY(0 == m_UsingList.GetNodeNum());

#ifdef DESTRUTOR_FREE_SYSMEM
		CListBNode<tagHeapMemInfo> *pBNode = m_FreeList.PopNode();
		while (NULL != pBNode)
		{
			free(pBNode);
			pBNode = m_FreeList.PopNode();
		}
#endif
	}

	//! 输出运行时信息
	template<typename _TLock>
	void CHeapPool<_TLock>::OutInfo(char *pInfoBuf)
	{
#ifdef LOG_ALLOC_FREE_INFO
	#ifndef USE_HEAP_MANAGE
		_snprintf(pInfoBuf, INFO_BUF_SIZE, "CHeapPool:\r\n	当前内存管理器没有进行内存分配！\r\n");
	#else
		m_InfoLock.Lock();
		_snprintf(pInfoBuf, INFO_BUF_SIZE, "CHeapPool:\r\n	当前向系统申请的内存情况：\r\n	总共分配了【%u】KB的内存，共计【%u】块，其中【%u】块正在使用、【%u】块暂时缓存、最大块【%u】KB，最小块【%u】KB\r\n",
			m_uAllocSize / 1024, m_UsingList.GetNodeNum() + m_FreeList.GetNodeNum(), m_UsingList.GetNodeNum(), m_FreeList.GetNodeNum(), (--m_mapAllocInfo.end())->first / 1024, m_mapAllocInfo.begin()->first / 1024);
		m_InfoLock.UnLock();
	#endif
#endif 
	}

	//!		
	template<typename _TLock>
	void* CHeapPool<_TLock>::Alloc(unsigned long uSize)
	{
#ifndef USE_HEAP_MANAGE
		return malloc(uSize);
#endif

		CListBNode<tagHeapMemInfo> *pBNode = m_FreeList.GetLikestNode(tagHeapMemInfo(uSize));
		if(NULL == pBNode)
		{
			pBNode = (CListBNode<tagHeapMemInfo>*)malloc(uSize + sizeof(CListBNode<tagHeapMemInfo>));
			pBNode->GetInfo().uSize = uSize;
#ifdef LOG_ALLOC_FREE_INFO
			m_InfoLock.Lock();
			m_uAllocSize += uSize;
			if(m_mapAllocInfo.end() == m_mapAllocInfo.find(uSize))
				m_mapAllocInfo[uSize] = 1;
			else
				++m_mapAllocInfo[uSize];
			m_InfoLock.UnLock();
#endif
		}
		
		MM_ASSERY(NULL != pBNode);
		m_UsingList.PushNode(pBNode);
		void *pRe = NULL;
		if (NULL != pBNode)
			pRe = pBNode + 1;
		return pRe;
	}

	//!		
	template<typename _TLock>
	void CHeapPool<_TLock>::Free(void *pMem)
	{
#ifndef USE_HEAP_MANAGE
		free(pMem);
		return;
#endif
		CListBNode<tagHeapMemInfo> *pBNode = ((CListBNode<tagHeapMemInfo>*)pMem) - 1;
		MM_ASSERY(m_UsingList.Find(pBNode));
		m_UsingList.PopNode(pBNode);

		if(0 < MAX_MANAGE_COUNT)
		{
			m_FreeList.PushNode(pBNode);

			//! 数量过多时，去掉最大和最小的块，让内存块大小平均化
			if(MAX_MANAGE_COUNT <= m_FreeList.GetNodeNum())
			{
				for (unsigned long i = 0; FREE_COUNT > i; ++i)
				{
					if((FREE_COUNT >> 1) < i)
						pBNode = m_FreeList.PopNode();
					else
						pBNode = m_FreeList.PopBackNode();
#ifdef LOG_ALLOC_FREE_INFO
					stdext::hash_map<unsigned long, unsigned long>::iterator ite = m_mapAllocInfo.find(pBNode->GetInfo().uSize);
					MM_ASSERY(m_mapAllocInfo.end() != ite);
					if (m_mapAllocInfo.end() != ite)
					{
						if(1 < ite->second)
							--ite->second;
						else
							m_mapAllocInfo.erase(ite);
					}
#endif
					MM_ASSERY(NULL != pBNode);
					free(pBNode);
				}
			}
		}
		else
			free(pBNode);
	}
};
#endif //! _HEAP_POOL_CPP_