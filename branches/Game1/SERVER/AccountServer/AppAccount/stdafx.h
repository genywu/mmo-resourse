// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4018)
#pragma warning(disable:4996)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4244)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
//#define DEBUG_NEW new
#define _WIN32_DCOM
#define _WIN32_WINNT 0x0501

#include <Windows.h>
#include <objbase.h>
#include <assert.h>
#include <time.h>
#include <Winsock2.h>

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
#include "..\public\debugnew.h"
#include "..\public\MsgType.h"
#include "AccountServer\game.h"

#include "..\public\ClientResource.h"
#include "..\public\RFile.h"
#include "..\public\AccountServerDef.h"

using namespace std;