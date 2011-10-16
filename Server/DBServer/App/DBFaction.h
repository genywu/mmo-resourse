#ifndef DB_FACTION_H
#define DB_FACTION_H
#include "BaseDBEntity.h"
#include "../../../public/OrganizingDef.h"
#include "../public/DBDefine.h"

#include <list>

using namespace std;


class CDBFactionApplyGroup;
class CDBFactionMemberGroup;
//
class CDBFaction : public CBaseDBEntity
{
	friend class CRsFaction;
public:
	CDBFaction(const CGUID& guid);
	~CDBFaction();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize);
	
	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);
	void				InitProperty(void);

	virtual CDBFaction& operator=(CDBFaction& right);

	CDBFactionApplyGroup* GetFactionApplyGroupPtr(void);
	CDBFactionMemberGroup* GetFactionMemberGroupPtr(void);
private:
	CDBFactionApplyGroup* p_FactionApplyGroup;
	CDBFactionMemberGroup* p_FactionMemberGroup;
};

#endif//DB_FACTION_H