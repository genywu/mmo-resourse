// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料



// TODO: 在此处引用程序需要的其他头文件
#include <Windows.h>
#include <objbase.h>
#include <assert.h>
#include <time.h>
#include <Winsock2.h>
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
using namespace std;

#include "..\..\..\public\public.h"
#include "..\..\..\public\tools.h"
#include "..\..\..\public\debugnew.h"
#include "..\..\..\public\MsgType.h"
#include "..\..\..\public\GUID.h"
#include "..\bounsserver\game.h"