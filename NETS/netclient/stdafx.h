// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4018)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件

#include <Windows.h>

#include <assert.h>
#include <time.h>
#include <mmsystem.h>
#include <process.h>

#define DIRECTINPUT_VERSION 0x0800
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>


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

#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\debugnew.h"
#include "..\public\guid.h"
#include "..\public\RFile.h"
#include "..\public\RCObject.h"
#include "..\public\loadstate.h"
#include "..\public\DataBlockSetWriteRead.h"
