




#include "stdafx.h"
#include "ServiceNet.h"
//#include "../../../../Public/Nets/Servers.h"
//#include "../../../../Public/DataStream/DataBlockSetWriteRead.h"
#include "../../../SCMPublic/def/NetDef.h"
#include "../../../SCMPublic/class/NetSetup.h"
#include "../App/Setup/SMSetup.h"
#include "../App/AppClass/UserManage.h"



ServiceNet::ServiceNet(VOID)
/*
:m_bIsWorking(TRUE)
,m_hConnThrd(NULL)
,m_bEndConnThrd(FALSE)
*/
{

}

ServiceNet::~ServiceNet(VOID)
{
}

ServiceNet ServiceNet::c_ServiceNet;

ServiceNet& ServiceNet::GetInstance(VOID)
{
	return c_ServiceNet;
}

//! 初始化
BOOL ServiceNet::Init(VOID)
{
	//! 服务器基本模块
	///////////////////////////////////////////////////
	//! 获取配置
	if(!NetSetup::GetInstance().LoadSetup("NetSetup.xml"))
	{
		LogTools::OutLog("load NetSetup.xml lost!!!");
		return FALSE;
	}
	else
	{
		LogTools::OutLog("load NetSetup.xml succeed!");
	}

	//! 分配数据块
	const tagMemSetup *pMemSetup = NetSetup::GetInstance().GetMemSetup();
	if(NULL == pMemSetup)
		return FALSE;
	m_pDBAllocator	= new CDataBlockAllocator(pMemSetup->dwDataBlockNum, pMemSetup->dwDataBlockSize);

	//! 初始化服务器网络
	CMessage::Initial(m_pDBAllocator, pMemSetup->dwFreeMsgNum);
	CMySocket::MySocketInit();

	//! 初始化网络对象
	if( !InitNetServer_SUC() )
	{
		return FALSE;
	}
	//! 初始化网络对象
	if( !InitNetServer_SM() )
	{
		return FALSE;
	}

	return TRUE;
}
//!
VOID ServiceNet::Release(VOID)
{
	//m_bIsWorking = FALSE;
	//! 逻辑模块
	//////////////////////////////////////////////////
	UserManage::GetInstance().Release();

	//if(NULL != m_hConnThrd)
	//{
	//	m_bEndConnThrd = TRUE;
	//	WaitForSingleObject(m_hConnThrd, 2 * 60 * 1000);
	//}
	//m_hConnThrd = NULL;

	//! 服务器基本模块
	///////////////////////////////////////////////////
	if(NULL != s_pNetServer_SUC)
		s_pNetServer_SUC->Exit();
	if(NULL != s_pNetServer_SM)
		s_pNetServer_SM->Exit();

	CMySocket::MySocketClearUp();
	
	m_AinMsgQueue.Release();

	SAFE_DELETE(s_pNetServer_SUC);
	SAFE_DELETE(s_pNetServer_SM);

	CMessage::Release();

	SAFE_DELETE(m_pDBAllocator);
}

//!
VOID ServiceNet::ProcessMessage(VOID)
{
	//OnConnectSM();

	msgQueue pTemptMsgQueue;
	s_pNetServer_SUC->GetRecvMessages()->GetAllMessage(pTemptMsgQueue);

	while(pTemptMsgQueue.begin() != pTemptMsgQueue.end())
	{
		CMessage* pMsg = (CMessage*)(*(pTemptMsgQueue.begin()));
		if( pMsg )
		{
			pMsg->Run();
			CBaseMessage::FreeMsg(pMsg);
		}
		pTemptMsgQueue.pop_front();
	}


	s_pNetServer_SM->GetRecvMessages()->GetAllMessage(pTemptMsgQueue);

	while(pTemptMsgQueue.begin() != pTemptMsgQueue.end())
	{
		CMessage* pMsg = (CMessage*)(*(pTemptMsgQueue.begin()));
		if( pMsg )
		{
			pMsg->Run();
			CBaseMessage::FreeMsg(pMsg);
		}
		pTemptMsgQueue.pop_front();
	}


	//! 处理自己发给自己的消息
	list<AinMessage*> listMsg;
	m_AinMsgQueue.PopMessage(listMsg);
	for(list<AinMessage*>::iterator ite = listMsg.begin(); listMsg.end() != ite; ite++)
	{
		if( NULL != *ite )
		{
			(*ite)->Run();
			AinMessage::Release(&(*ite));
		}
	}
}


BOOL ServiceNet::InitNetServer_SUC(VOID)
{
	const tagServerSetup *pServerSetup = NetSetup::GetInstance().GetServerSetupByID(1);
	if(NULL == pServerSetup)
		return FALSE;

	s_pNetServer_SUC = new CServer;
	long lRet = s_pNetServer_SUC->Start(
		3,
		m_pDBAllocator,
		pServerSetup->dwFreeSockOperNum,
		pServerSetup->dwFreeIOOperNum,
		pServerSetup->dwIOOperDataBufNum,
		(1 == pServerSetup->dwCheckNet),
		pServerSetup->dwBanIPTime,
		pServerSetup->dwMaxMsgLen,
		pServerSetup->dwMaxConnectNum,
		pServerSetup->dwMaxClientsNum,
		pServerSetup->dwPendingWrBufNum,
		pServerSetup->dwPendingRdBufNum,
		pServerSetup->dwMaxSendSizePerSecond,
		pServerSetup->dwMaxRecvSizePerSecond,
		pServerSetup->dwMaxBlockedSendMsgNum
	);
	if( !lRet )
	{
		LogTools::OutLog("InitNetServer_SUC() lost!!!");
		return FALSE;
	}

	BOOL bRet = s_pNetServer_SUC->Host(
		pServerSetup->dwListenOrConnectPort,
		NULL,
		NF_SCC_Server,
		SOCK_STREAM
	);
	if(!bRet)
	{
		LogTools::OutLog("InitNetServer_SUC() lost!!!");
	}

	CMessage::SetSUCServer(s_pNetServer_SUC);
	LogTools::OutLog("InitNetServer_SUC() succeed!");

	return TRUE;
}

BOOL ServiceNet::InitNetServer_SM(VOID)
{
	const tagServerSetup *pServerSetup = NetSetup::GetInstance().GetServerSetupByID(2);
	if(NULL == pServerSetup)
		return FALSE;

	s_pNetServer_SM = new CServer;
	long lRet = s_pNetServer_SM->Start(
		3,
		m_pDBAllocator,
		pServerSetup->dwFreeSockOperNum,
		pServerSetup->dwFreeIOOperNum,
		pServerSetup->dwIOOperDataBufNum,
		(1 == pServerSetup->dwCheckNet),
		pServerSetup->dwBanIPTime,
		pServerSetup->dwMaxMsgLen,
		pServerSetup->dwMaxConnectNum,
		pServerSetup->dwMaxClientsNum,
		pServerSetup->dwPendingWrBufNum,
		pServerSetup->dwPendingRdBufNum,
		pServerSetup->dwMaxSendSizePerSecond,
		pServerSetup->dwMaxRecvSizePerSecond,
		pServerSetup->dwMaxBlockedSendMsgNum
		);
	if( !lRet )
	{
		LogTools::OutLog("InitNetServer_SM() lost!!!");
		return FALSE;
	}

	BOOL bRet = s_pNetServer_SM->Host(
		pServerSetup->dwListenOrConnectPort,
		NULL,
		NF_SM_Server,
		SOCK_STREAM
		);
	if(!bRet)
	{
		LogTools::OutLog("InitNetServer_SM() lost!!!");
	}

	CMessage::SetSMServer(s_pNetServer_SM);
	LogTools::OutLog("InitNetServer_SM() succeed!");

	return TRUE;
}

////! 响应SM连接成功
//VOID ServiceNet::OnConnectSM(VOID)
//{
//	if(0 != m_listSocket.size())
//	{
//		m_OnConnLock.Lock();
//		CMessage::OnSMAccept(m_listSocket.begin()->lSocket, m_listSocket.begin()->dwID);
//		m_listSocket.erase(m_listSocket.begin());
//		m_OnConnLock.UnLock();
//	}
//}
//
////! 连接所有SM
//VOID ServiceNet::ConnectAllSM(VOID)
//{
//	vector<tagTempConnSetup> vSMSetup;
//	SMSetup::GetInstance().GetSMNetSetupList(vSMSetup);
//
//	for (LONG i = 0; i < vSMSetup.size(); ++i)
//	{
//		LogTools::OutLog("正在连接SM服务器[ID：%d]...", vSMSetup[i].dwID);
//		if(!ConnectSM(vSMSetup[i]))
//		{
//			LogTools::OutLog("连接SM服务器[ID：%d]失败，开始重连!!!", vSMSetup[i].dwID);
//			AddToAsynConnectSM(vSMSetup[i]);
//		}	
//	}
//}
//
////! 连接一个SM
//VOID ServiceNet::ConnectSM(DWORD dwSMID)
//{
//	const tagSMSetup *pSMSetup = SMSetup::GetInstance().GetSMSetup(dwSMID);
//	if(NULL != pSMSetup)
//	{
//		AddToAsynConnectSM(tagTempConnSetup(pSMSetup->szHostAddress, pSMSetup->dwPort, pSMSetup->dwID));
//	}
//}
//
////! 连接一个SM服务器，返回socket
//BOOL ServiceNet::ConnectSM(tagTempConnSetup &TempConnSetup)
//{
//	LONG lSocket = s_pNetServer_SUC->Connect(TempConnSetup.strHostAddress.c_str(), TempConnSetup.dwPort, NF_SM_Client);
//	if(-1 != lSocket)
//	{
//		LogTools::OutLog("连接SM服务器[ID：%d]成功，连接Socket[%d]!!!", TempConnSetup.dwID, lSocket);
//		m_OnConnLock.Lock();
//		m_listSocket.push_back(tagSocketAndID(TempConnSetup.dwID, lSocket));
//		m_OnConnLock.UnLock();
//		return TRUE;
//	}
//	
//	return  FALSE;
//}
//
//
//unsigned __stdcall ServiceNet::ConnectSMFunc( void* pThis )
//{
//	Sleep(10000);
//	ServiceNet *pServiceNet = (ServiceNet*)pThis;
//	if(pServiceNet == NULL)
//		return -1;
//
//	pServiceNet->m_ConnLock.Lock();
//	list<tagTempConnSetup> listCopy(pServiceNet->m_listLostSM);
//	pServiceNet->m_listLostSM.clear();
//	pServiceNet->m_ConnLock.UnLock();
//
//	while (!pServiceNet->m_bEndConnThrd && pServiceNet->m_bIsWorking)
//	{
//		//! 得到新添加的连接
//		pServiceNet->m_ConnLock.Lock();
//		if(0 != pServiceNet->m_listLostSM.size())
//		{
//			for (list<tagTempConnSetup>::iterator ite = pServiceNet->m_listLostSM.begin(); ite != pServiceNet->m_listLostSM.end(); ++ite)
//				listCopy.push_back(*ite);
//			pServiceNet->m_listLostSM.clear();
//		}
//		pServiceNet->m_ConnLock.UnLock();
//		
//		//! 开始连接
//		list<tagTempConnSetup>::iterator ite = listCopy.begin();
//		for (; ite != listCopy.end(); )
//		{
//			//LogTools::OutLog("正在重新连接SM服务器[ID：%d]...", ite->dwID);
//			LONG lSocket = pServiceNet->ConnectSM(*ite);
//			if(0 != lSocket)
//			{
//				ite = listCopy.erase(ite);
//				continue;
//			}
//			else
//				//LogTools::OutLog("重新连接SM服务器[ID：%d]...失败!!!", ite->dwID);
//
//			++ite;
//		}
//
//		pServiceNet->m_ConnLock.Lock();
//		if(0 == listCopy.size() && 0 == pServiceNet->m_listLostSM.size())
//		{
//			pServiceNet->m_ConnLock.UnLock();
//			break;
//		}
//		pServiceNet->m_ConnLock.UnLock();
//
//		Sleep(10000);
//	}
//
//	pServiceNet->m_hConnThrd = NULL;
//	return 0;
//}
//
////! 添加一个SM配置到异步连接
//VOID ServiceNet::AddToAsynConnectSM(tagTempConnSetup &TempConnSetup)
//{
//	m_ConnLock.Lock();
//	m_listLostSM.push_back(TempConnSetup);
//	m_ConnLock.UnLock();
//	
//	if(0 != m_listLostSM.size() && !m_bEndConnThrd)
//	{
//		unsigned int threadID = 0;
//		m_bEndConnThrd = FALSE;
//		m_hConnThrd = (HANDLE)_beginthreadex(NULL, 0, ServiceNet::ConnectSMFunc, this, 0, &threadID);
//	}
//}

