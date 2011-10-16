#pragma once

#include "../../../Public/Common/BounsDef.h"
#include "../../../Public/ServerPublic/db/CnPool.h"
class CRsBouns :
	public CMyAdoBase
{
private:
	CRITICAL_SECTION m_csBouns;
public:
	CRsBouns(void);
	virtual ~CRsBouns(void);

	// 加载奖励描述信息
	HRESULT LoadBounsDes(map<DWORD,tagBounsDes> &mapBD);

	// 加载某奖励描述信息
	HRESULT LoadBounsDesByID(_ConnectionPtr& cn, DWORD dwBounsID, tagBounsDes& tgBD);

	// 获取玩家领奖信息
	HRESULT LoadBounsInfo(_ConnectionPtr& cn, const char* strAccount, DWORD dwAreaServerID, DWORD dwMaxBounsNum, list<tagBounsInfo*>& listBounsInfo);

    // 领取奖品 bOper 1-领奖、 2-冻结
    long GetBouns(_ConnectionPtr& cn, const CGUID& guid,const CGUID& playerID, const char* strPlayerName, DWORD dwWSID, const char* strIP, long &lRet);

    HRESULT	FreezeBouns(_ConnectionPtr& cn, const CGUID& guid, DWORD& dwBounsID, long &lRet);

	// 领奖失败回滚
	HRESULT BounsRollBack(_ConnectionPtr& cn, const CGUID& guid);

	// SYSTEMTIME to time_t
	time_t SystemtimeToTime(const SYSTEMTIME &st);

};
