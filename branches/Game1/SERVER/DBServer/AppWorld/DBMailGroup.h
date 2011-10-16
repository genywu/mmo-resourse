#ifndef DBMailGroup_h
#define DBMailGroup_h

#include "DBMail.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"

class CDBMailGroup: public CBaseDBEntity
{
public:
	CDBMailGroup(const CGUID& guid);
	virtual ~CDBMailGroup();
private:
	CDBMailGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Insert(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1, const CGUID& ownerID=NULL_GUID);
	void Delete(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1, const CGUID& ownerID=NULL_GUID);

	void AddToByteArray(vector<BYTE>& pBA);

	
	virtual CDBMailGroup& operator=(CDBMailGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setMailGroup.clear();
			set<CDBMail*>::iterator itr = right.m_setMailGroup.begin();
			for(; itr != right.m_setMailGroup.end(); itr++)
			{
				CDBMail* tGoods = new CDBMail((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setMailGroup.insert(tGoods);
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBMail*>& GetMailGroupSet(void) { return m_setMailGroup; }
	void Clear(void);
private:
	CGUID m_OwnerGUID;
	set<CDBMail*> m_setMailGroup;
};

#endif//DBMailGoodsGroup_h