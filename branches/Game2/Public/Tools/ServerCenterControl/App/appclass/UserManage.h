#pragma once

#include "../../../../SCMPublic/def/UserDef.h"
#include "../Setup/UserSetup.h"
#include "OnlineUser.h"

class UserManage
{
public:
	UserManage(VOID);
	~UserManage(VOID);

public:
	//!
	static		UserManage&		GetInstance(VOID);

public:
	//!							初始化对象
	BOOL						Init(VOID);
	//!							释放对象
	VOID						Release(VOID);

public:
	VOID						SendMsgToUser(const char *pUserName, CMessage &Msg);
	//!							按权限发送给所有用户
	VOID						SendToAll(DWORD dwPurviewType, CMessage &Msg);

public:
	//!							响应客户端连接
	VOID						OnUserConn(LONG lSocket);
	//!							响应用户登陆
	VOID						OnUserLogin(tagUserLogin &UserLoginInfo, LONG lSocket);
	//!							响应用户断开
	VOID						OnUserClose(LONG lSocket);

	//!							响应用户修改密码
	VOID						OnUserChangePwd(tagChangePWD &ChangePWD, LONG lSocket);
	//!							找到在线用户
	OnlineUser*					FindUser(LONG lUserID);
	//!							找到用户名字
	const		string&			FindUserName(LONG lUserID);

private:
	//!							读取配置
	BOOL						LoadSetup(VOID);
	//!							初始化用户
	BOOL						InitUser(VOID);
	
	const		OnlineUser*		FindUser(const char *pUserName);
	
	//!							踢在线用户
	VOID						ClearUser(const char *pUserName, const char *pKickInfo = NULL);
	VOID						ClearUser(LONG lUserID);

private:
	//!							在线用户
	map<LONG, OnlineUser*>		m_mapOnlineUser;
	//!							在线用户的对应表
	map<string, LONG>			m_mapCdkeyToSocket;

private:
	//!
	static	UserManage		c_UserManage;
};