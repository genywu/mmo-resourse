// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
#include <afx.h>
#include <assert.h>

#include "../../../DefPublic.h"
#include "../../../ServerPublic/ServerPublic.h"
#include "../../../Nets/MemServer.h"
#include "../Utility/timerdef.h"

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

#include <hash_map>
using namespace stdext;