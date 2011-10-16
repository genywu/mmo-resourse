// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#include <CEGUI.h>
#include "../../Public/ClientPublic/DefWin.h"
#include "../../Public/ClientPublic/DefD3D.h"
#include "../../Public/DefSTL.h"
#include "../../Public/DefType.h"
#include "../../Public/Common/CStrConverter.h"
#include "../../Public/Module/Tools.h"
#include "../../Public/Package/RFile.h"
#include "../../Public/Module/Singleton.h"

#include "../../Public/Log/Log4c.h"
#include "../../Public/Log/LogDef.h"
#include "../../Public/AppFrame/TextRes.h"
using namespace AppFrame;

extern HWND g_hWnd;

// TODO: 在此处引用程序需要的其他头文件
