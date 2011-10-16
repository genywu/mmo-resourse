



#pragma once


class CPlayerPwd
{
public:
	CPlayerPwd(void);
	virtual ~CPlayerPwd(void);

public:
	//!
	enum ePwdState
	{
		//! 锁定
		ePS_Lock,
		//! 打开
		ePS_Free,
		//! 冻结
		ePS_Frost,
	};

public:
	//!
	void						Init(long MaxPwdErrCount, long FrostSec, const char *pPwd = NULL);
	//!							锁上
	void						Lock(void);
	//!							开锁
	ePwdState					Unlock(const char *pPassword);
	//!							测试锁定
	ePwdState					GetState(void);
	//!							修改密码
	ePwdState					ChangePassword(const char *pOldPwd, const char *pNewPwd);
	

	//!							冻结仓库
	void						frost(void);

public:
	//!							得到密码，这个接口只应用于编解码
	const char*					GetPwd(void){return m_szPassword;}
	//!							得到解冻时间，这个接口只应用于编解码
	DWORD						GetThawTime(void){return m_ThawDate;}
	//!							设置解冻时间，这个接口只应用于编解码
	void						SetThawTime(DWORD ThawDate){m_ThawDate = ThawDate; if(0 != ThawDate)m_State = ePS_Frost;};

private:
	//!							最大密码错误次数
	long						m_MaxPwdErrCount;
	//!							冻结秒数
	long						m_FrostSec;

	//!							密码
	char						m_szPassword[PASSWORD_SIZE];

	//!							状态
	ePwdState					m_State;


	
	//!							密码输入错误次数
	long						m_lInputPwdErrCount;
	//!							第一次错误的时刻（一段时间内）
	long						m_lFirstErrTime;
	//!							解冻时刻保存
	DWORD						m_ThawDate;
};