

#include "stdafx.h"
#include "NetSetup.h"


NetSetup NetSetup::c_NetSetup;

NetSetup::NetSetup(VOID)
:m_bIsReady(FALSE)
{

}
NetSetup::~NetSetup(VOID)
{

}

NetSetup& NetSetup::GetInstance(VOID)
{
	return c_NetSetup;
}

//! 得到网络相关内存配置
const tagMemSetup* NetSetup::GetMemSetup(VOID)
{
	return (m_bIsReady) ? (&m_MemSetup) : NULL;
}

//! 得到网络对象配置
const tagServerSetup* NetSetup::GetServerSetupByID(LONG lSetupID)
{
	if(m_bIsReady)
	{
		map<LONG, tagServerSetup>::iterator ite = m_mapServerSetup.find(lSetupID);
		if(m_mapServerSetup.end() != ite)
			return &(ite->second);
	}
	return NULL;
}

//! 读取配置
BOOL NetSetup::LoadSetup(const char *pXmlFilename)
{
	if(NULL == pXmlFilename)
		return FALSE;

	TiXmlNode *pNode = NULL; 		

	TiXmlDocument* m_Tdoc = new TiXmlDocument(pXmlFilename);
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode = m_Tdoc->FirstChild("NetSetup");

	if (pNode == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	TiXmlElement* pNetSetup = pNode->ToElement();//获取node 的指针

	if (pNetSetup == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	////////////////////////////////////////////
	m_bIsReady = FALSE;
	m_mapServerSetup.clear();

	m_MemSetup.dwDataBlockNum	= atoi(pNetSetup->Attribute("DataBlockNum"));
	m_MemSetup.dwDataBlockSize	= atoi(pNetSetup->Attribute("DataBlockSize"));
	m_MemSetup.dwFreeMsgNum		= atoi(pNetSetup->Attribute("FreeMsgNum"));

	TiXmlElement* pServerSetup = pNetSetup->FirstChildElement("ServerSetup");
	for (; pServerSetup != NULL; pServerSetup = pServerSetup->NextSiblingElement("ServerSetup"))
	{
		tagServerSetup ServerSetup;
		LONG lID = atoi(pServerSetup->Attribute("ID"));

		ServerSetup.dwListenOrConnectPort		= atoi(pServerSetup->Attribute("ListenOrConnectPort"));		// 侦听或者连接端口
		ServerSetup.dwFreeSockOperNum			= atoi(pServerSetup->Attribute("FreeSockOperNum"));			// 网络命令操作预分配数量
		ServerSetup.dwFreeIOOperNum				= atoi(pServerSetup->Attribute("FreeIOOperNum"));			// 完成端口上IO操作预分配数量
		ServerSetup.dwIOOperDataBufNum			= atoi(pServerSetup->Attribute("IOOperDataBufNum"));		// 默认IO操作的DataBuf数量

		ServerSetup.dwCheckNet					= atoi(pServerSetup->Attribute("CheckNet"));				// 是否对网络进行检测
		ServerSetup.dwBanIPTime					= atoi(pServerSetup->Attribute("BanIPTime"));				// 禁止IP的时间
		ServerSetup.dwMaxMsgLen					= atoi(pServerSetup->Attribute("MaxMsgLen"));				// 允许传输的最大消息长度		
		ServerSetup.dwMaxConnectNum				= atoi(pServerSetup->Attribute("MaxConnectNum"));			// 客户端的最大连接数量
		ServerSetup.dwMaxClientsNum				= atoi(pServerSetup->Attribute("MaxClientsNum"));			// 最大的客户端发送缓冲区大小
		ServerSetup.dwPendingWrBufNum			= atoi(pServerSetup->Attribute("PendingWrBufNum"));			// 最大的发送IO操作Buf总大小
		ServerSetup.dwPendingRdBufNum			= atoi(pServerSetup->Attribute("PendingRdBufNum"));			// 最大的接受IO操作Buf总大小
		ServerSetup.dwMaxSendSizePerSecond		= atoi(pServerSetup->Attribute("MaxSendSizePerSecond"));	// 向客户端每秒发送的最大字节数
		ServerSetup.dwMaxRecvSizePerSecond		= atoi(pServerSetup->Attribute("MaxRecvSizePerSecond"));	// 从客户端接受的每秒最大字节数
		ServerSetup.dwMaxBlockedSendMsgNum		= atoi(pServerSetup->Attribute("MaxBlockedSendMsgNum"));	// 最大阻塞的发送消息数量
		ServerSetup.dwConPendingWrBufNum		= atoi(pServerSetup->Attribute("ConPendingWrBufNum"));		// 客户端最大的发送IO操作Buf总大小
		ServerSetup.dwConPendingRdBufNum		= atoi(pServerSetup->Attribute("ConPendingRdBufNum"));		// 客户端最大的接受IO操作Buf总大小
		ServerSetup.dwConMaxSendSizePerSecond	= atoi(pServerSetup->Attribute("ConMaxSendSizePerSecond"));	// 向服务器发送的每秒最大字节数
		ServerSetup.dwConMaxRecvSizePerSecond	= atoi(pServerSetup->Attribute("ConMaxRecvSizePerSecond"));	// 从服务器接受的每秒最大字节数
		ServerSetup.dwConMaxBlockedSendMsgNum	= atoi(pServerSetup->Attribute("ConMaxBlockedSendMsgNum"));	// 最大阻塞的发送消息数量

		m_mapServerSetup[lID] = ServerSetup;
	}
	
	////////////////////////////////////////////
	m_bIsReady = TRUE;
	SAFE_DELETE(m_Tdoc);
	return TRUE;
}
