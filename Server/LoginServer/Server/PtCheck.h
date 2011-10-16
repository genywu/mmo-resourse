//-------------------------------------------------------------------------------
// PT’ ∫≈—È÷§. by Fox.		2008-01-17
//-------------------------------------------------------------------------------

#pragma once
#include "stdafx.h"
#include "ptsdk\inc\SDKStuff.h"

class CPtCheck : public CClientInterface, public CInfoViewer
{
public:
	CPtCheck(void);
	~CPtCheck(void);

private:
	CThreadMaster			m_InfoHandleThread;
	CQueueWithLock<string>	m_InfoQueue;
	CAccountServerPtr		m_pAS;
	static void InfoProcess(void *param);

public:
	CAccountServerPtr &GetAS() { return m_pAS; }
	void Initial();
	virtual void GotInfo(int intInfo, const char * szInfo);
	virtual void QuestECard( CSessionPtr &p_session, const char *ecardPos) {};
	virtual void QuestEKey( CSessionPtr &pSession, const char *szChallengeCode );
	virtual void OperateResult( CSessionPtr &pSession, int nOperation, int nRes );
	virtual void RecvDownMessage( CSDMessage& vMsg );
};
