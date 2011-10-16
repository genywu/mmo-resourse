
/*
*	file:		MultiObjPool.h
*	Brief:		线程安全内存池MultiObjPool
*	detail:		
*				为了提高并发性，
*				只有Alloc和Free两个接口，作了同步处理
*				其他接口还是需要线性调用
*				
*				
*				
*				为对象添加对象池提供了2对宏接口（分号结尾），作为是为类添加CreateObj和Release两个接口
*				
*				线程安全的（WIN32）：
*					在类申明中使用：ADD_OBJECT_MULTIPOOL_H（类名）
*					在CPP文件中：	ADD_OBJECT_MULTIPOOL_CPP（类名， 初始对象分配数量）
*				
*				非线程安全的：
*					在类申明中使用：ADD_OBJECT_POOL_H（类名）
*					在CPP文件中：	ADD_OBJECT_POOL_CPP（类名， 初始对象分配数量）
*				
*				
*				
*	Authtor:	张科智
*	Datae:		2007
*/

#include "pooldef.h"

#pragma once

//! 添加对象池宏（WIN32）
/////////////////////////////////////////////////////////////////////////////
//! 为类添加一个线程安全对象池需要调用两个宏：
//!		ADD_OBJECT_MULTIPOOL_H		在类定义中添加，需要分号结尾
//!		ADD_OBJECT_MULTIPOOL_CPP	在类定义文件中添加，需要分号结尾，参数lInitObjNum：初始分配的对象个数，设置小于0的值则不分配
//!
//!	这两段宏为一个类添加两个静态接口：
//!		CreateObj(BOOL bRunConstructor)：	创建一个对象，参数bRunConstructor表示是否调用默认构造函数
//!		Release(ClassName **ppObj):			回收一个对象，这里会调用析构函数


//! 申明宏
#define ADD_OBJECT_MULTIPOOL_H(ClassName)													\
public:																						\
	static ClassName*	CreateObj(BOOL bRunConstructor = TRUE){								\
	ClassName* pObj;																		\
	pObj = c_ObjPool.Multi_Alloc(LINK_OBJ_INFO(ClassName));															\
	if(NULL != pObj && bRunConstructor) new(pObj)ClassName();								\
	return pObj;																			\
		}																					\
		static VOID			Release(ClassName **ppObj, BOOL bRunDestructor = TRUE){			\
		if(NULL == ppObj || NULL == *ppObj)return;											\
		if(bRunDestructor)(*ppObj)->~ClassName();											\
		c_ObjPool.Multi_Free(*ppObj);														\
		*ppObj=NULL;																		\
		}																					\
private:																					\
	static MultiObjPool<ClassName, LockOfWin32> c_ObjPool				


//! 定义宏
#define ADD_OBJECT_MULTIPOOL_CPP(ClassName, lInitObjNum)									\
	MultiObjPool<ClassName, LockOfWin32> ClassName::c_ObjPool(lInitObjNum)


//! 添加对象池宏（非线程安全）
/////////////////////////////////////////////////////////////////////////////
//! 为类添加一个非线程安全对象池需要调用两个宏：
//!		ADD_OBJECT_MULTIPOOL_H		在类定义中添加，需要分号结尾
//!		ADD_OBJECT_MULTIPOOL_CPP	在类定义文件中添加，需要分号结尾，参数lInitObjNum：初始分配的对象个数，设置小于0的值则不分配
//!
//!	这两段宏为一个类添加两个静态接口：
//!		CreateObj(BOOL bRunConstructor)：	创建一个对象，参数bRunConstructor表示是否调用默认构造函数
//!		Release(ClassName **ppObj):			回收一个对象，这里会调用析构函数


//! 申明宏
#define ADD_OBJECT_POOL_H(ClassName)													\
public:																						\
	static ClassName*	CreateObj(BOOL bRunConstructor = TRUE){								\
	ClassName* pObj;																		\
	pObj = c_ObjPool.Multi_Alloc(LINK_OBJ_INFO(ClassName));															\
	if(NULL != pObj && bRunConstructor) new(pObj)ClassName();								\
	return pObj;																			\
		}																					\
		static VOID			Release(ClassName **ppObj, BOOL bRunDestructor = TRUE){			\
		if(NULL == ppObj || NULL == *ppObj)return;											\
		if(bRunDestructor)(*ppObj)->~ClassName();											\
		c_ObjPool.Multi_Free(*ppObj);														\
		*ppObj=NULL;																		\
		}																					\
private:																					\
	static MultiObjPool<ClassName, InvalidLock> c_ObjPool				


//! 定义宏
#define ADD_OBJECT_POOL_CPP(ClassName, lInitObjNum)									\
	MultiObjPool<ClassName, InvalidLock> ClassName::c_ObjPool(lInitObjNum)



//! 对象池类
/////////////////////////////////////////////////////////////////////////////
template<typename _MyObject, typename _MyLock>
class MultiObjPool
{
public:
	//! 构造函数
	MultiObjPool(LONG lInitObjNum = -1);
	//! 析构函数
	~MultiObjPool(VOID);

public:
	//! 			得到一块指定大小的内存
	//! 			该内存被0填充

	_MyObject*		Multi_Alloc(const char *pInfo);

	//!				释放一块指明大小的内存
	BOOL			Multi_Free(_MyObject *pObj);


	

private:
	//!				添加空闲对象
	BOOL			AddTileNum(LONG lObjNum = 0);

	//!				释放所有内存
	VOID			Release(VOID);






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

	tagHeap						m_ObjHeap;

#ifdef MEM_POOL_LOG
	//!							被分配出去的内存的信息
	map<LPVOID, string>			m_mapAllocMemInfo;
	_MyLock						m_setLogLock;
#endif

};



//!----------------------------------------------------------------------------------------------------------------
//! 构造函数
//!	初始对象不预分配内存
template<typename _MyObject, typename _MyLock>
MultiObjPool<_MyObject, _MyLock>::MultiObjPool(LONG lInitObjNum)
{
	if(0 > lInitObjNum)
		return;
	AddTileNum(lInitObjNum);
}
//! 析构函数
template<typename _MyObject, typename _MyLock>
MultiObjPool<_MyObject, _MyLock>::~MultiObjPool(VOID)
{
	Release();
}


//!----------------------------------------------------------------------------------------------------------------
//! 添加空闲对象
template<typename _MyObject, typename _MyLock>
BOOL MultiObjPool<_MyObject, _MyLock>::AddTileNum(LONG lObjNum)
{
	LONG lTileSize = sizeof(_MyObject);
	//! 对象大小超过块大小的一半
	if(2 > (USABLE_BLOCK_SIZE / lTileSize))
		return TRUE;

	//! 分配的具体块数取lMinTileNum的整数倍
	LONG lMinTileNum = USABLE_BLOCK_SIZE / lTileSize;
	lObjNum = (lMinTileNum > lObjNum) ? lMinTileNum : lObjNum;
	lObjNum = ((((lObjNum % lMinTileNum) == 0) ? 0 : 1) + lObjNum / lMinTileNum) * lMinTileNum;


	//! 实际分配的大小 = 块数量 * （可用块大小 + 块链表节点大小） + 大块内存的链表节点大小
	LONG lAllocSize = lObjNum * (lTileSize + ListNode<VOID>::GetAddedSize()) + ListNode<tagBlockNode>::GetAddedSize();

	void *pNewBuf = malloc(lAllocSize);
	if(NULL == pNewBuf)
		return FALSE;

	//! 内存块添加到管理
	ListNode<tagBlockNode> *pBlockNode = new(pNewBuf)ListNode<tagBlockNode>();
	tagBlockNode *p = pBlockNode->GetNodeData();
	p->lBlockSize = lAllocSize;

	m_ObjHeap._BlockList.PushNode(pBlockNode);
	m_ObjHeap._lTileNum += lObjNum;
	m_ObjHeap._lLastAddNum = lObjNum;

	//! 内存片添加到管理
	LPBYTE pTileNode = (LPBYTE)(pBlockNode->GetTile());

	for (LONG i = 0; i < lObjNum; ++i)
	{
		ListNode<VOID> *pTmpNode = (ListNode<VOID>*)(pTileNode + i * (lTileSize + ListNode<VOID>::GetAddedSize()));
		new(pTmpNode)ListNode<VOID>();
		m_ObjHeap._TileFreeList.PushNode((ListNode<VOID>*)pTmpNode);
	}

	return TRUE;
}

//!----------------------------------------------------------------------------------------------------------------
//! 释放所有内存
template<typename _MyObject, typename _MyLock>
VOID MultiObjPool<_MyObject, _MyLock>::Release(VOID)
{
#ifdef MEM_POOL_LOG
	m_setLogLock.Lock();
	if(0 != m_mapAllocMemInfo.size())
	{
		string strOutInfo("内存池析构报错：\r\n");

		for (map<LPVOID, string>::iterator ite = m_mapAllocMemInfo.begin(); m_mapAllocMemInfo.end() != ite; ++ite)
		{
			char szInfo[1024] = {0};
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

	//MyAssert(m_ObjHeap._lTileNum == m_ObjHeap._TileFreeList.Size());

	m_ObjHeap._TileFreeList.Release();
	ListNode<tagBlockNode> *pCurrNode = m_ObjHeap._BlockList.PopNode();
	while(NULL != pCurrNode)
	{
		pCurrNode->Rlease();
		free(pCurrNode);
		pCurrNode = m_ObjHeap._BlockList.PopNode();
	}
	m_ObjHeap._lTileNum = 0;
}

//!----------------------------------------------------------------------------------------------------------------
//! 得到一块指定大小的内存
//! 该内存被0填充
template<typename _MyObject, typename _MyLock>
_MyObject* MultiObjPool<_MyObject, _MyLock>::Multi_Alloc(const char *pInfo)
{
	LONG lSize = sizeof(_MyObject);
	//! 对象大小超过块大小的一半
	if(2 > (USABLE_BLOCK_SIZE / lSize))
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
		return (_MyObject*)pBigMem;
	}
	else
	{
		//! 对整个获取过程加锁，避免多次添加缓存块
		m_ObjHeap._FreeListLock.Lock();
		if( 0 == m_ObjHeap._TileFreeList.Size())
		{
			//! 如果空闲内存用完，则再分配新的大块
			if(!AddTileNum())
			{
				//! 内存耗尽
				m_ObjHeap._FreeListLock.UnLock();
				return NULL;
			}
		}

		//! 从空闲链表中删除一个节点，用0填充节点包含的内存
		VOID *pReBuf = m_ObjHeap._TileFreeList.PopNode()->GetTile();
		m_ObjHeap._FreeListLock.UnLock();

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
		memset(pReBuf, FILL_VALUE, lSize);
#endif

		return (_MyObject*)pReBuf;
	}
}


//!----------------------------------------------------------------------------------------------------------------
//! 释放一块指明大小的内存
template<typename _MyObject, typename _MyLock>
BOOL MultiObjPool<_MyObject, _MyLock>::Multi_Free(_MyObject *pObj)
{

#ifdef MEM_POOL_LOG
	m_setLogLock.Lock();
	MyAssert(m_mapAllocMemInfo.end() != m_mapAllocMemInfo.find(pObj));
	m_mapAllocMemInfo.erase(pObj);
	m_setLogLock.UnLock();
#endif

	LONG lSize = sizeof(_MyObject);
	//! 对象大小超过块大小的一半
	if(2 > (USABLE_BLOCK_SIZE / lSize))
	{
		try
		{
			free(pObj);
		}
		catch (...)
		{
			MyAssert(false);
			return FALSE;
		}
		return TRUE;
	}

	//! 将内存片节点归还空闲链表
	LPBYTE pByteBuf = (LPBYTE)pObj;
	pByteBuf = pByteBuf - ListNode<VOID>::GetAddedSize();
	m_ObjHeap._FreeListLock.Lock();
	m_ObjHeap._TileFreeList.PushNode((ListNode<VOID>*)pByteBuf);
	m_ObjHeap._FreeListLock.UnLock();

	return TRUE;
}


