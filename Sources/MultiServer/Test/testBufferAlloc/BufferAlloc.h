//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   BufferAlloc.h
//  Version     :   1.0
//  Creater     :   Freeway Chen 
//  Date        :   2003-6-26 15:03:30
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUFFERALLOC_H
#define _BUFFERALLOC_H  1

#include "windows.h"


// ===================================================================================
// 内部使用数据
extern long g_lBufferAllocInitCount;

int _BufferAlloc_Init_Internal();
int _BufferAlloc_UnInit_Internal();


// ===================================================================================
// 对外的程序接口

inline int BufferAlloc_Init()
{
    if (InterlockedIncrement(&g_lBufferAllocInitCount) == 1)
    {
        return _BufferAlloc_Init_Internal();
    }

    return true;
}

inline int BufferAlloc_UnInit()
{
    if (InterlockedDecrement(&g_lBufferAllocInitCount) == 0)
    {
        return _BufferAlloc_UnInit_Internal();
    }

    return true;
}

// 用来增加存放的最大空闲缓冲区的数目
int BufferAlloc_IncMaxFreeNodeCount(long lCount);

// 用来减少存放的最大空闲缓冲区的数目
int BufferAlloc_DecMaxFreeNodeCount(long lCount);

// 分配 uSize 大小的缓冲区
void *BufferAlloc_Alloc(size_t uSize);

// 释放 uSize 大小的缓冲区
int BufferAlloc_Free(void *pvBuffer, size_t uSize);

// ===================================================================================


#endif  // _BUFFERALLOC_H
