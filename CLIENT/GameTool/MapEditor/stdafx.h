// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4996)


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Commdlg.h>
#include <mmsystem.h>
// TODO: 在此处引用程序要求的附加头文件
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800


#include <dinput.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <commctrl.h>
#include <shlobj.h>

#include <list>
#include <algorithm>
#include <string>
#include <map>
#include <deque>
#include <set>
#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;


#include "..\..\..\public\RFile.h"
#include "..\..\..\public\LoadState.h"
#include "..\..\..\public\RCObject.h"
#include "..\..\..\public\tools.h"
#include "..\..\..\public\clientresource.h"

#include "..\..\frostengine\singleton.h"
#include "..\..\gameengine\displaymodel.h"
#include "..\..\gameengine\gamemodelmanager.h"
#include "..\..\gameengine\gamebuildlist.h"
#include "..\..\gameengine\textureprojective.h"


#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")


