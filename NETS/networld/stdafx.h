// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4244)
#pragma warning(disable:4018)

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#define _WIN32_WINNT 0x0501

// TODO: 在此处引用程序要求的附加头文件

#include <Windows.h>
#include <wtypes.h>
#include <assert.h>
#include <time.h>
#include <process.h>

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <deque>
#include <set>

#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\debugnew.h"
#include "..\public\DataBlockSetWriteRead.h"
#include "..\server\worldserver\worldserver\game.h"

using namespace std;