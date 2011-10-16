// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

// TODO: 在此处引用程序要求的附加头文件

#include "fmod.h"

#pragma once
#include "../../Public/DefPublic.h"
#include "../../Public/Log/Log4c.h"
#include "../../Public/Log/LogDef.h"
#include "../../Public/AppFrame/TextRes.h"
using namespace AppFrame;


extern HWND g_hWnd;
