
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
			Log4c::Warn(ROOT_MODULE, "在函数CDataBlockAllocator::Initial(),分配CDataBlock出错。");
		}
	}
	m_ShareDataBlocks.clear();
	m_AllocedShareDB.clear();
	if(bIniShareDB)
	{
		//1M
		long lSize = 1048576;
		uchar* pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//2M
		lSize = 1048576*2;
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//4M
		lSize = 1048576*4;
		pByte = new uchar[lSize];
		m_ShareDataBlocks.insert(ShareDBPair(lSize,pByte));
		//8M
		lSize = 1048576*8;
		pByte = new uchar[lSize];
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
	//char str[256];
	map<long,long>::iterator itTest = m_MapTest.begin();
	for(;itTest != m_MapTest.end();itTest++)
	{
		if((*itTest).second != 0)
		{
// 			sprintf(str,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
// 			PutStringToFile("DBAllocTest",str);

            Log4c::Trace(ROOT_MODULE,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
		}
	}
}

void CDataBlockAllocator::PutAllocInfo(const char* pszFileName)
{
	EnterCriticalSection(&m_CSFreeDB);
	//char str[256];
	map<long,long>::iterator itTest = m_MapTest.begin();
	for(;itTest != m_MapTest.end();itTest++)
	{
		if((*itTest).second != 0)
		{
// 			sprintf(str,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
// 			PutStringToFile(pszFileName,str);

            Log4c::Trace(ROOT_MODULE,"Flag:%d,Count:%d",(*itTest).first,(*itTest).second);
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
		pDB = new CDataBlock(m_nDBSize);
		if(NULL==pDB)
			Log4c::Warn(ROOT_MODULE, "警告， 没有足够的内存！");
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
uchar* CDataBlockAllocator::AllockShareDB(long lSize)
{
	uchar *pData = NULL;
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
		pData = new uchar[lSize];
		Log4c::Warn(ROOT_MODULE,"waring,When call DataBlockAllocator::AllockShareDB(%d),not find the datablock",lSize);
	}
	return pData;
}
//释放数据块
void CDataBlockAllocator::FreeShareDB(uchar* pData)
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


/*
* 使用内存池管理的代码

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
InitializeCriticalSection(&m_CSFreeDB);
}

void CDataBlockAllocator::Release()
{
DeleteCriticalSection(&m_CSFreeDB);
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
CDataBlock *pDB = NULL;
EnterCriticalSection(&m_CSFreeDB);
pDB = OBJ_CREATE_PVOID(CDataBlock, (void*)m_nDBSize);
if(pDB)
{
pDB->m_lTestFlag = lTestFlag;
pDB->SetCurSize(0);
}
LeaveCriticalSection(&m_CSFreeDB);
return pDB;
}
void CDataBlockAllocator::FreeDB(CDataBlock* pDB)
{
EnterCriticalSection(&m_CSFreeDB);
if(NULL != pDB)	
OBJ_RELEASE(CDataBlock, pDB);
LeaveCriticalSection(&m_CSFreeDB);
}

//申请共享数据块
uchar* CDataBlockAllocator::AllockShareDB(long lSize)
{
long *pData = (long*)M_ALLOC(lSize);
if(NULL != pData)
pData[0] = lSize;
return (uchar*)&pData[1];
}
//释放数据块
void CDataBlockAllocator::FreeShareDB(uchar* pData)
{
if(NULL == pData)
return;
long *pTrueData = (long*)pData - 1;
M_FREE(pTrueData, pTrueData[0]);
}

*/
