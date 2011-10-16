
/*
*	file:		pool.h
*	Brief:		非线程安全内存池MemPool
*	detail:		相关配置定义参见pooldef.h文件
*				
*				
*	Authtor:	张科智
*	Datae:		2007
*/


#include "pooldef.h"

#pragma once


//! 内存池类
/////////////////////////////////////////////////////////////////////////////
class MemPool
{
#pragma region 对象初始化

public:
	//! 构造函数
	//!	初始对象不预分配内存
	MemPool(VOID);
	//! 析构函数
	~MemPool(VOID);

public:
	//!				默认初始化对象
	//!				按默认的方式预分配内存
	VOID			DefaultInit(VOID);

	//!				指定一个尺寸的内存预先分配多少块
	BOOL			SetInitialTileNum(LONG lSize, LONG lNum);

	//!				释放所有内存
	VOID			Release(VOID);


#pragma  endregion


#pragma region 功能应用

public:
	//! 			得到一块指定大小的内存
	//! 			该内存被0填充
	VOID*			Alloc(LONG lSize);

	//!				释放一块指明大小的内存
	BOOL			Free(VOID *pBuf, LONG lSize);


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
		MyList<tagBlockNode>	_BlockList;		//! 已申请的内存块列表

		tagHeap(VOID):_lTileNum(0),_lLastAddNum(0){}
	};

	tagHeap						m_arrFreeList[MAX_TILE_BUF_SZIE];

#ifdef MEM_POOL_DEBUG
	set<LPVOID>					m_setBigMem;
#endif

#pragma endregion


};

