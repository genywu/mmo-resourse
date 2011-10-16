// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4018;disable:4244;disable:4267;disable:4311;disable:4312)

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
 #define DIRECTINPUT_VERSION 0x0800
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <stdio.h>
#include <io.h>
#include <mmsystem.h>   
#include <algorithm>
#include <stdlib.h>
#include <commctrl.h>

#include "..\FrostEngine\singleton.h"

#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <hash_map>
#include <string>
#include <deque>

using namespace std;
using namespace stdext;

#include "..\..\public\ClientResource.h"
#include "..\..\public\RFile.h"
#include "..\..\public\RCObject.h"
#include "..\..\public\LoadState.h"
#include "..\..\public\debugnew.h"
#include "..\..\public\tools.h"
#include "..\..\public\DataBlockSetWriteRead.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

