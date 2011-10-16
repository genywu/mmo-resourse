

#include "../../../../SCMPublic/def/PurviewDef.h"

#pragma once


class OnlineUser
{
public:
	static		OnlineUser*			CreateUser(LONG lSocket, DWORD dwUserType, DWORD dwPurview, const char *pName);
	static		VOID				ReleaseUser(OnlineUser **ppUser);

public:
	//!								
	LONG							GetID(VOID) const {return m_lSocket;}
	//!
	const char*						GetName(VOID) const {return m_szName;}
	//!								踢掉该用户
	VOID							Kick(VOID) const;

	//!								向用户发送文本信息
	VOID							SendInfo(const char *pText) const;
	//!								向用户发送外部消息
	VOID							SendMsg(CMessage &msg) const;

	//!								测试权限
	BOOL							HasPurview(DWORD dwPurviewType) const;

	//!								添加一个控制的GAPP
	VOID							AddCtrlGapp(DWORD dwGappID, DWORD dwSMID);
	//!								删除一个控制的GAPP
	VOID							DelCtrlGapp(DWORD dwGappID);
	

private:
	//!								
	LONG							m_lSocket;
	//!								GroupID
	DWORD							m_dwUserType;
	//!								权限
	DWORD							m_dwPurview;
	//!
	char							m_szName[MAX_USER_NAME_SZIE];

	//!								正在控制的GAPP<GAPPID, SMID>
	map<DWORD, DWORD>				m_mapControlingGappID;

private:
	OnlineUser(VOID);
	OnlineUser(LONG lSocket, DWORD dwUserType, DWORD dwPurview, const char *pName);
	~OnlineUser(VOID);
	OnlineUser(OnlineUser&);
};