
/*
*	file:		pool.h
*	Brief:		非线程安全内存池MemPool
*	detail:		相关配置定义参见pooldef.h文件
*				
*				
*	Authtor:	张科智
*	Datae:		2007
*/


#include "../BaseDef/pooldef.h"

#pragma once


//! 内存池类
/////////////////////////////////////////////////////////////////////////////
class MemPool
{
#pragma region 对象初始化

public:
	//! 构造函数
	//!	初始对象不预分配内存
	MemPool(void);
	//! 析构函数
	~MemPool(void);

public:
	//!				默认初始化对象
	//!				按默认的方式预分配内存
	void			DefaultInit(void);

	//!				指定一个尺寸的内存预先分配多少块
	bool			SetInitialTileNum(long lSize, long lNum);

	//!				释放所有内存
	void			Release(void);


#pragma  endregion


#pragma region 功能应用

public:
	//! 			得到一块指定大小的内存
	//! 			该内存被0填充
	void*			Alloc(long lSize);

	//!				释放一块指明大小的内存
	bool			Free(void *pBuf, long lSize);


#pragma  endregion


#pragma region 私有成员

private:
	//!				为某一索引内存池添加一块内存
	bool			AddBlock(long lIndex, long lTileNum = 0);

	//!				释放一个尺寸索引的所有内存块
	void			FreeAllBlockByIndex(long lIndex);

	//!				得到一个尺寸的索引
	long			GetSizeIndex(long lSize);

private:
	struct tagBlockNode
	{
		long lBlockSize;
	};

	struct tagHeap
	{
		long					_lTileNum;		//! 总片数
		long					_lLastAddNum;	//! 上一次添加的片数
		MyList<void>			_TileFreeList;	//! 未使用的内存片列表
		MyList<tagBlockNode>	_BlockList;		//! 已申请的内存块列表

		tagHeap(void):_lTileNum(0),_lLastAddNum(0){}
	};

	tagHeap						m_arrFreeList[MAX_TILE_BUF_SZIE];

#ifdef MEM_POOL_DEBUG
	set<LPVOID>					m_setBigMem;
#endif

#pragma endregion


};

