// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
 #define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h> 
//#include <new.h>
// C RunTime Header Files
#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <tchar.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <stdio.h>
#include <io.h>
#include <shlobj.h>

#include "..\..\frostengine\singleton.h"

#include <list>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <hash_map>
#include <set>
#include <deque>

using namespace std;
using namespace stdext;

#include "..\..\..\public\ClientResource.h"
#include "..\..\..\public\RFile.h"
#include "..\..\..\public\LoadState.h"
#include "..\..\..\public\RCObject.h"
#include "..\..\..\public\debugnew.h"
#include "..\..\..\public\basedef.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")

//#pragma comment (lib,"imm32.lib")
//#pragma comment(lib, "dbghelp.lib")
//#pragma comment(lib, "Winmm.lib")


// TODO: reference additional headers your program requires here
