#pragma once
#include "BounsOper.h"

#include "../DB/RsBouns.h"

// 获取奖励
class CGetBounsOper :
	public CBounsOper
{
private:
	CGUID					m_guidBouns;			// 奖励信息guid
	string					m_strPlayerName;		// 领奖的玩家名字
	string					m_strPlayerIP;			// 领奖玩家所在的IP
	DWORD					m_dwWSID;				// WorldServer ID

public:
	CGetBounsOper(eOperType eOt);
	~CGetBounsOper(void);

	void SetBounsGUID(CGUID& guid)			{ m_guidBouns = guid; }
	void SetPlayerName(const char* strName)	{ m_strPlayerName = strName; }
	void SetPlayerIP(const char* strIP)		{ m_strPlayerIP = strIP; }
	void SetWSID(DWORD dwID)				{ m_dwWSID = dwID; }

	virtual BOOL DoOperator();
};
