// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料



// TODO: 在此处引用程序需要的其他头文件
// Windows 头文件:
#include <objbase.h>
#include <mmsystem.h>
#include <windows.h>
#include <Winsock2.h>
// C 运行时头文件
#include <assert.h>
#include <process.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <winbase.h>

#include "../DefPublic.h"

#include "../Module/CrashDumper.h"
#include "../Module/GUID.h"
#include "../Module/Date.h"
#include "../Module/tools.h"
#include "../Package/RFile.h"
#include "../Package/ClientResource.h"

#include "../tinyxml/tinyxml.h"

#include "../DataStream/DataBlockSetWriteRead.h"

#include "../Common/OrganizingDef.h"
#include "../Common/MsgType.h"
#include "../Common/Public.h"
#include "../Common/BaseDef.h"

#include "ServerPublic.h"

#include "../Log/Log4c.h"
#include "../Log/LogDef.h"
#include "../AppFrame/TextRes.h"
using namespace AppFrame;

