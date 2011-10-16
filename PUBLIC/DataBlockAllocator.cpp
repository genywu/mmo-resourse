
//=============================================================================
/**
 *  file: DataBlockAllocator.cpp
 *
 *  Brief:固定大小的内存快分配器
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-6-13
 */
//=============================================================================

#include "StdAfx.h"
#include "DataBlockAllocator.h"
#include "DataBlock.h"
#include "GUID.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDataBlockAllocator::CDataBlockAllocator(int nDBNum,int nDBSize,int bIniShareDB)
:m_nMaxDBNum(nDBNum)
,m_nDBSize(nDBSize)
{
	Initial(bIniShareDB);
	m_MapTest.clear();
}

CDataBlockAllocator::~CDataBlockAllocator(void)
{
	Release();
}

void CDataBlockAllocator::Initial(int bIniShareDB)
{
	int i = 0;
	CDataBlock* pDB = NULL;
	for(;i<m_nMaxDBNum;i++)
	{
		pDB = new CDataBlock(m_nDBSize);
		if(pDB)
		{
			pDB->SetIsInitData(true);
			m_FreeDataBlocks.push_back(pDB);
		}
		else
		{
			PutDebugString("in Function CDataBlockAllocator::Initial(),Alloc CDataBlock error。");
		}
	}
	m_ShareDataBlocks.clear();
	m_AllocedShareDB.clear();
	if(bIniShareDB)
	{
		//1M
		long lSize = 1048576;
		BYTE* pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//2M
		lSize = 1048576*2;
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//4M
		lSize = 1048576*4;
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//8M
		lSize = 1048576*8;
		pByte = new BYTE[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
	}

	InitializeCriticalSection(&m_CSFreeDB);
}

void CDataBlockAllocator::Release()
{
	itFreeDB it = m_FreeDataBlocks.begin();
	CDataBlock* pDB = NULL;
	for(;it != m_FreeDataBlocks.end();it++)
	{
		pDB = (*it);
		if(pDB) SAFE_DELETE(pDB);
	}
	m_FreeDataBlocks.clear();

	itShareDB itShare = m_ShareDataBlocks.begin();
	for(;itShare !=m_ShareDataBlocks.end();itShare++ )
	{
		delete [](*itShare).second;
	}
	m_ShareDataBlocks.clear();

	DeleteCriticalSection(&m_CSFreeDB);
	char str[256];
	map<long,long>::iterator itTest = m_MapTest.begin();
	for(;itTest != m_MapTest.end();itTest++)
	{
		if((*itTest).second != 0)
		{
			sprintf(str,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
			PutStringToFile("DBAllocTest",str);
		}
	}
}

void CDataBlockAllocator::PutAllocInfo(const char* pszFileName)
{
	EnterCriticalSection(&m_CSFreeDB);
	char str[256];
	map<long,long>::iterator itTest = m_MapTest.begin();
	for(;itTest != m_MapTest.end();itTest++)
	{
		if((*itTest).second != 0)
		{
			sprintf(str,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
			PutStringToFile(pszFileName,str);
		}
	}
	LeaveCriticalSection(&m_CSFreeDB);
}


CDataBlock*	CDataBlockAllocator::AllocDB(long lTestFlag)
{
	CDataBlock* pDB = NULL;

	EnterCriticalSection(&m_CSFreeDB);
	if(m_FreeDataBlocks.size() > 0)
	{
		pDB = m_FreeDataBlocks.front();
		m_FreeDataBlocks.pop_front();
	}
	else
	{
		//PutDebugString("警告， 缓存中没有足够的数据块！");
		pDB = new CDataBlock(m_nDBSize);
		if(NULL==pDB)
			PutDebugString("Warning， Not Enough Memory！");
	}

	if(pDB)
	{
		pDB->m_lTestFlag = lTestFlag;
		m_MapTest[lTestFlag]++;
	}
	LeaveCriticalSection(&m_CSFreeDB);

	if(pDB)
	{
		pDB->SetCurSize(0);
	}
	return pDB;
}
void CDataBlockAllocator::FreeDB(CDataBlock* pDB)
{
	if(NULL == pDB)	return;

	EnterCriticalSection(&m_CSFreeDB);
	m_MapTest[pDB->m_lTestFlag]--;
	if(!pDB->GetIsInitData())
	{
		SAFE_DELETE(pDB);
	}
	else
	{
		m_FreeDataBlocks.push_back(pDB);
	}
	LeaveCriticalSection(&m_CSFreeDB);
}

//申请共享数据块
BYTE* CDataBlockAllocator::AllockShareDB(long lSize)
{
	BYTE *pData = NULL;
	itShareDB it = m_ShareDataBlocks.begin();
	for(;it !=m_ShareDataBlocks.end();it++ )
	{
		if( lSize <= (*it).first )
		{
			pData = (*it).second;
			m_AllocedShareDB[pData] = (*it).first;
			it = m_ShareDataBlocks.erase(it);
			break;
		}
	}
	if(!pData)
	{
		pData = new BYTE[lSize];
		char szInfo[256]="";
		sprintf(szInfo,"waring,When call DataBlockAllocator::AllockShareDB(%d),not find the datablock",lSize);
		PutDebugString(szInfo);
	}
	return pData;
}
//释放数据块
void CDataBlockAllocator::FreeShareDB(BYTE* pData)
{
	itAllocR it = m_AllocedShareDB.find(pData);
	if(it != m_AllocedShareDB.end())
	{
		m_ShareDataBlocks.insert(ShareDBPair((*it).second,pData));
	}
	else
	{
		delete []pData;
	}
}
