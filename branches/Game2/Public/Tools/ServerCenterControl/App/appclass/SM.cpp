

#include "stdafx.h"
#include "SM.h"
#include "UserManage.h"
#include "PackManage.h"



SM::SM(DWORD dwSMID, const char *pName, const char *pExplain, DWORD dwInRegionID, LPCSTR pRegionName, DWORD dwInGroupID, LPCSTR pGroupName)
:m_lSocket(-1)
,m_HardInfoList(dwSMID)
{
	dwID = dwSMID;
	dwRegionID = dwInRegionID;
	dwGroupID = dwInGroupID;
	strcpy(szName, pName);
	strcpy(szExplain, pExplain);
	strcpy(szRegionName, pRegionName);
	strcpy(szGroupName, pGroupName);
}
SM::~SM(VOID)
{
	Release();
}

SM* SM::CreateSM(DWORD dwSMID, const char *pName, const char *pExplain, DWORD dwInRegionID, LPCSTR pRegionName, DWORD dwInGroupID, LPCSTR pGroupName)
{
	SM *pReSM = (SM*)M_ALLOC(sizeof(SM));
	if(NULL != pReSM)
	{
		new(pReSM)SM(dwSMID, pName, pExplain, dwInRegionID, pRegionName, dwInGroupID, pGroupName);
	}
	else
		LogTools::OutErrLog("SM::CreateSM error!!!");
	return pReSM;
}

VOID SM::ReleaseSM(SM **ppSM)
{
	if(NULL != ppSM && NULL != (*ppSM))
	{
		(*ppSM)->~SM();
		M_FREE((*ppSM), sizeof(SM));
		(*ppSM) = NULL;
	}
}

//!
VOID SM::Release(VOID)
{
	if(-1 != GetSocket())
	{
		CMessage msg(MSG_SCC2SUC_INFO_Notify_Sm_Close);
	//	msg.Add(GetID());
        Json::Value var;
        var["smID"] = (int)GetID();
        msg.AddJson(var);
		UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
	}

	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		Gapp::ReleaseGapp(&(m_vGapp[i]));
	}
	m_vGapp.clear();

	SetSocket(-1);

	m_HardInfoList.Release();	
}
//!
VOID SM::SetSocket(LONG lSocket)
{
	m_lSocket = lSocket;
	if(-1 == m_lSocket)
		dwState = eSS_Stop;
	else
		dwState = eSS_Running;
}

//!
Gapp* SM::FindGapp(DWORD dwGappID)
{
	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		if(m_vGapp[i]->GetID() == dwGappID)
			return m_vGapp[i];
	}
	return NULL;
}

//! 发送自身信息到客户端
VOID SM::SendInfoToSocket(LONG lSocket)
{
	//! SM基本信息
	tagSmInfo *pSmBaseInfo = this;
	CMessage msg(MSG_SCC2SUC_INFO_Notify_Sm_BaseInfo);

	/*DBWriteSet setWriteSet;
	msg.GetDBWriteSet(setWriteSet);
	setWriteSet.AddToByteArray(pSmBaseInfo, sizeof(tagSmInfo));
	setWriteSet.AddToByteArray((BYTE)((m_lSocket != -1) ? 1 : 0));*/
    Json::Value var;
    var["infoTime"] = (double)pSmBaseInfo->dwTime;
    var["smID"] = (int)pSmBaseInfo->dwID;
    var["name"] = pSmBaseInfo->szName;
    var["explain"] = pSmBaseInfo->szExplain;
    var["state"] = (int)pSmBaseInfo->dwState;
    var["regionID"] = (int)pSmBaseInfo->dwRegionID;
    var["regionName"] = pSmBaseInfo->szRegionName;
    var["groupID"] = (int)pSmBaseInfo->dwGroupID;
    var["groupName"] = pSmBaseInfo->szGroupName;
    if(m_lSocket != -1)
        var["result"] = true;
    else
        var["result"] = false;
    msg.AddJson(var);
	msg.SendToSuc(lSocket);

	//! 硬件信息
	m_HardInfoList.SendInfoToSocket(lSocket);

	//! GAPP信息
	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		m_vGapp[i]->SendInfoToSocket(lSocket);
	}
}

//! 发送自身信息到所有客户端
VOID SM::SendInfoToAll(VOID)
{
	//! SM基本信息
	tagSmInfo *pSmBaseInfo = this;
	CMessage msg(MSG_SCC2SUC_INFO_Notify_Sm_BaseInfo);
	/*DBWriteSet setWriteSet;
	msg.GetDBWriteSet(setWriteSet);
	setWriteSet.AddToByteArray(pSmBaseInfo, sizeof(tagSmInfo));
	setWriteSet.AddToByteArray((BYTE)((m_lSocket != -1) ? 1 : 0));*/
    Json::Value var;
    var["infoTime"] = (double)pSmBaseInfo->dwTime;
    var["smID"] = (int)pSmBaseInfo->dwID;
    var["name"] = pSmBaseInfo->szName;
    var["explain"] = pSmBaseInfo->szExplain;
    var["state"] = (int)pSmBaseInfo->dwState;
    var["regionID"] = (int)pSmBaseInfo->dwRegionID;
    var["regionName"] = pSmBaseInfo->szRegionName;
    var["groupID"] = (int)pSmBaseInfo->dwGroupID;
    var["groupName"] = pSmBaseInfo->szGroupName;
    if(m_lSocket != -1)
        var["result"] = true;
    else
        var["result"] = false;
    msg.AddJson(var);
	UserManage::GetInstance().SendToAll(ePT_Penniless, msg);

	//! 硬件信息
	m_HardInfoList.SendInfoToAll();

	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		m_vGapp[i]->SendInfoToAll();
	}
}

//! 输出所有信息
VOID SM::ShowAllInfo(VOID)
{
	LogTools::OutLogAndTxt("SM：ID=%d Name=%s GappNum=%d", dwID, szName, (long)m_vGapp.size());

	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		m_vGapp[i]->ShowAllInfo();
	}
}


//! 得到所有需要更新到最新版本的GAPP信息
VOID SM::GetUpdateInfo(tagUpdateInfo &UpdateInfo)
{
	for (size_t i = 0; i < m_vGapp.size(); ++i)
	{
		DWORD dwType	= m_vGapp[i]->GetType();

		if (!PackManage::GetInstance().IsHigher(dwType, m_vGapp[i]->GetVersion()))
		{
			UpdateInfo.setType.insert(dwType);
		}
	}
}

//! 添加一个Gapp到SM
VOID SM::AddGapp(tagGappBaseInfo &GappBaseInfo)
{
	if(0 == GappBaseInfo._dwGappID)
	{
		LogTools::OutErrLog("SM::AddGapp error, GappID = 0!!!");
		return;
	}
	Gapp *pGapp = FindGapp(GappBaseInfo._dwGappID);
	if(NULL != pGapp)
	{
		LogTools::OutErrLog("SM::AddGapp error, GAPP being.");
		return;
	}

	pGapp = Gapp::CreateGapp(GappBaseInfo, GetID());
	if(NULL != pGapp)
	{
		m_vGapp.push_back(pGapp);
		pGapp->SendInfoToAll();
	}
}
