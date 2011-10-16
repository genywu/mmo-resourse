#ifndef DB_CITY_WAR_H
#define DB_CITY_WAR_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include <list>
#include "../public/DBDefine.h"

using namespace std;


//
//class CDBCityWar : public CBaseDBEntity
//{
//public:
//
//	typedef std::map<std::string, DB_CITY_WAR_ATTR_TYPE> AttrNameToEnumMap;
//	typedef AttrNameToEnumMap::iterator AttrNameToEnumMapItr;
//
//	CDBCityWar();
//	CDBCityWar(const CGUID& id);
//	~CDBCityWar();
//
//	void SetID(long id) { m_ID = id; }
//	long GetID(void)    { return m_ID; }
//
//	// virtual interfaces
//	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos);
//
//	void Create(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Delete(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA);
//	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Update(long msgid, BYTE* msgBuf, long& msgBufPos);
//
//virtual void ThreadProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos){ };
//	//Get Interfaces
//	long			GetRegionID(void) { return m_lRegionID;}
//	tagTime&		GetWarStartTime(void) { return m_WarStartTime;}
//	list<CGUID>&		GetDecWarFactions(void) { return m_listDecWarFactions;}
//	//Set Interfaces
//	void SetRegionID(long id) { m_lRegionID = id;}
//	void SetWarStartTime(tagTime& time) { memcpy(&m_WarStartTime, &time, sizeof(time));}
//	void SetDecWarFactions(list<CGUID>& listDF) { m_listDecWarFactions = listDF;}
//
//	AttrNameToEnumMap& GetAttrNameToEnumMap(void) { return m_AttrNameToEnumMap; }
//
//private:
//	AttrNameToEnumMap m_AttrNameToEnumMap;
//	void InitAttrNameToEnumMap(void);
//
//private:
//	//////////////////////////////////////////////////////////////////////////
//	long			m_ID;
//	long			m_lRegionID;
//	tagTime			m_WarStartTime;
//	list<CGUID>		m_listDecWarFactions;			
//	
//};

#endif//DB_CITY_WAR_H