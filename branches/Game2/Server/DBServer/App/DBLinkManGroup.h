#ifndef DBLinkmanGroup_h
#define DBLinkmanGroup_h

#include "DBLinkman.h"
#include <string>
#include <set>
#include <map>
#include "../public/DBDefine.h"

class CDBLinkmanGroup: public CBaseDBEntity
{
public:
	CDBLinkmanGroup(const CGUID& guid);
	virtual ~CDBLinkmanGroup();
private:
	CDBLinkmanGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);


	virtual CDBLinkmanGroup& operator=(CDBLinkmanGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setLinkmanGroup.clear();
			set<CDBLinkMan*>::iterator itr = right.m_setLinkmanGroup.begin();
			for(; itr != right.m_setLinkmanGroup.end(); itr++)
			{
				CDBLinkMan* tGoods = new CDBLinkMan((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setLinkmanGroup.insert(tGoods);
			}
		}
		return *this;
	}

	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBLinkMan*>& GetLinkmanGroupSet(void) { return m_setLinkmanGroup; }
private:
	set<CDBLinkMan*> m_setLinkmanGroup;
	CGUID m_OwnerGUID;
};

#endif//DBGoodsGroup_h