#pragma once

struct tagUserSetup
{
	DWORD	dwUserType;
	char	szName[MAX_USER_NAME_SZIE];
	char	szPwd[BASE_PASSWORD_SIZE];
	BOOL	bIsCmd;

	tagUserSetup(VOID){memset(this, 0, sizeof(tagUserSetup));}
};

////////////////////////////////////
class UserSetup
{
public:
	UserSetup(VOID);
	~UserSetup(VOID);

public:
	static			UserSetup&			GetInstance(VOID);

public:
	//!									获取配置
	BOOL								LoadSetup(const char *pXmlFilename);
	//!									找到一个用户
	const			tagUserSetup*		FindUser(const char *pName) const;

	//!
	DWORD								GetTimeOutSec(VOID) const {return m_TimeOutSec;}

	//!									开始验证用户的密码和初始密码是否一致
	VOID								BeginUserPwdCmp(VOID);
	//!									响应在数据库中比较用户密码的结果
	VOID								OnUserPwdCmp(LONG lResult, const char *pName, const char *pPwd);
	//!									查看验证是否完成
	BOOL								IsFinishCmp(VOID);

	//!									修改密码（不影响配置文件）
	VOID								ChangePwd(const char *pName, const char *pPwd);

	//!									将二进制密码转换成字符串
	VOID								TransitionMD5ToStr(char *pOut, const byte *pIn);


private:
	//!									用户配置
	map<string, tagUserSetup>			m_mapUserSetup;
	//!									超时秒数
	DWORD								m_TimeOutSec;


private:
	static	UserSetup	c_UserSetup;
};