/*
* Copyright (c) 2009,盛大在线 计费平台中心
* All rights reserved.
* 
* 文件名称：sdoa4server.h
* 摘    要：各接口函数定义
* 当前版本：2.0
* 作    者：SNDA
* 完成日期：2009年2月23日
*
* 修改内容：2.0最终对外接口
* 修改时间：2009年3月10日
*/

#ifndef SDOA4SERVER_H
#define SDOA4SERVER_H

#include "sndabase.h"

#define SDOA_ITEM_SEND_REQUEST 	0X01000001
#define SDOA_DEPOSIT_REQUEST 	0X01000003

interface ISDOAMsg : ISDUnknown 
{
	//----------------------------------------------------------------------------------------
	//名称：GetValue
	//描述：获取指定消息内指定字段的值
	//参数：
	//	[in]szKey：         数据关键字
	//返回：                关键字对应的内容，没有该关键字返回NULL
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(const char*)	GetValue(const char* szKey) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：SetValue
	//描述：给指定消息添加指定字段的值
	//参数：
	//	[in]szKey：         数据关键字
	//	[in]szValue：       数据值
	//返回：                0成功，其他失败（一般为数据长度溢出）
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int)	SetValue(const char* szKey, const char* szValue) = 0;
};

SNDAAPI(ISDOAMsg*) sdoaCreateMsg(void);
//---------------------------------------------------------------------------------------
//名称：UserInfoCallback
//描述：认证信息回调处理函数，由sdk使用方实现
//参数：
//	[out]nResult：          返回消息结果(具体错误类型说明参见相关错误文档)
//	[out]nUniqueId：        关联ID
//	[in/out]pMsg：          消息解析设置接口
//返回：无
//----------------------------------------------------------------------------------------
typedef int (SNDACALLBACK *UserInfoCallback)(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg);

interface ISDOAUserInfoAuthen : ISDUnknown
{
	//----------------------------------------------------------------------------------------
	//名称：Initialize
	//描述：初始化
	//参数：
	//	[in]szConfPath：    配置文件，如果为NULL则使用缺省的ConfClt.conf配置文件
	//	[in]pCallbackFunc： 回调函数地址
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) Initialize(const char *szConfPath, UserInfoCallback pCallbackFunc) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：AsyncGetUserInfo
	//描述：异步获取认证结果(通过Initialize设置回调函数)
	//参数：
	//	[in]szSessionId：   需要验证的Token或者SessionId
	//	[in]szEndpointIp：  客户端IP
	//	[in]nUniqueId：     token唯一id
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) AsyncGetUserInfo(const char *szSessionId, const char *szEndpointIp, const unsigned long nUniqueId) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：SyncGetUserInfo
	//描述：同步获取认证结果
	//参数：
	//	[in]szSessionId：   需要验证的Token或者SessionId
	//	[in]szEndpointIp：  客户端IP
	//	[in]nUniqueId：     token唯一id
	//	[in/out]pMsg：      返回的用户信息		
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) SyncGetUserInfo(const char *szSessionId, const char *szEndpointIp, const unsigned long nUniqueId, ISDOAMsg* pMsg) = 0;

};

SNDAAPI(ISDOAUserInfoAuthen*) sdoaCreateUserInfoAuthen(void);

#ifdef USE_SDOARSA
interface ISDOARSASign : ISDUnknown
{
	//----------------------------------------------------------------------------------------
	//名称：InitialPriKey
	//描述：指定订单进行RSA签名时所需的私钥文件位置，初始化RSA私钥签名接口
	//参数：
	//	[in]szPriKeyFile：  私钥文件名(全路径或相对路径)
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) InitialPriKey(const char *szPriKeyFile) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：Sign
	//描述：基于商户私钥对订单参数进行签名，获得签名字符串
	//参数：
	//	[in]szData：        要签名的数据
	//	[in]nDataLength：   要签名的数据长度 
	//	[out]szSignature：  签名,长度512Byte.
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) Sign(const char *szData, int nDataLength, char* szSignature) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：InitialPubKey
	//描述：指定订单进行RSA签名验证时所需的公钥文件位置，初始化RSA公钥验证接口
	//参数：
	//	[in]szPubKeyFile：  公钥文件名(全路径或相对路径)
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) InitialPubKey(const char *szPubKeyFile) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：VerifySignature
	//描述：基于盛大公钥对盛大计费接口提交的道具发送通知参数进行验证
	//参数：
	//	[in]szData：        待验证源数据
	//	[in]nDataLength：   待验证源数据长度 
	//	[in]szSignature：   待验证签名
	//返回：                0 成功；!0 失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) VerifySignature(const char *szData,int nDataLength,const char* szSignature) = 0;
};

SNDAAPI(ISDOARSASign*) sdoaCreateRSASign(void);
#endif //#ifdef USE_SDOARSA

//---------------------------------------------------------------------------------------
//名称：RecvMsgCallback
//描述：订单信息回调处理函数，由sdk使用方实现
//参数：
//	nMsgType：              消息类型：目前有道具和充值两个消息，定义参照itemproto.h
//	pRequest：              请求消息对象：参照接口ISDOAMsg，内容只包含包体 定义参照itemproto.h
//	pResponse：             回调处理完后应答：参照接口ISDOAMsg，内容只包含包体 定义参照itemproto.h
//返回：                    无
//----------------------------------------------------------------------------------------

typedef int (SNDACALLBACK *RecvMsgCallback)(int nMsgType, ISDOAMsg* pRequest, ISDOAMsg* pResponse);

interface ISDOAOrderHandler : ISDUnknown
{
	//----------------------------------------------------------------------------------------
	//名称：Initialize
	//描述：初始化全付通客户端
	//参数：
	//	[in]szConfPath：    配置文件，如果为NULL则使用缺省的ConfClt.conf配置文件
	//	[in]pCallbackFunc： 消息处理回调函数
	//返回：                0 成功；!0 失败；
	//                      -10225001 配置文件不存在
	//                      -10225002 生成授权配置文件失败
	//                      -10225003 授权SDK初始化失败
	//                      -10225004 创建网络调度线程失败
	//                      -10225005 创建本地侦听SOCKET失败
	//                      -10225006 创建回调线程失败
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) Initialize(const char *szConfPath, RecvMsgCallback pCallbackFunc) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：CreateOrderInfo
	//描述：创建订单信息
	//参数：
	//	[in]nType：         1.扣费信息，2.道具发送信息
	//返回：                订单信息，0 失败
	//                      !0为ULONG对象指针
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(ULONG) CreateOrderInfo(const int nType) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：DestroyOrderInfo
	//描述：销毁订单信息
	//参数：
	//	[in]pOrderInfo：    订单信息
	//返回：                0 成功，无其他返回
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) DestroyOrderInfo(ULONG pOrderInfo) = 0;
	
	//----------------------------------------------------------------------------------------
	//名称：SetOrderInfoValue
	//描述：往订单信息添加值
	//参数：
	//	[in]pOrderInfo：    订单信息
	//	[in]szKey[in]：     数据关键字
	//	[in]szValue：       数据值
	//返回：0               成功 无其他返回
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) SetOrderInfoValue(ULONG pOrderInfo, const char *szKey, const char *szValue) = 0;

	//----------------------------------------------------------------------------------------
	//名称：BuildOrderUrl
	//描述：签名订单信息，并返回相应http请求内容
	//参数：
	//	[in]pOrderInfo：    扣费对象
	//	[out]szUrl：        http请求URL,最大2048字节如：char szUrl[2048]
	//返回：                0成功，无其他返回
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) BuildOrderUrl(ULONG pOrderInfo, char *szUrl) = 0;

	//----------------------------------------------------------------------------------------
	//名称：SignData
	//描述：数据包签名
	//参数：
	//	[in]szData：        要签名的数据
	//	[in]nDataLength：   要签名的数据长度 
	//	[out]szSignature：  返回32字节的数据包签名,32字节结尾加上'\0'，即需要33字节空间如char szUrl[33]
	//返回：                0 成功；!0 失败；
	//                      -10225007 失败
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) SignData(const char* szData, const int nDataLength, char* szSignature) = 0;

	//----------------------------------------------------------------------------------------
	//名称：VerifySignature
	//描述：数据包签名校验
	//参数：
	//	[in]szData：        待验证源数据
	//	[in]nDataLength：   待验证源数据长度
	//	[in]szSignature：   待验证签名
	//返回：                0 成功；!0 失败；
	//                      -10225008	失败
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) VerifySignature(const char* szData, const int nDataLength, const char* szSignature) = 0;

	//----------------------------------------------------------------------------------------
	//名称：GetUniqueId
	//描述：获取唯一ID
	//参数：
	//	[out]szUniqueId：   唯一ID输出地址,32字节结尾加上'\0'，即需要33字节空间，如char szUrl[33]
	//返回：                0 成功；!0 失败；
	//                      -10225009	参数为NULL
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) GetUniqueId(char *szUniqueId) = 0;

	//----------------------------------------------------------------------------------------
	//名称：UnInitialize
	//描述：全付通客户端卸载
	//参数：
	//	无
	//返回：                0 成功；无其他返回失败；
	//----------------------------------------------------------------------------------------
	SNDAMETHOD(int) UnInitialize() = 0;
};

SNDAAPI(ISDOAOrderHandler*) sdoaCreateOrderHandler();

#endif