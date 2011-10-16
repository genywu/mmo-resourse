


#pragma once

//! 前置结构申明
///////////////////////////////////////////////////////////////////////
//! 网络相关内存配置
struct tagMemSetup
{
	DWORD dwDataBlockNum;				//数据块数量
	DWORD dwDataBlockSize;				//数据块大小
	DWORD dwFreeMsgNum;					//预分配的消息数量
};

//! 网络对象配置
struct tagServerSetup
{
	DWORD dwListenOrConnectPort;		// 侦听或者连接端口
	DWORD dwFreeSockOperNum;			// 网络命令操作预分配数量
	DWORD dwFreeIOOperNum;				// 完成端口上IO操作预分配数量
	DWORD dwIOOperDataBufNum;			// 默认IO操作的DataBuf数量

	DWORD dwCheckNet;					// 是否对网络进行检测
	DWORD dwBanIPTime;					// 禁止IP的时间
	DWORD dwMaxMsgLen;					// 允许传输的最大消息长度		
	DWORD dwMaxConnectNum;				// 客户端的最大连接数量
	DWORD dwMaxClientsNum;				// 最大的客户端发送缓冲区大小
	DWORD dwPendingWrBufNum;			// 最大的发送IO操作Buf总大小
	DWORD dwPendingRdBufNum;			// 最大的接受IO操作Buf总大小
	DWORD dwMaxSendSizePerSecond;		// 向客户端每秒发送的最大字节数
	DWORD dwMaxRecvSizePerSecond;		// 从客户端接受的每秒最大字节数
	DWORD dwMaxBlockedSendMsgNum;		// 最大阻塞的发送消息数量
	DWORD dwConPendingWrBufNum;			// 客户端最大的发送IO操作Buf总大小
	DWORD dwConPendingRdBufNum;			// 客户端最大的接受IO操作Buf总大小
	DWORD dwConMaxSendSizePerSecond;	// 向服务器发送的每秒最大字节数
	DWORD dwConMaxRecvSizePerSecond;	// 从服务器接受的每秒最大字节数
	DWORD dwConMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
};

//! 网络配置功能类
//! 读取、管理网络配置
////////////////////////////////////////////////////////////////////////
class NetSetup
{
public:
	NetSetup(VOID);
	~NetSetup(VOID);
	static						NetSetup&					GetInstance(VOID);

public:
	//!														读取配置
	BOOL													LoadSetup(const char *pXmlFilename);
	//!														得到网络相关内存配置
	const						tagMemSetup*				GetMemSetup(VOID);
	//!														得到网络对象配置
	const						tagServerSetup*				GetServerSetupByID(LONG lSetupID);


private:
	static						NetSetup					c_NetSetup;

private:
	//!														只有一个内存配置
	tagMemSetup												m_MemSetup;
	//!														包含多个网络对象配置
	map<LONG, tagServerSetup>								m_mapServerSetup;
	//!														初始化完成标记
	BOOL													m_bIsReady;
};