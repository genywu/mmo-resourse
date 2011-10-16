#ifndef DBMailGoodsGroup_h
#define DBMailGoodsGroup_h

#include "DBMailGoods.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"

class CDBMailGoodsGroup: public CBaseDBEntity
{
public:
	CDBMailGoodsGroup(const CGUID& guid);
	virtual ~CDBMailGoodsGroup();
private:
	CDBMailGoodsGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	
	virtual CDBMailGoodsGroup& operator=(CDBMailGoodsGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setGoodsGroup.clear();
			set<CDBMailGoods*>::iterator itr = right.m_setGoodsGroup.begin();
			for(; itr != right.m_setGoodsGroup.end(); itr++)
			{
				CDBMailGoods* tGoods = new CDBMailGoods((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setGoodsGroup.insert(tGoods);
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBMailGoods*>& GetGoodsGroupSet(void) { return m_setGoodsGroup; }
private:
	CGUID m_OwnerGUID;
	set<CDBMailGoods*> m_setGoodsGroup;
};

#endif//DBMailGoodsGroup_h