#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CBillOper.h"

#include "../../../public/GUID.h"


class CLockRollbackOper:public CBillOper
{
private:

	string m_strCDKey;
	LONG m_lLockNum;	
	LONG m_lGsId;
	CGUID m_OperGuid;

public:			
	CLockRollbackOper(eBillOperType eType);

	~CLockRollbackOper();	

	const char* GetCDKey(){return m_strCDKey.c_str();}
	VOID SetCDKey(char* cdkey){m_strCDKey=cdkey;}	

	VOID SetGSID(LONG lId){m_lGsId=lId;}
	
	VOID SetLockNum(LONG lNum);
	LONG GetLockNum(){return m_lLockNum;}

	VOID SetOperGuid(const CGUID& operGuid){m_OperGuid=operGuid;}
	CGUID& GetOperGuid(){return m_OperGuid;}

public:	
	virtual BOOL DoOpertaion();

	VOID WriteLog();
};