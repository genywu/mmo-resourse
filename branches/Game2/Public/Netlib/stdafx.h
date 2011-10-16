// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "../DefSTL.h"
#include "../DefPublic.h"

#include "../Log/Log4c.h"
#include "../Log/LogDef.h"
#include "../AppFrame/TextRes.h"

using namespace AppFrame;

#ifndef PutDebugString
#define PutDebugString(module, msg, file, code)
#endif

#ifndef PutErrorString
#define PutErrorString(module, fmt, where, msg)
#endif

#ifndef PutTraceString
#define PutTraceString(module, msg)
#endif

