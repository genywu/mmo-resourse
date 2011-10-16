#pragma once
#include "BounsOper.h"

// 冻结奖励
class CFreezeBounsOper :
	public CBounsOper
{
private:
	string					m_strPlayerIP;			// 玩家IP
	tagBounsInfo			*m_pBInfo;				// 奖励信息

public:
	CFreezeBounsOper(eOperType eOt);
	~CFreezeBounsOper(void);

	void SetPlayerIP(const char* strIP)	{ m_strPlayerIP = strIP; }
	tagBounsInfo * GetBounsInfo()		{ return m_pBInfo; }

	virtual BOOL DoOperator();

};
