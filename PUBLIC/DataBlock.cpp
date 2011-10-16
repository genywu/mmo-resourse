//=============================================================================
/**
 *  file: DataBlock.cpp
 *
 *  Brief:有固定大小的内存快，动态分配数据块的基本机构。
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-6-13
 */
//=============================================================================
#include "StdAfx.h"
#include "DataBlock.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDataBlock::CDataBlock(long maxsize)
:m_nMaxSize(maxsize)
,m_nCurSize(0)
,m_bInitData(false)
{
	m_pBase = new BYTE[maxsize];
	if(NULL == m_pBase)
	{
		PutDebugString("In Function CDataBlock::CDataBlock(...),Alloc Memory Error。");
	}
}

CDataBlock::~CDataBlock(void)
{
	SAFE_DELETE_ARRAY(m_pBase);
}

void CDataBlock::SetCurSize(long lSize)
{
	m_nCurSize = (lSize > 0 && lSize <= m_nMaxSize) ? lSize : 0;
}

/*
* 使用内存池管理的代码

CDataBlock::CDataBlock(long maxsize)
:m_nMaxSize(maxsize)
,m_nCurSize(0)
,m_bInitData(false)
{
m_pBase = (BYTE*)M_ALLOC(m_nMaxSize);
if(NULL == m_pBase)
{
PutDebugString("在函数CDataBlock::CDataBlock(...),分配内存出错。");
}
}

CDataBlock::CDataBlock(void *maxsize)
:m_nMaxSize((long)maxsize)
,m_nCurSize(0)
,m_bInitData(false)
{
m_pBase = (BYTE*)M_ALLOC(m_nMaxSize);
if(NULL == m_pBase)
{
PutDebugString("在函数CDataBlock::CDataBlock(...),分配内存出错。");
}
}

CDataBlock::~CDataBlock(void)
{
M_FREE(m_pBase, m_nMaxSize);
}

void CDataBlock::SetCurSize(long lSize)
{
m_nCurSize = (lSize > 0 && lSize <= m_nMaxSize) ? lSize : 0;
}
*/