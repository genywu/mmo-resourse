// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4018;disable:4244;disable:4267;disable:4311;disable:4312)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#define _WIN32_WINNT 0x0501

// Windows 头文件:
#include <windows.h>
#include <objbase.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>

// TODO: 在此处引用程序需要的其他头文件
#include <mmsystem.h>
#include <assert.h>
#include <math.h>

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <sstream>
using namespace std;

#include "../../public/RFile.h"
#include "../../public/ClientResource.h"
#include "../../public/tools.h"
#include "../../public/public.h"
#include "../../public/readwrite.h"
#include "../../public/DataBlockSetWriteRead.h"

