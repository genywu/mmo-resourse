#pragma once
#include "BounsOper.h"

#include "../DB/RsBouns.h"

class CGetBounsInfoOper :
	public CBounsOper
{
private:
	string					m_strAccount;		// 玩家账号
	DWORD					m_dwAreaServerID;	// 区服ID
	list<tagBounsInfo*>		m_listBounsInfo;

public:
	CGetBounsInfoOper(eOperType eOt);
	virtual ~CGetBounsInfoOper(void);


	void SetAccount(const char* strAccount)	{ m_strAccount = strAccount; }
	const char* GetAccount()	{ return m_strAccount.c_str(); }

	void SetAreaServerID(DWORD dwID)	{ m_dwAreaServerID = dwID; }
	DWORD GetAreaServerID()				{ return m_dwAreaServerID; }


	virtual BOOL DoOperator();

	// 检测奖励信息是否完整
	void CheckBounsDes();
};
