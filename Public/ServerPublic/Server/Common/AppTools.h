#pragma once

#include "../../../DefType.h"

    /// 装载指定模块
    /// @param name 模块ID(配置在文本资源中)
    /// @param state 0: 装载中; 1: 装载成功; 2:装载失败.
    void LoadLog(const char* name, const char* file, uint state);