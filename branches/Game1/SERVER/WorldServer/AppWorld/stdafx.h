// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4018)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4313)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#define _WIN32_WINNT 0x0501

#include <Windows.h>
#include <wtypes.h>

#include <assert.h>
#include <time.h>
#include <process.h>

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <set>
using namespace std;

#include "mmsystem.h"
#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\debugnew.h"
#include "..\public\ini.h"
#include "..\public\MsgType.h"
#include "..\public\tinystr.h"
#include "..\public\tinyxml.h"

#include "..\public\NetSession.h"
#include "..\public\NetSessionManager.h"
//#include "..\public\Timer.h"
#include "..\public\Date.h"
#include "..\public\DataBlockSetWriteRead.h"

#include "worldserver\game.h"
#include "..\LogClient\ws\LogicLogInterface.h"

#include "..\public\ClientResource.h"
#include "..\public\RFile.h"
#include "..\public\StrUnifyInput.h"
#include "..\public\Singleton.h"
#include "..\public\Date.h"
#include "..\public\GUID.h"
#include "..\..\..\public\WSUseMemManage.h"