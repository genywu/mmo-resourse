//=============================================================================
/**
 *  file: DataBlockAllocator.h
 *
 *  Brief:固定大小的内存快分配器
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-6-13
 */
//=============================================================================

#ifndef __DATABLOCK_ALLOCATOR__H__
#define __DATABLOCK_ALLOCATOR__H__

#pragma once

class CDataBlock;
class CGUID;

struct tagDBOpParam
{
	//当前读写数据块编号
	ulong	nCurNum;
	//当前读写数据块的大小
	ulong	nCurDBSize;
	//当前读写数据的位置
	ulong	nCurPos;
	//当前读写数据块的指针
	uchar	*pDBPtr;
};

class CDataBlockAllocator
{
public:
	CDataBlockAllocator(int nDBNum,int nDBSize,int bIniShareDB = false);
public:
	~CDataBlockAllocator(void);

private:
	typedef list<CDataBlock*>	FreeDataBlocks;
	typedef list<CDataBlock*>::iterator itFreeDB;

	FreeDataBlocks m_FreeDataBlocks;
	CRITICAL_SECTION m_CSFreeDB;
	int	m_nMaxDBNum;
	const int	m_nDBSize;
	//用在逻辑层临时使用的数据块
	//个数不多，使用较大的数据块来满足各种大小的需求
	typedef multimap<long,uchar*>	MMapShareDB;
	typedef pair <long, uchar*> ShareDBPair;
	typedef multimap<long,uchar*>::iterator itShareDB;
	MMapShareDB m_ShareDataBlocks;

	typedef map<uchar*,long>	mapAllocedRecord;
	typedef map<uchar*,long>::iterator itAllocR;
	mapAllocedRecord	m_AllocedShareDB;
	//允许的最大共享数据块个数
	map<long,long>	m_MapTest;
private:
	void Initial(int bIniShareDB = false);
	void Release();

public:
	CDataBlock*	AllocDB(long lTestFlag=0);
	void FreeDB(CDataBlock*);

	uchar* AllockShareDB(long lSize);
	void FreeShareDB(uchar* pData);

	void PutAllocInfo(const char* pszFileName);
};

#endif
