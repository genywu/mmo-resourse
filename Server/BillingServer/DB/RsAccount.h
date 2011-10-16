#pragma once

#include "../../../Public/Common/GoodsExDef.h"
#include "../../../Public/ServerPublic/db/CnPool.h"

class CRsAccount :
	public CMyAdoBase
{
private:
	CRITICAL_SECTION m_CreateAccountCS;
	CRITICAL_SECTION m_LockCS;
	CRITICAL_SECTION m_UnlockCS;
	CRITICAL_SECTION m_RollbackCS;
private:
	//static CRITICAL_SECTION m_LockCS;

public:
	CRsAccount(void);
	~CRsAccount(void);

	CRITICAL_SECTION& GetLockCriticalSection(){return m_LockCS;}

	BOOL CreateNewAccount(_ConnectionPtr& cn,const char* cdkey,int nBalance=0,int nFrozen=0);
	DWORD GetBalance(_ConnectionPtr& cn,const char* cdkey);
	eAccountLockRetVal AccountLock(_ConnectionPtr& cn,const CGUID& lockGuid,const char* cdkey,long lNum);
	BOOL AccountUnlock(_ConnectionPtr& cn,const CGUID& operGuid,const char* cdkey,long lLockNum);
	BOOL AccountUnlockRollBack(_ConnectionPtr& cn,const CGUID& operGuid,const char* cdkey,long lLockNum);

	eAccountLockRetVal RunConsumeProc(_ConnectionPtr& cn,const CGUID& lockGuid,const char* cdkey,long lNum);
	BOOL RunConsumeSuccessProc(_ConnectionPtr& cn,const CGUID& lockGuid);
	BOOL RunConsumeFail(_ConnectionPtr& cn,const CGUID& lockGuid);
	int RunGetBalanceProc(_ConnectionPtr& cn,const char* cdkey);
	void SystemRollback();
};

