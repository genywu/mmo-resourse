// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#include <stdio.h>
#include <tchar.h>

#include <process.h>
#include <Windows.h>
#include <assert.h>
#include "time.h"

#include <string>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <memory>


#include <vector>
using namespace std;


#define SHRINK_SCALE 8
#include "..\MemManage\MemTools.h"


class AAA
{
public:
	AAA(){pLong1 = (long*)M_ALLOC(sizeof(long) * 8);}
	virtual ~AAA(){M_FREE(pLong1, sizeof(long) * 8);}
	long* pLong1;
};

class BBB :public AAA
{
public:
	BBB(){pLong2 = (long*)M_ALLOC(sizeof(long) * 8);}
	virtual ~BBB(){M_FREE(pLong2, sizeof(long) * 8);printf("~BBB()\r\n");}
	char arr[128];
	string str;
	long* pLong2;
};

class CCC
{
public:
	CCC(void *pBBB)
	{
		m_pBBB = (BBB*)pBBB;
	}
	BBB *m_pBBB;
};

// TODO: 在此处引用程序需要的其他头文件
