#include "stdafx.h"
#include "AccountClient.h"

#include "../Net/Message.h"



AccountClient::AccountClient(VOID)
:m_lASSocket(0)
,m_pLoaclAccountValidate(NULL)
{}
AccountClient::~AccountClient(VOID)
{
	//! 将还没有执行的任务忽略
	LoaclAccountValidate::ReleasValidateOperator(&m_pLoaclAccountValidate);
	m_pLoaclAccountValidate = NULL;
}


//! 初始化
BOOL AccountClient::Init()
{
	return FALSE;
}

//! 验证一个账号
BOOL AccountClient::ValidateAccount(tagAccountInfo_ToAS &AccountInfo)
{
	if(0 != GetASSocket())
	{
		//! AS验证
		CMessage msg(MSG_O2A_ACCOUNT_Validate_Request);
		msg.AddEx(&AccountInfo, sizeof(tagAccountInfo_ToAS));
		msg.SendToAS();
		return TRUE;
	}

	//! 本地验证
	return LocalValidate(AccountInfo);
}

//! 在本地保存一个通过验证的账号
VOID AccountClient::SaveAccount(tagAccountInfo_FromAS &AccountInfo_FromAS, const tagSecurityData *pSecurityData)
{
	if(NULL == m_pLoaclAccountValidate)
		m_pLoaclAccountValidate = LoaclAccountValidate::CreateSaveOperator();

	//! 如果添加失败
	if(!m_pLoaclAccountValidate->AddSaveItem(AccountInfo_FromAS, pSecurityData))
	{
		//! 说明缓存已满，执行
		LoaclAccountValidate *pLoaclAccountValidate = m_pLoaclAccountValidate;
		m_pLoaclAccountValidate = NULL;
		if(!pLoaclAccountValidate->SaveResult())
			LoaclAccountValidate::ReleasValidateOperator(&pLoaclAccountValidate);

		//! 重新分配一个可缓存的对象
		m_pLoaclAccountValidate = LoaclAccountValidate::CreateSaveOperator();
		//! 进行添加
		if(NULL != m_pLoaclAccountValidate)
			m_pLoaclAccountValidate->AddSaveItem(AccountInfo_FromAS, pSecurityData);
	}
}

//! 封号
VOID AccountClient::RejectAccount(const char *pCdKey, LONG lEndTime)
{
	if(NULL == pCdKey) return;
	if(0 == strlen(pCdKey)) return;

	CMessage msg(MSG_O2A_ACCOUNT_Reject_Request);
	msg.Add(pCdKey);
	msg.Add(lEndTime);
	msg.SendToAS();
}

//! 本地验证
BOOL AccountClient::LocalValidate(tagAccountInfo_ToAS &AccountInfo)
{
	//assert(0 == GetASSocket());
	AccountInfo._bLocalValidate = true;
	LoaclAccountValidate *pLoaclAccountValidate = LoaclAccountValidate::CreateValidateOperator(AccountInfo);

	assert(NULL != pLoaclAccountValidate);
	if(NULL == pLoaclAccountValidate)
	{
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((LONG)LOGIN_PASSWORDERROR);
		msg.SendToClient(AccountInfo._lSocket);
		return FALSE;
	}
	if(!pLoaclAccountValidate->BeginValidate())
	{
		//! 错误
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((LONG)LOGIN_PASSWORDERROR);
		msg.SendToClient(AccountInfo._lSocket);
		LoaclAccountValidate::ReleasValidateOperator(&pLoaclAccountValidate);
		return FALSE;
	}

	return TRUE;
}