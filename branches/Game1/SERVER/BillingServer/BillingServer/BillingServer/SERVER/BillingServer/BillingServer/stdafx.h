// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <MMSystem.h>

// TODO: 在此处引用程序要求的附加头文件
#include <process.h>			// 多线程使用
#include <objbase.h>			// 临界段

#include <map>
using namespace std;

//#include "..\public\public.h"
//#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\debugnew.h"
#include "..\public\tools.h"

extern HWND g_hWnd;