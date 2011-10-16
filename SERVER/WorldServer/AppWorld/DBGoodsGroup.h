#ifndef DBGoodsGroup_h
#define DBGoodsGroup_h
#include "BaseDBEntity.h"
#include "DBGood.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"

class CDBGoods;

class CDBGoodsGroup: public CBaseDBEntity
{
public:
	CDBGoodsGroup(const CGUID& guid);
	virtual ~CDBGoodsGroup();
private:
	CDBGoodsGroup();
public:
	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);
		
	virtual CDBGoodsGroup& operator=(CDBGoodsGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;

			map<CGUID, CDBGoods*>::iterator itr = m_mapGoodsGroup.begin();
			for(; itr != m_mapGoodsGroup.end(); itr++)
			{
				SAFE_DELETE(itr->second);
			}
			m_mapGoodsGroup.clear();

			itr = right.m_mapGoodsGroup.begin();
			for(; itr != right.m_mapGoodsGroup.end(); itr++)
			{
				CDBGoods* tGoods = new CDBGoods(itr->first);
				*tGoods = *(itr->second);
				m_mapGoodsGroup[itr->first] = tGoods;
			}
		}
		return *this;
	}

	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	map<CGUID, CDBGoods*>& GetGoodsGroupMap(void) { return m_mapGoodsGroup; }

	void Clear(void);
private:
	CGUID m_OwnerGUID;
	map<CGUID, CDBGoods*> m_mapGoodsGroup;
};

#endif//DBGoodsGroup_h