

#include "Gapp.h"
#include "HardInfoList.h"
#include "../SccPublic.h"

#pragma once

class SM : tagSmInfo
{
public:
	static		SM*					CreateSM(DWORD dwSMID, const char *pName, const char *pExplain, DWORD dwRegionID, LPCSTR pRegionName, DWORD dwGroupID, LPCSTR pGroupName);
	static		VOID				ReleaseSM(SM **ppSM);

public:
	//!
	VOID							Release(VOID);
	//!
	DWORD							GetID(VOID){return dwID;}
	//!
	LONG							GetSocket(VOID){return m_lSocket;}
	//!
	VOID							SetSocket(LONG lSocket);

	//!								添加一个Gapp到SM
	VOID							AddGapp(tagGappBaseInfo &GappBaseInfo);
	//!
	Gapp*							FindGapp(DWORD dwGappID);

	//!								硬件信息管理对象
	HardInfoList&					GetHardInfoList(VOID){return m_HardInfoList;}

	//!								发送自身信息到客户端
	VOID							SendInfoToSocket(LONG lSocket);
	//!								发送自身信息到所有客户端
	VOID							SendInfoToAll(VOID);

	//!								输出所有信息
	VOID							ShowAllInfo(VOID);

public:
	//!								得到所有需要更新到最新版本的GAPP信息
	VOID							GetUpdateInfo(tagUpdateInfo &UpdateInfo);

private:
	//!								
	LONG							m_lSocket;

	//!
	vector<Gapp*>					m_vGapp;

	//!
	HardInfoList					m_HardInfoList;


private:
	SM(DWORD dwSMID, const char *pName, const char *pExplain, DWORD dwRegionID, LPCSTR pRegionName, DWORD dwGroupID, LPCSTR pGroupName);
	~SM(VOID);
	SM(SM&);
};