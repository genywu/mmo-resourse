


#ifndef _ShopMgr_H_
#define _ShopMgr_H_
#pragma once

class CPlayerShop
{
public:
	CPlayerShop(void)	{ }
	~CPlayerShop(void) { Release(); }

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead);

	void SetPlugID(long lID)					{ m_lPlugID = lID; }
	long GetPlugID(void)						{ return m_lPlugID; }

	void SetSssionID(const CGUID& guid)		{ m_SessionGuid = guid; }
	const CGUID GetSessionID() const				{ return m_SessionGuid; }

	bool IsOpened(void)					{ return m_bOpened; }
	void SetOpened(bool bOpened)			{ m_bOpened = bOpened; } 

	void SetName(const char* szName)		{ strcpy(m_szName,szName); }

private:
	bool	m_bOpened;
	char	m_szName[32];
	char	m_szTradePlayerName[32];
	CGUID	m_SessionGuid;
	long	m_lPlugID;

};
#endif//_ShopMgr_H_