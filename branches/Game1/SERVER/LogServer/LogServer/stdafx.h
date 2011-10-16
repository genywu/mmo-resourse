// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4018)
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#define _WIN32_WINNT 0x0500
#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

// Windows 头文件:
#include <windows.h>
#include <wtypes.h>
#include <WinBase.h>
#include <process.h>


// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

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
#include <deque>
#include <set>

#include "..\..\..\public\public.h"
#include "..\..\..\public\tools.h"
#include "..\..\..\public\readwrite.h"
#include "..\..\..\public\debugnew.h"
#include "..\..\..\public\MsgType.h"

#include "..\..\..\public\NetSession.h"
#include "..\..\..\public\NetSessionManager.h"
#include "..\..\..\public\Timer.h"
#include "..\..\..\public\Date.h"

#include "..\..\..\public\ClientResource.h"
#include "..\..\..\public\RFile.h"
#include "../../../public/LogDef.h"
using namespace std;

extern HWND g_hWnd;