#pragma once
#include "MyNetClient.h"

class CCostNetClient:public CMyNetClient
{
public:
	CCostNetClient();
	~CCostNetClient();
	void HandleClose();
	//virtual void OnReceive(int nErrorCode=0);
	bool GetConnect() {return m_bConnect;}	
	virtual CBaseMessage* CreateMsg(void* pBuf, unsigned long nSize);
	virtual bool IsCheckByCRC()	{return true;}
	CSocketCommands& GetSocketCommand();

	long GetCostTimePoint();
	bool GetIsEverConnectSuccess() {return m_bIsEverConnectSuccess;}
	void SetIsEverConnectSuccess(bool bIsSuccess) {m_bIsEverConnectSuccess=bIsSuccess;}
	bool GetIsRecvConnectMsg() {return m_bIsRecvConnectMsg;}
	void SetIsRecvConnectMsg(bool bFlag) {m_bIsRecvConnectMsg=bFlag;}
private:
	bool m_bIsEverConnectSuccess;
    bool m_bIsRecvConnectMsg;
};