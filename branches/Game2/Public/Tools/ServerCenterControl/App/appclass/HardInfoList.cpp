

#include "stdafx.h"
#include "HardInfoList.h"
#include "UserManage.h"
#include "DbOperator.h"

//!							正在写入数据库的数据
set<tagHardwareInfo*>		HardInfoList::m_setWriting;


HardInfoList::HardInfoList(DWORD dwSMID)
:m_dwSMID(dwSMID)
{

}
HardInfoList::~HardInfoList(VOID)
{
	Release();
}

//!
VOID HardInfoList::Release(VOID)
{
	list<tagHardInfoGroup>::iterator ite = m_queueCacheInfo.begin();
	for (; m_queueCacheInfo.end() != ite; ++ite)
	{
		RequestWriteDB(*ite);
	}
	m_queueCacheInfo.clear();

	if(NULL != m_CurrGroup.pHardwareInfo)
	{
		RequestWriteDB(m_CurrGroup);
	}
	m_CurrGroup.dwUseNum = 0;
	m_CurrGroup.pHardwareInfo = NULL;
}

//! 添加一个硬件信息节点
BOOL HardInfoList::AddNode(tagHardwareInfo &HardwareInfo, BOOL bSendToUser)
{
	if(NULL == m_CurrGroup.pHardwareInfo)
	{
		m_CurrGroup.pHardwareInfo = (tagHardwareInfo*)M_ALLOC(sizeof(tagHardwareInfo) * HARD_INFO_GROUP_NUM);
		m_CurrGroup.dwUseNum = 0;
		if (NULL == m_CurrGroup.pHardwareInfo)
		{
			return FALSE;
		}
	}
	
	if(HARD_INFO_GROUP_NUM <= m_CurrGroup.dwUseNum)
	{
		AddGroup(m_CurrGroup);

		m_CurrGroup.dwUseNum = 0;
		m_CurrGroup.pHardwareInfo = (tagHardwareInfo*)M_ALLOC(sizeof(tagHardwareInfo) * HARD_INFO_GROUP_NUM);
		if (NULL == m_CurrGroup.pHardwareInfo)
		{
			return FALSE;
		}
	}

	m_CurrGroup.pHardwareInfo[m_CurrGroup.dwUseNum] = HardwareInfo;
	++m_CurrGroup.dwUseNum;

	if(bSendToUser)
	{
		CMessage msg(MSG_SCC2SUC_INFO_Notify_AddHardInfo);
		DBWriteSet setWriteSet;
		msg.GetDBWriteSet(setWriteSet);

		setWriteSet.AddToByteArray(m_dwSMID);
		setWriteSet.AddToByteArray(&HardwareInfo, sizeof(tagHardwareInfo));	

		UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
	}

	return TRUE;
}

//! 添加一组硬件信息节点
BOOL HardInfoList::AddNodeList(tagHardwareInfo *pHardwareInfo, LONG lNum)
{
	if(NULL == pHardwareInfo)
		return FALSE;
	for (LONG i = 0; i < lNum; ++i)
	{
		AddNode(pHardwareInfo[i]);
	}

	CMessage msg(MSG_SCC2SUC_INFO_Notify_HardInfoList);
	DBWriteSet setWriteSet;
	msg.GetDBWriteSet(setWriteSet);

	setWriteSet.AddToByteArray(m_dwSMID);
	setWriteSet.AddToByteArray(lNum);
	setWriteSet.AddToByteArray(pHardwareInfo, sizeof(tagHardwareInfo) * lNum);

	UserManage::GetInstance().SendToAll(ePT_Penniless, msg);

	return TRUE;
}

//! 发送自身信息到客户端
VOID HardInfoList::SendInfoToSocket(LONG lSocket)
{
	tagHardInfoGroup HardInfoGroup;
	GetCacheAllInfo(HardInfoGroup);

	CMessage msg(MSG_SCC2SUC_INFO_Notify_HardInfoList);
	DBWriteSet setWriteSet;
	msg.GetDBWriteSet(setWriteSet);

	setWriteSet.AddToByteArray(m_dwSMID);
	setWriteSet.AddToByteArray(HardInfoGroup.dwUseNum);
	setWriteSet.AddToByteArray(HardInfoGroup.pHardwareInfo, sizeof(tagHardwareInfo) * HardInfoGroup.dwUseNum);

	msg.SendToSuc(lSocket);
}
//! 发送自身信息到所有客户端
VOID HardInfoList::SendInfoToAll(VOID)
{
	tagHardInfoGroup HardInfoGroup;
	GetCacheAllInfo(HardInfoGroup);

	CMessage msg(MSG_SCC2SUC_INFO_Notify_HardInfoList);
	DBWriteSet setWriteSet;
	msg.GetDBWriteSet(setWriteSet);

	setWriteSet.AddToByteArray(m_dwSMID);
	setWriteSet.AddToByteArray(HardInfoGroup.dwUseNum);
	setWriteSet.AddToByteArray(HardInfoGroup.pHardwareInfo, sizeof(tagHardwareInfo) * HardInfoGroup.dwUseNum);

	UserManage::GetInstance().SendToAll(ePT_Scout2, msg);
}

//! 得到当前缓存的所有数据
VOID HardInfoList::GetCacheAllInfo(tagHardInfoGroup &HardInfoGroup)
{
	static tagHardwareInfo arrHardwareInfo[MAX_MESSAGE_INCLUDE_HARD_INFO];

	HardInfoGroup.pHardwareInfo = arrHardwareInfo;
	HardInfoGroup.dwUseNum = 0;

	list<tagHardInfoGroup>::iterator ite = m_queueCacheInfo.begin();
	for (; m_queueCacheInfo.end() != ite; ++ite)
	{
		memcpy(HardInfoGroup.pHardwareInfo, ite->pHardwareInfo, ite->dwUseNum * sizeof(tagHardwareInfo));
		HardInfoGroup.dwUseNum += ite->dwUseNum;
	}

	if(NULL != m_CurrGroup.pHardwareInfo)
	{
		memcpy(HardInfoGroup.pHardwareInfo, m_CurrGroup.pHardwareInfo, m_CurrGroup.dwUseNum * sizeof(tagHardwareInfo));
		HardInfoGroup.dwUseNum += m_CurrGroup.dwUseNum;
	}
}

//!
VOID HardInfoList::AddGroup(tagHardInfoGroup &HardInfoGroup)
{
	if(HARD_INFO_CACHE_GROUP_NUM < m_queueCacheInfo.size())
	{
		RequestWriteDB(*(m_queueCacheInfo.begin()));
		m_queueCacheInfo.pop_front();
	}

	m_queueCacheInfo.push_back(HardInfoGroup);
}

//!	请求向数据库写入数据				
VOID HardInfoList::RequestWriteDB(tagHardInfoGroup &HardInfoGroup)
{
	DbOperator *pDbOperator = DbOperator::CreateInHardOperator(m_dwSMID, HardInfoGroup.dwUseNum, HardInfoGroup.pHardwareInfo);
	if(NULL != pDbOperator)
	{
		if(pDbOperator->Begin())
		{
			m_setWriting.insert(HardInfoGroup.pHardwareInfo);
			return;
		}
		DbOperator::ReleaseOperator(&pDbOperator);
	}
	else
	{
		M_FREE(HardInfoGroup.pHardwareInfo, sizeof(tagHardwareInfo) * HARD_INFO_GROUP_NUM);
	}
}

//! 响应写入完成
VOID HardInfoList::OnDBWriteFinish(tagHardwareInfo *pHardwareInfo, DWORD dwNum, DWORD dwSMID)
{
	if(0 == dwNum)
		return;
	if(0 != dwSMID)
	{
		//! 这里有值说明数据库操作失败了
		LogTools::OutErrLog("SM[%d]'s HardwareInfo[%d] write db lost!!!", dwSMID, dwNum);
	}

	set<tagHardwareInfo*>::iterator ite = m_setWriting.find(pHardwareInfo);
	if(m_setWriting.end() != ite)
	{
		m_setWriting.erase(ite);
		M_FREE(pHardwareInfo, sizeof(tagHardwareInfo) * HARD_INFO_GROUP_NUM);
	}
	else
	{
		LogTools::OutErrLog("HardInfoList::OnDBWriteFinish() error!!! may be memory leak.");
	}
}