#ifndef DBFactionApplyGroup_h
#define DBFactionApplyGroup_h

#include "DBFactionApply.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"

class CDBFactionApplyGroup: public CBaseDBEntity
{
public:
	CDBFactionApplyGroup(const CGUID& guid);
	virtual ~CDBFactionApplyGroup();
private:
	CDBFactionApplyGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	void Clear(void);

	virtual CDBFactionApplyGroup& operator=(CDBFactionApplyGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setApplyGroup.clear();
			set<CDBFactionApply*>::iterator itr = right.m_setApplyGroup.begin();
			for(; itr != right.m_setApplyGroup.end(); itr++)
			{
				CDBFactionApply* tGoods = new CDBFactionApply((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setApplyGroup.insert(tGoods);
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBFactionApply*>& GetApplayGroupSet(void) { return m_setApplyGroup; }
private:
	CGUID m_OwnerGUID;
	set<CDBFactionApply*> m_setApplyGroup;
};

#endif//DBGoodsGroup_h