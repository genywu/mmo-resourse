/**
* @file    NetDef.h
* @author  Fox (yulefox@gmail.com)
* @date    Mar.04, 2010
* @brief   网络模块相关配置
*/

#pragma once

#include <map>

/// @enum NetFlag
/// 网络端标识
enum NetFlag
{
    NF_NULL            = 0x0000,     ///< NULL
    NF_THIS            = 0x0001,     ///< 当前端
	NF_GATE_SERVER     = 0x0002,     ///< 网关服务器(暂未使用)
	NF_ACCOUNT_SERVER  = 0X0004,     ///< 账号服务器
	NF_LOGIN_SERVER    = 0X0010,     ///< 登录服务器
	NF_LOG_SERVER      = 0x0020,     ///< 日志服务器
	NF_DB_SERVER       = 0x0040,     ///< 数据库服务器
	NF_WORLD_SERVER    = 0X0100,     ///< 世界服务器
	NF_GAME_SERVER     = 0X0200,     ///< 游戏服务器
	NF_GAME_CLIENT     = 0X1000,     ///< 游戏客户端
};

struct Connection
{
    string ip;                       ///< 连接IP
    ushort port;                     ///< 连接端口
    int socket;                      ///< Socket ID

    Connection()
        : socket(-1)
    {}
};

typedef int Socket;
