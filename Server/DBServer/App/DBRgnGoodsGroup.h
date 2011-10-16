#ifndef DBRgnGoodsGroup_h
#define DBRgnGoodsGroup_h

#include "DBRgnGoods.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"

class CDBRgnGoodsGroup: public CBaseDBEntity
{
public:
	CDBRgnGoodsGroup(const CGUID& guid);
	virtual ~CDBRgnGoodsGroup();
private:
	CDBRgnGoodsGroup();

public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Insert(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	
	virtual CDBRgnGoodsGroup& operator=(CDBRgnGoodsGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_mapGoodsGroup.clear();
			map<CGUID, CDBRgnGoods*>::iterator itr = right.m_mapGoodsGroup.begin();
			for(; itr != right.m_mapGoodsGroup.end(); itr++)
			{
				CDBRgnGoods* tGoods = new CDBRgnGoods(itr->first);
				*tGoods = *(itr->second);
				m_mapGoodsGroup[itr->first] = tGoods;
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	map<CGUID, CDBRgnGoods*>& GetGoodsGroupMap(void) { return m_mapGoodsGroup; }
	void AddDBRgnGoods(CDBRgnGoods* goods);

	CDBRgnGoods* FindRgnGoods(const CGUID& guid);
private:
	CGUID m_OwnerGUID;
	map<CGUID, CDBRgnGoods*> m_mapGoodsGroup;
};

#endif//DBGoodsGroup_h