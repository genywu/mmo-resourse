


#ifndef _TreadMgr_H_
#define _TreadMgr_H_
#pragma once


class CTradeMgr
{
public:
	CTradeMgr(void);
	~CTradeMgr(void);

	void	SetRequestGuid(const CGUID& guid)	{ m_RequestGuid = guid; }
	void	SetSessionGuid(const CGUID& guid)	{ m_SessionGuid = guid; }

	void	StartTrade(const CGUID& sessionGuid,const CGUID& myGuid,long myPlugID,const CGUID& youGuid,long youPlugID);

	void	SetYouLock(bool bLock)	{ m_bYouLocked = bLock; }
	void	SetMeLock(bool bLock)		{ m_bMyLocked = bLock; }

	void	SetCanTrade( bool bCanTrade)	{ m_bCanTrade = bCanTrade; }
	bool	IsCanTrade(void)				{ return m_bCanTrade ; }

private:
	//	请求者GUID
	CGUID	m_RequestGuid;
	//	交易Session
	CGUID	m_SessionGuid;
	//	我的玩家ID
	CGUID		m_MyGuid;				
	//	对方玩家ID
	CGUID		m_YourGuid;			
	//	自己在此次会话中的ID
	long		m_lMyPlugID;			
	//	对方在此次会话中的ID
	long		m_lYouPlugID;			
	//	我方锁定状态标志
	bool		m_bMyLocked;		
	//	对方锁定状态标志
	bool		m_bYouLocked;		
	//	交易是否成功
	bool		m_bResult;
	//	交易是否能够进行
	bool		m_bCanTrade;
};
#endif//_TreadMgr_H_