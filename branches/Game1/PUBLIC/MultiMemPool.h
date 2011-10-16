
/*
*	file:		MultiMemPool.h
*	Brief:		线程安全内存池MultiMemPool
*	detail:		MultiMemPool是在MemPool的基础上修改获得
*
*				为了提高并发性，
*				只有Alloc和Free两个接口，作了同步处理
*				其他接口还是需要线性调用
*				
*				本文件还实现了一个基本的Win32锁类型LockOfWin32，可以直接放入MultiMemPool模板。
*				如需其他实现，请按照LockOfWin32编写
*				
*				
*	Authtor:	张科智
*	Datae:		2007
*/

#include "pooldef.h"

#pragma once


//! 线程安全内存池类
//! 只有Multi_前缀的接口是线程安全的
/////////////////////////////////////////////////////////////////////////////
template<typename _MyLock>
class MultiMemPool
{
#pragma region 对象初始化

public:
	//! 构造函数
	//!	初始对象不预分配内存
	MultiMemPool(VOID);
	//! 析构函数
	~MultiMemPool(VOID);

public:
	//!				默认初始化对象
	//!				按默认的方式预分配内存
	VOID			DefaultInit(VOID);

	//!				指定一个尺寸的内存预先分配多少块
	BOOL			SetInitialTileNum(LONG lSize, LONG lNum);

private:
	//!				释放所有内存
	VOID			Release(VOID);



#pragma  endregion


#pragma region 功能应用

public:
	//! 			得到一块指定大小的内存
	//! 			该内存被0填充
	VOID*			Multi_Alloc(LONG lSize, const char *pInfo);

	//!				释放一块指明大小的内存
	BOOL			Multi_Free(VOID *pBuf, LONG lSize);


	VOID			OutInfo(VOID);


#pragma  endregion


#pragma region 私有成员

private:
	//!				为某一索引内存池添加一块内存
	BOOL			AddBlock(LONG lIndex, LONG lTileNum = 0);

	//!				释放一个尺寸索引的所有内存块
	VOID			FreeAllBlockByIndex(LONG lIndex);

	//!				得到一个尺寸的索引
	LONG			GetSizeIndex(LONG lSize);

private:
	struct tagBlockNode
	{
		LONG lBlockSize;
	};

	struct tagHeap
	{
		LONG					_lTileNum;		//! 总片数
		LONG					_lLastAddNum;	//! 上一次添加的片数
		MyList<VOID>			_TileFreeList;	//! 未使用的内存片列表
		_MyLock					_FreeListLock;
		MyList<tagBlockNode>	_BlockList;		//! 已申请的内存块列表

		tagHeap(VOID):_lTileNum(0),_lLastAddNum(0){}
	};

	tagHeap						m_arrFreeList[MAX_TILE_BUF_SZIE];


#ifdef MEM_POOL_LOG
	//!							被分配出去的内存的信息
	map<LPVOID, string>			m_mapAllocMemInfo;
	_MyLock						m_setLogLock;
#endif

#pragma endregion


};


//! MultiMemPool定义
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4311;disable:4291)



#pragma region 对象初始化


//!----------------------------------------------------------------------------------------------------------------
//! 构造函数
//!	初始对象不预分配内存
template<typename _MyLock>
MultiMemPool<_MyLock>::MultiMemPool(VOID)
{

}
//! 析构函数
template<typename _MyLock>
MultiMemPool<_MyLock>::~MultiMemPool(VOID)
{
	Release();
}

//!----------------------------------------------------------------------------------------------------------------
//! 默认初始化对象
//! 按默认的方式预分配内存
template<typename _MyLock>
VOID MultiMemPool<_MyLock>::DefaultInit(VOID)
{
	for(LONG i = 0; i < MAX_TILE_BUF_SZIE; ++i)
	{
		AddBlock(i);
	}
}

//!----------------------------------------------------------------------------------------------------------------
//! 指定一个尺寸的内存预先分配多少块
template<typename _MyLock>
BOOL MultiMemPool<_MyLock>::SetInitialTileNum(LONG lSize, LONG lNum)
{
	return AddBlock(GetSizeIndex(lSize), lNum);
}

//!----------------------------------------------------------------------------------------------------------------
//! 释放所有内存
template<typename _MyLock>
VOID MultiMemPool<_MyLock>::Release(VOID)
{
#ifdef MEM_POOL_LOG
	m_setLogLock.Lock();
	if(0 != m_mapAllocMemInfo.size())
	{
		string strOutInfo("内存池析构报错：\r\n");

		for (map<LPVOID, string>::iterator ite = m_mapAllocMemInfo.begin(); m_mapAllocMemInfo.end() != ite; ++ite)
		{
			char szInfo[10240] = {0};
			sprintf(szInfo, "	未释放内存：地址[%x]、相关信息[%s]\r\n", (DWORD)ite->first, ite->second.c_str());
			strOutInfo.append(szInfo);
		}

		char szFileName[128] = {0};
		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		sprintf(szFileName, "log/memory_leak_%04d-%02d-%02d.txt", stTime.wYear, stTime.wMonth, stTime.wDay);
		OverloadFile(szFileName, strOutInfo.c_str());
		m_mapAllocMemInfo.clear();
	}
	m_setLogLock.UnLock();
#endif

#ifdef MEM_POOL_DEBUG
	//m_setBigMemLock.Lock();
	//MyAssert(0 == m_setBigMem.size());
	//m_setBigMemLock.UnLock();
#endif
	for(LONG i = 0; i < MAX_TILE_BUF_SZIE; ++i)
	{
		FreeAllBlockByIndex(i);
	}
}


#pragma  endregion


#pragma region 功能应用

//!----------------------------------------------------------------------------------------------------------------
//! 得到一块指定大小的内存
//! 该内存被0填充
template<typename _MyLock>
VOID* MultiMemPool<_MyLock>::Multi_Alloc(LONG lSize, const char *pInfo)
{
	if(MAX_TILE_BUF_SZIE * MIN_BUF_SIZE_DIFFERENCE < lSize)
	{
		LPVOID pBigMem = malloc(lSize);


#ifdef MEM_POOL_LOG
		m_setLogLock.Lock();
		MyAssert(m_mapAllocMemInfo.end() == m_mapAllocMemInfo.find(pBigMem));
		if(NULL == pInfo)
			m_mapAllocMemInfo[pBigMem] = string("无创建信息");
		else
			m_mapAllocMemInfo[pBigMem] = string(pInfo);
		m_setLogLock.UnLock();
#endif
		return pBigMem;
	}
	else
	{
		LONG lIndex = GetSizeIndex(lSize);
		MyAssert(-1 != lIndex);

		//! 对整个获取过程加锁，避免多次添加缓存块
		m_arrFreeList[lIndex]._FreeListLock.Lock();
		if( 0 == m_arrFreeList[lIndex]._TileFreeList.Size())
		{
			//! 如果空闲内存用完，则再分配新的大块
			if(!AddBlock(lIndex))
			{
				//! 内存耗尽
				return NULL;
			}
		}

		//! 从空闲链表中删除一个节点，用0填充节点包含的内存
		VOID *pReBuf = m_arrFreeList[lIndex]._TileFreeList.PopNode()->GetTile();
		m_arrFreeList[lIndex]._FreeListLock.UnLock();

#ifdef MEM_POOL_LOG
		m_setLogLock.Lock();
		MyAssert(m_mapAllocMemInfo.end() == m_mapAllocMemInfo.find(pReBuf));
		if(NULL == pInfo)
			m_mapAllocMemInfo[pReBuf] = string("无创建信息");
		else
			m_mapAllocMemInfo[pReBuf] = string(pInfo);
		m_setLogLock.UnLock();
#endif

#ifdef FILL_MEM
		memset(pReBuf, FILL_VALUE, (lIndex * MIN_BUF_SIZE_DIFFERENCE));
#endif

		return pReBuf;
	}
}

template<typename _MyLock>
VOID MultiMemPool<_MyLock>::OutInfo(VOID)
{
#ifdef MEM_POOL_LOG
	char szInfo[512 * 64] = {0};
	strcat(szInfo, "\r\n");
	for (LONG i = 0; i < MAX_TILE_BUF_SZIE; ++i)
	{
		tagHeap &memHeap = m_arrFreeList[i];
		if(0 == memHeap._lTileNum)
			continue;

		memHeap._FreeListLock.Lock();

		char szTmp[512] = {0};
		sprintf(szTmp, "内存堆[Size:%d]：总快数[%d]、总片数[%d]、未使用片数[%d]\r\n", MIN_BUF_SIZE_DIFFERENCE * (i + 1), memHeap._BlockList.Size(), memHeap._lTileNum, memHeap._TileFreeList.Size());
		strcat(szInfo, szTmp);

		memHeap._FreeListLock.UnLock();
	}

	PutStringToFile("MemAllocInfo", szInfo);
#endif
}

//!----------------------------------------------------------------------------------------------------------------
//! 释放一块指明大小的内存
template<typename _MyLock>
BOOL MultiMemPool<_MyLock>::Multi_Free(VOID *pBuf, LONG lSize)
{
#ifdef MEM_POOL_LOG
	m_setLogLock.Lock();
	MyAssert(m_mapAllocMemInfo.end() != m_mapAllocMemInfo.find(pBuf));
	m_mapAllocMemInfo.erase(pBuf);
	m_setLogLock.UnLock();
#endif

	if(MAX_TILE_BUF_SZIE * MIN_BUF_SIZE_DIFFERENCE < lSize)
	{
		try
		{
			free(pBuf);
		}
		catch (...)
		{
			MyAssert(false);
			return FALSE;
		}
		return TRUE;
	}

	LONG lIndex = GetSizeIndex(lSize);
	MyAssert(-1 != lIndex);
	tagHeap &MemHeap = m_arrFreeList[lIndex];

	//! 将内存片节点归还空闲链表
	LPBYTE pByteBuf = (LPBYTE)pBuf;
	pByteBuf = pByteBuf - ListNode<VOID>::GetAddedSize();
	MemHeap._FreeListLock.Lock();
	MemHeap._TileFreeList.PushNode((ListNode<VOID>*)pByteBuf);
	MemHeap._FreeListLock.UnLock();

	return TRUE;
}


#pragma  endregion


#pragma region 私有成员

//!----------------------------------------------------------------------------------------------------------------
//! 为某一尺寸内存池添加一块内存
template<typename _MyLock>
BOOL MultiMemPool<_MyLock>::AddBlock(LONG lIndex, LONG lTileNum)
{
	if(MAX_TILE_BUF_SZIE <= lIndex || 0 > lIndex) return FALSE;
	tagHeap &MemHeap = m_arrFreeList[lIndex];

	LONG lTileSize = MIN_BUF_SIZE_DIFFERENCE * (lIndex + 1);

	//! 分配的具体块数取lMinTileNum的整数倍
	LONG lMinTileNum = USABLE_BLOCK_SIZE / lTileSize;
	lTileNum = (lMinTileNum > lTileNum) ? lMinTileNum : lTileNum;
	lTileNum = ((((lTileNum % lMinTileNum) == 0) ? 0 : 1) + lTileNum / lMinTileNum) * lMinTileNum;


	//! 实际分配的大小 = 块数量 * （可用块大小 + 块链表节点大小） + 大块内存的链表节点大小
	LONG lAllocSize = lTileNum * (lTileSize + ListNode<VOID>::GetAddedSize()) + ListNode<tagBlockNode>::GetAddedSize();

	void *pNewBuf = malloc(lAllocSize);
	if(NULL == pNewBuf)
		return FALSE;

	//! 内存块添加到管理
	ListNode<tagBlockNode> *pBlockNode = new(pNewBuf)ListNode<tagBlockNode>();
	tagBlockNode *p = pBlockNode->GetNodeData();
	p->lBlockSize = lAllocSize;

	MemHeap._BlockList.PushNode(pBlockNode);
	MemHeap._lTileNum += lTileNum;
	MemHeap._lLastAddNum = lTileNum;

	//! 内存片添加到管理
	LPBYTE pTileNode = (LPBYTE)(pBlockNode->GetTile());

	for (LONG i = 0; i < lTileNum; ++i)
	{
		ListNode<VOID> *pTmpNode = (ListNode<VOID>*)(pTileNode + i * (lTileSize + ListNode<VOID>::GetAddedSize()));
		new(pTmpNode)ListNode<VOID>();
		MemHeap._TileFreeList.PushNode((ListNode<VOID>*)pTmpNode);
	}

	return TRUE;
}

//!----------------------------------------------------------------------------------------------------------------
//! 释放一个尺寸索引的所有内存块
template<typename _MyLock>
VOID MultiMemPool<_MyLock>::FreeAllBlockByIndex(LONG lIndex)
{
	MyAssert(MAX_TILE_BUF_SZIE > lIndex);
	tagHeap &MemHeap = m_arrFreeList[lIndex];

	//MyAssert(MemHeap._lTileNum == MemHeap._TileFreeList.Size());

	MemHeap._TileFreeList.Release();
	ListNode<tagBlockNode> *pCurrNode = MemHeap._BlockList.PopNode();
	while(NULL != pCurrNode)
	{
		pCurrNode->Rlease();
		free(pCurrNode);
		pCurrNode = MemHeap._BlockList.PopNode();
	}
	MemHeap._lTileNum = 0;
}

//!----------------------------------------------------------------------------------------------------------------
//! 得到一个尺寸的索引
template<typename _MyLock>
inline LONG MultiMemPool<_MyLock>::GetSizeIndex(LONG lSize)
{
	if(MAX_TILE_BUF_SZIE * MIN_BUF_SIZE_DIFFERENCE < lSize)
		return -1;
	MyAssert(0 < lSize);
	return lSize / MIN_BUF_SIZE_DIFFERENCE - ((0 == (lSize % MIN_BUF_SIZE_DIFFERENCE)) ? 1 : 0);
}


