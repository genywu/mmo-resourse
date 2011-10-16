// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4995)
#pragma warning(disable:4267)
#pragma warning(disable:4018)
#pragma warning(disable:4244)

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif	

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif


// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序要求的附加头文件

#include <Windows.h>
#include <assert.h>
#include <time.h>
#include <mmsystem.h>


// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
using namespace std;

#define DIRECTINPUT_VERSION 0x0800
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dshow.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "strmiids.lib")


#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\ini.h"
#include "..\public\rfile.h"
#include "..\public\rcobject.h"
#include "..\public\loadstate.h"
#include "..\public\clientresource.h"

#include "..\public\debugnew.h"

#include "..\public\NetSession.h"
#include "..\public\NetSessionManager.h"
#include "..\public\MsgType.h"
#include "..\public\DataBlockSetWriteRead.h"

#include "..\input\mouse.h"
#include "..\input\keyboard.h"
#include "gameclient\game.h"

extern HWND g_hWnd;