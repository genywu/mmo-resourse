#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CBillOper.h"


class CGetBalanceOper:public CBillOper
{
private:

	string m_strCDKey;
	CGUID m_UserGuid;
	LONG m_lGsId;
	
public:			
	CGetBalanceOper(eBillOperType eType);

	~CGetBalanceOper();	

	const char* GetCDKey(){return m_strCDKey.c_str();}
	VOID SetCDKey(char* cdkey){m_strCDKey=cdkey;}

	VOID SetUserGuid(CGUID& userid){m_UserGuid=userid;}
	CGUID& GetUserGuid(){return m_UserGuid;}

	VOID SetGSID(LONG lId){m_lGsId=lId;}

	


public:	
	virtual BOOL DoOpertaion();
};