#ifndef DB_FactionMemberGroup_H
#define DB_FactionMemberGroup_H

#include "DBFactionMember.h"
#include <string>
#include <list>
#include <map>
#include <set>
#include "../public/DBDefine.h"


class CDBFactionMemberGroup: public CBaseDBEntity
{
public:
	CDBFactionMemberGroup(const CGUID& guid);
	virtual ~CDBFactionMemberGroup();
private:
	CDBFactionMemberGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	void Clear(void);

	virtual CDBFactionMemberGroup& operator=(CDBFactionMemberGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setMemberGroup.clear();
			set<CDBFactionMember*>::iterator itr = right.m_setMemberGroup.begin();
			for(; itr != right.m_setMemberGroup.end(); itr++)
			{
				CDBFactionMember* tGoods = new CDBFactionMember((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setMemberGroup.insert(tGoods);
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBFactionMember*>& GetMemberGroupSet(void) { return m_setMemberGroup; }
private:
	CGUID m_OwnerGUID;
	set<CDBFactionMember*> m_setMemberGroup;
};

#endif//DBGoodsGroup_h