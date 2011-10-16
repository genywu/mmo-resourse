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
//	CDBCityWar(const CGUID& guid);
//	~CDBCityWar();
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
//	//Get Interfaces
//	long			GetRegionID(void) { return m_lRegionID;}
//	tagTime&		GetWarStartTime(void) { return m_WarStartTime;}
//	list<CGUID>&		GetDecWarFactions(void) { return m_listDecWarFactions;}
//	//Set Interfaces
//	void SetRegionID(long id) { m_lRegionID = id;}
//	void SetWarStartTime(tagTime& time) { memcpy_s(&m_WarStartTime, sizeof(tagTime), &time, sizeof(tagTime));}
//	void SetDecWarFactions(list<CGUID>& listDF) { m_listDecWarFactions = listDF;}
//
//	AttrNameToEnumMap& GetAttrNameToEnumMap(void) { return m_AttrNameToEnumMap; }
//
//	// Other interfaces
//	// 准备发送的属性
//	//vector<DB_CARD_ATTR_TYPE>& GetSendAttrEnumVector(void) { return m_SendAttrEnumVector; }
//	// 打开准备发送的属性在Vector中的相应位置的标志
//	void OpenAttrFlagInVector(DB_CITY_WAR_ATTR_TYPE attrType);
//	// 关闭准备发送的属性在Vector中的相应位置的标志
//	void CloseAttrFlagInVector(DB_CITY_WAR_ATTR_TYPE attrType);
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
//	
//};
//
#endif//DB_CITY_WAR_H