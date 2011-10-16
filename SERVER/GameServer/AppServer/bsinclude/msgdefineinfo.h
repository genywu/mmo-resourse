 /*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
* 
* 文件名称：msgdefineinfo.h
* 摘    要：计费系统公共的头文件信息
*           client与LBS、CBS间通讯用的消息体定义
* 当前版本：1.5.0
* 作    者：熊汗庆
* 完成日期：2007年1月12日
*
* 修改内容：版本号更新为1.6
* 修改时间：2007年5月3日
*
*/

#ifndef _MSG_DEFINE_INFO_H_
#define _MSG_DEFINE_INFO_H_

#include "msgmacro.h"
#include "msgerrcode.h"
#include "commonerrcode.h"

// zlc 2007-8-14 13:10 Ack消息
#define BSIP_ACK              0x800000ff	//ack消息

// message version
#ifndef BSIP_MESSAGE_VERSION
#define BSIP_MESSAGE_VERSION			0x00010601   //main version 1, sub version 4, second sub version 2
#endif

#define BSIP_HEARTBEAT_REQ				0x00000014	//心跳请求消息
#define BSIP_HEARTBEAT_RES				0x80000014	//心跳响应消息

#define BSIP_FLAG_LOCAL_NORMAL			0x00100000	//正常数据
#define BSIP_FLAG_LOCAL_BUFFER			0x00100001	//客户端API缓存文件
#define BSIP_FLAG_LOCAL_ROUND			0x00100002	//客户端API环形文件

#define BSIP_FLAG_LBS_NORMAL			0x00200000	//LBS正常数据
#define BSIP_FLAG_LBS_BUFFER			0x00200001	//LBS API缓存文件
#define BSIP_FLAG_LBS_ROUND				0x00200002	//LBS API环形文件
#define BSIP_FLAG_LBS_SYNC              0x00200003  //LBS 同步数据

#define BSIP_FLAG_CBS_NORMAL			0x00300000	//CBS正常数据
#define BSIP_FLAG_CBS_BUFFER			0x00300001	//CBS API缓存文件
#define BSIP_FLAG_CBS_ROUND				0x00300002	//CBS API环形文件
#define BSIP_FLAG_CBS_SYNC              0x00300003  //CBS 同步数据

#define IND_TOTAL_COSUMPTION            0x00000100  //查询或者累计消耗

//md5签名范围(所有int, short都是网络字节序):
//	private_key(16 bytes)+msg_head(msg_len+msg_type+seq_id+send_time+priority+version, total 40 bytes)				
// 	+the whole msg_body 
/*---------------------------------消息头---------------------------------*/
typedef struct BsipHead
{
	int 			msg_len;						// 整个消息包的字节长度，包括head和body
	int	 			msg_type;						// 消息类型，如BSIP_AUTHOR_REQ, BSIP_LOCK_ACCOUNT_RES等
	int  			version;						// 版本号码
	int				host_id;						// 游戏服务器编号
	int				flag;							// 消息来源标志，如BSIP_FLAG_LOCAL_NORMAL等
	char 			priority;						// 优先级
	int 			seq_id;							// 消息流水号,顺序累加,步长为1,循环使用（一对请求和应答消息的流水号必须相同）
	int 			gs_send_time;					// gs数据块发送时刻
	int 			lbs_send_time;					// lbs数据块发送时刻
	int				api_ip; 	 					// 引擎ip地址
	int				gs_ip; 	 						// GS ip地址
	int				lbs_ip; 	 					// LBS ip地址
	int				api_port; 	 					// 引擎port端口
	int				gs_port; 	 					// GS port端口
	int				lbs_port; 	 					// LBS port端口
	unsigned char 	digest[MAX_DIGEST_LEN];			// 加密结果（Md5 digest，其中ShareSecretKey 是client端和server之间维护的共享key）	
}BsipHeadDef;

/*-------------------------------心跳消息-------------------------------*/
typedef struct BsipHeartBeatReq
{
    char ip[IP_ADDRESS_LEN+1];         //ip游戏客户端地址， 如：61.172.252.37
}BsipHeartBeatReqDef;

typedef struct BsipHeartBeatRes
{
    int  result;                       //返回结果：0－成功
    char ip[IP_ADDRESS_LEN+1];         //ip游戏客户端地址， 如：61.172.252.37
}BsipHeartBeatResDef;

/*-------------------------------心跳消息-------------------------------*/
typedef struct BsipHeartBeatInfoReq
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipHeartBeatReqDef		msg_body;
}BsipHeartBeatInfoReqDef;

typedef struct BsipHeartBeatInfoRes
{
	BsipHeadDef				msg_head;				// 消息类型，如BSIP_AUTHEN_REQ, BSIP_LOCK_ACCOUNT_RES等
	BsipHeartBeatResDef		msg_body;
}BsipHeartBeatInfoResDef;


// 扣费ack
typedef struct BsipAck
{
    int msg_type;
    char order_id[33];
}BsipAckDef;

typedef struct BsipAckInfo
{
	BsipHeadDef       	msg_head;
	BsipAckDef  		msg_body;
}BsipAckInfoDef;    						//

#endif
