

#pragma once

struct tagUserLogin 
{
	DWORD dwVersion;
	char szName[MAX_USER_NAME_SZIE];
	byte szMD5Pwd[MD5_16_PASSWORD_SIZE];			//md5_16加密后的

	tagUserLogin(VOID){memset(this, 0, sizeof(tagUserLogin));}
};

struct tagChangePWD 
{
	byte szMD5PwdOld[MD5_16_PASSWORD_SIZE];
	byte szMD5PwdNew[MD5_16_PASSWORD_SIZE];
	tagChangePWD()	{ memset(this,0,sizeof(tagChangePWD)); }
};


enum eUserLoginError
{
	eULE_NoError,		//! 没有错误

	eULE_VersionErr,	//! SUC与SM对应版本错误
	eULE_Nobody,		//! 无此用户
	eULE_PwdErr,		//! 密码错误

	eULE_SystemErr,		//! 系统错误
};


//! 用户控制类型
enum eUserGappCtrlType
{
	eUGCT_Close,		//! 关闭
	eUGCT_ReStart,		//! 重启或开启
};