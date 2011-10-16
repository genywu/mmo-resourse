#ifndef DB_SETUP_H
#define DB_SETUP_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include "../public/DBDefine.h"
#include <list>

using namespace std;



class CDBSetup : public CBaseDBEntity
{
public:
	CDBSetup();
	CDBSetup(const CGUID& id);
	~CDBSetup();

	long GetID(void) { return m_ID; }
	void SetID(long id) { m_ID = id; }

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
	
	long			GetLeaveWordID(void) { return m_lLeaveWordID;}
	const CGUID&		GetPlayerID(void) { return m_lPlayerID;}

	void SetLeaveWordID(long value) { m_lLeaveWordID = value;}
	void SetPlayerID(const CGUID& value) { m_lPlayerID = value;}


private:
	//////////////////////////////////////////////////////////////////////////
	long	m_lLeaveWordID;
	CGUID	m_lPlayerID;
	long	m_ID;
};

#endif//DB_SETUP_H