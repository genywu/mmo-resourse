#pragma once
#include "..\..\gameserver\billsession.h"

// ’ ªß≤È—Ø
class CAccountAuthen :
	public CCallbackAccountAuthen
{
public:
	CAccountAuthen(const CGUID &guidPlayer, const DWORD dwIP);
	~CAccountAuthen(void);

public:
	virtual void OnCallback(const string strOrderID, const GSBsipAccountAuthenResDef *pRes);
	virtual void OnTimeOut(const string &strOrderID);

private:
	CGUID			m_guidPlayer;
	DWORD			m_dwIP;
};