#ifndef DB_VILLAGE_WAR_H
#define DB_VILLAGE_WAR_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include "../public/DBDefine.h"
#include <list>

using namespace std;

//
//class CDBVillageWar : public CBaseDBEntity
//{
//public:
//	typedef std::map<std::string, DB_VILLAGE_WAR_ATTR_TYPE> AttrNameToEnumMap;
//	typedef AttrNameToEnumMap::iterator AttrNameToEnumMapItr;
//	CDBVillageWar();
//	CDBVillageWar(const CGUID& id);
//	~CDBVillageWar();
//
//	long GetID(void) { return m_ID; }
//	void SetID(long id) { m_ID = id; }
//
//	// virtual interfaces
//	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos);
//
//	void Create(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Delete(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Load(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);
//	void Update(long msgid, BYTE* msgBuf, long& msgBufPos);
//
//	long			GetRegionID(void)		{ return m_lRegionID; }
//	tagTime&		GetWarStartTime(void)	{ return m_WarStartTime;}
//	list<CGUID>&		GetDecWarFactions(void) { return m_listDecWarFactions;}
//
//	void SetRegionID(long value)				{ m_lRegionID = value; }
//	void SetWarStartTime(tagTime& value)		{ m_WarStartTime = value;}
//	void SetDecWarFactions(list<CGUID>& value)	{ m_listDecWarFactions = value;}
//
//	AttrNameToEnumMap& GetAttrNameToEnumMap(void) { return m_AttrNameToEnumMap; }
//
//	// Other interfaces
//	// 准备发送的属性
//	//vector<DB_CARD_ATTR_TYPE>& GetSendAttrEnumVector(void) { return m_SendAttrEnumVector; }
//	// 打开准备发送的属性在Vector中的相应位置的标志
//	void OpenAttrFlagInVector(DB_VILLAGE_WAR_ATTR_TYPE attrType);
//	// 关闭准备发送的属性在Vector中的相应位置的标志
//	void CloseAttrFlagInVector(DB_VILLAGE_WAR_ATTR_TYPE attrType);
//	// 组装消息
//	void AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA);
//
//private:
//	AttrNameToEnumMap m_AttrNameToEnumMap;
//	void InitAttrNameToEnumMap(void);
//
//private:
//	//////////////////////////////////////////////////////////////////////////
//	long			m_lRegionID;
//	tagTime			m_WarStartTime;
//	list<CGUID>		m_listDecWarFactions;
//	long			m_ID;
//};


#endif//DB_VILLAGE_WAR_H