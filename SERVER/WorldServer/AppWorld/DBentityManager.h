#ifndef DB_ENTITY_MANAGER_H
#define DB_ENTITY_MANAGER_H

#include "DBEntityDef.h"

class CBaseDBEntity;
class CDBCard;
class CDBLoginPlayer;
class CDBPlayer;
class CDBFaction;
//class CDBCityWar;
//class CDBVillageWar;
class CDBUnion;
class CDBCountry;
//class CDBEnemyFaction;
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
class CMail;
class CDBMail;
class CDBMailGroup;
class CDBMailGoods;
class CDBMailGoodsGroup;
class CDBGenVarGroup;

class CGoods;
class CPlayer;
class CFaction;
class CCard;
struct tagAttackCityTime;
class CCountry;
struct tagEnemyFaction;
class CVariableList;
class CWorldRegion;
class CUnion;
struct tagVilWarSetup;

#include <map>

#define MAX_DB_ENTITY_ID 10240
class CDataEntityManager;

class CDBEntityManager
{
public:
	CDBEntityManager();
	~CDBEntityManager();

	struct tagLoginInfo
	{
		set<CGUID> setLoginGuid;
		long		lSignCode;	// 登录验证码
		long		lLastLoginTime; // 上次退出时间
		ePlayerState  PlayerState;

		tagLoginInfo(void);
		~tagLoginInfo(void);
	};

	// CDBcard
	typedef std::map<CGUID, CDBCard*> DBCardMap;
	typedef DBCardMap::iterator DBCardMapItr;
	// CDBLoginPlayer
	// CGUID 是唯一的
	typedef std::map<CGUID, CDBLoginPlayer*> DBLoginPlayerMap;
	typedef DBLoginPlayerMap::iterator DBLoginPlayerMapItr;
	// CDKEY 是唯一的 同一个CDBLoginPlayer拥有多个CDBPlayer对象
	typedef std::map<string, tagLoginInfo*> CdkeyDBLoginPlayerMap;
	typedef CdkeyDBLoginPlayerMap::iterator CdkeyDBLoginPlayerMapItr;

	// CDBPlayer
	typedef std::map<CGUID, CDBPlayer*> DBPlayerMap;
	typedef DBPlayerMap::iterator DBPlayerMapItr;
	
	// CDBFaction
	typedef std::map<CGUID, CDBFaction*> DBFactionMap;
	typedef DBFactionMap::iterator DBFactionMapItr;
	//// CDBCityWar
	//typedef std::map<CGUID, CDBCityWar*> DBCityWarMap;
	//typedef DBCityWarMap::iterator DBCityWarMapItr;
	//// CDBVillageWar
	//typedef std::map<CGUID, CDBVillageWar*> DBVillageWarMap;
	//typedef DBVillageWarMap::iterator DBVillageWarMapItr;
	// CDBUnion
	typedef std::map<CGUID, CDBUnion*> DBUnionMap;
	typedef DBUnionMap::iterator DBUnionMapItr;
	// CDBCountry
	typedef std::map<CGUID, CDBCountry*> DBCountryMap;
	typedef DBCountryMap::iterator DBCountryMapItr;
	//// CDBEnemyFaction
	//typedef std::map<CGUID, CDBEnemyFaction*> DBEnemyFactionMap;
	//typedef DBEnemyFactionMap::iterator DBEnemyFactionMapItr;

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

	void ProcessMsg(CMessage* msg);

	//-------------- interfaces
	//CDBCard
	// 添加一个DBCard到Map末尾
	void AddOneDBCard(CDBCard* card);
	// 删除一个DBCard
	void DelOneDBCard(CDBCard* card);
	// 删除一个DBCard
	void DelOneDBCard(const CGUID& cardID);
	// 清空链表
	void ClearCardMap(void);
	// 取得个数
	long GetDBCardNum(void) { return m_mapDBCards.size(); }
	DBCardMapItr DBCardMapBegin(void) { return m_mapDBCards.begin(); }
	DBCardMapItr DBCardMapEnd(void) { return m_mapDBCards.end(); }
	DBCardMap& GetDBCardMap(void) { return m_mapDBCards; }

	// CDBLoginPlayer
	// CGUID只能标识玩家角色的GUID,一个CDKEY可以有多个角色
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
	// 玩家第一次登录时初始化
	bool InitLoginPlayer(const char* szCdkey);
	// 添加一个CDBLoginPlayer到Map末尾
	void AddLoginPlayerByCdkey(CDBLoginPlayer* player);
	// 删除一个CDBLoginPlayer
	void DelLoginPlayerByCdkey(CDBLoginPlayer* player);
	// 清空链表
	void ClearCdkeyLoginPlayerMap(void);
	// 取得个数
	CdkeyDBLoginPlayerMap& GetCdkeyLoginPlayerMap(void) { return m_mapCdkeyLoginPlayers; }
	tagLoginInfo* FindLoginPlayerByCdkey(const char* szCdkey);

	
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
	CDBRegion* FindRegion(const CGUID& guid);

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
	void ClearDBLinkmanMap(const CGUID& PlayerGuid);
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

	void CGoodsToCDBGood(CGoods* good, CDBGoods* dbGood, const CGUID& ownerID);
	void CPlayerToCDBPlayer(CPlayer* player, CDBPlayer* dbPlayer);
	void CPlayerToCDBLoginPlayer(CPlayer* player, CDBLoginPlayer* dbLPlayer);
	void CFactionToCDBFaction(CFaction* fac, CDBFaction* dbFac);
	void CCardToCDBCard(CCard* card, CDBCard* dbCard);
	//void CCityWarToCDBCityWar(tagAttackCityTime* CityWarParam, CDBCityWar* cw);
	void CCountryToCDBCountry(CCountry* country, CDBCountry* dbCountry);
	//void CEnemyFactionToCDBEnemyFaction(tagEnemyFaction* efParam, CDBEnemyFaction* ef);
	void CVarsListToCDBGenVar(CVariableList* vl, CDBGenVarGroup* pGenVarGroup);
	void CWorldRgnToCDBRgn(CWorldRegion* wRgn, CDBRegion* dbRgn);
	void CSetupToCDBSetup(CDBSetup* dbSetup);
	void CUnionToCDBUnion(CUnion* u, CDBUnion* dbU);
	//void CVillageWarToCDBVillageWar(tagVilWarSetup* vws, CDBVillageWar* dbs);
	// 副本场景转换
	void CWorldRgnToCDBDupRgn(CWorldRegion* wRgn, CDBDupRgn* dbDupRgn);
	// 邮件对象转化
	void CMailToCDBMail(CMail* pMail, CDBMail* pDBMail);
	// 邮件物品对象转换
	void CGoodsToCDBMailGoods(CGoods* pGoods, CDBMailGoods* pDBMailGoods, const CGUID& ownerID);
	void CPlayerToCDBPlayerMails(CPlayer* player, CDBPlayer* dbPlayer);

	void CDBGoodToCGoods		(CDBGoods* dbGood, CGoods* good, const CGUID& ownerID);
	void CDBPlayerToCPlayer		(CDBPlayer* dbPlayer, CPlayer* player);
	void CDBLoginPlayerToCPlayer(CDBLoginPlayer* dbLPlayer, CPlayer* player);
	void CDBFactionToCFaction	(CDBFaction* dbFac, CFaction* fac);
	void CDBCardToCCard(CDBCard* dbCard, CCard* card);
	//void CCDBCityWarToCityWar(CDBCityWar* cw);
	void CDBCountryToCCountry(CDBCountry* dbCountry, CCountry* country);
	//void CDBEnemyFactionToCEnemyFaction(CDBEnemyFaction* ef);
	void CCDBGenVarToVarsList(CDBGenVarGroup* pGenVarGroup, CVariableList* vl);
	void CCDBRgnToWorldRgn(CDBRegion* dbRgn, CWorldRegion* wRgn);
	void CDBSetupToCSetup(CDBSetup* dbSetup);
	void CDBUnionToCUnion(CDBUnion* dbU, CUnion* u);
	//void CCDBVillageWarToVillageWar(CDBVillageWar* dbs);
	// 副本场景转换
	void CDBDupRgnToCWorldRgn(CDBDupRgn* dbDupRgn, CWorldRegion* wRgn);
	// 邮件对象转化
	void CDBMailToCMail(CDBMail* pDBMail, CMail* pMail);
	// 邮件物品对象转换
	void CDBMailGoodsToCGoods(CDBMailGoods* pDBMailGoods, CGoods* pGoods, const CGUID& ownerID);
	void CDBPlayerToCPlayerMails(CDBPlayer* dbPlayer, CPlayer* player);

	// 从消息取得操作对象
	CBaseDBEntity* FindEntityFromMsg(DB_OBJ_TYPE type, const CGUID& entityid, const CGUID& ownerID);
	// 创建对象						 
	CBaseDBEntity* CreateEntityByMsg(DB_OBJ_TYPE type, const CGUID& guid);

	// 分层转换 CDBPlayer 到 CPlayer
	void DetailPropertyCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailGoodsGroupCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	//void DetailEquipmentCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	//void DetailOrignalPackCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	//void DetailSubpackCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* player, long subPackPos);
	//void DetailWalletCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	//void DetailSilverCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailQuestCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailMerQuestCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);

	void DetailSkillCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailStateCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailVarDataCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);
	void DetailMedalCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer);

	// 分层转换 CPlayer 到 CDBPlayer
	void DetailPropertyCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailGoodsGroupCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer);
	//void DetailEquipmentCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer);
	//void DetailOrignalPackCPlayerToCDBPlayer(CPlayer* player, CDBPlayer* dbPlayer);
	//void DetailSubpackCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer, long subpackPos);
	//void DetailWalletCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	//void DetailSilverCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailQuestCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailMerQuestCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer);//
	void DetailSkillCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailStateCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailVarDataCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer);
	void DetailMedalCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer);
	//void DetailDepotCPlayerToCDBPlayer		(CPlayer* player, CDBPlayer* dbPlayer, eDepotContainerType eCContainerType);
	//void DetailSubDepotCPlayerToCDBPlayer	(CPlayer* player, CDBPlayer* dbPlayer, eDepotContainerType eCContainerType);

	//!					在DB中创建表格（按照配置中读取的表格信息），并为每个表格创建一个时间聚集索引
	//!					为防止日志记录被误删除，不生成DROP语句
	// 初始化属性配置
	std::map<string/*attrname*/, CDataEntityManager*>& GetObjAttrDef(void)		{ return m_mapObjAttrDef; }

	CDBGenVarGroup*   GetGenVarGroup(void);
private:
	CDBGenVarGroup*   m_pTmpGenVarGroup;
	DBCardMap m_mapDBCards;
	DBLoginPlayerMap m_mapDBLoginPlayers;
	CdkeyDBLoginPlayerMap m_mapCdkeyLoginPlayers;
	DBPlayerMap m_mapDBPlayers;
	DBLinkmanMap  m_mapDBLinkman;
	//DBCityWarMap m_mapDBCityWars;
	DBFactionMap m_mapDBFactions;
	//DBVillageWarMap m_mapDBVillageWars;
	DBUnionMap m_mapDBUnions;
	DBCountryMap m_mapDBCountrys;
	//DBEnemyFactionMap m_mapDBEnemyFactions;
	DBGenVarMap m_mapDBGenVars;
	DBSetupMap m_mapDBSetups;
	DBRegionMap m_mapDBRegions;
	DBDupRgnMap m_mapDBDupRgns;

	// Obj属性信息,Obj对象的属性名,id
	std::map<string, CDataEntityManager*> m_mapObjAttrDef;
};

//*************************
//输出SomeThing log信息
extern void PutSomeThingLogInfo(const char *msg);
//*************************
// 保存日志
extern void SaveSomeThingLogText(bool bForce);
// 加入一条记录
extern void AddSomeThingLogText(char* msg, ...);

#endif//DB_ENTITY_MANAGER_H