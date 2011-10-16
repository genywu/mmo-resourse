// Fox@20080929----------------------------------------------
// File:    GateMsgType.h
// Brief:   网关与客户端间消息类型的声明, 现有消息类型一个大类
//          的消息数量最多为0x00010000, 当前最大消息枚举远远
//			小于0x010000000, 为保证与现有消息类型兼容, 将消息
//			基数置为0x10000000, 格式不变.
// Author:  Fox (yulefox@gmail.com)
// Date:    Sep.29, 2008
// Fox@20080929----------------------------------------------

#ifndef _NET_GATE_MSG_TYPE_H_
#define _NET_GATE_MSG_TYPE_H_

enum
{
	GMSG_SUB			= 0x00000100,
	GMSG_SEC			= 0x00010000,
};

enum MsgType
{
	GMSG_GATE_BASE			= 0x10000000,

	// Fox@20081010----------------------------------------------
	// C2G : 客户端发送到Gate, Gate需进行逻辑处理.
	// Fox@20081010----------------------------------------------
	GMSG_C2G				= GMSG_GATE_BASE + GMSG_SEC * 1,
	GMSG_C2G_CONNECTED,					                    // 客户端连接成功

	GMSG_C2G_GTOUP			= GMSG_C2G + GMSG_SUB * 1,		// 客户端连接成功

	// Fox@20081010----------------------------------------------
	// C2C : Gate转发, 此处枚举仅为注册运行函数用.
	// Fox@20081010----------------------------------------------
	GMSG_C2C				= GMSG_GATE_BASE + GMSG_SEC * 2,
	GMSG_C2C_TRANSMIT		= GMSG_C2G + GMSG_SUB * 1,		// 消息转发
	GMSG_C2C_TRANSMIT_GROUP,								// 消息群发
	GMSG_C2C_TRANSMIT_SINGLE,								// 消息单发
};

#endif