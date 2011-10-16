#pragma once

/// 包含Windows库、C运行时库及Public头文件 

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#define _WIN32_WINNT 0x0501

// Windows 头文件:
#include <objbase.h>
#include <mmsystem.h>
#include <windows.h>
#include <Winsock2.h>
#include <Commdlg.h>

// C 运行时头文件
#include <assert.h>
#include <process.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <crtdbg.h>
#include <io.h>

#include "DefType.h"
#include "DefSTL.h"
#include "DefCommon.h"
#include "AppFrame/AppFrameDef.h"
