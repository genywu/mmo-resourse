#ifndef DB_ENEMY_FACTION_H
#define DB_ENEMY_FACTION_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include "../public/DBDefine.h"

#include <list>

using namespace std;


//
//class CDBEnemyFaction : public CBaseDBEntity
//{
//public:
//	typedef std::map<std::string, DB_ENEMY_FACTION_ATTR_TYPE> AttrNameToEnumMap;
//	typedef AttrNameToEnumMap::iterator AttrNameToEnumMapItr;
//	
//	CDBEnemyFaction();
//	CDBEnemyFaction(const CGUID& id);
//	~CDBEnemyFaction();
//
//	long GetID(void) { return m_ID; }
//	void SetID(long id) { m_ID = id; }
//	// virtual interfaces
//	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos);
//
//	void Create(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Delete(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA);
//	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Update(long msgid, BYTE* msgBuf, long& msgBufPos);
//	
//
//	//Get Interfaces
//	const CGUID&	GetFactionID1(void) { return m_lFactionID1;}
//	const CGUID&	GetFactionID2(void) { return m_lFactionID2;}
//	DWORD		GetDisandTime(void) { return m_dwDisandTime;}
//	//Set Interfaces
//	void SetFactionID1(const CGUID& id) { m_lFactionID1 = id;}
//	void SetFactionID2(const CGUID& id) { m_lFactionID2 = id;}
//	void SetDisandTime(DWORD time) { m_dwDisandTime = time;}
//
//	AttrNameToEnumMap& GetAttrNameToEnumMap(void) { return m_AttrNameToEnumMap; }
//
//private:
//	AttrNameToEnumMap m_AttrNameToEnumMap;
//	void InitAttrNameToEnumMap(void);
//
//private:
//	//////////////////////////////////////////////////////////////////////////
//	CGUID	m_lFactionID1;
//	CGUID	m_lFactionID2;
//	DWORD	m_dwDisandTime;	
//	long    m_ID;
//};

#endif//DB_ENEMY_FACTION_H