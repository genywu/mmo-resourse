// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
#define _WIN32_WINNT 0x0501

#include <Windows.h>

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
#include <hash_set>

#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\debugnew.h"
#include "..\public\GUID.h"
#include "../public/LogDef.h"

#include "..\server\gameserver\gameserver\game.h"

using namespace std;
using namespace stdext;