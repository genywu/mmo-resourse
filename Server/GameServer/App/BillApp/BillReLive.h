#pragma once
#include "../../Server/billsession.h"
#include "../../Server/BillClient.h"

//扣除元宝原地复活
class CBillReLive :
	public CCallbackAccountProcess
{
public:
	CBillReLive(const CGUID &guidBuyer, const CGUID &guidSeller, const DWORD dwIP, const CBillClient::tagAccLockReq &reqLock);
	~CBillReLive(void);

public:
	virtual void OnCallback(const string strOrderID, const GSBsipAccountLockResDef *pRes);
	virtual void OnCallback(const string strOrderID, const GSBsipAccountUnlockResDef *pRes);
	virtual void OnCallback(const string strOrderID, const GSBsipConsignTransferResDef *pRes);
	virtual void OnTimeOut(const string &strOrderID);

	CBillClient::tagAccLockReq &GetLockReq()					{ return m_tagAccLockReq; }
	void SetLockReq(const CBillClient::tagAccLockReq &reqLock)	{ memcpy(&m_tagAccLockReq, &reqLock, sizeof(CBillClient::tagAccLockReq)); }
private:
	CGUID			m_guidBuyer;
	CGUID			m_guidSeller;
	DWORD			m_dwIP;
	CBillClient::tagAccLockReq	m_tagAccLockReq;
};