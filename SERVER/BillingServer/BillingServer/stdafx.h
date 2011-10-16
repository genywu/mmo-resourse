// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#define _WIN32_WINNT 0x0500
// Windows 头文件:
#include <windows.h>
#include <wtypes.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>

// TODO: 在此处引用程序要求的附加头文件
#include <assert.h>
#include <time.h>
#include <MMSystem.h>

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <deque>

using namespace std;

#include "..\..\..\public\public.h"
#include "..\..\..\public\tools.h"
#include "..\..\..\public\ini.h"
#include "..\..\..\public\debugnew.h"
#include "..\..\..\public\MsgType.h"
#include "..\..\..\public\GUID.h"
#include "..\..\..\public\timer.h"


extern HWND g_hWnd;