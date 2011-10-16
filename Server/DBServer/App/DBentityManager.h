#ifndef DB_ENTITY_MANAGER_H
#define DB_ENTITY_MANAGER_H
#include "../public/BaseDef.h"
#include "../public/DBDefine.h"
#include "DBEntityDef.h"
#include "..\..\..\dbaccess\myadobase.h"


class CBaseDBEntity;
class CDBCard;
class CDBGoods;
class CDBLoginPlayer;
class CDBPlayer;
class CDBFaction;
class CDBCityWar;
class CDBVillageWar;
class CDBUnion;
class CDBCountry;
class CDBEnemyFaction;
class CDBGenVar;
class CDBSetup;
class CDBRegion;
class CDBDupRgn;
class CDBGoodsGroup;
class CDBRgnGoods;
class CDBRgnGoodsGroup;
class CDBLinkMan;
class CDBLinkmanGroup;
class CDBFactionApply;
class CDBFactionApplyGroup;
class CDBFactionMember;
class CDBFactionMemberGroup;
class CDBMail;
class CDBMailGroup;
class CDBMailGoods;
class CDBMailGoodsGroup;
class CDBGenVarGroup;

class DBAdoBase;
class DBLoadThread;
class DBSaveThread;
class DBAccessThread;

struct LoadObj;
struct SaveObj;
struct LoadThreadData;
struct SaveThreadData;
#include <set>
#include <map>

#define MAX_DB_ENTITY_NUM 1024

class CDataEntityManager;

class CDBEntityManager
{
public:
	CDBEntityManager();
	~CDBEntityManager();

	// CDBLoginPlayer
	typedef std::map<CGUID, CDBLoginPlayer*> DBLoginPlayerMap;
	typedef DBLoginPlayerMap::iterator DBLoginPlayerMapItr;
	// CDKEY 是唯一的 同一个CDBLoginPlayer拥有多个CDBPlayer对象
	typedef std::map<string, set<CGUID>> CdkeyDBLoginPlayerMap;
	typedef CdkeyDBLoginPlayerMap::iterator CdkeyDBLoginPlayerMapItr;

	// CDBPlayer
	typedef std::map<CGUID, CDBPlayer*> DBPlayerMap;
	typedef DBPlayerMap::iterator DBPlayerMapItr;

	// CDBFaction
	typedef std::map<CGUID, CDBFaction*> DBFactionMap;
	typedef DBFactionMap::iterator DBFactionMapItr;
	// CDBCityWar
	typedef std::map<CGUID, CDBCityWar*> DBCityWarMap;
	typedef DBCityWarMap::iterator DBCityWarMapItr;
	// CDBVillageWar
	typedef std::map<CGUID, CDBVillageWar*> DBVillageWarMap;
	typedef DBVillageWarMap::iterator DBVillageWarMapItr;
	// CDBUnion
	typedef std::map<CGUID, CDBUnion*> DBUnionMap;
	typedef DBUnionMap::iterator DBUnionMapItr;
	// CDBCountry
	typedef std::map<CGUID, CDBCountry*> DBCountryMap;
	typedef DBCountryMap::iterator DBCountryMapItr;
	// CDBEnemyFaction
	typedef std::map<CGUID, CDBEnemyFaction*> DBEnemyFactionMap;
	typedef DBEnemyFactionMap::iterator DBEnemyFactionMapItr;

	// CDBGenVar
	typedef std::map<CGUID, CDBGenVar*> DBGenVarMap;
	typedef DBGenVarMap::iterator DBGenVarMapItr;
	// CDBSetup
	typedef std::map<CGUID, CDBSetup*> DBSetupMap;
	typedef DBSetupMap::iterator DBSetupMapItr;
	// CDBRegion
	typedef std::map<CGUID, CDBRegion*> DBRegionMap;
	typedef DBRegionMap::iterator DBRegionMapItr;
	// CDBDupRgn
	typedef std::map<CGUID, CDBDupRgn*> DBDupRgnMap;
	typedef DBDupRgnMap::iterator DBDupRgnMapItr;

	typedef std::map<CGUID, CDBLinkMan*> DBLinkmanMap;
	typedef DBLinkmanMap::iterator DBLinkmanMapItr;

	typedef std::map<CGUID, CDBFactionApply*> DBFactionApplyMap;
	typedef DBFactionApplyMap::iterator DBFactionApplyMapItr;

	typedef std::map<CGUID, CDBFactionMember*> DBFactionMemberMap;
	typedef DBFactionMemberMap::iterator DBFactionMemberMapItr;

	// 初始化DBThread
	void InitDBThread(string strProvider, 
		string strDataSource, 
		string strInitialCatalog,
		string strUserID, 
		string strPassword);

	void ProcessMsg(CMessage* msg);

	//-------------- interfaces

	// CDBLoginPlayer
	// 添加一个CDBLoginPlayer到Map末尾
	void AddOneDBLoginPlayer(CDBLoginPlayer* player);
	// 删除一个CDBLoginPlayer
	void DelOneDBLoginPlayer(CDBLoginPlayer* player);
	// 删除一个CDBLoginPlayer
	void DelOneDBLoginPlayer(const CGUID& playerID);
	// 清空链表
	void ClearLoginPlayerMap(void);
	// 取得个数
	long GetDBLoginPlayerNum(void) { return m_mapDBLoginPlayers.size(); }
	DBLoginPlayerMapItr DBLoginPlayerMapBegin(void) { return m_mapDBLoginPlayers.begin(); }
	DBLoginPlayerMapItr DBLoginPlayerMapEnd(void) { return m_mapDBLoginPlayers.end(); }
	DBLoginPlayerMap& GetDBLoginPlayerMap(void) { return m_mapDBLoginPlayers; }
	CDBLoginPlayer* FindLoginPlayer(const CGUID& guid);

	// 根据 CDKEY 查找 CDBLoginPlayer,此为唯一关键字
	// 同一CDKEY对应一个vecotor<CDBLoginPlayer>:即所有同CDKEY下的所有角色
	// 添加一个CDBLoginPlayer到Map末尾
	void AddLoginPlayerByCdkey(CDBLoginPlayer* player);
	// 删除一个CDBLoginPlayer
	void DelLoginPlayerByCdkey(CDBLoginPlayer* player);
	// 清空链表
	void ClearCdkeyLoginPlayerMap(void);
	// 取得个数
	CdkeyDBLoginPlayerMap& GetCdkeyLoginPlayerMap(void) { return m_mapCdkeyLoginPlayers; }
	set<CGUID>* FindLoginPlayerByCdkey(const char* szCdkey);

	// CDBPlayer
	// 添加一个DBPlayer到Map末尾
	void AddOneDBPlayer(CDBPlayer* player);
	// 删除一个DBPlayer
	void DelOneDBPlayer(const CGUID& playerID);
	// 清空链表
	void ClearPlayerMap(void);
	// 取得个数
	long GetDBPlayerNum(void) { return m_mapDBPlayers.size(); }
	DBPlayerMapItr DBPlayerMapBegin(void) { return m_mapDBPlayers.begin(); }
	DBPlayerMapItr DBPlayerMapEnd(void) { return m_mapDBPlayers.end(); }
	DBPlayerMap& GetDBPlayerMap(void) { return m_mapDBPlayers; }
	CDBPlayer* FindPlayer(const CGUID& guid);
	

	// CDBFaction
	// 添加一个DBFaction到Map末尾
	void AddOneDBFaction(CDBFaction* faction);
	// 删除一个DBFaction
	void DelOneDBFaction(CDBFaction* faction);
	// 删除一个DBFaction
	void DelOneDBFaction(const CGUID& factionID);
	// 清空链表
	void ClearFactionMap(void);
	// 取得个数
	long GetDBFactionNum(void) { return m_mapDBFactions.size(); }
	DBFactionMapItr DBFactionMapBegin(void) { return m_mapDBFactions.begin(); }
	DBFactionMapItr DBFactionMapEnd(void) { return m_mapDBFactions.end(); }
	DBFactionMap& GetDBFactionMap(void) { return m_mapDBFactions; }
	CDBFaction* FindFaction(const CGUID& guid);

	//!							工会自定义徽章列表
	map<CGUID, vector<BYTE>>	m_mapFactionIcon;

	void AddOneDBUnion(CDBUnion* Union);
	// 删除一个DBUnion
	void DelOneDBUnion(CDBUnion* Union);
	// 删除一个DBUnion
	void DelOneDBUnion(const CGUID& UnionID);
	// 清空链表
	void ClearUnionMap(void);
	// 取得个数
	long GetDBUnionNum(void) { return m_mapDBUnions.size(); }
	DBUnionMapItr DBUnionMapBegin(void) { return m_mapDBUnions.begin(); }
	DBUnionMapItr DBUnionMapEnd(void) { return m_mapDBUnions.end(); }
	DBUnionMap& GetDBUnionMap(void) { return m_mapDBUnions; }

	// CDBCountry
	// 添加一个DBCountry到Map末尾
	void AddOneDBCountry(CDBCountry* Country);
	// 删除一个DBCountry
	void DelOneDBCountry(CDBCountry* Country);
	// 删除一个DBCountry
	void DelOneDBCountry(const CGUID& CountryID);
	// 清空链表
	void ClearCountryMap(void);
	// 取得个数
	long GetDBCountryNum(void) { return m_mapDBCountrys.size(); }
	DBCountryMapItr DBCountryMapBegin(void) { return m_mapDBCountrys.begin(); }
	DBCountryMapItr DBCountryMapEnd(void) { return m_mapDBCountrys.end(); }
	DBCountryMap& GetDBCountryMap(void) { return m_mapDBCountrys; }

	void AddOneDBGenVar(CDBGenVar* GenVar);
	// 删除一个DBGenVar
	void DelOneDBGenVar(CDBGenVar* GenVar);
	// 删除一个DBGenVar
	void DelOneDBGenVar(const CGUID& GenVarID);
	// 清空链表
	void ClearGenVarMap(void);
	// 取得个数
	long GetDBGenVarNum(void) { return m_mapDBGenVars.size(); }
	DBGenVarMapItr DBGenVarMapBegin(void) { return m_mapDBGenVars.begin(); }
	DBGenVarMapItr DBGenVarMapEnd(void) { return m_mapDBGenVars.end(); }
	DBGenVarMap& GetDBGenVarMap(void) { return m_mapDBGenVars; }

	// CDBSetup
	// 添加一个DBSetup到Map末尾
	void AddOneDBSetup(CDBSetup* Setup);
	// 删除一个DBSetup
	void DelOneDBSetup(CDBSetup* Setup);
	// 删除一个DBSetup
	void DelOneDBSetup(const CGUID& SetupID);
	// 清空链表
	void ClearSetupMap(void);
	// 取得个数
	long GetDBSetupNum(void) { return m_mapDBSetups.size(); }
	DBSetupMapItr DBSetupapBegin(void) { return m_mapDBSetups.begin(); }
	DBSetupMapItr DBSetupMapEnd(void) { return m_mapDBSetups.end(); }
	DBSetupMap& GetDBSetupMap(void) { return m_mapDBSetups; }

	// CDBRegion
	// 添加一个DBRegion到Map末尾
	void AddOneDBRegion(CDBRegion* Region);
	// 删除一个DBRegion
	void DelOneDBRegion(CDBRegion* Region);
	// 删除一个DBRegion
	void DelOneDBRegion(const CGUID& RegionID);
	// 清空链表
	void ClearRegionMap(void);
	// 取得个数
	long GetDBRegionNum(void) { return m_mapDBRegions.size(); }
	DBRegionMapItr DBRegionMapBegin(void) { return m_mapDBRegions.begin(); }
	DBRegionMapItr DBRegionMapEnd(void) { return m_mapDBRegions.end(); }
	DBRegionMap& GetDBRegionMap(void) { return m_mapDBRegions; }

	// CDBDupRgn
	// 添加一个CDBDupRgn到Map末尾
	void AddOneDBDupRgn(CDBDupRgn* Region);
	// 删除一个CDBDupRgn
	void DelOneDBDupRgn(CDBDupRgn* Region);
	// 删除一个CDBDupRgn
	void DelOneDBDupRgn(const CGUID& RegionID);
	// 清空链表
	void ClearDupRgnMap(void);
	// 取得个数
	long GetDBDupRgnNum(void) { return m_mapDBDupRgns.size(); }
	DBDupRgnMapItr DBDupRgnMapBegin(void) { return m_mapDBDupRgns.begin(); }
	DBDupRgnMapItr DBDupRgnMapEnd(void) { return m_mapDBDupRgns.end(); }
	DBDupRgnMap& GetDBDupRgnMap(void) { return m_mapDBDupRgns; }

	// CDBLinkman
	// 添加一个CDBLinkman到Map末尾
	void AddOneDBLinkman(CDBLinkMan* good);
	// 删除一个DBGood
	void DelOneDBLinkman(CDBLinkMan* good);
	// 删除一个DBGood
	void DelOneDBLinkman(const CGUID& goodID);
	// 清空链表
	void ClearDBLinkmanMap(void);
	// 清空所属玩家的信息
	void ClearDBLinkmanMap(const CGUID& ownerID);
	// 取得个数
	long GetDBLinkmanNum(void) { return m_mapDBLinkman.size(); }
	DBLinkmanMapItr DBLinkmanMapBegin(void) { return m_mapDBLinkman.begin(); }
	DBLinkmanMapItr DBLinkmanMapEnd(void) { return m_mapDBLinkman.end(); }
	DBLinkmanMap& GetDBLinkmanMap(void) { return m_mapDBLinkman; }
	CDBLinkMan* FindDBLinkman(const CGUID& guid) 
	{
		CDBLinkMan* ret = NULL;
		DBLinkmanMapItr itr = m_mapDBLinkman.find(guid);
		if(itr != m_mapDBLinkman.end())
			ret = itr->second;
		return ret;
	}
	bool HasDBLinkman(const CGUID& ownerID, long place);

	// 产生ID
	bool GenerateGUID(CGUID& guid);

	// 从消息取得操作对象
	CBaseDBEntity* FindEntityFromMsg(DB_OBJ_TYPE type, const CGUID& entityid, const CGUID& ownerID);
	// 创建对象
	CBaseDBEntity* CreateEntityByMsg(DB_OBJ_TYPE type, const CGUID& guid);

	// 多线程数据库连接管理对象
public:
	map<long, DBLoadThread*>& GetLoadThreadMap(void) { return m_mapLoadThread; }
	map<long, DBSaveThread*>& GetSaveThreadMap(void) { return m_mapSaveThread; }
	DBLoadThread* FindLoadThread(long id);
	DBSaveThread* FindSaveThread(long id);
	DBAccessThread* GetAccessThread(void) { return m_pAccessThread; }

	// 在内存中验证玩家Account是否存在
	bool	ValidatePlayerIDInCdkey(const char* szCDKEY, const CGUID& guid);
private:
	map<long, DBLoadThread*>	m_mapLoadThread;
	map<long, DBSaveThread*>	m_mapSaveThread;
	DBAccessThread*				m_pAccessThread;

public:
	//*************************
	//输出指定内容 log信息
	void PutSomeThingLogInfo(const char *msg);
	//*************************
	// 保存日志
	void SaveSomeThingLogText(bool bForce);
	// 加入指定内容 一条记录
	void AddSomeThingLogText(char* msg, ...);

	//!	压入一个LoadThreadData
	bool PushLoadThreadData(LoadThreadData* loadData);
	//!	弹出一个LoadThreadData
	bool PopLoadThreadData(LoadThreadData** loadData,long dwThreadID);
	//!	压入一个已被处理的LoadThreadData
	bool PushDisposedLoadThreadData(LoadThreadData* loadData);
	//!	压入多个已被处理的LoadThreadData
	bool PushDisposedLoadThreadData(std::list<LoadThreadData*>& loadDataList);
	//! 弹出一个已被处理的LoadThreadData
	bool PopDisposedLoadThreadData(std::list<LoadThreadData*>& loadDataList);

	//!	压入一个SaveThreadData
	bool PushSaveThreadData(SaveThreadData* saveData);
	//!	弹出一个SaveThreadData
	bool PopSaveThreadData(std::list<SaveThreadData*>& saveDataList);
	//!	压入多个已被处理的SaveThreadData
	bool PushDisposedSaveThreadData(SaveThreadData* saveData);
	//! 弹出一个已被处理的SaveThreadData
	bool PopDisposedSaveThreadData(SaveThreadData** saveData);

	//!								设置LoadEvent
	static void						SetLoadEvent(void);
	//!								等待LoadEvent
	static void						WaitLoadEvent(void);
	static void						ResetLoadEvent(void);

	//!								设置SaveEvent
	static void						SetSaveEvent(void);
	//!								等待SaveEvent
	static void						WaitSaveEvent(void);
	static void						ResetSaveEvent(void);

	//!					在DB中创建表格（按照配置中读取的表格信息），并为每个表格创建一个时间聚集索引
	//!					为防止日志记录被误删除，不生成DROP语句
	void				CreateOtherTableInDb(const string& tableName, const string& iniName, bool isIndexed=true);
	// 初始化属性配置
	void				InitPropertyIniData();
	void				LoadObjStructInfo(const char* szPath);
	
	std::map<string/*attrname*/, CDataEntityManager*>& GetObjAttrDef(void)		{ return m_mapObjAttrDef; }


	void				ExecSqlString(const string& sqlStr);

	// 预读策略
	void PreLoadPlayerBase();
	// 预读策略
	void PreLoadPlayerAttrs();

	CDBGenVarGroup*   GetGenVarGroup(void);
private:
	CDBGenVarGroup*   m_pTmpGenVarGroup;

	//!					SQL队列临界对象
	CRITICAL_SECTION	m_LoadCriticalSection;
	CRITICAL_SECTION	m_DisposedLoadCriticalSection;

	//!					SQL队列临界对象
	CRITICAL_SECTION	m_SaveCriticalSection;
	CRITICAL_SECTION	m_DisposedSaveCriticalSection;

	DBLoginPlayerMap m_mapDBLoginPlayers;
	CdkeyDBLoginPlayerMap m_mapCdkeyLoginPlayers;
	DBPlayerMap m_mapDBPlayers;

	DBLinkmanMap  m_mapDBLinkman;
	//DBCityWarMap m_mapDBCityWars;
	//!							工会列表
	DBFactionMap				m_mapDBFactions;

	//DBVillageWarMap m_mapDBVillageWars;
	DBUnionMap m_mapDBUnions;
	DBCountryMap m_mapDBCountrys;
	//DBEnemyFactionMap m_mapDBEnemyFactions;
	DBGenVarMap m_mapDBGenVars;
	DBSetupMap m_mapDBSetups;
	DBRegionMap m_mapDBRegions;
	DBDupRgnMap m_mapDBDupRgns;

	// 将被处理的读取数据
	std::list<LoadThreadData*> m_listLoadThreadData;
	// 已被处理的读取数据
	std::list<LoadThreadData*> m_listDisposedLoadThreadData;

	// 将被处理的写入数据
	std::list<SaveThreadData*> m_listSaveThreadData;
	// 已被处理的写入数据
	std::list<SaveThreadData*> m_listDisposedSaveThreadData;
	//!					Load事件句柄
	static HANDLE		m_hLoadEvent;
	//!					Save事件句柄
	static HANDLE		m_hSaveEvent;

	// Obj属性信息,Obj对象的属性名,id
	std::map<string, CDataEntityManager*> m_mapObjAttrDef;
};

#endif//DB_ENTITY_MANAGER_H