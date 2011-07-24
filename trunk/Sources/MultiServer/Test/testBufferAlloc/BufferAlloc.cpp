//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   BufferAlloc.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-6-26 15:04:15
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <crtdbg.h>
#include "BufferAlloc.h"

#include <vector>
using namespace std;

#define defMAX_BUFFER_ALLOC_ITEM_NUM    64

long g_lBufferAllocInitCount = 0;
CRITICAL_SECTION g_BufferAllocCrit;
int g_BufferAllocCritInitFlag = false;

HANDLE g_hBufferAllocHeap = NULL;

// 用来存放最大释放节点的数目
long g_lMaxFreeNodeCount = 1024 * 64;
long g_lFreeNodeCount    = 0;

// 定义 第 n 个数据指针的一个动态数组 
typedef vector<unsigned char *> BufferAllocFreeItemType;

// 用来存放一个所有数据的数组，数组的每一项表示，指向 第n K的一个动态数组
// 为什么不用静态分配数组，而是使用动态分配，主要是担心有些全局的类已经使用该数组，但是数组还没有初始化
BufferAllocFreeItemType *g_pBufferAllocFreeArray = NULL;

int _BufferAlloc_Init_Internal()
{
    int nResult = false;
    
    InitializeCriticalSection(&g_BufferAllocCrit);
    g_BufferAllocCritInitFlag = true;

    EnterCriticalSection(&g_BufferAllocCrit);

    g_hBufferAllocHeap = HeapCreate(HEAP_NO_SERIALIZE, 1024 * 1024 * 4, 0);
    if (!g_hBufferAllocHeap)
        goto Exit0;

    try
    {
        g_pBufferAllocFreeArray = new BufferAllocFreeItemType[defMAX_BUFFER_ALLOC_ITEM_NUM];
    }
    catch (...) 
    {
        goto Exit0;
    }

    nResult = true;
Exit0:

    if (!nResult)
    {
        if (g_hBufferAllocHeap)
        {
            HeapDestroy(g_hBufferAllocHeap);
            g_hBufferAllocHeap = NULL;
        }
    }

    LeaveCriticalSection(&g_BufferAllocCrit);

    if (!nResult)
    {
        if (g_BufferAllocCritInitFlag)
        {
            DeleteCriticalSection(&g_BufferAllocCrit);
            g_BufferAllocCritInitFlag = false;
        }
    }

    return nResult;
}

int _BufferAlloc_UnInit_Internal()
{
    if (!g_BufferAllocCritInitFlag)
        return true;

    EnterCriticalSection(&g_BufferAllocCrit);

    if (g_pBufferAllocFreeArray)
    {
        for (int i = 0; i < defMAX_BUFFER_ALLOC_ITEM_NUM; i++)
        {
            BufferAllocFreeItemType::iterator pPos;
            for (pPos = g_pBufferAllocFreeArray[i].begin(); pPos != g_pBufferAllocFreeArray[i].end(); pPos++)
            {
                HeapFree(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, *pPos);
                *pPos = NULL;
            }
            
            g_pBufferAllocFreeArray[i].clear();
        }

        delete []g_pBufferAllocFreeArray;
        g_pBufferAllocFreeArray = NULL;
    }

    LeaveCriticalSection(&g_BufferAllocCrit);

    if (g_hBufferAllocHeap)
    {
        HeapDestroy(g_hBufferAllocHeap);
        g_hBufferAllocHeap = NULL;
    }

    DeleteCriticalSection(&g_BufferAllocCrit);
    g_BufferAllocCritInitFlag = false;

    return false;
}

int BufferAlloc_IncMaxFreeNodeCount(long lCount)
{
    EnterCriticalSection(&g_BufferAllocCrit);
    
    g_lMaxFreeNodeCount += lCount;
    
    LeaveCriticalSection(&g_BufferAllocCrit);

    return true;
}

int BufferAlloc_DecMaxFreeNodeCount(long lCount)
{
    EnterCriticalSection(&g_BufferAllocCrit);
    
    g_lMaxFreeNodeCount -= lCount;
    
    LeaveCriticalSection(&g_BufferAllocCrit);

    return true;
}

void *BufferAlloc_Alloc(size_t uSize)
{
    _ASSERT(uSize > 0);
    if (uSize == 0)
        return NULL;

    // 将分配的字节转换为以 K 为单位
    size_t uAllocKSize = (uSize + (1024 - 1)) / 1024;

    void *pvResult = NULL;
    
    // 由于空闲数组的下标从 0 开始，因此需要 - 1
    size_t uAllocFreeIndex = uAllocKSize - 1;

    EnterCriticalSection(&g_BufferAllocCrit);

    if (uAllocKSize > defMAX_BUFFER_ALLOC_ITEM_NUM)
    {
        // 如果分配的数据大小超过  defMAX_BUFFER_ALLOC_ITEM_NUM(64) K，直接向系统分配
        pvResult = HeapAlloc(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, uAllocKSize * 1024);
        goto Exit1;
    }

    if (g_lFreeNodeCount == 0)
    {
        // 如果空闲列表中没有多余的节点，就直接向系统申请内存
        pvResult = HeapAlloc(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, uAllocKSize * 1024);
        goto Exit1;
    }

    if (g_pBufferAllocFreeArray[uAllocFreeIndex].empty())
    {
        // 如果找不到合适的空间，就直接申请分配
        pvResult = HeapAlloc(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, uAllocKSize * 1024);
        goto Exit1;
    }

    // 如果有空闲的缓冲区

    g_lFreeNodeCount--; // 减少空闲缓冲区的计数
    //InterlockedDecrement(&g_lFreeNodeCount);

    
    pvResult = g_pBufferAllocFreeArray[uAllocFreeIndex].back();
    g_pBufferAllocFreeArray[uAllocFreeIndex].pop_back();

Exit1:
    LeaveCriticalSection(&g_BufferAllocCrit);


    return pvResult;

}

// 释放 uSize 大小的缓冲区
int BufferAlloc_Free(void *pvBuffer, size_t uSize)
{
    _ASSERT(pvBuffer);
    if (!pvBuffer)
        return true;

    _ASSERT(uSize > 0);
    if (uSize == 0)
        return true;

    // 将分配的字节转换为以 K 为单位
    size_t uAllocKSize = (uSize + (1024 - 1)) / 1024;

    int nResult = false;

    // 由于空闲数组的下标从 0 开始，因此需要 - 1
    size_t uAllocFreeIndex = uAllocKSize - 1;

    EnterCriticalSection(&g_BufferAllocCrit);

    if (uAllocKSize > defMAX_BUFFER_ALLOC_ITEM_NUM)
    {
        // 如果分配的数据大小超过  defMAX_BUFFER_ALLOC_ITEM_NUM(64) K，直接释放内存
        HeapFree(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, pvBuffer);
        goto Exit1;
    }

    if (g_lFreeNodeCount >= g_lMaxFreeNodeCount)
    {
        // 如果空闲列表中已经达到最大值，直接释放内存
        HeapFree(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, pvBuffer);
        goto Exit1;
    }

    try
    {
        g_pBufferAllocFreeArray[uAllocFreeIndex].push_back((unsigned char *)pvBuffer);
    }
    catch (...)
    {
        goto Exit0;
    }
                    
    g_lFreeNodeCount++; // 增加空闲缓冲区的计数
    //InterlockedIncrement(&g_lFreeNodeCount);

Exit1:
    nResult = true;
Exit0:
    
    if (!nResult)
    {
        // 空闲缓冲区放置不成功，就直接释放内存
        HeapFree(g_hBufferAllocHeap, HEAP_NO_SERIALIZE, pvBuffer);
        nResult = true;
    }
    LeaveCriticalSection(&g_BufferAllocCrit);

    return nResult;
}


class KHeap 
{
private:
    HANDLE m_hHeap;

    #ifdef _DEBUG
    long   m_lAllocCount;
    long   m_lFreeCount;
    #endif

public:
    KHeap(size_t uInitialSize = 0); 
    ~KHeap();
    
    void *Alloc(size_t uSize);
    void Free(void *pvBuffer, size_t uSize);
};

inline KHeap::KHeap(size_t uInitialSize)
{
    #ifdef _DEBUG
    m_lAllocCount = 0;
    m_lFreeCount  = 0;
    #endif

    m_hHeap = HeapCreate(0, uInitialSize, 0);

    _ASSERT(m_hHeap);
}

inline KHeap::~KHeap()
{
    #ifdef _DEBUG
    _ASSERT(m_lAllocCount == m_lFreeCount);
    #endif

    if (m_hHeap)
    {
        HeapDestroy(m_hHeap);
        m_hHeap = NULL;
    }
}

inline void *KHeap::Alloc(size_t uSize)
{
    if (!m_hHeap)
        return NULL;
        
    #ifdef _DEBUG
    InterlockedIncrement(&m_lAllocCount);
    #endif

    return HeapAlloc(m_hHeap, 0, uSize);
}

inline void KHeap::Free(void *pvBuffer, size_t uSize)
{
    if (!m_hHeap)
    {
        _ASSERT(false);
        return;
    }

    uSize = uSize;      // make compiler no warning
    
    HeapFree(m_hHeap, 0, pvBuffer);

    #ifdef _DEBUG
    InterlockedIncrement(&m_lFreeCount);
    #endif
}
