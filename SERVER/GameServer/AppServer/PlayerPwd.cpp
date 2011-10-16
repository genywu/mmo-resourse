



#include "StdAfx.h"
#include "PlayerPwd.h"



CPlayerPwd::CPlayerPwd(void)
:m_State(ePS_Lock)
,m_lInputPwdErrCount(0)
{
	memset(m_szPassword, 0, PASSWORD_SIZE);
}

CPlayerPwd::~CPlayerPwd(void)
{

}

//!
void CPlayerPwd::Init(long MaxPwdErrCount, long FrostSec, const char *pPwd)
{
	m_MaxPwdErrCount = MaxPwdErrCount;
	m_FrostSec = FrostSec;
	if (NULL != pPwd && PASSWORD_SIZE > strlen(pPwd))
	{
		strcpy_s(m_szPassword, PASSWORD_SIZE, pPwd);
		if (0 == m_szPassword[0])
			m_State = ePS_Free;
	}
}

//! 	锁上
void CPlayerPwd::Lock(void)
{
	if(ePS_Free == m_State && 0 != m_szPassword[0])
		m_State = ePS_Lock;
}

//! 	开锁
CPlayerPwd::ePwdState CPlayerPwd::Unlock(const char *pPassword)
{
	if (0 == strcmp(pPassword, m_szPassword))
	{	
		m_State = ePS_Free;
		m_lFirstErrTime = 0;
	}
	else
	{
		++m_lInputPwdErrCount;
		if (1 == m_lInputPwdErrCount)
		{
			time(&m_lFirstErrTime);
		}
		else
		{
			if (m_MaxPwdErrCount < m_lInputPwdErrCount)
			{
				if (time(NULL) > m_lFirstErrTime)
				{
					frost();
				}
				else
				{
					time(&m_lFirstErrTime);
				}
			}
		}
	}

	return m_State;
}

//! 	修改密码
CPlayerPwd::ePwdState CPlayerPwd::ChangePassword(const char *pOldPwd, const char *pNewPwd)
{
	if (ePS_Free == GetState())
		if (PASSWORD_SIZE > strlen(pNewPwd) && 0 != strcmp(pNewPwd, m_szPassword))
			strcpy_s(m_szPassword, PASSWORD_SIZE, pNewPwd);

	return m_State;
}

//! 测试锁定
CPlayerPwd::ePwdState CPlayerPwd::GetState(void)
{
	//! 如果到达解冻时间，则解冻
	if (ePS_Frost == m_State && time(NULL) > m_ThawDate)
	{
		if (0 == m_szPassword[0])
			m_State = ePS_Free;
		else
			m_State = ePS_Lock;
		m_ThawDate = 0;
	}
	return m_State;
}

void CPlayerPwd::frost(void)
{
	Lock();
	m_State = ePS_Frost;
	m_ThawDate = time(NULL) + m_FrostSec;
	m_lFirstErrTime = 0;
}