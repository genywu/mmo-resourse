#ifndef DB_GEN_VAR_GROUP_H
#define DB_GEN_VAR_GROUP_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include "../public/DBDefine.h"

using namespace std;
class CDBGenVar;
class CDBGenVarGroup: public CBaseDBEntity
{
public:
	CDBGenVarGroup(const CGUID& guid);
	virtual ~CDBGenVarGroup();
private:
	CDBGenVarGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	virtual CDBGenVarGroup& operator=(CDBGenVarGroup& right);
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBGenVar*>& GetGenVarGroupSet(void) { return m_setGenVarGroup; }
	void Clear(void);
private:
	CGUID m_OwnerGUID;
	set<CDBGenVar*> m_setGenVarGroup;
};


#endif//DB_GEN_VAR_H