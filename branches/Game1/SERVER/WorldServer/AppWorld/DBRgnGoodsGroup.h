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
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	
	virtual CDBRgnGoodsGroup& operator=(CDBRgnGoodsGroup& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			m_OwnerGUID = right.m_OwnerGUID;
			m_setGoodsGroup.clear();
			set<CDBRgnGoods*>::iterator itr = right.m_setGoodsGroup.begin();
			for(; itr != right.m_setGoodsGroup.end(); itr++)
			{
				CDBRgnGoods* tGoods = new CDBRgnGoods((*itr)->GetExID());
				*tGoods = *(*itr);
				m_setGoodsGroup.insert(tGoods);
			}
		}
		return *this;
	}
	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void)      { return m_OwnerGUID; }
	set<CDBRgnGoods*>& GetGoodsGroupSet(void) { return m_setGoodsGroup; }
	void	AddOneRgnGoods(CDBRgnGoods* goods);
	void    DelOneRgnGoods(const CGUID& guid);
private:
	CGUID m_OwnerGUID;
	set<CDBRgnGoods*> m_setGoodsGroup;
};

#endif//DBGoodsGroup_h