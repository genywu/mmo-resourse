//-------------------------------------------------------------------------------
// 统一计费会话. by Fox.		2008-01-23
// 处理 GS2BS 的一次请求到响应的异步回调
//-------------------------------------------------------------------------------

#pragma once
#include "..\AppServer\bsinclude\bsip\bsipmsg.h"

class CCallbackBill
{
public:
	virtual void OnTimeOut(const string &strOrderID) = 0;	// 超时处理

	DWORD	GetLeftTime()				{ return m_dwTimeOut; }
	void	SetLeftTime(DWORD dwTime)	{ m_dwTimeOut=dwTime; }

private:
	DWORD	m_dwTimeOut;									// 超时时间
};

class CCallbackAccountAuthen :
	public CCallbackBill
{
public:
	virtual void OnCallback(const string strOrderID, const GSBsipAccountAuthenResDef *pRes) = 0;
};

class CCallbackAccountProcess :
	public CCallbackBill
{
public:
	virtual void OnCallback(const string strOrderID, const GSBsipAccountLockResDef *p) = 0;
	virtual void OnCallback(const string strOrderID, const GSBsipAccountUnlockResDef *p) = 0;
	virtual void OnCallback(const string strOrderID, const GSBsipConsignTransferResDef *p) = 0;
};

class CCallbackAwardAuthen :
	public CCallbackBill
{
public:
	virtual void OnCallback(const string strOrderID, const GSBsipAwardAuthenResDef *p) = 0;
};

class CCallbackAwardAck :
	public CCallbackBill
{
public:
	virtual void OnCallback(const string strOrderID, const GSBsipAwardAckResDef *p) = 0;
};