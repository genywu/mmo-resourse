// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4018)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4313)


#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0500	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料



// TODO: 在此处引用程序需要的其他头文件

#include <assert.h>

// Windows 头文件:
#include <windows.h>
#include <wtypes.h>
#include <WinBase.h>
#include <process.h>
#include <MMSystem.h>
#include "time.h"

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <deque>
using namespace std;


// 自定义库
#include "../../../public/tools.h"
#include "../../../public/public.h"
#include "../../../public/GUID.h"
#include "../../../public/LogDef.h"
#include "../../../public/tinystr.h"
#include "../../../public/tinyxml.h"

#include "LogServer\game.h"