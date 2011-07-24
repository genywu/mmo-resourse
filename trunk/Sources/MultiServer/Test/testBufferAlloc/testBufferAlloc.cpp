// testBufferAlloc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#pragma  warning(disable: 4786)
#include <map>
#include "BufferAlloc.h"

using namespace std;

typedef multimap<unsigned __int64, char *> MultiMap;

// Add by Freeway Chen in 2003.6.29
inline unsigned __int64 XYToCoord(int X, int Y)
{
    unsigned __int64 ullResult;

    ((unsigned *)&ullResult)[0] = X;
    ((unsigned *)&ullResult)[1] = Y;
    
    return ullResult;
}

int main(int argc, char* argv[])
{
//	//printf("Hello World!\n");
//    MultiMap TestMap;
//    pair<MultiMap::iterator, MultiMap::iterator> TestRange;
//
//    TestMap.insert(MultiMap::value_type(XYToCoord(0,1), "test11"));
//    TestMap.insert(MultiMap::value_type(1, "test12"));
//    TestMap.insert(MultiMap::value_type(1, "test13"));
//    TestMap.insert(MultiMap::value_type(2, "test21"));
//    TestMap.insert(MultiMap::value_type(2, "test22"));
//    TestMap.insert(MultiMap::value_type(2, "test23"));
//
//    MultiMap::iterator pPos;
//    
//    // 可以先调用  TestMap.lower_bound判断是否存在，然后再调用TestMap.upper_bound，取得上限
//     
//    TestRange = TestMap.equal_range(1);
//
//    for (pPos = TestRange.first; pPos != TestRange.second; ++pPos)
//    {
//        printf("pair<%I64d, %s>\n", pPos->first, pPos->second);
//    }
//    printf("\n", pPos->first, pPos->second);
//
//    TestRange = TestMap.equal_range(2);
//
//    for (pPos = TestRange.first; pPos != TestRange.second; ++pPos)
//    {
//        printf("pair<%I64d, %s>\n", pPos->first, pPos->second);
//    }
//    printf("\n", pPos->first, pPos->second);
//
//    TestRange = TestMap.equal_range(3);
//
//    for (pPos = TestRange.first; pPos != TestRange.second; ++pPos)
//    {
//        printf("pair<%I64d, %s>\n", pPos->first, pPos->second);
//    }
//    printf("\n", pPos->first, pPos->second);
//
//
//    return true;

    unsigned char *pTestArray[1024 * 64];
    int i = 0;

    int dwTickCount1 = 0;
    int dwTickCount2 = 0;
    int dwTickCount3 = 0;
    int dwTickCount4 = 0;

    BufferAlloc_Init();

    dwTickCount1 = GetTickCount();

    for (i = 0; i < 1024 * 64; i++)
    {
        pTestArray[i] = (unsigned char *)BufferAlloc_Alloc(1024);
    }
    
    for (i = 0; i < 1024 * 64; i++)
    {
        BufferAlloc_Free(pTestArray[i], 1024);
    }

    for (i = 0; i < 1024 * 64; i++)
    {
        pTestArray[i] = (unsigned char *)BufferAlloc_Alloc(1024);
    }
    
    for (i = 0; i < 1024 * 64; i++)
    {
        BufferAlloc_Free(pTestArray[i], 1024);
    }

    dwTickCount2 = GetTickCount();


    BufferAlloc_UnInit();


    dwTickCount3 = GetTickCount();
    for (i = 0; i < 1024 * 64; i++)
    {
        pTestArray[i] = new unsigned char[1024];
    }
    
    for (i = 0; i < 1024 * 64; i++)
    {
        delete []pTestArray[i];
    }

    for (i = 0; i < 1024 * 64; i++)
    {
        pTestArray[i] = new unsigned char[1024];
    }
    
    for (i = 0; i < 1024 * 64; i++)
    {
        delete []pTestArray[i];
    }

    dwTickCount4 = GetTickCount();

    printf(
        "First TickCount = %d, Second TickCount = %d\n", 
        dwTickCount2 - dwTickCount1,
        dwTickCount4 - dwTickCount3
    );




	return 0;
}

